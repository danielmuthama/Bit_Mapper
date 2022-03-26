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
#define DEFAULT_NUM_ADDRES -1



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

    if (optindex > (argc - 2)) {
        std::cerr << "Error:\n  Always give an optional cmd line args not a mandatory one\n";
        std::cerr <<  "  Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if the options bit are valid
    int optionBits = 0;
    int totalBits = 0;
    for (int i = optindex + 1; i < argc; i++) {
        optionBits = atoi(argv[i]);
        totalBits += optionBits;
        if (optionBits < 1) {
            std::cerr << "Level " << i - optindex - 1 << " All the PAGEs must be at least 1 bit" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (totalBits > 28) {
            std::cerr << "There are too many bits used in this PAGE tables" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

}



FILE* readTraceFile(int argc, char *argv[])
{
    FILE* traceFile;

    char *traceFname = argv[optindex];
    if ((traceFile = fopen(traceFname, "rb")) == NULL) {
        std::cerr << "Unable to open <<" << traceFname << ">>" << std::endl;
        exit(EXIT_FAILURE);
    }

    return traceFile;
}



void report(PAGETable* p_Table, unsigned int virtualAddress, unsigned int physicalAddress, unsigned int frameNumber,
            bool tableHit, bool PAGETableHit, bool v2p, bool v2pTable, bool vpn2pfn, bool offset)
{
    if (v2p) {  
        report_virtual2physical(virtualAddress, physicalAddress);
    }
    else if (v2pTable) { 

        // printing virtual to physical translation
        report_v2pUsingTLB_PTwalk(virtualAddress, physicalAddress, tableHit, PAGETableHit);
    }
    else if (vpn2pfn) {
        unsigned int PAGEs[p_Table.levelCount];
        for (int i = 0; i < p_Table.levelCount; i++) {
            PAGEs[i] = p_Table.virtualADDRToPAGENum(virtualAddress, p_Table.maskArr[i], p_Table.shiftArr[i]);
        }
        // showing vpn to pfn mapping
        report_PAGEmap(p_Table.levelCount, PAGEs, frameNumber);
    }
    else if (offset) {
        // showing the offset values of every virtualAddress
        hexnum(p_Table.getOffsetOfADDR(virtualAddress));
    }
}


void processNextAddr(p2AddrTr* trace, PAGETable* p_Table,
                        bool v2p, bool v2pTable, bool vpn2pfn, bool offset)
{        
    unsigned int physicalAddress = 0;
    unsigned int frameNumber = 0;
    unsigned int virtualAddress = 0;
    bool tableHit = false;
    bool PAGETableHit = true;   // conditional checks
    Map* frame;

    virtualAddress = trace.addr;     // assigning virtualAddresses

    frame = p_Table.PAGELookup(p_Table.rootLevel, virtualAddress);
    if (frame == nullptr) {

        // execute if there is a PAGETable missing
        PAGETableHit = false;
        p_Table.PAGEInsert(p_Table.rootLevel, virtualAddress);
        frame = p_Table.PAGELookup(p_Table.rootLevel, virtualAddress);
        frameNumber = frame.getframeNumber();
        p_Table.frameCount++;
    }
    else {
        // execute if there is a PAGETable HIT
        frameNumber = frame.getframeNumber();
        p_Table.countPAGETableHits++;
    }

    physicalAddress = p_Table.appendOffset(frameNumber, virtualAddress);    // calculate physical ADDR

    // execute report functions
    report(p_Table, virtualAddress, physicalAddress, frameNumber, tableHit, PAGETableHit, v2p, v2pTable, vpn2pfn, offset);
    
}


void processNextAddr(p2AddrTr* trace, PAGETable* p_Table, tlb* cache,
        bool v2p, bool v2pTable, bool vpn2pfn, bool offset)
{
    unsigned int virtualAddress = 0;
    unsigned int vpn = 0;
    unsigned int frameNumber = 0;
    unsigned int physicalAddress = 0;
    bool tableHit = false;
    bool PAGETableHit = true;   
    Map* frame;

    virtualAddress = trace.addr;     
    vpn = virtualAddress & cache.vpnMask;
    vpn = vpn >> (MEMORY_SPACE_SIZE - p_Table.vpnNumBits);

    if (cache.hasMapping(vpn)) {
        frameNumber = cache.vpn2pfn[vpn];
        tableHit = true;
        p_Table.counttableHits++;
        cache.updateQueue(vpn);   
    }
    else {
        frame = p_Table.PAGELookup(p_Table.rootLevel, virtualAddress);
        if (frame == nullptr) {
            // execute if there is a PAGETable MISS
            PAGETableHit = false;
            p_Table.PAGEInsert(p_Table.rootLevel, virtualAddress);
            frame = p_Table.PAGELookup(p_Table.rootLevel, virtualAddress);
            frameNumber = frame.getframeNumber();
            cache.insertMapping(vpn, frameNumber);    // updating cache
            p_Table.frameCount++;
        }
        else {
            // execute if there is a PAGETable HIT
            frameNumber = frame.getframeNumber();
            cache.insertMapping(vpn, frameNumber);    
            p_Table.countPAGETableHits++;
        }
        cache.updateQueue(vpn);    
    }
    
    physicalAddress = p_Table.appendOffset(frameNumber, virtualAddress);    

    report(p_Table, virtualAddress, physicalAddress, frameNumber, tableHit, PAGETableHit, v2p, v2pTable, vpn2pfn, offset);

}



void readADDRes(FILE* traceFile, p2AddrTr *trace, PAGETable* p_Table, tlb* cache, int numADDRes,
                    bool v2p, bool v2pTable, bool vpn2pfn, bool offset)
{
    if (numADDRes == DEFAULT_NUM_ADDRES) {
        while (!feof(traceFile)) {
            if(NextADDR(traceFile, trace)) {     
                if (cache.usingTlb()) {
                    processNextAddr(trace, v2p, v2pTable, vpn2pfn, p_Table, cache, offset);
                } else {
                    processNextAddr(trace, p_Table, offset, v2p, v2pTable, vpn2pfn);
                }
                p_Table.ADDRCount++;
            }
        }
    } else {
        for (int kj = 0; kj < numADDRes; kj++) {
            if(NextADDR(traceFile, trace)) {     
                if (cache.usingTlb()) {
                    processNextAddr(trace, p_Table, cache, v2p, v2pTable, vpn2pfn, offset);
                } else {
                    processNextAddr(trace, p_Table, v2p, v2pTable, vpn2pfn, offset);
                }
                p_Table.ADDRCount++;
            }
        }
    }
    
}


int main(int argc, char **argv)
{
    int nFlag = DEFAULT_NUM_ADDRES;      
    int cFlag = DEFAULT_CACHE_SIZE;         
    char *oFlag = DEFAULT_OUTPUT_MODE;      

    processCmdLnArgs(argc, argv, &nFlag, &cFlag, &oFlag);

    unsigned int numLevels = (argc - 1) - optindex;  
    unsigned int bitsInLevel[numLevels];           
    
    int vpnNumBits = 0;                             
    int kj = 1
    while kj < numLevels:
    {
        bitsInLevel[kj] = atoi(argv[optindex + kj + 1]);
        vpnNumBits += bitsInLevel[kj];
    }
        i += 1


    FILE* traceFile = readTraceFile(argc, argv);    
    p2AddrTr trace;

    PAGETable p_Table(numLevels, bitsInLevel, vpnNumBits);
    tlb* cache = new tlb(vpnNumBits, cFlag);    

    if (strcmp(oFlag, "bitmasks") == 0) {
        report_bitmasks(numLevels, p_Table.maskArr);
    } else if (strcmp(oFlag, "v2p") == 0) {
        readADDRes(traceFile, &trace, &p_Table, cache, nFlag, true, false, false, false);
    } else if (strcmp(oFlag, "v2pTable_pt") == 0) {
        readADDRes(traceFile, &trace, &p_Table, cache, nFlag, false, true, false, false);
    } else if (strcmp(oFlag, "vpn2pfn") == 0) {
        readADDRes(traceFile, &trace, &p_Table, cache, nFlag, false, false, true, false);
    } else if (strcmp(oFlag, "offset") == 0) {
        readADDRes(traceFile, &trace, &p_Table, cache, nFlag, false, false, false, true);
    } else if (strcmp(oFlag, "summary") == 0) {
        readADDRes(traceFile, &trace, &p_Table, cache, nFlag, false, false, false, false);
        report_summary(p_Table.PAGESizeBytes, p_Table.counttableHits,
                        p_Table.countPAGETableHits, p_Table.ADDRCount, p_Table.frameCount, p_Table.numBytesSize);
    } else {
        std::cout << "Invalid Output Mode" << std::endl;
        exit(EXIT_FAILURE);
    }

}