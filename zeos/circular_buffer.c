#include <circular_buffer.h>

void cbuf_init(cbuf *buf) {
	buf->head = 0;
	buf->tail = 0;
}

int cbuf_len(cbuf *buf) {
	int x = buf->tail - buf->head;
	if (x == 0) return buf->full ? CBUF_SIZE : 0;
	if (x > 0) return x;
	return CBUF_SIZE + x;
}

int cbuf_full(cbuf *buf) {
	return (buf->head == buf->tail) && buf->full;
}

int cbuf_empty(cbuf *buf) {
	return (buf->head == buf->tail) && !buf->full;
}

int cbuf_write(cbuf *buf, char b) {
	if (cbuf_full(buf)) return -1;
	
	buf->data[buf->tail] = b;
	buf->tail = (buf->tail+1) % CBUF_SIZE;

	if (buf->tail == buf->head) buf->full = 1;
	return 1;
}

int cbuf_read(cbuf *buf) {
	if (cbuf_empty(buf)) return -1;
	
	char ret = buf->data[buf->head];
	buf->head = (buf->head+1) % CBUF_SIZE;
	buf->full = 0;

	return ret;
}




