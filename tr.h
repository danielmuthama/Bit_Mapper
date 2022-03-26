

#ifdef __cplusplus
#include <stdint.h>
#include <fstream>
#else
#include <inttypes.h>
#endif



typedef struct BYUADDRTRACE
{
  unsigned char attr;
  unsigned char proc;
  uint32_t time;
  uint32_t addr;
  unsigned char reqtype;
  unsigned char size;

} p2AddrTr;
typedef enum {
  UNKNOWN,
  LITTLE,
  BIG 
} ENDIAN;

int NextADDR(FILE *trace_file, p2AddrTr *addr_ptr);
#define FETCH 0x00 
#define MEMREAD 0x01 
#define MEMREADINV 0x02 
#define MEMWRITE 0x03 
#define IOREAD 0x10 
#define IOWRITE 0x11 
#define DEFERREPLY 0x20 
#define INTA 0x21 
#define CNTRLAGNTRES 0x22 
#define BRTRACEREC 0x23 
#define SHUTDOWN 0x31 
#define FLUSH 0x32 
#define HALT 0x33 
#define SYNC 0x34 
#define FLUSHACK 0x35 
#define STOPCLKACK 0x36 
#define SMIACK 0x37 
						

