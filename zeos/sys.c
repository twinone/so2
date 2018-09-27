/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <system.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}



int sys_write(int fd, char *buf, int size) {
	int e = check_fd(fd, ESCRIPTURA);

	if (e) return e;
	
	if (buf == NULL) return -22;  /* Invalid argument */
	if (size < 0) return -22; // use 0 for flush?
	
	char mybuf[size]; // buffer it!
	copy_from_user(buf, mybuf, size);
	sys_write_console(mybuf, size);
}

int sys_gettime() {
	return zeos_ticks;
}


int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}
