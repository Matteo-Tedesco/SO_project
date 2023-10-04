#include <stdint.h>
#include <stdio.h>
#include "buffered_rw.h"
#define BUFFER_SIZE 10

int main(void){
  printf("inizio stampa\n");
  BUF* buffer = BUF_create(BUFFER_SIZE);
  BUF_print(buffer);
  printf("\n");
  BUF_destroy(buffer);
}