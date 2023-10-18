#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/iom2560.h>
#include <util/delay.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
extern void schedule(void);

void waitWrite(void);

void resumeWrite(void);

void waitRead(void);

void resumeRead(void);