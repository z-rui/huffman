#include <string.h>	/* strncpy, memset */

#include "heapq.h"	/* heap_init, heap_push, heap_pop */
#include "huffman.h"

void HE_init(HuffEncoder *e, int n, HuffTreeNode alloc[], void *heap[])
{
	e->n = n;
	e->alloc = alloc;
	e->heap = heap;
	e->maxnode = n; /* reserve alloc[0...n-1] for leaf (char) nodes */
	e->maxheap = 0;
	memset(alloc, 0, n * sizeof alloc[0]);
	memset(heap, 0, n * sizeof heap[0]);
}

void HE_read(HuffEncoder *e, int ch)
{
	HuffTreeNode *n = &e->alloc[ch];

	if (!n->freq++)
		e->heap[e->maxheap++] = n;
}

static
int cmp_node_by_freq(const void *a, const void *b)
{
	return ((const HuffTreeNode *) a)->freq - ((const HuffTreeNode *) b)->freq;
}

void HE_encode(HuffEncoder *e)
{
	heap_init(e->heap, e->maxheap, cmp_node_by_freq);

	while (e->maxheap > 1) {
		const HuffTreeNode *a, *b;
		HuffTreeNode *c;

		a = heap_pop(e->heap, e->maxheap--, cmp_node_by_freq);
		b = heap_pop(e->heap, e->maxheap--, cmp_node_by_freq);
		c = &e->alloc[e->maxnode++];

		c->freq = a->freq + b->freq;
		c->left = a;
		c->right = b;

		e->heap[e->maxheap] = c;
		heap_push(e->heap, e->maxheap++, cmp_node_by_freq);
	}
}

HuffTreeNode const *HE_root(const HuffEncoder *e)
{
	return (e->maxheap) ? e->heap[0] : 0;
}

int HE_idx(const HuffEncoder *e, const HuffTreeNode *n)
{
	int i = n - e->alloc;
	return (i < e->n) ? i : e->n - i - 1;
}
