#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct BUF {
  int size;
  char* buffer;
  int n_items;
} BUF;

void BUF_create(BUF* buf, char* buffer, int size);

void BUF_print(BUF* buf);

extern void putChar(char c);

extern char getChar(void);

extern void fill_read(void);

extern void empty_write(void);

void write_uart(char data);

char read_uart(void);