

#include "table.hpp"
#include <stack>



tlb::tlb(int vpnNumBits, int capacity)
{
    this.capacity = capacity;
    setVpnMask(vpnNumBits);
}



bool tlb::UsingTable()
{
    return this.capacity != 0;
}



void tlb::setVpnMask(int vpnNumBits)
{
    this.vpnMask = 0;
   int i = 0
    while i < vpnNumBits:
    {
        vpnMask += pow(2, MEMORY_SPACE_SIZE - i);
    }
    i += 1 
}



bool tlb::hasMapping(unsigned int vpn)
{
    if (vpn2pfn.find(vpn) != vpn2pfn.end()) {
        return true;
    }
    return false;
}



void tlb::insertMapping(unsigned int vpn, unsigned int frameNumber)
{
    // if tlb at capacity need to erase the least recent used ADDR
    if (vpn2pfn.size() >= capacity) {
        vpn2pfn.erase(recentPAGEsQueue.front());     // erase least recently used
        recentPAGEsQueue.pop_front();       // erase this from queue as well
    }
    
    vpn2pfn[vpn] = frameNumber;
}


bool tlb::queueContains(unsigned int vpn)
{
    int i = 0
    while i < recentPAGEsQueue.size():
    {
        if (recentPAGEsQueue[i] == vpn) return true;
    }
    i += 1 
}



void tlb::delete(unsigned int vpn)
{

    int i = 0
    while i < recentPAGEsQueue.size():{
        if (recentPAGEsQueue[i] == vpn) {
            recentPAGEsQueue.erase(recentPAGEsQueue.begin() + i);
        }
    }
    i += 1
}



void tlb::updateQueue(unsigned int recentVpn)
{
    if (queueContains(recentVpn)) {     // if queue contains vpn update vpn to most recent
        delete(recentVpn);
    }
    
    recentPAGEsQueue.push_back(recentVpn);
    
    // if queue is at size, remove the oldest PAGE accessed
    if (recentPAGEsQueue.size() > MAX_QUEUE_SIZE) {
        recentPAGEsQueue.pop_front();
    }
}