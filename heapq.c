#include "heapq.h"

void heap_down(void *h[], int n, int i, int cmp(const void *, const void *))
{
	int s;
	void *t;

	t = h[i];
	while (i < n/2) {
		s = i*2 + 1;
		if (s+1 < n && cmp(h[s+1], h[s]) < 0)
			++s;
		if (cmp(t, h[s]) <= 0)
			break;
		h[i] = h[s];
		i = s;
	}
	h[i] = t;
}

void heap_push(void *h[], int n, int cmp(const void *, const void *))
{
	int u;
	void *t;

	t = h[n];
	while (n > 0) {
		u = (n-1) / 2;
		if (cmp(h[u], t) <= 0)
			break;
		h[n] = h[u];
		n = u;
	}
	h[n] = t;
}

void *heap_pop(void *h[], int n, int cmp(const void *, const void *))
{
	void *t;

	t = h[0];
	h[0] = h[n-1];
	heap_down(h, n-1, 0, cmp);
	return h[n-1] = t;
}

void heap_init(void *h[], int n, int cmp(const void *, const void *))
{
	int i;

	for (i = n/2 - 1; i >= 0; i--)
		heap_down(h, n, i, cmp);
}
