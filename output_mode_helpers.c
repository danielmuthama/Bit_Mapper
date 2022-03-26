#include <stdio.h>
#include "output_mode_helpers.h"


#ifdef __cplusplus
using namespace std;
#endif

/*
 * Map between page number and frame: mapping(page, frame)
 */
void report_virtual2physical(uint32_t src, uint32_t dest) {
  fprintf(stdout, "%08X -> %08X\n", src, dest);
  fflush(stdout);
}

/*
 * Report translation/mapping from virtual page number to physical frame: 
 */
void report_v2pUsingTLB_PTwalk(uint32_t src, uint32_t dest, bool tlbhit, bool pthit) {
  
  fprintf(stdout, "%08X -> %08X, ", src, dest);

  if (tlbhit)
    fprintf(stdout, "tlb hit\n");
  else {
    fprintf(stdout, "tlb miss, pagetable %s\n", pthit ? "hit" : "miss");
  } 

  fflush(stdout);
}

/*
 * hexnum
 * Used for writing out a number in hex, one per line
 */
void hexnum(uint32_t number) {
  printf("%08X\n", number);
  fflush(stdout);
}


/*
 * Write out a mesasge that indicates summary information for the page table.
 */
void report_summary(unsigned int page_size, 
        unsigned int cacheHits,
        unsigned int pageTableHits,
        unsigned int addresses, unsigned int frames_used,
		    unsigned int bytes) {
  unsigned int misses;
  double hit_percent;

  printf("Page size: %d bytes\n", page_size);
  /* Compute misses (page faults) and hit percentage */
  int totalhits = cacheHits + pageTableHits;
  misses = addresses - totalhits;
  hit_percent = (double) (totalhits) / (double) addresses * 100.0;
  printf("Addresses processed: %d\n", addresses);
  printf("Cache hits: %d, Page hits: %d, Total hits: %d, Misses: %d\n", 
         cacheHits, pageTableHits, totalhits, misses);
  printf("Total hit percentage: %.2f%%, miss percentage: %.2f%%\n", 
         hit_percent, 100 - hit_percent);
  printf("Frames allocated: %d\n", frames_used);
  printf("Bytes used:  %d\n", bytes);

  fflush(stdout);
}

/*
 * report_bitmasks
 * Write out bitmasks.
 * levels - Number of levels
 * masks - Pointer to array of bitmasks
 */
void report_bitmasks(int levels, uint32_t *masks) {
  printf("Bitmasks\n");
  for (int idx = 0; idx < levels; idx++) 
    /* show mask entry and move to next */
    printf("level %d mask %08X\n", idx, masks[idx]);

  fflush(stdout);
}

/*
 * Write out page numbers and frame number
 */
void report_pagemap(int levels, uint32_t *pages, uint32_t frame) {
  /* outpuing pages */
  for (int idx=0; idx < levels; idx++)
    printf("%X ", pages[idx]);
  /* outputing frame */
  printf("-> %X\n", frame);

  fflush(stdout);
}
    
