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



extern int read_num_pending_chars;
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

	if (list_empty(&keyboardqueue)) return;

	// if buffer full or we can satisfy current read op, unblock first(kb q)
	if (cbuf_full(&keyboard_buffer) || cbuf_len(&keyboard_buffer) >= read_num_pending_chars) {
		// add the current executing to the ready queue
		// they can go to the end... that's fine.
		update_process_state_rr(current(), &readyqueue);

		struct list_head *e = list_first(&keyboardqueue);
		struct task_struct *t = list_entry(e, struct task_struct, anchor);
			
		// insert t to the start of the ready queue and switch to it
		update_process_state_rr_impl(t, &readyqueue, 1);
		sched_next_rr();
	}
}




void clock_routine() {
	zeos_ticks++;
	zeos_show_clock();


if(!zeos_ticks%5000){
	printk("clock interrupt\nready processes:\n");
	struct list_head *h=list_first(&readyqueue);
	if(list_empty(&readyqueue))printk("no ready process\n");
	else list_for_each(h,&readyqueue){
		printk("there is someone ready\n");
	}
	printk("end readyqueue\n");
	if(list_empty(&semaphores[0].procs))printk("no blocked process\n");
	else {	
		h=list_first(&semaphores[0].procs);
		list_for_each(h,&semaphores[0].procs){
			printk("there is someone in semaphores \n");
		}

	}
}



	schedule();
}




