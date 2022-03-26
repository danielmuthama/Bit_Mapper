
#include "pTable.hpp"


PageTable::PageTable(unsigned int numLevels, unsigned int bitsInLevel[], int vpnNumBits)
{
    // initializing
    this->addressCount = 0;
    this->frameCount = 0;
    this->numBytesSize = 0;
    this->countTlbHits = 0;
    this->countPageTableHits = 0;
    this->currFrameNum = 0;

    // initializing constructor args
    this->vpnNumBits = vpnNumBits;
    this->levelCount = numLevels;
    this->pageSizeBytes = (unsigned int)pow(2, (MEMORY_SPACE_SIZE - vpnNumBits));     // 2^(bits in offset)

    // initializing common arrays
    this->entryCountArr = new unsigned int[numLevels];
    this->maskArr = new unsigned int[numLevels];
    this->shiftArr = new unsigned int[numLevels];
    this->bitsInLevel = bitsInLevel;

    // setting up arrays, masks and shifts
    setOffsetMask(vpnNumBits);
    setOffsetShift(vpnNumBits);
    setEntryCountArr();
    setMaskArr();
    setShiftArr();

    // initialize rootLevels
    this->rootLevel = new Level(0, this);        

    this->numBytesSize += sizeof(Level);         
}



void PageTable::setEntryCountArr()
{
    for (int i = 0; i < levelCount; i++) {
        this->entryCountArr[i] = pow(2, bitsInLevel[i]);
    }
}



void PageTable::setMaskArr()
{
    unsigned int mask;
    for (int i = 0; i < levelCount; i++) {
        mask = 0;
        for (int j = 0; j <= bitsInLevel[i]; j++) {
            mask += (unsigned int)pow(2, (MEMORY_SPACE_SIZE - j));
        }
        this->maskArr[i] = mask;
    }
    shiftMaskArr();
}



void PageTable::shiftMaskArr()
{
    // shift mask arr
    int shiftAmmount = 0;
    for (int i = 1; i < levelCount; i++) {       
        shiftAmmount += bitsInLevel[i - 1];
        this->maskArr[i] = this->maskArr[i] >> shiftAmmount;
    }
}



void PageTable::setShiftArr()
{
    int shift = MEMORY_SPACE_SIZE;
    for (int i = 0; i < levelCount; i++) {
        shift = shift - bitsInLevel[i];
        this->shiftArr[i] = shift;
    }
}



void PageTable::setOffsetMask(unsigned int vpnNumBits)
{
    this->offsetMask = 0;
    unsigned int numBitsOffset = MEMORY_SPACE_SIZE - vpnNumBits;

    for (int i = 0; i < numBitsOffset; i++) {
        this->offsetMask += pow(2, i);
    }
}



void PageTable::setOffsetShift(unsigned int vpnNumBits)
{
    this->offsetShift = MEMORY_SPACE_SIZE - vpnNumBits;
}



unsigned int PageTable::getOffsetOfAddress(unsigned int virtAddr)
{
    return virtAddr & offsetMask;
}



unsigned int PageTable::virtualAddressToPageNum(unsigned int virtualAddress, unsigned int mask, unsigned int shift)
{
    // FIXME: Needs testing
    unsigned int pageNum;
    pageNum = virtualAddress & mask;
    pageNum = pageNum >> shift;
    return pageNum;
}



void PageTable::pageInsert(Level* levelPointer, unsigned int virtualAddress)
{
    unsigned int mask = maskArr[levelPointer->currentDepth];
    unsigned int shift = shiftArr[levelPointer->currentDepth];
    unsigned int pageNum = virtualAddressToPageNum(virtualAddress, mask, shift);

    // go here if levelPointer is a leaf node
    if (levelPointer->currentDepth == levelCount - 1) {
        // go here if mapPointer array hasn't been instantiated
        if (levelPointer->mapPointer == nullptr) {
            levelPointer->setmapPointer();    // instantiate mapPointer
            numBytesSize += sizeof(Map) * entryCountArr[levelPointer->currentDepth];
        }
        levelPointer->mapPointer[pageNum].setFrameNum(currFrameNum);
        levelPointer->mapPointer[pageNum].setValid();
        currFrameNum++;
    } 
    // go here if levelPointer is interior node
    else {
        // go here if pageNum at this level has already been set
        if (levelPointer->nextLevel[pageNum] != nullptr) {
            pageInsert(levelPointer->nextLevel[pageNum], virtualAddress);
        }
        // go here if nextLevel[pageNum] has not been set yet
        else {
            Level* newLevel = new Level(levelPointer->currentDepth + 1, this);   // newLevels depth is currentDepth + 1
            levelPointer->nextLevel[pageNum] = newLevel;
            numBytesSize += sizeof(Level) * entryCountArr[levelPointer->currentDepth];
            pageInsert(newLevel, virtualAddress);
        }
    }
}



Map* PageTable::pageLookup(Level* levelPointer, unsigned int virtualAddress)
{
    unsigned int mask = maskArr[levelPointer->currentDepth];
    unsigned int shift = shiftArr[levelPointer->currentDepth];
    unsigned int pageNum = virtualAddressToPageNum(virtualAddress, mask, shift);

    // go here if levelPointer is a leaf node
    if (levelPointer->currentDepth == levelCount - 1) {
        // // go here if mapPointer not set
        if (levelPointer->mapPointer == nullptr) {
            return nullptr;
        }
        // go here if map invalid
        if (!levelPointer->mapPointer[pageNum].isValid()) {
            return nullptr;
        }
        // returns this if page hit
        return &(levelPointer->mapPointer[pageNum]);
    } 
    // go here if levelPointer is interior node
    if (levelPointer->nextLevel[pageNum] == nullptr) {
        return nullptr;
    }

    pageLookup(levelPointer->nextLevel[pageNum], virtualAddress);     // recursion to next level
    
}



unsigned int PageTable::appendOffset(unsigned int frameNum, unsigned int virtualAddress)
{
    unsigned int physicalAddr = frameNum << offsetShift;
    physicalAddr = physicalAddr | getOffsetOfAddress(virtualAddress);
    return physicalAddr;
}





