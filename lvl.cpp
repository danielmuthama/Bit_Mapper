
#include "pTable.hpp"

// adding default constructor for the nextlevel
Level::Level()
{
    currentDepth = 0;
    p_Table = NULL;
}

// constructing sets depth, pageTable and mapPointer
Level::Level(int depth, PageTable* tablePointer)
{
    currentDepth = depth;
    p_Table = tablePointer;

// constructing sets for the nextlevel
    setNextLevel();
    setNextLevelNull();  
    mapPointer = nullptr;       
}

// assigning an array to the nextLevel*
void Level::setNextLevel()
{
// nextLevel size will be = all possible levels at the currentDepth
    nextLevel = new Level*[p_Table->entryCountArr[currentDepth]];
}

// setting all possible levels to Null
void Level::setNextLevelNull()
{
    for (int i = 0; i < p_Table->entryCountArr[currentDepth]; i++) {
        nextLevel[i] = nullptr;
    }
}

// using new operator to assign mapPointer a Mapping array
void Level::setmapPointer()
{
    // mapPointer size will be = all possible levels based on numBits in level
    this->mapPointer = new Map[p_Table->entryCountArr[currentDepth]];
}