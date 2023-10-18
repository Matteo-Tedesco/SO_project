#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "buffered_rw.h"
#include "interrupt.h"
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"

extern BUF rd;
extern BUF wr;

extern TCBList writing_queue;
extern TCBList reading_queue;

void BUF_create(BUF* buf,char* buffer, int size){
  buf->buffer=buffer;
  buf->size=size;
  buf->n_items=0;

  // Initialize empty space with break
  for(int i = 0;i<size;i++){
    buffer[i]='\0';
  }

  return;
};

// Used only for debug purposes
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
  cli();
  char c = rd.buffer[0];
  for(int i = 0;i < rd.size - 1; i++){
    rd.buffer[i] = rd.buffer[i+1];
  }
  rd.buffer[rd.size - 1]='\0';
  rd.n_items--;
  sei();
  return c;
};

// Put a char in the read buffer
void fill_read(){
  if(rd.n_items == rd.size)return;
  char c = read_uart();
  if(c == '\n' || c == '\r') return;
  rd.buffer[rd.n_items] = c;
  rd.n_items++;
};

// Put a char in the write buffer
void putChar(char c){
  if(wr.n_items == wr.size) waitRead();
  cli();
  wr.buffer[wr.n_items] = c;
  wr.n_items++;
  sei();
};

// Consume char from write buffer
void empty_write(){
  if(wr.n_items == 0) return;
  cli();
  char c = wr.buffer[0];
  for(int i = 0;i < wr.size - 1; i++){
    wr.buffer[i] = wr.buffer[i+1];
  }
  wr.buffer[wr.size - 1]='\0';
  wr.n_items--;
  sei();
  write_uart(c);
  write_uart('\n');
  //if(writing_queue.size != 0 && wr.n_items == wr.size-1) readInt();
  //return c;
};

void write_uart(char data) {
    // Wait for empty transmit buffer
    while ( !(UCSR0A & (_BV(UDRE0))) );
    // Start transmission
    UDR0 = data; 
}

char read_uart() {
    // Wait for incoming data
    while ( !(UCSR0A & (_BV(RXC0))) );
    // Return the data
    return UDR0;
}