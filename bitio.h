#include <stdio.h>

typedef struct {
	FILE *f;
	unsigned char byte;
	size_t bitcnt;
} BitIO;

extern void BitIO_init      (BitIO *, FILE *);
extern int  BitIO_read      (BitIO *);
extern int  BitIO_read_byte (BitIO *);
extern void  BitIO_write     (BitIO *, int);
extern void  BitIO_write_byte(BitIO *, int);
extern void  BitIO_flush     (BitIO *);
