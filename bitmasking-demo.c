#include <stdio.h>

int main(int argc, char **argv) {

  int	Page;
  
  /* The three level page table will be:
   * Level 0:  4 bits
   * Level 1:  8 bits
   * Level 2:  8 bits
   * offset bits will be computed as follows 32 - (4 + 8 + 8) = 12 bits
   * page size will be 2^12 = 4096 bytes
   */

  int	Address = 0xFe98f982;

 
  int	Mask = 0x0FF00000;


  printf("Address:\t0x%08x (%d decimal)\n", Address, Address);

  /* Pulling out all the relevant bits in bitwise manner */

  Page = Mask & Address;
  printf("ON masking:\t0x%08x\n", Page);

  /* Shifting right all the relevant number of bits */
  Page = Page >> 20;

  printf("Page NUM = 0x%x\n", Page);
}
