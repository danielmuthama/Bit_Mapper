
#ifndef LEVEL
#define LEVEL

#include <stdlib.h>
#include "Mapper.hpp"
#include <math.h>

class PAGETable;  

class Level
{
    public:
// setting up a default constructor
        Level();    
        Level(int, PAGETable*);    
        Level** nextLevel;        
        Map* mapPointer;                
        unsigned int currentDepth;     
        PAGETable* p_Table;          
        void setNextLevel();        
        void setNextLevelNull();    
        void setmapPointer();          
};




#endif