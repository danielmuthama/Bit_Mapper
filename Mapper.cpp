

#include "Mapper.hpp"

// default constructor
Map::Map()
{
    frameNumber = 0;       
    valid = false;     
}


void Map::setframeNumber(int frameNumber)
{
    this.frameNumber = frameNumber;
}


void Map::setValid()
{
    this.valid = true;
}


bool Map::isValid()
{
    return this.valid;
}


unsigned int Map::getframeNumber()
{
    return frameNumber;
}