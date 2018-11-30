/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <system.h>
#include <sched.h>
#include <zeos_interrupt.h>
#include <circular_buffer.h>


cbuf keyboard_buffer;

extern struct list_head keyboardqueue;


Gate idt[IDT_ENTRIES];
Register idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','�','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','�',
  '\0','�','\0','�','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0',' ','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}


void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  // Add our keyboard_handler to the IDT at position 33
  setInterruptHandler(33, keyboard_handler, 0);
  // add the syscall handler
//  setTrapHandler(0x80, system_call_handler, 3);
  // add clock interrupt handler
  setInterruptHandler(32, clock_handler, 0);

	// init the MSR registers
	writeMSR(0x174, __KERNEL_CS);
	writeMSR(0x175, INITIAL_ESP);
	writeMSR(0x176, (int)syscall_handler_sysenter);

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */

  set_idt_reg(&idtR);
}



void initKeyboard() {
	cbuf_init(&keyboard_buffer);
}




void keyboard_routine() {
	volatile unsigned char keyCode = inb(0x60);
	char make = !(keyCode >> 7);
	if (!make) return;

	volatile unsigned char c = char_map[keyCode];

        // < 32 are non printable ascii
	if (c < 32) printc_xy(0, 0, 'C');		
	else printc_xy(0,0,c);
	
	// circular buffer
	cbuf_write(&keyboard_buffer, c);

	// update any blocked processes
	if (!list_empty(&keyboardqueue)) {
		struct list_head *l=list_first(&keyboardqueue);
		struct task_struct *t = list_head_to_task_struct(l);
		list_del(l);
		//update proces data before changing context

		update_process_state_rr(current(),&readyqueue);
		//add process from keyboardqueue to 1st place in ready queue
		unsigned long total_ticks = get_ticks();
		t->stats.blocked_ticks += total_ticks - t->stats.elapsed_total_ticks;
		t->stats.elapsed_total_ticks = total_ticks;
		list_add(&t->anchor, &readyqueue);
		t->state = ST_RUN;
		sched_next_rr();
		
		
	}

	if (keyCode == 28) {
		char x[2];
		x[1] = '\0';
		while (!cbuf_empty(&keyboard_buffer)) {
			x[0] = cbuf_read(&keyboard_buffer);
			printk(x);
		}
	}

}




void clock_routine() {
	zeos_ticks++;
	zeos_show_clock();

	schedule();
}




