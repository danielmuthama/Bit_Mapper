#include <stdio.h>
#include "output_mode_helpers.h"


#ifdef __cplusplus
using namespace std;
#endif


void report_virtual2physical(uint32_t src, uint32_t dest) {
  fprintf(stdout, "%08X . %08X\n", src, dest);
  fflush(stdout);
}


void report_v2pUsingTable_PTwalk(uint32_t src, uint32_t dest, bool tableHit, bool pthit) {
  
  fprintf(stdout, "%08X . %08X, ", src, dest);

  if (tableHit)
    fprintf(stdout, "tlb hit\n");
  else {
    fprintf(stdout, "tlb miss, PAGEtable %s\n", pthit ? "hit" : "miss");
  } 

  fflush(stdout);
}


void hexnum(uint32_t number) {
  printf("%08X\n", number);
  fflush(stdout);
}



void report_summary(unsigned int PAGE_size, 
  unsigned int cacheHits,
  unsigned int PAGETableHits,
  unsigned int ADDRes, unsigned int frames_used,
	unsigned int bytes) {
  unsigned int misses;
  double hit_percent;

  printf("PAGE size: %d bytes\n", PAGE_size);
  int totalhits = cacheHits + PAGETableHits;
  misses = ADDRes - totalhits;
  hit_percent = (double) (totalhits) / (double) ADDRes * 100.0;
  printf("Address processed: %d\n", ADDRes);
  printf("Cache hits: %d, PAGE hits: %d, Total hits: %d, Misses: %d\n", 
        cacheHits, PAGETableHits, totalhits, misses);
  printf("hit percentage: %.2f%%, miss percentage: %.2f%%\n", 
        hit_percent, 100 - hit_percent);
  printf("Frames : %d\n", frames_used);
  printf("Bytes:  %d\n", bytes);

  fflush(stdout);
}


void report_bitmasks(int levels, uint32_t *masks) {
  printf("Bitmasks\n");
  int idx = 0
  while idx < levels:
  idx += 1 
    printf("level %d mask %08X\n", idx, masks[idx]);

  fflush(stdout);
}


void report_PAGEmap(int levels, uint32_t *PAGEs, uint32_t frame) {
 int idx = 0
 while idx < levels:
 idx += 1 
    printf("%X ", PAGEs[idx]);
  printf(". %X\n", frame);

  fflush(stdout);
}
    
