#include "interrupt.h"
#include "scheduler.h"
#include "tcb.h"
#include "tcb_list.h"

#define PCINT0_MASK 0x0F // last four bit of PORT B pins (50-53)
#define INT0_MASK 0x1
extern void archContextSwitch(TCB* a,TCB* b);

// the reading queue
TCBList reading_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};

// the writing queue
TCBList writing_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};

// interrupt on read (this interrupt will trigger a write after a spot in the buffer is emptied)
ISR(INT0_vect) {
  printf("eseguito interrupt read\n");
  resumeWrite();
};

// interrupt on write (this interrupt will trigger a read after a new char is written in the buffer)
ISR(PCINT0_vect) {
  printf("eseguito interrupt write\n");
  resumeRead();
}

// set arduino ports to enable interrupts INT0 and PCINT0 
void enableRWint(){
  //printf_init();
  // INT0 and PCINT0 will be used as software interrupt also known as trap

  DDRD |= INT0_MASK; // pin 21 PD0 set as output
  PORTD &= ~INT0_MASK; // pin 21 PD0 output 0v
  EIMSK |= (1<<INT0); // enable interrupt INT0
  EICRA = 1<<ISC01 | 1<<ISC00; // trigger INT0 on rising edge

  DDRB |= PCINT0_MASK; //set PIN_MASK pins as output
  PORTB &= ~PCINT0_MASK; //0v 
  PCICR |= (1 << PCIE0); // set interrupt on change, looking up PCMSK0
  PCMSK0 |= PCINT0_MASK;   // set PCINT0 to trigger an interrupt on state change 

  sei();
}

// trigger interrupt INT0 by flipping bit on pin 21 PD0 
void readInt(){
  PORTD |= INT0_MASK;
  PORTD &= ~INT0_MASK;
  return;
}

// trigger interrupt PCINT0 by flipping bits on pins (50-53) 
void writeInt(){
  PORTB &= ~PCINT0_MASK;
  PORTB |= PCINT0_MASK;
  return;
}

// save the current TCB in the reading queue and jump to the next TCB in the running queue
void waitWrite(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&reading_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the writing queue and jump to the next TCB in the running queue
void waitRead(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&writing_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the running queue and restore the TCB in the reading queue
void resumeRead(void) {
  if(reading_queue.size == 0) return; // if the reading queue is empty continue with normal execution
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&running_queue, current_tcb);
  current_tcb=TCBList_dequeue(&reading_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the running queue and restore the TCB in the writing queue
void resumeWrite(void) {
  if(writing_queue.size == 0) return; // if the writing queue is empty continue with normal execution
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&running_queue, current_tcb);
  current_tcb=TCBList_dequeue(&writing_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}