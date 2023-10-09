#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <assert.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "encoder_new.h"
#include "atomport_asm.h"
#include "scheduler.h"
#include "buffered_rw.h"
#include "interrupt.h"
#define THREAD_STACK_SIZE 128
#define IDLE_STACK_SIZE 128
#define READ_BUFFER_SIZE 5
#define WRITE_BUFFER_SIZE 5

void pwm_thread (uint32_t useless_numba);
char read_buffer[READ_BUFFER_SIZE];
char write_buffer[WRITE_BUFFER_SIZE];
BUF rd;
BUF wr;

//statically allocated variables where we put our stuff

TCB r1_tcb;
uint8_t r1_stack[THREAD_STACK_SIZE];
void r1_fn(uint32_t arg ){
  while(1){
    cli();
    BUF_print(&rd);
    char c = getChar();
    printf("r1: read on  read_buffer '%c' -->",c);
    BUF_print(&rd);
    sei();
    _delay_ms(1000);
  }
}

TCB r2_tcb;
uint8_t r2_stack[THREAD_STACK_SIZE];
void r2_fn(uint32_t arg ){
  while(1){
    cli();
    BUF_print(&rd);
    char c = getChar();
    printf("r2: read on  read_buffer '%c' -->",c);
    BUF_print(&rd);
    sei();
    _delay_ms(1000);
  }
}

TCB w1_tcb;
uint8_t w1_stack[THREAD_STACK_SIZE];
void w1_fn(uint32_t arg ){
  while(1){
    cli();
    fill_read('|');
    printf("w1: write on read_buffer '|' -->");
    BUF_print(&rd);
    sei();
    _delay_ms(1000);
  }
}

TCB w2_tcb;
uint8_t w2_stack[THREAD_STACK_SIZE];
void w2_fn(uint32_t arg ){
  while(1){
    cli();
    fill_read('X');
    printf("w2: write on read_buffer 'X' -->");
    BUF_print(&rd);
    sei();
    _delay_ms(1000);
  }
}

int main(void){
  BUF_create(&wr, write_buffer,WRITE_BUFFER_SIZE);
  BUF_create(&rd, write_buffer,READ_BUFFER_SIZE);
  // we need printf for debugging
  printf_init();
  enableRWint();

  TCB_create(&r1_tcb, r1_stack+THREAD_STACK_SIZE-1, r1_fn, 0);
  TCB_create(&r2_tcb, r2_stack+THREAD_STACK_SIZE-1, r2_fn, 0);
  TCB_create(&w1_tcb, w1_stack+THREAD_STACK_SIZE-1, w1_fn, 0);
  TCB_create(&w2_tcb, w2_stack+THREAD_STACK_SIZE-1, w2_fn, 0);

  

  TCBList_enqueue(&running_queue, &r1_tcb);
  TCBList_enqueue(&running_queue, &w1_tcb);
  TCBList_enqueue(&running_queue, &r2_tcb);
  TCBList_enqueue(&running_queue, &w2_tcb);

  cli();
  printf("starting\n");
  sei();
  startSchedule();
}
