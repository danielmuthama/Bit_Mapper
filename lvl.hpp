
#ifndef LEVEL
#define LEVEL

#include <stdlib.h>
#include "Mapper.hpp"
#include <math.h>

class PageTable;  

class Level
{
    public:
// setting up a default constructor
        Level();    
        Level(int, PageTable*);    
        Level** nextLevel;        
        Map* mapPointer;                
        unsigned int currentDepth;     
        PageTable* p_Table;          
        void setNextLevel();        
        void setNextLevelNull();    
        void setmapPointer();          
};




#endif