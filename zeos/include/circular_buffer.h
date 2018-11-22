#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#define CBUF_SIZE 20

typedef struct cbuf {
	int head; // used for reads
	int tail; // used for writes
	int full; // true if the buffer is full, if false, check head==tail for empty
	char data[CBUF_SIZE];	
} cbuf;

void cbuf_init(cbuf *buf);
int cbuf_len(cbuf *buf);
int cbuf_full(cbuf *buf);
int cbuf_empty(cbuf *buf);
int cbuf_write(cbuf *buf, char b);
int cbuf_read(cbuf *buf);
#endif // __CIRCULAR_BUFFER_H__
