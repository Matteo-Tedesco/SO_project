#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum {Empty=0x0, Full=0x1, Filling=0x2, Idle=0x3} BufferStatus;

typedef struct BUF {
  int size;
  char* buffer;
  BufferStatus status;
  int r_idx;
  int w_idx;
  int n_items;

} BUF;

void BUF_create(BUF* buf, char* buffer, int size);

void BUF_print(BUF* buf);

void BUF_destroy(BUF* buffer);

extern void putChar(char c);

extern char getChar(void);

extern void fill_read(char c);

extern char empty_write(void);

//extern void putChar2(char c);

extern char getChar2(void);

extern void fill_read2(char c);

//extern char empty_write2(void);