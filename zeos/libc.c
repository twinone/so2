/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}
int write (int fd, char *buffer, int size) {
	int ret;
	__asm__ volatile( 
				"int $0x80"
				:"=a" (ret), 		// resultat de %eax a ret
				"+b" (fd), 
				"+c" (buffer),
				"+d" (size)
				:"a"  (4)
	);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return ret;
}


int write2 (int fd, char * buffer, int size)
{
volatile int ret = 0;

  __asm__ volatile(
	"movl 8(%ebp), %ebx;"
	"movl 12(%ebp), %ecx;"
	"movl 16(%ebp), %edx;"
	"movl $4, %eax;"
	"int $0x80;"
	// errno <- %eax
  );
 // so GCC doesn't complain
}

int gettime() {
/*
  __asm__ volatile(
	
  );
*/

	__asm__ volatile( 
		"movl $10, %eax;"
		"int $0x80;"
	);
}

/* lo que queremos
000000d0 <gettime>:
  d0:   55                      push   %ebp
  d1:   b8 0a 00 00 00          mov    $0xa,%eax
  d6:   89 e5                   mov    %esp,%ebp
  d8:   cd 80                   int    $0x80
  da:   5d                      pop    %ebp
  db:   c3                      ret    
  dc:   8d 74 26 00             lea    0x0(%esi,%eiz,1),%esi

*/

void perror() {
	switch (errno) {
	// write a message depending on errno
	default:
		write(1, "Error\n", 6);
	}
}

















