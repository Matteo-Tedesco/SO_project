#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct BUF {
  uint8_t size;
  uint8_t n_items;
  char* buffer;
} BUF;

void BUF_create(BUF* buf, char* buffer, uint8_t size);

void BUF_print(BUF* buf);

extern void putChar(char c);

extern char getChar(void);

extern void fill_read(void);

extern void empty_write(void);

void write_uart(char data);

char read_uart(void);