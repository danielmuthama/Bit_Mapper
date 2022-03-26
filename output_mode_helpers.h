#ifndef OUTPUT_MODE_HELPERS_H
#define OUTPUT_MODE_HELPERS_H


#include <stdint.h>
#else
/* C includes */
#include <inttypes.h>
#include <stdbool.h>
#endif 

\
typedef struct {
  bool bitmasks;  
  bool virtual2physical;  
  bool v2pTable_pt; 
  bool vpn2pfn;  
  bool offset; 
  bool summary; 
} OutputOptionsType;


void report_virtual2physical(uint32_t src, uint32_t dest);


void report_v2pUsingTLB_PTwalk(uint32_t src, uint32_t dest, bool tableHit, bool pthit);


void hexnum(uint32_t number);


void report_summary(unsigned int PAGE_size, 
                    unsigned int cacheHits,
                    unsigned int PAGETableHits, 
                    unsigned int ADDRes, unsigned int frames_used,
                    unsigned int bytes);


void report_bitmasks(int levels, uint32_t *masks);


void report_PAGEmap(int levels, uint32_t *PAGEs, uint32_t frame);

#endif
