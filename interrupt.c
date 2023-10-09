#include "interrupt.h"
#include "scheduler.h"
#include "tcb.h"
#include "tcb_list.h"

#define PIN_MASK 0x0F // last four bit of PORT B (50-53)
#define MASK 0x1

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

ISR(INT0_vect) {
  printf("eseguito interrupt read\n");
  schedule();
};

ISR(PCINT0_vect) {
  printf("eseguito interrupt write\n");
  resumeRead();
}

void enableRWint(){
  //printf_init(); 
  DDRD=0x1; // all pins on port b set as output
  PORTD=0x0; // output 0v

  // enable interrupt 0
  EIMSK |= (1<<INT0);

  // trigger int0 on rising edge
  EICRA= 1<<ISC01 | 1<<ISC00;

  DDRB |= PIN_MASK;; //set PIN_MASK pins as input
  PORTB &= ~PIN_MASK; //enable pull up resistors
  PCICR |= (1 << PCIE0); // set interrupt on change, looking up PCMSK0
  PCMSK0 |= PIN_MASK;   // set PCINT0 to trigger an interrupt on state change 

  sei();
}

void readInt(){
  PORTD |= MASK;
  PORTD &= ~MASK;
  return;
}

void writeInt(){
  PORTB &= ~PIN_MASK;
  PORTB |= PIN_MASK;
  return;
}

void waitWrite(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&reading_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

void waitRead(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&writing_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

void resumeRead(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&running_queue, current_tcb);
  current_tcb=TCBList_dequeue(&reading_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

void resumeWrite(void) {
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&writing_queue, current_tcb);
  current_tcb=TCBList_dequeue(&reading_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}