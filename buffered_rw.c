#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "buffered_rw.h"
#include "interrupt.h"
#include "tcb.h"
#include "tcb_list.h"

extern BUF rd;
extern BUF wr;

extern TCBList writing_queue;
extern TCBList reading_queue;

void BUF_create(BUF* buf,char* buffer, int size){
  buf->buffer=buffer;
  buf->size=size;
  buf->n_items=0;

  for(int i = 0;i<size;i++){
    buffer[i]='\0';
  }

  return;
};

void BUF_print(BUF* buffer){
  printf("\tD %d\tN_ITEMS: %d\t -> ",buffer->size, buffer->n_items);
  for(int i = 0;i < buffer->size;i++){
    if(buffer->buffer[i] == '\0') {
      printf("*");
      continue;
    }
    printf("%c",buffer->buffer[i]);
  }
  printf("\n");
  return;
};

// Consume char from read buffer
char getChar(){
  if(rd.n_items == 0) waitWrite();
  char c = rd.buffer[0];
  for(int i = 0;i < rd.size - 1; i++){
    rd.buffer[i] = rd.buffer[i+1];
  }
  rd.buffer[rd.size - 1]='\0';
  rd.n_items--;
  //if(writing_queue.size != 0 && rd.n_items == rd.size-1) readInt();
  return c;
};

// Put a char in the read buffer
void fill_read(char c){
  if(rd.n_items == rd.size)return;
  rd.buffer[rd.n_items] = c;
  rd.n_items++;
  printf("writing on read buffer char -> '%c'            \tBUFFER STATUS -->",c);
  BUF_print(&rd);
  if(reading_queue.size != 0 && rd.n_items == 1) writeInt();
};

// Put a char in the write buffer
void putChar(char c){
  if(wr.n_items == wr.size) waitRead();
  wr.buffer[wr.n_items] = c;
  wr.n_items++;
  //if(reading_queue.size != 0) writeInt();
};

// Consume char from write buffer
char empty_write(){
  if(wr.n_items == 0) return;
  char c = wr.buffer[0];
  for(int i = 0;i < wr.size - 1; i++){
    wr.buffer[i] = wr.buffer[i+1];
  }
  wr.buffer[wr.size - 1]='\0';
  wr.n_items--;
  printf("removing from write buffer char -> '%c'        \tBUFFER STATUS -->",c);
  BUF_print(&wr);
  if(writing_queue.size != 0 && wr.n_items == wr.size-1) readInt();
  return c;
};