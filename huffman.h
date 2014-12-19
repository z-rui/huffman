typedef struct HuffTreeNode {
	int freq;
	struct HuffTreeNode const *left, *right;
} HuffTreeNode;

/* Huffman encoder for n characters */
typedef struct {
	int n;
	HuffTreeNode *alloc;	/* [2*n-1] */
	void **heap;		/* [n] */
	int maxnode, maxheap;
} HuffEncoder;

extern void HE_init(HuffEncoder *, int, HuffTreeNode [], void *[]);
extern void HE_read(HuffEncoder *, int);
extern void HE_encode(HuffEncoder *);

extern HuffTreeNode const *
            HE_root(const HuffEncoder *);
extern int  HE_idx(const HuffEncoder *, const HuffTreeNode *);
