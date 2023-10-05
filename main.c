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
    if(rd.status != Empty){
      BUF_print(&rd);
      char c = getChar();
      printf("r1: read on  read_buffer '%c' -->",c);
      BUF_print(&rd);
    } else {
      BUF_print(&rd);
      printf("r1 null read\n");
      //r1_tcb.status = Terminated;
    }
    sei();
    _delay_ms(200);
  }
}

TCB r2_tcb;
uint8_t r2_stack[THREAD_STACK_SIZE];
void r2_fn(uint32_t arg ){
  while(1){
    cli();
    if(rd.status != Empty){
      char c = getChar();
      printf("r2: getChar() : ");
      printf("%c\n",c);
    } else {
      printf("r2 null read\n");
    }
    sei();
    _delay_ms(100);
  }
}

TCB w1_tcb;
uint8_t w1_stack[THREAD_STACK_SIZE];
void w1_fn(uint32_t arg ){
  while(1){
    cli();
    printf("w1: write on read_buffer '|' -->");
    fill_read('|');
    BUF_print(&rd);
    sei();
    _delay_ms(200);
  }
}

TCB w2_tcb;
uint8_t w2_stack[THREAD_STACK_SIZE];
void w2_fn(uint32_t arg ){
  while(1){
    cli();
    printf("w2: write on read_buffer 'o' -->");
    fill_read('o');
    BUF_print(&rd);
    sei();
    _delay_ms(200);
  }
}

TCB w3_tcb;
uint8_t w3_stack[THREAD_STACK_SIZE];
void w3_fn(uint32_t arg ){
  while(1){
    cli();
    printf("w3: write on write_buffer 's'\n");
    putChar('s');
    sei();
    _delay_ms(100);
  }
}

TCB w4_tcb;
uint8_t w4_stack[THREAD_STACK_SIZE];
void w4_fn(uint32_t arg ){
  while(1){
    cli();
    printf("w4: write on write_buffer 't'\n");
    putChar('t');
    sei();
    _delay_ms(100);
  }
}


int main(void){
  BUF_create(&wr, write_buffer,WRITE_BUFFER_SIZE);
  BUF_create(&rd, write_buffer,READ_BUFFER_SIZE);

  // we need printf for debugging
  printf_init();

  TCB_create(&r1_tcb,
             r1_stack+THREAD_STACK_SIZE-1,
             r1_fn,
             0);
  
  TCB_create(&r2_tcb,
            r2_stack+THREAD_STACK_SIZE-1,
            r2_fn,
            0);
  
  TCB_create(&w1_tcb,
            w1_stack+THREAD_STACK_SIZE-1,
            w1_fn,
            0);

  TCB_create(&w2_tcb,
            w2_stack+THREAD_STACK_SIZE-1,
            w2_fn,
            0);

  TCB_create(&w3_tcb,
            w3_stack+THREAD_STACK_SIZE-1,
            w3_fn,
            0);

  TCB_create(&w4_tcb,
            w4_stack+THREAD_STACK_SIZE-1,
            w4_fn,
            0);

  TCBList_enqueue(&running_queue, &r1_tcb);
  // TCBList_enqueue(&running_queue, &r2_tcb);
  TCBList_enqueue(&running_queue, &w1_tcb);
  TCBList_enqueue(&running_queue, &w2_tcb);
  // TCBList_enqueue(&running_queue, &w3_tcb);
  // TCBList_enqueue(&running_queue, &w4_tcb);
  for(int i=0;i<2;i++){
    fill_read('x');
  }
  cli();
  printf("starting\n");
  sei();
  startSchedule();
}
