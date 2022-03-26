

#include "table.hpp"
#include <stack>



tlb::tlb(int vpnNumBits, int capacity)
{
    this->capacity = capacity;
    setVpnMask(vpnNumBits);
}



bool tlb::usingTlb()
{
    return this->capacity != 0;
}



void tlb::setVpnMask(int vpnNumBits)
{
    this->vpnMask = 0;
    for (int i = 0; i <= vpnNumBits; i++) {
        vpnMask += pow(2, MEMORY_SPACE_SIZE - i);
    }
}



bool tlb::hasMapping(unsigned int vpn)
{
    if (vpn2pfn.find(vpn) != vpn2pfn.end()) {
        return true;
    }
    return false;
}



void tlb::insertMapping(unsigned int vpn, unsigned int frameNum)
{
    // if tlb at capacity need to erase the least recent used address
    if (vpn2pfn.size() >= capacity) {
        vpn2pfn.erase(recentPagesQueue.front());     // erase least recently used
        recentPagesQueue.pop_front();       // erase this from queue as well
    }
    
    vpn2pfn[vpn] = frameNum;
}


bool tlb::queueContains(unsigned int vpn)
{
    for (int i = 0; i < recentPagesQueue.size(); i++) {
        if (recentPagesQueue[i] == vpn) return true;
    }
    return false;
}



void tlb::eraseVpnFromQueue(unsigned int vpn)
{
    for (int i = 0; i < recentPagesQueue.size(); i++) {
        if (recentPagesQueue[i] == vpn) {
            recentPagesQueue.erase(recentPagesQueue.begin() + i);
        }
    }
}



void tlb::updateQueue(unsigned int recentVpn)
{
    if (queueContains(recentVpn)) {     // if queue contains vpn update vpn to most recent
        eraseVpnFromQueue(recentVpn);
    }
    
    recentPagesQueue.push_back(recentVpn);
    
    // if queue is at size, remove the oldest page accessed
    if (recentPagesQueue.size() > MAX_QUEUE_SIZE) {
        recentPagesQueue.pop_front();
    }
}