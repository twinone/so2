/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size);

int read(int fd, char *buffer, int size);

void itoa(int a, char *b);

int strlen(char *a);

int getpid();

int fork();

int clone(void (*)(), void *);

int get_stats(int pid, struct stats* st);

void perror();

void exit();

int sem_init(int id, unsigned int value);
int sem_wait(int id);
int sem_signal(int id);
int sem_destroy(int id);


void yield();


// syscall function wrappers
extern int write(int fd, char *buffer, int size);
extern int gettime ();

extern int errno;


#endif  /* __LIBC_H__ */
