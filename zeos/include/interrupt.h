/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>
#include <circular_buffer.h>

#define IDT_ENTRIES 256

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();


void keyboard_routine();
void clock_routine();
void keyboard_handler();
void clock_handler();
void system_call_handler();

void initKeyboard();


extern void syscall_handler_sysenter();
extern void writeMSR(int addr, int val);




#endif  /* __INTERRUPT_H__ */
