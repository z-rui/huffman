#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "bitio.h"

typedef struct DictNode {
	int ch;
	struct DictNode *left, *right;
} DictNode;

#define MAXCHARS (UCHAR_MAX+2)

static
DictNode *parsedict(BitIO *i)
{
	static DictNode alloc[2*MAXCHARS - 1];
	static DictNode *stack[MAXCHARS];
	int maxalloc = 0, maxstack = 0;

	for (;;) {
		int bit;
		DictNode *p;

		bit = BitIO_read(i);
		if (feof(stdin) || ferror(stdin)) goto fatal;
		if (maxalloc >= 2*MAXCHARS - 1) goto fatal;
		p = &alloc[maxalloc++];
		if (bit == 0) {				/* BIT0 */
			/* dict-entry ::= BIT0 byte */
			int ch;

			ch = BitIO_read_byte(i);	/* byte */
			if (ch == 0 && BitIO_read(i))
				ch = EOF;
			if (feof(stdin) || ferror(stdin)) goto fatal;
			p->ch = ch;
			p->left = p->right = 0;
		} else {				/* BIT1 */
			if (maxstack == 0) goto fatal;
			if (maxstack == 1) {
				/* dict ::= dict-entry BIT1 */
				return stack[0];
			}
			/* dict-entry ::= dict-entry dict-entry BIT1 */
			p = &alloc[maxalloc++];
			p->ch = 0;
			p->right = stack[--maxstack];	/* dict-entry */
			p->left = stack[--maxstack];	/* dict-entry */
		}
		stack[maxstack++] = p;
	}
fatal:
	fprintf(stderr, "FATAL: disaster when reading dict; cannot recover.\n");
	exit(EXIT_FAILURE);
}

static
void translate(BitIO *i, const DictNode *hufftree)
{
	const DictNode *pos = hufftree;

	/* content ::= { BIT } */
	while (pos) {
		int bit;

		if (pos->left == 0 || pos->right == 0) {
			if (pos->ch == EOF)
				return;
			putchar(pos->ch);
			pos = hufftree;
		}
		bit = BitIO_read(i);	/* BIT */
		if (feof(stdin) || ferror(stdin)) goto fatal;
		pos = (bit == 0) ? pos->left : pos->right;
	}
fatal:
	fprintf(stderr, "FATAL: disaster when reading content; cannot recover.\n");
	exit(EXIT_FAILURE);
}

#ifdef _WIN32
# include <fcntl.h>
# include <io.h>
#endif

int main()
{
	BitIO bitinput[1];
	DictNode *hufftree;

#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	BitIO_init(bitinput, stdin);

	hufftree = parsedict(bitinput);
	translate(bitinput, hufftree);

	return 0;
}
