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
#include <errno.h>

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



#define BLOCK_SIZE 4
int sys_write(int fd, char *buf, int size) {
	int e = check_fd(fd, ESCRIPTURA);

	if (e) return -e;
	
	if (buf == NULL) return EFAULT;  /* Invalid argument */
	if (size < 0) return EINVAL; // use 0 for flush?
	
	char mybuf[BLOCK_SIZE];
	int offset = 0;
	while (offset + BLOCK_SIZE < size) {
		copy_from_user(buf+offset, mybuf, BLOCK_SIZE);
		sys_write_console(mybuf, BLOCK_SIZE);
		offset += BLOCK_SIZE;
	}
	if (offset < size) {
		int res = size - offset;
		copy_from_user(buf+offset, mybuf, res);
		sys_write_console(mybuf, res);
	}
	return 0;
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
