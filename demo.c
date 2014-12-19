#include <stdio.h>	/* getchar, printf */
#include <stdlib.h>	/* qsort */
#include <string.h>	/* strlen */
#include <ctype.h>	/* isprint */
#include <limits.h>

#include "huffman.h"

typedef struct {
	unsigned char ch;
	int freq;
	char code[UCHAR_MAX];
} HuffStat;

typedef struct {
	const HuffEncoder *e;
	int prefix;
	char buf[UCHAR_MAX];
	HuffStat *s;
} makestat_context;

static
void do_makestat(makestat_context *c, const HuffTreeNode *n)
{
	int i = HE_idx(c->e, n);

	if (i >= 0) { /* leaf node */
		c->s->ch = i;
		c->s->freq = n->freq;
		strncpy(c->s->code, c->buf, c->prefix);
		++c->s;
	} else {	/* interior node */
		char *bit = &c->buf[c->prefix];

		++c->prefix;
		*bit = '0';
		do_makestat(c, n->left);
		*bit = '1';
		do_makestat(c, n->right);
		--c->prefix;
	}
}

static
void makestat(makestat_context *c)
{
	const HuffTreeNode *root;

	if ((root = HE_root(c->e)) != 0)
		do_makestat(c, root);
}

static
int cmp_stat_by_ch(const void *a, const void *b)
{
	return ((const HuffStat *) a)->ch - ((const HuffStat *) b)->ch;
}

int main()
{
	static HuffTreeNode nodealloc[2*UCHAR_MAX+1];
	static void *heapalloc[UCHAR_MAX+1];
	static HuffStat stat[UCHAR_MAX+2];

	int ch;
	unsigned bitcnt[2] = {0, 0};
	HuffEncoder e[1];
	HuffStat *p;
	makestat_context c = {e, 0, "", stat};

	HE_init(e, UCHAR_MAX+1, nodealloc, heapalloc);
	while ((ch = getchar()) != EOF) {
		if (isgraph(ch)) HE_read(e, ch);
	}
	HE_encode(e);
	makestat(&c);
	for (p = stat; p->freq; p++)
		;
	qsort(stat, p - stat, sizeof stat[0], cmp_stat_by_ch);
	for (p = stat; p->freq; p++) {
		printf("%c\t%d\t%s\n", p->ch, p->freq, p->code);
		bitcnt[0] += p->freq * CHAR_BIT;
		bitcnt[1] += p->freq * strlen(p->code);
	}
	printf("uncompressed = %u\ncompressed = %u\nratio = %lf%%\n", bitcnt[0], bitcnt[1], 100.0 * bitcnt[1] / bitcnt[0]);
	return 0;
}
