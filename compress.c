#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "huffman.h"
#include "bitio.h"

#define MAXCODELEN (UCHAR_MAX)
#define MAXCHARS (UCHAR_MAX+2)
#define EOF_IDX (UCHAR_MAX+1)

typedef struct {
	HuffEncoder *e;
	int prefix;
	char buf[MAXCODELEN];
	/* dict[ch] = huffman code for ch, or for EOF when ch == EOF_IDX */
	const char **dict;
	char *dictalloc;
} makedict_context;

static
void do_makedict(makedict_context *c, const HuffTreeNode *n)
{
	int i = HE_idx(c->e, n);

	if (i >= 0) { /* leaf */
		strncpy(c->dictalloc, c->buf, c->prefix);
		c->dict[i] = c->dictalloc;
		c->dictalloc += c->prefix + 1;
	} else {
		char *bit = &c->buf[c->prefix];

		++c->prefix;
		*bit = '0';
		do_makedict(c, n->left);
		*bit = '1';
		do_makedict(c, n->right);
		--c->prefix;
	}
}

static
void makedict(makedict_context *c)
{
	int ch;

	while ((ch = getchar()) != EOF)
		HE_read(c->e, ch);
	HE_read(c->e, EOF_IDX);
	HE_encode(c->e);
	do_makedict(c, HE_root(c->e));
}

typedef struct {
	const HuffEncoder *e;
	BitIO *o;
} dumptree_context;

static
void do_dumptree(dumptree_context *c, const HuffTreeNode *n)
{
	int i = HE_idx(c->e, n);

	if (i >= 0) {	/* leaf */
		/* dict-entry ::= BIT0 BYTE */
		BitIO_write(c->o, 0);		/* BIT0 */
		BitIO_write_byte(c->o, (unsigned char) i);	/* BYTE */
		if ((unsigned char) i == 0)
			BitIO_write(c->o, !!i);
	} else {
		/* dict-entry ::= dict-entry dict-entry BIT1 */
		do_dumptree(c, n->left);	/* dict-entry */
		do_dumptree(c, n->right);	/* dict-entry */
		BitIO_write(c->o, 1);		/* BIT1 */
	}
}

static
void dumptree(dumptree_context *c)
{
	/* dict ::= dict-entry BIT1 */
	do_dumptree(c, HE_root(c->e));	/* dict-entry */
	BitIO_write(c->o, 1);		/* BIT1 (sentinel) */
}

static
void writecontent(BitIO *o, const char *const dict[])
{
	int ch;
	const char *code;

	/* content ::= { BIT } */
	do {
		ch = getchar();
		code = dict[(ch == EOF) ? EOF_IDX : ch];
		while (*code)
			BitIO_write(o, *code++ != '0');	/* BIT */
	} while (ch != EOF);
	BitIO_flush(o);
}

#ifdef _WIN32
# include <fcntl.h>
# include <io.h>
#endif

int main()
{
	static HuffTreeNode nodealloc[2*MAXCHARS-1];
	static void *heapalloc[MAXCHARS];
	static const char *dict[MAXCHARS];
	static char dictalloc[MAXCODELEN * MAXCHARS];
	/* TODO: what's the minimum upper bound for dictalloc's size? */

	BitIO bitoutput[1];
	HuffEncoder e[1];
	makedict_context mc = {e, 0, "", dict, dictalloc};
	dumptree_context wc = {e, bitoutput};

#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif

	BitIO_init(bitoutput, stdout);
	HE_init(e, MAXCHARS, nodealloc, heapalloc);

	makedict(&mc);	/* 1st pass reading stdin */
	dumptree(&wc);			/* dict */

	rewind(stdin);
	if (ferror(stdin)) {
		perror("compress");
		exit(EXIT_FAILURE);
	}

	writecontent(bitoutput, dict);	/* content (2nd pass reading stdin) */

	return 0;
}
