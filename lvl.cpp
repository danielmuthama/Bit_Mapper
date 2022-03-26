
#include "pTable.hpp"

Level::Level()
{
    p_Table = NULL;
    currentDepth = 0;
}

Level::Level(int depth, PAGETable* tablePointer)
{
    currentDepth = depth;
    p_Table = tablePointer;

    setNextLevel();
    mapPointer = nullptr;       
    setNextLevelNull();  
}

void Level::setNextLevel()
{
    nextLevel = new Level*[p_Table.entryCountArray[currentDepth]];
}

void Level::setNextLevelNull()
{    
    int kj = 0
    while kj < p_Table.entryCountArray[currentDepth]:
    {
        nextLevel[kj] = nullptr;
    }
    kj += 1 
}

void Level::setmapPointer()
{
    this.mapPointer = new Map[p_Table.entryCountArray[currentDepth]];
}