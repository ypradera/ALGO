/*
 * STACK & QUEUE PATTERNS
 * Use: Monotonic stack, sliding window max, valid parentheses
 * Embedded focus: static allocation, circular buffer
 */

#include <string.h>

#define MAX_SIZE 1000

// === STATIC STACK ===
typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

void stack_init(Stack *s) { s->top = -1; }
int stack_empty(Stack *s) { return s->top == -1; }
int stack_full(Stack *s) { return s->top == MAX_SIZE - 1; }
void stack_push(Stack *s, int val) { s->data[++s->top] = val; }
int stack_pop(Stack *s) { return s->data[s->top--]; }
int stack_peek(Stack *s) { return s->data[s->top]; }

// === CIRCULAR QUEUE (Ring Buffer) ===
typedef struct {
    int data[MAX_SIZE];
    int head, tail, count;
} CircularQueue;

void queue_init(CircularQueue *q) { q->head = q->tail = q->count = 0; }
int queue_empty(CircularQueue *q) { return q->count == 0; }
int queue_full(CircularQueue *q) { return q->count == MAX_SIZE; }

void queue_enqueue(CircularQueue *q, int val) {
    q->data[q->tail] = val;
    q->tail = (q->tail + 1) % MAX_SIZE;
    q->count++;
}

int queue_dequeue(CircularQueue *q) {
    int val = q->data[q->head];
    q->head = (q->head + 1) % MAX_SIZE;
    q->count--;
    return val;
}

int queue_front(CircularQueue *q) { return q->data[q->head]; }

// === MONOTONIC STACK (Next Greater Element) ===
void next_greater_element(int *arr, int n, int *result) {
    Stack s;
    stack_init(&s);

    for (int i = n - 1; i >= 0; i--) {
        // pop smaller elements
        while (!stack_empty(&s) && stack_peek(&s) <= arr[i]) {
            stack_pop(&s);
        }

        result[i] = stack_empty(&s) ? -1 : stack_peek(&s);
        stack_push(&s, arr[i]);
    }
}

// === MONOTONIC STACK (Previous Smaller Element Index) ===
void prev_smaller_index(int *arr, int n, int *result) {
    Stack s;  // stores indices
    stack_init(&s);

    for (int i = 0; i < n; i++) {
        while (!stack_empty(&s) && arr[stack_peek(&s)] >= arr[i]) {
            stack_pop(&s);
        }

        result[i] = stack_empty(&s) ? -1 : stack_peek(&s);
        stack_push(&s, i);
    }
}

// === SLIDING WINDOW MAXIMUM (Monotonic Deque) ===
typedef struct {
    int data[MAX_SIZE];  // stores indices
    int front, back;
} Deque;

void deque_init(Deque *d) { d->front = d->back = 0; }
int deque_empty(Deque *d) { return d->front == d->back; }
void deque_push_back(Deque *d, int val) { d->data[d->back++] = val; }
void deque_pop_front(Deque *d) { d->front++; }
void deque_pop_back(Deque *d) { d->back--; }
int deque_front(Deque *d) { return d->data[d->front]; }
int deque_back(Deque *d) { return d->data[d->back - 1]; }

void sliding_window_max(int *nums, int n, int k, int *result) {
    Deque dq;
    deque_init(&dq);

    for (int i = 0; i < n; i++) {
        // remove indices out of window
        while (!deque_empty(&dq) && deque_front(&dq) < i - k + 1) {
            deque_pop_front(&dq);
        }

        // remove smaller elements (maintain decreasing order)
        while (!deque_empty(&dq) && nums[deque_back(&dq)] < nums[i]) {
            deque_pop_back(&dq);
        }

        deque_push_back(&dq, i);

        if (i >= k - 1) {
            result[i - k + 1] = nums[deque_front(&dq)];
        }
    }
}

// === VALID PARENTHESES ===
int is_valid_parentheses(const char *s) {
    char stack[MAX_SIZE];
    int top = -1;

    while (*s) {
        char c = *s++;
        if (c == '(' || c == '[' || c == '{') {
            stack[++top] = c;
        } else {
            if (top < 0) return 0;
            char open = stack[top--];
            if ((c == ')' && open != '(') ||
                (c == ']' && open != '[') ||
                (c == '}' && open != '{')) {
                return 0;
            }
        }
    }
    return top == -1;
}

// === LARGEST RECTANGLE IN HISTOGRAM ===
int largest_rectangle_area(int *heights, int n) {
    Stack s;  // stores indices
    stack_init(&s);

    int max_area = 0;

    for (int i = 0; i <= n; i++) {
        int h = (i == n) ? 0 : heights[i];

        while (!stack_empty(&s) && heights[stack_peek(&s)] > h) {
            int height = heights[stack_pop(&s)];
            int width = stack_empty(&s) ? i : i - stack_peek(&s) - 1;
            int area = height * width;
            if (area > max_area) max_area = area;
        }
        stack_push(&s, i);
    }

    return max_area;
}
