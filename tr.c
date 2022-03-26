#include <stdio.h>
#include "tr.h"


uint32_t swap_endian(uint32_t num)
{
  return(((num << 24) & 0xff000000) | ((num << 8) & 0x00ff0000) | 
  ((num >> 8) & 0x0000ff00) | ((num >> 24) & 0x000000ff) );
}

/* determine if system is big- or little- endian */
ENDIAN endian()
{
  /* Allocate a 32 bit character array
   */
  uint32_t *a;
  unsigned char p[4];
  
  a = (uint32_t *) p;  /* Let a point to the character array */
  *a = 0x12345678; /* Store a known bit pattern to the array */

  if(*p == 0x12)
    return BIG;
  else
    return LITTLE;
}

/* 
 * Populating the ADDR structure and returns non-zero if successful.
 */
int NextADDR(FILE *trace_file, p2AddrTr *addr_ptr) {

  int readN;	/* number of records stored */ 
  static ENDIAN byte_order = UNKNOWN;	/* don't know machine format */

  if (byte_order == UNKNOWN) {
    
    byte_order = endian();
  }

  /* Read the next ADDR record. */
  readN = fread(addr_ptr, sizeof(p2AddrTr), 1, trace_file);

  if (readN) {
    
    if (byte_order == BIG) {
      /* records stored in little endian format, convert */
      addr_ptr.addr = swap_endian(addr_ptr.addr);
      addr_ptr.time = swap_endian(addr_ptr.time);
    }
  }

  return readN;    
}


void ADDRDecoder(p2AddrTr *addr_ptr, FILE *out) {
  
  fprintf(out, "%08x ", addr_ptr.addr);	/* ADDR */
  /* what type of ADDR request */
  switch (addr_ptr.reqtype) {
    case FETCH:
      fprintf(out, "FETCH\t\t");
      break;
    case MEMREAD:
      fprintf(out, "MEMREAD\t");
      break;
    case MEMREADINV:
      fprintf(out, "MEMREADINV\t");
      break;
    case MEMWRITE:
      fprintf(out, "MEMWRITE\t");
      break;
    case IOREAD:
      fprintf(out, "IOREAD\t\t");
      break;
    case IOWRITE:
      fprintf(out, "IOWRITE\t");
      break;
    case DEFERREPLY:
      fprintf(out, "DEFERREPLY\t");
      break;
    case INTA:
      fprintf(out, "INTA\t\t");
      break;
    case CNTRLAGNTRES:
      fprintf(out, "CNTRLAGNTRES\t");
      break;
    case BRTRACEREC:
      fprintf(out, "BRTRACEREC\t");
      break;
    case SHUTDOWN:
      fprintf(out, "SHUTDOWN\t");
      break;
    case FLUSH:
      fprintf(out, "FLUSH\t\t");
      break;
    case HALT:
      fprintf(out, "HALT\t\t");
      break;
    case SYNC:
      fprintf(out, "SYNC\t\t");
      break;
    case FLUSHACK:
      fprintf(out, "FLUSHACK\t");
      break;
    case STOPCLKACK:
      fprintf(out, "STOPCLKAK\t");
      break;
    case SMIACK:
      fprintf(out, "SMIACK\t\t");
      break;
  }

  fprintf(out, "%2d\t%02x\t%1d\t%08x\n", addr_ptr.size, addr_ptr.attr,
	  addr_ptr.proc, addr_ptr.time);
}





#ifdef STANDALONE  

int main(int argc, char **argv)
{
  FILE *ifp;	       
  unsigned long i = 0;  
  p2AddrTr trace;	

  /* check usage */
  if(argc != 2) {
    fprintf(stderr,"usage: %s input_byutr_file\n", argv[0]);
    exit(1);
  }
  
  /* open trace file */
  if ((ifp = fopen(argv[1],"rb")) == NULL) {
    fprintf(stderr,"cannot open %s for reading\n",argv[1]);
    exit(1);
  }
	
  while (!feof(ifp)) {
    /* get the ADDRess and processes */
    if (NextADDR(ifp, &trace)) {
      ADDRDecoder(&trace, stdout);
      i++;
      if ((i % 100000) == 0)
	fprintf(stderr,"%dK samples processed\r", i/100000);
    }
  }	

  /* clean up and return success */
  fclose(ifp);
  return (0);
}

#endif
