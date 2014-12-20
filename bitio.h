#include <stdio.h>

typedef struct {
	unsigned char *s;
	size_t bitcnt;	/* bit already read/write in *s (<CHAR_BIT) */
} BitIOBuf;

typedef struct {
	FILE *f;
	BitIOBuf buf;
	unsigned char byte[2];
} BitIO;

extern void BitIOBuf_init      (BitIOBuf *b, unsigned char *s);
extern void BitIOBuf_flush     (BitIOBuf *b);
extern int  BitIOBuf_read      (BitIOBuf *b);
extern int  BitIOBuf_read_byte (BitIOBuf *b);
extern void BitIOBuf_write     (BitIOBuf *b, int bit);
extern void BitIOBuf_write_byte(BitIOBuf *b, int byte);

extern void  BitIO_init      (BitIO *, FILE *);
extern int   BitIO_read      (BitIO *);
extern int   BitIO_read_byte (BitIO *);
extern void  BitIO_write     (BitIO *, int);
extern void  BitIO_write_byte(BitIO *, int);
extern void  BitIO_flush     (BitIO *);
