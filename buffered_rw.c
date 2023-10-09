#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "buffered_rw.h"

extern BUF rd;
extern BUF wr;

void BUF_create(BUF* buf,char* buffer, int size){
  buf->buffer=buffer;
  buf->size=size;
  buf->status=Empty;
  buf->r_idx=0;
  buf->w_idx=0;
  buf->n_items=0;

  for(int i = 0;i<size;i++){
    buffer[i]='\0';
  }

  return;
};

void BUF_print(BUF* buffer){
  printf("\tD %d\tN_ITEMS: %d\tRD_I: %d\tWR_I: %d\tS: %x -> ",buffer->size, buffer->n_items, buffer->r_idx, buffer->w_idx, buffer->status);
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

// Put a char in the write buffer
void putChar(char c){
  if(wr.status == Full) return;
  if(wr.w_idx == wr.size - 1 && wr.buffer[wr.w_idx] == '\0'){
    wr.buffer[wr.w_idx] = c;
    wr.w_idx=wr.r_idx;
    if(wr.buffer[0] == '\0'){
      wr.status = Filling;
      wr.w_idx=0;
    } else {
      wr.status = Full;
    }
  } else if(wr.buffer[wr.w_idx] != '\0'){
    wr.status = Full;
  } else {
    wr.buffer[wr.w_idx] = c;
    wr.w_idx++;
    wr.status = Filling;
  }
  return;
};

// Put a char in the read buffer
void fill_read(char c){
  if(rd.status == Full) return;
  if(rd.w_idx == rd.size - 1 && rd.buffer[rd.w_idx] == '\0'){
    rd.buffer[rd.w_idx] = c;
    rd.w_idx=rd.r_idx;
    if(rd.buffer[0] == '\0'){
      rd.status = Filling;
      rd.w_idx=0;
    } else {
      rd.status = Full;
    }
  } else if(rd.buffer[rd.w_idx] != '\0'){
    rd.status = Full;
  } else {
    rd.buffer[rd.w_idx] = c;
    rd.w_idx++;
    rd.status = Filling;
  }
  return;
};

// Consume char from read buffer
char getChar(){
  if(rd.status == Empty) return '\0';
  if(rd.r_idx == rd.size -1 && rd.buffer[rd.r_idx] != '\0'){
    char c = rd.buffer[rd.r_idx];
    rd.buffer[rd.r_idx] = '\0';
    rd.r_idx = 0;
    if(rd.buffer[rd.r_idx] == '\0'){
      rd.status = Empty;
    } else {
      rd.status = Filling;
    }
    return c;
  } else if( rd.buffer[rd.r_idx] == '\0'){
    rd.r_idx = 0;
    rd.status = Empty;
    return '\0';
  } else {
    char c = rd.buffer[rd.r_idx];
    rd.buffer[rd.r_idx] = '\0';
    rd.r_idx++;
    rd.status = Filling;
    return c;
  }
};

// Consume char from write buffer
char empty_write(){
  //printf("w_idx %d",wr.r_idx);
  if(wr.status == Empty) return '\0';
  if(wr.r_idx == wr.size -1){
    char c = wr.buffer[wr.r_idx];
    wr.buffer[wr.r_idx] = '\0';
    wr.r_idx = 0;
    if(wr.buffer[wr.r_idx] == '\0'){
      wr.status = Empty;
    }else if( wr.buffer[wr.r_idx] == '\0'){
      wr.r_idx = 0;
      return '\0';
    } else {
      wr.status = Filling;
    }
    return c;
  } else {
    char c = wr.buffer[wr.r_idx];
    wr.buffer[wr.r_idx] = '\0';
    wr.r_idx++;
    return c;
  }
};

char getChar2(){
  if(rd.n_items == 0) return '\0';
  char c = rd.buffer[0];
  for(int i = 0;i < rd.size - 1; i++){
    rd.buffer[i] = rd.buffer[i+1];
  }
  rd.buffer[rd.size - 1]='\0';
  rd.n_items--;
  return c;
};

void fill_read2(char c){
  if(rd.n_items == rd.size) return;
  rd.buffer[rd.n_items] = c;
  rd.n_items++;
};