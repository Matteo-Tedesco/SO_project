#include "interrupt.h"
#include "scheduler.h"
#include "tcb.h"
#include "tcb_list.h"
#include "buffered_rw.h"

extern void archContextSwitch(TCB* a,TCB* b);
extern BUF wr;
extern BUF rd;
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

// interrupt on uart transmission completed (this interrupt will trigger a write after a spot in the buffer is emptied)
ISR(USART0_TX_vect) {
  cli();
  if(wr.n_items<wr.size) resumeWrite();
  sei();
};

// interrupt on uart receive complete (this interrupt will trigger a read after a new char is written in the buffer)
ISR(USART0_RX_vect) {
  cli();
  fill_read();
  resumeRead();
  sei();
}

// save the current TCB in the reading queue and jump to the next TCB in the running queue
void waitWrite(void) {
  //printf("\tREAD BUFFER EMPTY WAITING IN READING QUEUE\n");
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&reading_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the writing queue and jump to the next TCB in the running queue
void waitRead(void) {
  //printf("\tWRITE BUFFER FULL WAITING IN WRITING QUEUE\n");
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&writing_queue, current_tcb);
  current_tcb=TCBList_dequeue(&running_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the running queue and restore the TCB in the reading queue
void resumeRead(void) {
  if(reading_queue.size == 0) return; // if the reading queue is empty continue with normal execution
  //printf("\tRESTORING TCB FROM READING QUEUE\n");
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&running_queue, current_tcb);
  current_tcb=TCBList_dequeue(&reading_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}

// save the current TCB in the running queue and restore the TCB in the writing queue
void resumeWrite(void) {
  if(writing_queue.size == 0) return; // if the writing queue is empty continue with normal execution
  //printf("\tRESTORING TCB FROM WRITING QUEUE\n");
  TCB* old_tcb=current_tcb;
  TCBList_enqueue(&running_queue, current_tcb);
  current_tcb=TCBList_dequeue(&writing_queue);
  if (old_tcb!=current_tcb)
    archContextSwitch(old_tcb, current_tcb);
}