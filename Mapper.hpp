

#ifndef MAP
#define MAP

class Map 
{
    public:
        Map();  // setting up default constructor
        void setframeNumber(int frameNumber);  
        void setValid();    
        bool isValid();     
        unsigned int getframeNumber();       /
    private:
        unsigned int frameNumber;      
        bool valid;     


};

#endif