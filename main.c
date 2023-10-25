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
BUF rd; // READ BUFFER
BUF wr; // WRITE BUFFER

//statically allocated variables where we put our stuff

// idle process
TCB idle_tcb;
uint8_t idle_stack[THREAD_STACK_SIZE];
void idle_fn(uint32_t arg ){
  while(1){
    _delay_ms(100);
  }
}

// tries to empty the write buffer on the serial 
TCB empty_write_tcb;
uint8_t empty_write_stack[THREAD_STACK_SIZE];
void empty_write_fn(uint32_t arg ){
  while(1){
    empty_write();
    _delay_ms(1000);
  }
}

// tries to write a char in the write buffer
TCB putChar_tcb;
uint8_t putChar_stack[THREAD_STACK_SIZE];
void putChar_fn(uint32_t arg ){
  while(1){
    putChar('A');
    cli();
    BUF_print(&wr); // print the write buffer (debug)
    sei();
    _delay_ms(500);
  }
}

// tries to read a char from the read buffer 
TCB getChar_tcb;
uint8_t getChar_stack[THREAD_STACK_SIZE];
void getChar_fn(uint32_t arg ){
  while(1){
    char c = getChar();
    cli();
    printf("\nChar letto: %c\n",c);
    sei();
    _delay_ms(500);
  }
}

int main(void){
  BUF_create(&wr, write_buffer,WRITE_BUFFER_SIZE); // initialize WRITE BUFFER
  BUF_create(&rd, read_buffer,READ_BUFFER_SIZE); // initialize READ BUFFER
  // we need printf for debugging
  printf_init();

  // create all needed TCB
  TCB_create(&empty_write_tcb, empty_write_stack+THREAD_STACK_SIZE-1, empty_write_fn, 0);
  TCB_create(&putChar_tcb, putChar_stack+THREAD_STACK_SIZE-1, putChar_fn, 0);
  TCB_create(&getChar_tcb, getChar_stack+THREAD_STACK_SIZE-1, getChar_fn, 0);
  TCB_create(&idle_tcb, idle_stack+THREAD_STACK_SIZE-1, idle_fn, 0);


  // enqueue the TCB in the running queue
  TCBList_enqueue(&running_queue, &empty_write_tcb);
  TCBList_enqueue(&running_queue, &putChar_tcb);
  TCBList_enqueue(&running_queue, &getChar_tcb);
  TCBList_enqueue(&running_queue, &idle_tcb);

  cli();
  printf("starting\n");
  sei();
  startSchedule(); // start the process scheduling
}
