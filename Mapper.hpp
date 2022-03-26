

#ifndef MAP
#define MAP

class Map 
{
    public:
        Map();  // setting up default constructor
        void setFrameNum(int frameNumber);  
        void setValid();    
        bool isValid();     
        unsigned int getFrameNum();       // returns getframeNum function
    private:
        unsigned int frameNum;      
        bool valid;     


};

#endif