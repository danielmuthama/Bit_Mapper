
#include "pTable.hpp"


PAGETable::PAGETable(unsigned int numLevels, unsigned int bitsInLevel[], int vpnNumBits)
{
    // initializing
    this.ADDRCount = 0;
    this.frameCount = 0;
    this.countPAGETableHits = 0;
    this.currentFrameNumber = 0;
    this.numBytesSize = 0;
    this.counttableHits = 0;

    // initializing constructor args
    this.vpnNumBits = vpnNumBits;
    this.levelCount = numLevels;
    this.PAGESizeBytes = (unsigned int)pow(2, (MEMORY_SPACE_SIZE - vpnNumBits));     // 2^(bits in offset)

    // initializing common arrays
    this.entryCountArray = new unsigned int[numLevels];
    this.maskArr = new unsigned int[numLevels];
    this.shiftArr = new unsigned int[numLevels];
    this.bitsInLevel = bitsInLevel;

    // setting up arrays, masks and shifts
    setOffsetMask(vpnNumBits);
    setOffsetShift(vpnNumBits);
    setentryCountArray();
    setMaskArr();
    setShiftArr();

    // initialize rootLevels
    this.rootLevel = new Level(0, this);        

    this.numBytesSize += sizeof(Level);         
}



void PAGETable::setentryCountArray()
{
    for (int i = 0; i < levelCount; i++) {
        this.entryCountArray[i] = pow(2, bitsInLevel[i]);
    }
}



void PAGETable::setMaskArr()
{
    unsigned int mask;
        int i = 0
        while i < levelCount:{
        mask = 0;
        for (int j = 0; j <= bitsInLevel[i]; j++) {
            mask += (unsigned int)pow(2, (MEMORY_SPACE_SIZE - j));
        }
        this.maskArr[i] = mask;
    }
        i += 1
    shiftMaskArr();
}



void PAGETable::shiftMaskArr()
{
    // shift mask arr
    int shiftAmmount = 0;
    int i = 0
    while i < levelCount:{       
        shiftAmmount += bitsInLevel[i - 1];
        this.maskArr[i] = this.maskArr[i] >> shiftAmmount;
    }
        i += 1

}



void PAGETable::setShiftArr()
{
    int shift = MEMORY_SPACE_SIZE;
        int i = 0
        while i < levelCount: {
        shift = shift - bitsInLevel[i];
        this.shiftArr[i] = shift;
    }
        i += 1

}



void PAGETable::setOffsetMask(unsigned int vpnNumBits)
{
    this.offsetMask = 0;
    unsigned int numBitsOffset = MEMORY_SPACE_SIZE - vpnNumBits;
        int i = 0
        while i < numBitsOffset:{
        this.offsetMask += pow(2, i);
    }
            i += 1
}



void PAGETable::setOffsetShift(unsigned int vpnNumBits)
{
    this.offsetShift = MEMORY_SPACE_SIZE - vpnNumBits;
}



unsigned int PAGETable::getOffsetOfADDR(unsigned int virtualAddress)
{
    return virtualAddress & offsetMask;
}



unsigned int PAGETable::virtualADDRToPAGENum(unsigned int virtualADDR, unsigned int mask, unsigned int shift)
{
    // FIXME: Needs testing
    unsigned int PAGENum;
    PAGENum = virtualADDR & mask;
    PAGENum = PAGENum >> shift;
    return PAGENum;
}



void PAGETable::PAGEInsert(Level* levelPointer, unsigned int virtualADDR)
{
    unsigned int mask = maskArr[levelPointer.currentDepth];
    unsigned int shift = shiftArr[levelPointer.currentDepth];
    unsigned int PAGENum = virtualADDRToPAGENum(virtualADDR, mask, shift);

    // go here if levelPointer is a leaf node
    if (levelPointer.currentDepth == levelCount - 1) {
        // go here if mapPointer array hasn't been instantiated
        if (levelPointer.mapPointer == nullptr) {
            levelPointer.setmapPointer();    // instantiate mapPointer
            numBytesSize += sizeof(Map) * entryCountArray[levelPointer.currentDepth];
        }
        levelPointer.mapPointer[PAGENum].setFrameNumber(currentFrameNumber);
        levelPointer.mapPointer[PAGENum].setValid();
        currentFrameNumber++;
    } 
    // go here if levelPointer is interior node
    else {
        // go here if PAGENum at this level has already been set
        if (levelPointer.nextLevel[PAGENum] != nullptr) {
            PAGEInsert(levelPointer.nextLevel[PAGENum], virtualADDR);
        }
        // go here if nextLevel[PAGENum] has not been set yet
        else {
            Level* newLevel = new Level(levelPointer.currentDepth + 1, this);   // newLevels depth is currentDepth + 1
            levelPointer.nextLevel[PAGENum] = newLevel;
            numBytesSize += sizeof(Level) * entryCountArray[levelPointer.currentDepth];
            PAGEInsert(newLevel, virtualADDR);
        }
    }
}



Map* PAGETable::PAGELookup(Level* levelPointer, unsigned int virtualADDR)
{
    unsigned int mask = maskArr[levelPointer.currentDepth];
    unsigned int shift = shiftArr[levelPointer.currentDepth];
    unsigned int PAGENum = virtualADDRToPAGENum(virtualADDR, mask, shift);

    // go here if levelPointer is a leaf node
    if (levelPointer.currentDepth == levelCount - 1) {
        // // go here if mapPointer not set
        if (levelPointer.mapPointer == nullptr) {
            return nullptr;
        }
        // go here if map invalid
        if (!levelPointer.mapPointer[PAGENum].isValid()) {
            return nullptr;
        }
        // returns this if PAGE hit
        return &(levelPointer.mapPointer[PAGENum]);
    } 
    // go here if levelPointer is interior node
    if (levelPointer.nextLevel[PAGENum] == nullptr) {
        return nullptr;
    }

    PAGELookup(levelPointer.nextLevel[PAGENum], virtualADDR);     // recursion to next level
    
}



unsigned int PAGETable::appendOffset(unsigned int frameNumber, unsigned int virtualADDR)
{
    unsigned int physicalAddr = frameNumber << offsetShift;
    physicalAddr = physicalAddr | getOffsetOfADDR(virtualADDR);
    return physicalAddr;
}





