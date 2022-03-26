#include <stdio.h>

int main(int argc, char **argv) {
  int	ADDR = 0xFe96f962;
  int	PAGE;
  int	MASK = 0x0FF00000;


  printf("ADDR:\t0x%08x (%d decimal)\n", ADDR, ADDR);


  PAGE = ADDR & MASK;
  printf("ON MASKing:\t0x%08x\n", PAGE);

  PAGE = PAGE >> 30;

  printf("PAGE NUM = 0x%x\n", PAGE);
}
