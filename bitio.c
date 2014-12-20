#include <limits.h>

#include "bitio.h"

void BitIOBuf_init(BitIOBuf *b, unsigned char *s)
{
	b->s = s;
	b->bitcnt = 0;
}

void BitIOBuf_flush(BitIOBuf *b)
{
	if (b->bitcnt) {
		++b->s;
		b->bitcnt = 0;
	}
}

int BitIOBuf_read(BitIOBuf *b)
{
	int r;

	r = *b->s & (1 << (CHAR_BIT - ++b->bitcnt));
	if (b->bitcnt == CHAR_BIT) { /* have read an entire byte */
		BitIOBuf_flush(b); /* look for next byte */
	}
	return !!r;
}

int BitIOBuf_read_byte(BitIOBuf *b)
{
	int r;

	r = (unsigned char) (b->s[0] << b->bitcnt) | (b->s[1] >> (CHAR_BIT - b->bitcnt));
	++b->s;
	return r;
}

void BitIOBuf_write(BitIOBuf *b, int bit)
{
	*b->s |= bit << (CHAR_BIT - ++b->bitcnt);
	if (b->bitcnt == CHAR_BIT) { /* have written an entire byte */
		BitIOBuf_flush(b); /* look for next byte */
	}
}

void BitIOBuf_write_byte(BitIOBuf *b, int byte)
{
	*b->s++ |= byte >> b->bitcnt;
	*b->s = byte << (CHAR_BIT - b->bitcnt);
}

void BitIO_init(BitIO *io, FILE *f)
{
	io->f = f;
	BitIOBuf_init(&io->buf, io->byte);
	io->byte[0] = 0;
	io->byte[1] = 0;
}

int BitIO_read(BitIO *i)
{
	if (!i->buf.bitcnt) {
		i->buf.s = i->byte;
		i->byte[0] = fgetc(i->f);
	}
	return BitIOBuf_read(&i->buf);
}

int BitIO_read_byte(BitIO *i)
{
	int r;

	if (!i->buf.bitcnt)
		return fgetc(i->f); /* in case i->buf.s == &i->byte[0] */
	i->byte[1] = fgetc(i->f);
	r = BitIOBuf_read_byte(&i->buf);
	i->buf.s = i->byte;
	i->byte[0] = i->byte[1];
	return r;
}

void BitIO_write(BitIO *o, int bit)
{
	BitIOBuf_write(&o->buf, bit);
	if (!o->buf.bitcnt) {
		fputc(o->byte[0], o->f);
		o->buf.s = o->byte;
		o->byte[0] = 0;
	}
}

void BitIO_write_byte(BitIO *o, int byte)
{
	BitIOBuf_write_byte(&o->buf, byte);
	fputc(o->byte[0], o->f);
	o->buf.s = o->byte;
	o->byte[0] = o->byte[1];
}

void BitIO_flush(BitIO *o)
{
	BitIOBuf_flush(&o->buf);
	if (o->buf.s == &o->byte[1]) { /* buffer full */
		fputc(o->byte[0], o->f);
		o->buf.s = o->byte;
		o->byte[0] = 0;
	}
}
