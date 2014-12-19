#include <stdio.h>

typedef struct {
	FILE *f;
	char byte;
	size_t bitcnt;
} BitIO;

extern void BitIO_init      (BitIO *, FILE *);
extern int  BitIO_read      (BitIO *);
extern int  BitIO_read_byte (BitIO *);
extern int  BitIO_write     (BitIO *, int);
extern int  BitIO_write_byte(BitIO *, int);
extern int  BitIO_flush     (BitIO *);
