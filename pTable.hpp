
#ifndef PAGETABLE
#define PAGETABLE

#include "lvl.hpp"
#include "table.hpp"
#include "tr.h"

#define MEMORY_SPACE_SIZE 32





class PAGETable
{
    public:
        // constructor
        PAGETable(unsigned int, unsigned int*, int);

        // ptr to root level
        Level* rootLevel;
        
        // bit arrays and entryCountArray
        unsigned int *maskArr;
        unsigned int *shiftArr;
        unsigned int *entryCountArray;
        unsigned int *bitsInLevel;
        unsigned int offsetMask;        
        unsigned int offsetShift;

        // PAGETable information
        unsigned int levelCount;
        unsigned int ADDRCount;
        unsigned int numBytesSize;
        unsigned int frameCount;
        unsigned int vpnNumBits;
        unsigned int PAGESizeBytes;
        unsigned int currentFrameNumber;      

        // hit counts
        unsigned int countPAGETableHits;
        unsigned int counttableHits;

        // set array, mask and shift methods
        void setMaskArr();
        void shiftMaskArr();      
        void setShiftArr();
        void setentryCountArray();
        void setOffsetMask(unsigned int vpnNumBits);
        void setOffsetShift(unsigned int vpnNumBits);

        // calculation methods
        unsigned int getOffsetOfADDR(unsigned int virtualAddress);
        unsigned int virtualADDRToPAGENum(unsigned int virtualADDR, unsigned int mask, unsigned int shift);
        unsigned int appendOffset(unsigned int frameNum, unsigned int virtualADDR);
        
        // PAGE walk methods
        void PAGEInsert(Level* levelPointer, unsigned int virtualADDR);
        Map* PAGELookup(Level* levelPointer, unsigned int virtualADDR);
        
};




#endif

