#include <limits.h>

#include "bitio.h"

void BitIO_init(BitIO *io, FILE *f)
{
	io->f = f;
	io->byte = 0;
	io->bitcnt = 0;
}

int BitIO_read(BitIO *i)
{
	int r;

	if (!i->bitcnt) {
		i->byte = fgetc(i->f);
		i->bitcnt = CHAR_BIT;
	}
	r = i->byte >> (CHAR_BIT - 1);
	i->byte <<= 1;
	--i->bitcnt;
	return r;
}

int BitIO_read_byte(BitIO *i)
{
	int r;
	int nextbyte;

	nextbyte = fgetc(i->f);
	r = i->byte | (nextbyte >> i->bitcnt);
	i->byte = nextbyte << (CHAR_BIT - i->bitcnt);
	return r;
}

void BitIO_write(BitIO *o, int bit)
{
	o->byte |= bit << (CHAR_BIT - 1 - o->bitcnt);
	if (++o->bitcnt == CHAR_BIT) {
		BitIO_flush(o);
	}
}

void BitIO_write_byte(BitIO *o, int byte)
{
	fputc(o->byte | (byte >> o->bitcnt), o->f);
	o->byte = byte << (CHAR_BIT - o->bitcnt);
}

void BitIO_flush(BitIO *o)
{
	if (o->bitcnt) {
		fputc(o->byte, o->f);
		o->byte = 0;
		o->bitcnt = 0;
	}
}
