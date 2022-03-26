

#include "Mapper.hpp"

// default constructor
Map::Map()
{
    frameNum = 0;       // default set to 0 until updated
    valid = false;      // default set to false until updated
}


void Map::setFrameNum(int frameNumber)
{
    this->frameNum = frameNumber;
}


void Map::setValid()
{
    this->valid = true;
}


bool Map::isValid()
{
    return this->valid;
}


unsigned int Map::getFrameNum()
{
    return frameNum;
}