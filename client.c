#include <errno.h>
#include <signal.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include "serial_linux.h"
#include <stdlib.h>
#include <ucontext.h>
#define QUIT_COMMAND "QUIT\n"
#define READ_COMMAND "READ\n"
#define WRITE_COMMAND "WRITE\n"
#define STACK_SIZE 16384
char buf_in [1024];
char buf [1024];
int fd;

char* quit_command = QUIT_COMMAND;
char* read_command = READ_COMMAND;
char* write_command = WRITE_COMMAND;

char f1_stack[STACK_SIZE];
char f2_stack[STACK_SIZE];



ucontext_t main_context, f1_context, f2_context;

void read_serial(){
    while (1) {
    int n_read=read(fd, buf, sizeof(buf));
    for (int i=0; i<n_read; ++i) {
      printf("%c", buf[i]);
    }
  }
}

void write_serial(){
  printf("\nInsert a char to be sent on serial: ");
  if (fgets(buf_in, sizeof(buf_in), stdin) != (char*)buf_in){
    fprintf(stderr, "Error while reading from stdin, exiting...\n");
    exit(EXIT_FAILURE);
  };
  int n_write=write(fd, buf_in, 1);
  if(n_write < 1){
    printf("Error while writing on serial, exiting...\n");
  };
  setcontext(&f1_context);
}

void catch_int(){
  printf("\nUse command QUIT to exit, READ to read from serial, WRITE to write to serial\n");
  if (fgets(buf_in, sizeof(buf_in), stdin) != (char*)buf_in) {
    fprintf(stderr, "Error while reading from stdin, exiting...\n");
    exit(EXIT_FAILURE);
  } else if(memcmp(buf_in, quit_command, strlen(quit_command)) == 0){
    printf("\nCOMANDO QUIT\n");
    setcontext(&main_context);
  } else if(memcmp(buf_in, read_command, strlen(read_command)) == 0){
    printf("\nCOMANDO READ\n");
    //read_serial(fd,buf,sizeof(buf));
    setcontext(&f1_context);
  } else if(memcmp(buf_in, write_command, strlen(write_command)) == 0){
    printf("\nCOMANDO WRITE\n");
    //read_serial(fd,buf,sizeof(buf));
    setcontext(&f2_context);
  }
  //printf(buf_in);
  exit(0);
}

int main(void){
  signal(SIGINT, catch_int);
  char* portname = "/dev/ttyACM0";
  int baudrate=19200;
  printf( "opening serial device [%s] ... ", portname);
  fd=serial_open(portname);
  if (fd<=0) {
    printf ("Error\n");
    return 0;
  } else {
    printf ("Success\n");
  }
  printf( "setting baudrate [%d] ... ", baudrate);
  int attribs=serial_set_interface_attribs(fd, baudrate, 0);
  if (attribs) {
    printf("Error\n");
    return 0;
  } else {
    printf("Success\n");
  }

  serial_set_blocking(fd, 0);

  //get a context from main
  getcontext(&f1_context);

  // set the stack of f1 to the right place
  f1_context.uc_stack.ss_sp=f1_stack;
  f1_context.uc_stack.ss_size = STACK_SIZE;
  f1_context.uc_stack.ss_flags = 0;
  f1_context.uc_link=&main_context;

  // create a trampoline for the first function
  makecontext(&f1_context, read_serial, 0, 0);

  // always remember to initialize
  // a new context from something known
  f2_context=f1_context;
  
  f2_context.uc_stack.ss_sp=f2_stack;
  f2_context.uc_stack.ss_size = STACK_SIZE;
  f2_context.uc_stack.ss_flags = 0;
  f2_context.uc_link=&main_context;

  // create a trampoline for the second function
  makecontext(&f2_context, write_serial, 0, 0);

  // this passes control to f2.
  // and saves the current context in main_context
  swapcontext(&main_context, &f1_context); // we will jump back here

  //read_serial(fd,buf,sizeof(buf));
  return 0;
}
