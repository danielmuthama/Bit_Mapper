

#ifndef TLB
#define TLB

#include <map>
#include <deque>
#include "math.h"
#define MEMORY_SPACE_SIZE 32
#define MAX_QUEUE_SIZE 10


class tlb
{
    public:
        // constructor
        tlb(int vpnNumBits, int capacity);

        // cache mapping
        std::map<unsigned int /*vpn*/, unsigned int /*pfn*/> vpn2pfn;

        // queue of most recently accessed PAGEs.
        std::deque<unsigned int> recentPAGEsQueue;    // always be size 10

        // cache information
        int capacity; 
        unsigned int vpnMask;   

        // method setter
        void setVpnMask(int vpnNumBits);

        // cache methods
        bool UsingTable();
        bool hasMapping(unsigned int vpn);
        void insertMapping(unsigned int vpn, unsigned int frameNumber);

        // queue methods
        void updateQueue(unsigned int recentVpn);
        bool queueContains(unsigned int vpn);
        void delete(unsigned int vpn);
        
        
        
};

#endif
