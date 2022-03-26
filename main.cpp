#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "pTable.hpp"
#include "output_mode_helpers.h"
#define MEMORY_SPACE_SIZE 32
#define DEFAULT_CACHE_SIZE 0
#define DEFAULT_OUTPUT_MODE (char*)"summary"
#define DEFAULT_NUM_ADDRESSES -1



void processCmdLnArgs(int argc, char *argv[], int *nFlag, int *cFlag, char **oFlag)
{
    // check that the minimum # of cmd-line args are given
    if (argc < 3)
    {
        std::cerr << "Error: Not enough command-line args given" << std::endl;
        std::cerr << "  \nExiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    int opt;

    // flags validation
    while ((opt = getopt (argc, argv, "n:c:o:")) != -1)
    {
        switch (opt)
        {
            case 'n':
                *nFlag = atoi(optarg);
                break;
            case 'c':
                *cFlag = atoi(optarg);

                // checking the cFlag is valid
                if (*cFlag < 0) {
                    std::cerr << "Cache should always be a number, equal to or equal to 0" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o':
                *oFlag = optarg;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (optind > (argc - 2)) {
        std::cerr << "Error:\n  Always give an optional cmd line args not a mandatory one\n";
        std::cerr <<  "  Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if the options bit are valid
    int totBits = 0;
    int bitOption = 0;
    for (int i = optind + 1; i < argc; i++) {
        bitOption = atoi(argv[i]);
        totBits += bitOption;
        if (bitOption < 1) {
            std::cerr << "Level " << i - optind - 1 << " All the page table must be at least 1 bit" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (totBits > 28) {
            std::cerr << "There are too many bits used in this page tables" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

}



FILE* readTraceFile(int argc, char *argv[])
{
    FILE* traceFile;

    // check that trace file can be opened
    char *traceFname = argv[optind];
    if ((traceFile = fopen(traceFname, "rb")) == NULL) {
        std::cerr << "Unable to open <<" << traceFname << ">>" << std::endl;
        exit(EXIT_FAILURE);
    }

    return traceFile;
}



void report(PageTable* p_Table, unsigned int virtAddr, unsigned int physAddr, unsigned int frameNum,
            bool tlbHit, bool pageTableHit, bool v2p, bool v2p_tlb, bool vpn2pfn, bool offset)
{
    if (v2p) {  
        report_virtual2physical(virtAddr, physAddr);
    }
    else if (v2p_tlb) { 

        // printing virtual to physical translation
        report_v2pUsingTLB_PTwalk(virtAddr, physAddr, tlbHit, pageTableHit);
    }
    else if (vpn2pfn) {
        unsigned int pages[p_Table->levelCount];
        for (int i = 0; i < p_Table->levelCount; i++) {
            pages[i] = p_Table->virtualAddressToPageNum(virtAddr, p_Table->maskArr[i], p_Table->shiftArr[i]);
        }
        // showing vpn to pfn mapping
        report_pagemap(p_Table->levelCount, pages, frameNum);
    }
    else if (offset) {
        // showing the offset values of every virtAddr
        hexnum(p_Table->getOffsetOfAddress(virtAddr));
    }
}


void processNextAddress(p2AddrTr* trace, PageTable* p_Table,
                        bool v2p, bool v2p_tlb, bool vpn2pfn, bool offset)
{    
    unsigned int frameNum = 0;
    unsigned int virtAddr = 0;
    unsigned int physAddr = 0;
    bool tlbHit = false;
    bool pageTableHit = true;   // conditional checks
    Map* frame;

    virtAddr = trace->addr;     // assigning virtAddresses

    frame = p_Table->pageLookup(p_Table->rootLevel, virtAddr);
    if (frame == nullptr) {

        // execute if there is a PageTable missing
        pageTableHit = false;
        p_Table->pageInsert(p_Table->rootLevel, virtAddr);
        frame = p_Table->pageLookup(p_Table->rootLevel, virtAddr);
        frameNum = frame->getFrameNum();
        p_Table->frameCount++;
    }
    else {
        // execute if there is a PageTable HIT
        frameNum = frame->getFrameNum();
        p_Table->countPageTableHits++;
    }

    physAddr = p_Table->appendOffset(frameNum, virtAddr);    // calculate physical address

    // execute report functions
    report(p_Table, virtAddr, physAddr, frameNum, tlbHit, pageTableHit, v2p, v2p_tlb, vpn2pfn, offset);
    
}


void processNextAddress(p2AddrTr* trace, PageTable* p_Table, tlb* cache,
                        bool v2p, bool v2p_tlb, bool vpn2pfn, bool offset)
{
    unsigned int virtAddr = 0;
    unsigned int vpn = 0;
    unsigned int frameNum = 0;
    unsigned int physAddr = 0;
    bool tlbHit = false;
    bool pageTableHit = true;   
    Map* frame;

    virtAddr = trace->addr;     // assigning virtualAddress
    vpn = virtAddr & cache->vpnMask;
    vpn = vpn >> (MEMORY_SPACE_SIZE - p_Table->vpnNumBits);

    if (cache->hasMapping(vpn)) {
        frameNum = cache->vpn2pfn[vpn];
        tlbHit = true;
        p_Table->countTlbHits++;
        cache->updateQueue(vpn);   
    }
    else {
        frame = p_Table->pageLookup(p_Table->rootLevel, virtAddr);
        if (frame == nullptr) {
            // execute if there is a PageTable MISS
            pageTableHit = false;
            p_Table->pageInsert(p_Table->rootLevel, virtAddr);
            frame = p_Table->pageLookup(p_Table->rootLevel, virtAddr);
            frameNum = frame->getFrameNum();
            cache->insertMapping(vpn, frameNum);    // updating cache
            p_Table->frameCount++;
        }
        else {
            // execute if there is a PageTable HIT
            frameNum = frame->getFrameNum();
            cache->insertMapping(vpn, frameNum);    // updating cache
            p_Table->countPageTableHits++;
        }
        cache->updateQueue(vpn);    // updating the most recently used cache
    }
    
    physAddr = p_Table->appendOffset(frameNum, virtAddr);    // calculating the physicalAddress

//reporting a fuction
    report(p_Table, virtAddr, physAddr, frameNum, tlbHit, pageTableHit, v2p, v2p_tlb, vpn2pfn, offset);

}



void readAddresses(FILE* traceFile, p2AddrTr *trace, PageTable* p_Table, tlb* cache, int numAddresses,
                    bool v2p, bool v2p_tlb, bool vpn2pfn, bool offset)
{
    // reading all relevant virtual addresses
    if (numAddresses == DEFAULT_NUM_ADDRESSES) {
        while (!feof(traceFile)) {
            if(NextAddress(traceFile, trace)) {     // file handling
                if (cache->usingTlb()) {
                    processNextAddress(trace, p_Table, cache, v2p, v2p_tlb, vpn2pfn, offset);
                } else {
                    processNextAddress(trace, p_Table, v2p, v2p_tlb, vpn2pfn, offset);
                }
                p_Table->addressCount++;
            }
        }
    // reading the numAddresses
    } else {
        for (int i = 0; i < numAddresses; i++) {
            if(NextAddress(traceFile, trace)) {     // file handling
                if (cache->usingTlb()) {
                    processNextAddress(trace, p_Table, cache, v2p, v2p_tlb, vpn2pfn, offset);
                } else {
                    processNextAddress(trace, p_Table, v2p, v2p_tlb, vpn2pfn, offset);
                }
                p_Table->addressCount++;
            }
        }
    }
    
}


int main(int argc, char **argv)
{
    int nFlag = DEFAULT_NUM_ADDRESSES;      
    int cFlag = DEFAULT_CACHE_SIZE;         
    char *oFlag = DEFAULT_OUTPUT_MODE;      

    processCmdLnArgs(argc, argv, &nFlag, &cFlag, &oFlag);

    unsigned int numLevels = (argc - 1) - optind;  
    unsigned int bitsInLevel[numLevels];           
    int vpnNumBits = 0;                             

    // calculating vpnNumBits
    for (int i = 0; i < numLevels; i++) {
        bitsInLevel[i] = atoi(argv[optind + i + 1]);
        vpnNumBits += bitsInLevel[i];
    }

    FILE* traceFile = readTraceFile(argc, argv);    
    p2AddrTr trace;

    // instantiating PageTable
    PageTable p_Table(numLevels, bitsInLevel, vpnNumBits);
    tlb* cache = new tlb(vpnNumBits, cFlag);    

    // v2p validation
    if (strcmp(oFlag, "bitmasks") == 0) {
        report_bitmasks(numLevels, p_Table.maskArr);
    } else if (strcmp(oFlag, "virtual2physical") == 0) {
        readAddresses(traceFile, &trace, &p_Table, cache, nFlag, true, false, false, false);
    } else if (strcmp(oFlag, "v2p_tlb_pt") == 0) {
        readAddresses(traceFile, &trace, &p_Table, cache, nFlag, false, true, false, false);
    } else if (strcmp(oFlag, "vpn2pfn") == 0) {
        readAddresses(traceFile, &trace, &p_Table, cache, nFlag, false, false, true, false);
    } else if (strcmp(oFlag, "offset") == 0) {
        readAddresses(traceFile, &trace, &p_Table, cache, nFlag, false, false, false, true);
    } else if (strcmp(oFlag, "summary") == 0) {
        readAddresses(traceFile, &trace, &p_Table, cache, nFlag, false, false, false, false);
        report_summary(p_Table.pageSizeBytes, p_Table.countTlbHits,
                        p_Table.countPageTableHits, p_Table.addressCount, p_Table.frameCount, p_Table.numBytesSize);
    } else {
        std::cout << "Invalid Output Mode" << std::endl;
        exit(EXIT_FAILURE);
    }

}