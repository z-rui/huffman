extern void  heap_down(void *h[], int n, int i, int cmp(const void *, const void *));
extern void  heap_push(void *h[], int n,        int cmp(const void *, const void *));
extern void *heap_pop (void *h[], int n,        int cmp(const void *, const void *));
extern void  heap_init(void *h[], int n,        int cmp(const void *, const void *));
