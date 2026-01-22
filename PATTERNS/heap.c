/*
 * HEAP (Priority Queue) PATTERNS
 * Use: Top K, median, merge K lists, scheduling
 * Embedded focus: static array-based heap, no malloc
 */

#include <string.h>

#define MAX_HEAP_SIZE 1000
#define PARENT(i)     (((i) - 1) / 2)
#define LEFT(i)       (2 * (i) + 1)
#define RIGHT(i)      (2 * (i) + 2)

// === MIN HEAP ===
typedef struct {
    int data[MAX_HEAP_SIZE];
    int size;
} MinHeap;

void min_heap_init(MinHeap *h) { h->size = 0; }
int min_heap_empty(MinHeap *h) { return h->size == 0; }
int min_heap_peek(MinHeap *h) { return h->data[0]; }

void min_heap_push(MinHeap *h, int val) {
    int i = h->size++;
    h->data[i] = val;

    // bubble up
    while (i > 0 && h->data[PARENT(i)] > h->data[i]) {
        int tmp = h->data[i];
        h->data[i] = h->data[PARENT(i)];
        h->data[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

int min_heap_pop(MinHeap *h) {
    int result = h->data[0];
    h->data[0] = h->data[--h->size];

    // bubble down
    int i = 0;
    while (1) {
        int smallest = i;
        int l = LEFT(i), r = RIGHT(i);

        if (l < h->size && h->data[l] < h->data[smallest]) smallest = l;
        if (r < h->size && h->data[r] < h->data[smallest]) smallest = r;

        if (smallest == i) break;

        int tmp = h->data[i];
        h->data[i] = h->data[smallest];
        h->data[smallest] = tmp;
        i = smallest;
    }

    return result;
}

// === MAX HEAP ===
typedef struct {
    int data[MAX_HEAP_SIZE];
    int size;
} MaxHeap;

void max_heap_init(MaxHeap *h) { h->size = 0; }
int max_heap_empty(MaxHeap *h) { return h->size == 0; }
int max_heap_peek(MaxHeap *h) { return h->data[0]; }

void max_heap_push(MaxHeap *h, int val) {
    int i = h->size++;
    h->data[i] = val;

    while (i > 0 && h->data[PARENT(i)] < h->data[i]) {
        int tmp = h->data[i];
        h->data[i] = h->data[PARENT(i)];
        h->data[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

int max_heap_pop(MaxHeap *h) {
    int result = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int largest = i;
        int l = LEFT(i), r = RIGHT(i);

        if (l < h->size && h->data[l] > h->data[largest]) largest = l;
        if (r < h->size && h->data[r] > h->data[largest]) largest = r;

        if (largest == i) break;

        int tmp = h->data[i];
        h->data[i] = h->data[largest];
        h->data[largest] = tmp;
        i = largest;
    }

    return result;
}

// === TOP K SMALLEST (using max heap of size k) ===
void top_k_smallest(int *nums, int n, int k, int *result) {
    MaxHeap h;
    max_heap_init(&h);

    for (int i = 0; i < n; i++) {
        if (h.size < k) {
            max_heap_push(&h, nums[i]);
        } else if (nums[i] < max_heap_peek(&h)) {
            max_heap_pop(&h);
            max_heap_push(&h, nums[i]);
        }
    }

    // extract results
    for (int i = k - 1; i >= 0; i--) {
        result[i] = max_heap_pop(&h);
    }
}

// === TOP K LARGEST (using min heap of size k) ===
void top_k_largest(int *nums, int n, int k, int *result) {
    MinHeap h;
    min_heap_init(&h);

    for (int i = 0; i < n; i++) {
        if (h.size < k) {
            min_heap_push(&h, nums[i]);
        } else if (nums[i] > min_heap_peek(&h)) {
            min_heap_pop(&h);
            min_heap_push(&h, nums[i]);
        }
    }

    for (int i = 0; i < k; i++) {
        result[i] = min_heap_pop(&h);
    }
}

// === MEDIAN FINDER (Two Heaps) ===
typedef struct {
    MaxHeap lo;  // smaller half (max heap)
    MinHeap hi;  // larger half (min heap)
} MedianFinder;

void median_init(MedianFinder *mf) {
    max_heap_init(&mf->lo);
    min_heap_init(&mf->hi);
}

void median_add(MedianFinder *mf, int num) {
    max_heap_push(&mf->lo, num);

    // balance: move max of lo to hi
    min_heap_push(&mf->hi, max_heap_pop(&mf->lo));

    // ensure lo.size >= hi.size
    if (mf->lo.size < mf->hi.size) {
        max_heap_push(&mf->lo, min_heap_pop(&mf->hi));
    }
}

double median_find(MedianFinder *mf) {
    if (mf->lo.size > mf->hi.size) {
        return max_heap_peek(&mf->lo);
    }
    return (max_heap_peek(&mf->lo) + min_heap_peek(&mf->hi)) / 2.0;
}

// === MERGE K SORTED ARRAYS ===
typedef struct {
    int val;
    int arr_idx;
    int elem_idx;
} HeapNode;

typedef struct {
    HeapNode data[MAX_HEAP_SIZE];
    int size;
} NodeHeap;

void node_heap_init(NodeHeap *h) { h->size = 0; }

void node_heap_push(NodeHeap *h, HeapNode node) {
    int i = h->size++;
    h->data[i] = node;

    while (i > 0 && h->data[PARENT(i)].val > h->data[i].val) {
        HeapNode tmp = h->data[i];
        h->data[i] = h->data[PARENT(i)];
        h->data[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

HeapNode node_heap_pop(NodeHeap *h) {
    HeapNode result = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int smallest = i;
        int l = LEFT(i), r = RIGHT(i);

        if (l < h->size && h->data[l].val < h->data[smallest].val) smallest = l;
        if (r < h->size && h->data[r].val < h->data[smallest].val) smallest = r;

        if (smallest == i) break;

        HeapNode tmp = h->data[i];
        h->data[i] = h->data[smallest];
        h->data[smallest] = tmp;
        i = smallest;
    }

    return result;
}

void merge_k_arrays(int arrays[][MAX_HEAP_SIZE], int *sizes, int k,
                    int *result, int *result_size) {
    NodeHeap h;
    node_heap_init(&h);

    // push first element of each array
    for (int i = 0; i < k; i++) {
        if (sizes[i] > 0) {
            node_heap_push(&h, (HeapNode){arrays[i][0], i, 0});
        }
    }

    *result_size = 0;
    while (h.size > 0) {
        HeapNode node = node_heap_pop(&h);
        result[(*result_size)++] = node.val;

        // push next element from same array
        if (node.elem_idx + 1 < sizes[node.arr_idx]) {
            node_heap_push(&h, (HeapNode){
                arrays[node.arr_idx][node.elem_idx + 1],
                node.arr_idx,
                node.elem_idx + 1
            });
        }
    }
}

// === HEAPSORT (in-place) ===
void heapify(int *arr, int n, int i) {
    int largest = i;
    int l = LEFT(i), r = RIGHT(i);

    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;

    if (largest != i) {
        int tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        heapify(arr, n, largest);
    }
}

void heapsort(int *arr, int n) {
    // build max heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // extract elements
    for (int i = n - 1; i > 0; i--) {
        int tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        heapify(arr, i, 0);
    }
}
