#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include "serial_linux.h"

int main(void){
  char* portname = "/dev/ttyACM0";
  int baudrate=19200;
  printf( "opening serial device [%s] ... ", portname);
  int fd=serial_open(portname);
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

  serial_set_blocking(fd, 1);

  char buf [100];
  while (1) {
    int n_read=read(fd, buf, 100);
    for (int i=0; i<n_read; ++i) {
      if(buf[i]=='\n') {
        printf(" EOL\n");
        return 0;
      } else {
        printf("%c", buf[i]);
      }
    }
  }
  return 0;
}
