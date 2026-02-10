/*
 * PRIORITY QUEUE / HEAP IMPLEMENTATIONS
 * Use: Task scheduling, event handling, timer management, interrupt priorities
 *
 * Features:
 * - Min-heap and max-heap
 * - Static allocation (no malloc)
 * - Task/event priority queues
 * - Timer queue with deadlines
 *
 * Time: O(log n) insert/delete, O(1) peek
 * Space: O(n)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC MIN-HEAP (Integer)
 * ============================================ */

#define HEAP_MAX_SIZE   64

typedef struct {
    int32_t data[HEAP_MAX_SIZE];
    uint16_t size;
} MinHeap;

void min_heap_init(MinHeap *heap) {
    heap->size = 0;
}

bool min_heap_is_empty(MinHeap *heap) {
    return heap->size == 0;
}

bool min_heap_is_full(MinHeap *heap) {
    return heap->size >= HEAP_MAX_SIZE;
}

static void min_heap_swap(int32_t *a, int32_t *b) {
    int32_t temp = *a;
    *a = *b;
    *b = temp;
}

static void min_heap_sift_up(MinHeap *heap, uint16_t idx) {
    while (idx > 0) {
        uint16_t parent = (idx - 1) / 2;
        if (heap->data[idx] < heap->data[parent]) {
            min_heap_swap(&heap->data[idx], &heap->data[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

static void min_heap_sift_down(MinHeap *heap, uint16_t idx) {
    while (true) {
        uint16_t smallest = idx;
        uint16_t left = 2 * idx + 1;
        uint16_t right = 2 * idx + 2;

        if (left < heap->size && heap->data[left] < heap->data[smallest]) {
            smallest = left;
        }
        if (right < heap->size && heap->data[right] < heap->data[smallest]) {
            smallest = right;
        }

        if (smallest != idx) {
            min_heap_swap(&heap->data[idx], &heap->data[smallest]);
            idx = smallest;
        } else {
            break;
        }
    }
}

bool min_heap_push(MinHeap *heap, int32_t value) {
    if (min_heap_is_full(heap)) return false;

    heap->data[heap->size] = value;
    min_heap_sift_up(heap, heap->size);
    heap->size++;

    return true;
}

bool min_heap_pop(MinHeap *heap, int32_t *value) {
    if (min_heap_is_empty(heap)) return false;

    *value = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];
    min_heap_sift_down(heap, 0);

    return true;
}

bool min_heap_peek(MinHeap *heap, int32_t *value) {
    if (min_heap_is_empty(heap)) return false;
    *value = heap->data[0];
    return true;
}


/* ============================================
 * BASIC MAX-HEAP (Integer)
 * ============================================ */

typedef struct {
    int32_t data[HEAP_MAX_SIZE];
    uint16_t size;
} MaxHeap;

void max_heap_init(MaxHeap *heap) {
    heap->size = 0;
}

bool max_heap_is_empty(MaxHeap *heap) {
    return heap->size == 0;
}

bool max_heap_is_full(MaxHeap *heap) {
    return heap->size >= HEAP_MAX_SIZE;
}

static void max_heap_sift_up(MaxHeap *heap, uint16_t idx) {
    while (idx > 0) {
        uint16_t parent = (idx - 1) / 2;
        if (heap->data[idx] > heap->data[parent]) {
            min_heap_swap(&heap->data[idx], &heap->data[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

static void max_heap_sift_down(MaxHeap *heap, uint16_t idx) {
    while (true) {
        uint16_t largest = idx;
        uint16_t left = 2 * idx + 1;
        uint16_t right = 2 * idx + 2;

        if (left < heap->size && heap->data[left] > heap->data[largest]) {
            largest = left;
        }
        if (right < heap->size && heap->data[right] > heap->data[largest]) {
            largest = right;
        }

        if (largest != idx) {
            min_heap_swap(&heap->data[idx], &heap->data[largest]);
            idx = largest;
        } else {
            break;
        }
    }
}

bool max_heap_push(MaxHeap *heap, int32_t value) {
    if (max_heap_is_full(heap)) return false;

    heap->data[heap->size] = value;
    max_heap_sift_up(heap, heap->size);
    heap->size++;

    return true;
}

bool max_heap_pop(MaxHeap *heap, int32_t *value) {
    if (max_heap_is_empty(heap)) return false;

    *value = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];
    max_heap_sift_down(heap, 0);

    return true;
}


/* ============================================
 * TASK PRIORITY QUEUE
 * For RTOS-style task scheduling
 * ============================================ */

#define MAX_TASKS   32

typedef void (*TaskFunc)(void *arg);

typedef struct {
    uint8_t priority;       // Lower = higher priority
    TaskFunc func;
    void *arg;
    bool active;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    uint8_t heap[MAX_TASKS];    // Indices into tasks array
    uint16_t size;
} TaskQueue;

void task_queue_init(TaskQueue *tq) {
    tq->size = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        tq->tasks[i].active = false;
    }
}

static void task_queue_sift_up(TaskQueue *tq, uint16_t idx) {
    while (idx > 0) {
        uint16_t parent = (idx - 1) / 2;
        uint8_t curr_task = tq->heap[idx];
        uint8_t parent_task = tq->heap[parent];

        if (tq->tasks[curr_task].priority < tq->tasks[parent_task].priority) {
            tq->heap[idx] = parent_task;
            tq->heap[parent] = curr_task;
            idx = parent;
        } else {
            break;
        }
    }
}

static void task_queue_sift_down(TaskQueue *tq, uint16_t idx) {
    while (true) {
        uint16_t smallest = idx;
        uint16_t left = 2 * idx + 1;
        uint16_t right = 2 * idx + 2;

        if (left < tq->size) {
            if (tq->tasks[tq->heap[left]].priority <
                tq->tasks[tq->heap[smallest]].priority) {
                smallest = left;
            }
        }
        if (right < tq->size) {
            if (tq->tasks[tq->heap[right]].priority <
                tq->tasks[tq->heap[smallest]].priority) {
                smallest = right;
            }
        }

        if (smallest != idx) {
            uint8_t temp = tq->heap[idx];
            tq->heap[idx] = tq->heap[smallest];
            tq->heap[smallest] = temp;
            idx = smallest;
        } else {
            break;
        }
    }
}

// Returns task ID or -1 on failure
int task_queue_add(TaskQueue *tq, uint8_t priority, TaskFunc func, void *arg) {
    if (tq->size >= MAX_TASKS) return -1;

    // Find free task slot
    int task_id = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tq->tasks[i].active) {
            task_id = i;
            break;
        }
    }
    if (task_id < 0) return -1;

    // Initialize task
    tq->tasks[task_id].priority = priority;
    tq->tasks[task_id].func = func;
    tq->tasks[task_id].arg = arg;
    tq->tasks[task_id].active = true;

    // Add to heap
    tq->heap[tq->size] = task_id;
    task_queue_sift_up(tq, tq->size);
    tq->size++;

    return task_id;
}

// Get and remove highest priority task
bool task_queue_pop(TaskQueue *tq, Task *task) {
    if (tq->size == 0) return false;

    uint8_t task_id = tq->heap[0];
    *task = tq->tasks[task_id];
    tq->tasks[task_id].active = false;

    tq->size--;
    tq->heap[0] = tq->heap[tq->size];
    task_queue_sift_down(tq, 0);

    return true;
}

// Execute highest priority task
bool task_queue_run_one(TaskQueue *tq) {
    Task task;
    if (!task_queue_pop(tq, &task)) return false;

    if (task.func) {
        task.func(task.arg);
    }
    return true;
}


/* ============================================
 * TIMER PRIORITY QUEUE
 * For deadline-based timer events
 * ============================================ */

#define MAX_TIMERS  32

typedef void (*TimerCallback)(uint16_t timer_id, void *arg);

typedef struct {
    uint32_t deadline;      // Absolute time when timer fires
    uint32_t period;        // For periodic timers (0 = one-shot)
    TimerCallback callback;
    void *arg;
    bool active;
} TimerEntry;

typedef struct {
    TimerEntry timers[MAX_TIMERS];
    uint8_t heap[MAX_TIMERS];
    uint16_t size;
    uint32_t current_time;
} TimerQueue;

void timer_queue_init(TimerQueue *tq) {
    tq->size = 0;
    tq->current_time = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        tq->timers[i].active = false;
    }
}

static void timer_queue_sift_up(TimerQueue *tq, uint16_t idx) {
    while (idx > 0) {
        uint16_t parent = (idx - 1) / 2;
        uint8_t curr_timer = tq->heap[idx];
        uint8_t parent_timer = tq->heap[parent];

        if (tq->timers[curr_timer].deadline < tq->timers[parent_timer].deadline) {
            tq->heap[idx] = parent_timer;
            tq->heap[parent] = curr_timer;
            idx = parent;
        } else {
            break;
        }
    }
}

static void timer_queue_sift_down(TimerQueue *tq, uint16_t idx) {
    while (true) {
        uint16_t smallest = idx;
        uint16_t left = 2 * idx + 1;
        uint16_t right = 2 * idx + 2;

        if (left < tq->size &&
            tq->timers[tq->heap[left]].deadline < tq->timers[tq->heap[smallest]].deadline) {
            smallest = left;
        }
        if (right < tq->size &&
            tq->timers[tq->heap[right]].deadline < tq->timers[tq->heap[smallest]].deadline) {
            smallest = right;
        }

        if (smallest != idx) {
            uint8_t temp = tq->heap[idx];
            tq->heap[idx] = tq->heap[smallest];
            tq->heap[smallest] = temp;
            idx = smallest;
        } else {
            break;
        }
    }
}

int timer_queue_add(TimerQueue *tq, uint32_t delay, uint32_t period,
                    TimerCallback callback, void *arg) {
    if (tq->size >= MAX_TIMERS) return -1;

    int timer_id = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!tq->timers[i].active) {
            timer_id = i;
            break;
        }
    }
    if (timer_id < 0) return -1;

    tq->timers[timer_id].deadline = tq->current_time + delay;
    tq->timers[timer_id].period = period;
    tq->timers[timer_id].callback = callback;
    tq->timers[timer_id].arg = arg;
    tq->timers[timer_id].active = true;

    tq->heap[tq->size] = timer_id;
    timer_queue_sift_up(tq, tq->size);
    tq->size++;

    return timer_id;
}

bool timer_queue_cancel(TimerQueue *tq, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_TIMERS) return false;
    if (!tq->timers[timer_id].active) return false;

    tq->timers[timer_id].active = false;

    // Find and remove from heap
    for (uint16_t i = 0; i < tq->size; i++) {
        if (tq->heap[i] == timer_id) {
            tq->size--;
            tq->heap[i] = tq->heap[tq->size];
            timer_queue_sift_down(tq, i);
            timer_queue_sift_up(tq, i);
            return true;
        }
    }

    return false;
}

// Process expired timers
void timer_queue_tick(TimerQueue *tq, uint32_t current_time) {
    tq->current_time = current_time;

    while (tq->size > 0) {
        uint8_t timer_id = tq->heap[0];
        TimerEntry *timer = &tq->timers[timer_id];

        if (timer->deadline > current_time) {
            break;  // No more expired timers
        }

        // Remove from heap
        tq->size--;
        tq->heap[0] = tq->heap[tq->size];
        if (tq->size > 0) {
            timer_queue_sift_down(tq, 0);
        }

        // Execute callback
        if (timer->callback) {
            timer->callback(timer_id, timer->arg);
        }

        // Re-add if periodic
        if (timer->period > 0 && timer->active) {
            timer->deadline = current_time + timer->period;
            tq->heap[tq->size] = timer_id;
            timer_queue_sift_up(tq, tq->size);
            tq->size++;
        } else {
            timer->active = false;
        }
    }
}

// Get time until next timer fires (0 if expired)
uint32_t timer_queue_next_deadline(TimerQueue *tq) {
    if (tq->size == 0) return UINT32_MAX;

    uint32_t deadline = tq->timers[tq->heap[0]].deadline;
    if (deadline <= tq->current_time) return 0;
    return deadline - tq->current_time;
}


/* ============================================
 * EVENT PRIORITY QUEUE
 * For interrupt-driven event processing
 * ============================================ */

#define MAX_EVENTS  64

typedef enum {
    EVENT_NONE = 0,
    EVENT_BUTTON,
    EVENT_UART_RX,
    EVENT_ADC_COMPLETE,
    EVENT_TIMER,
    EVENT_I2C_DONE,
    EVENT_SPI_DONE,
    EVENT_DMA_COMPLETE,
    EVENT_ERROR,
    EVENT_USER
} EventType;

typedef struct {
    EventType type;
    uint8_t priority;       // Lower = higher priority
    uint32_t timestamp;
    uint32_t data;
} Event;

typedef struct {
    Event events[MAX_EVENTS];
    uint16_t size;
} EventQueue;

void event_queue_init(EventQueue *eq) {
    eq->size = 0;
}

bool event_queue_is_empty(EventQueue *eq) {
    return eq->size == 0;
}

static void event_queue_sift_up(EventQueue *eq, uint16_t idx) {
    while (idx > 0) {
        uint16_t parent = (idx - 1) / 2;

        // Compare by priority first, then by timestamp
        bool swap = false;
        if (eq->events[idx].priority < eq->events[parent].priority) {
            swap = true;
        } else if (eq->events[idx].priority == eq->events[parent].priority &&
                   eq->events[idx].timestamp < eq->events[parent].timestamp) {
            swap = true;
        }

        if (swap) {
            Event temp = eq->events[idx];
            eq->events[idx] = eq->events[parent];
            eq->events[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void event_queue_sift_down(EventQueue *eq, uint16_t idx) {
    while (true) {
        uint16_t smallest = idx;
        uint16_t left = 2 * idx + 1;
        uint16_t right = 2 * idx + 2;

        if (left < eq->size) {
            bool better = eq->events[left].priority < eq->events[smallest].priority ||
                         (eq->events[left].priority == eq->events[smallest].priority &&
                          eq->events[left].timestamp < eq->events[smallest].timestamp);
            if (better) smallest = left;
        }
        if (right < eq->size) {
            bool better = eq->events[right].priority < eq->events[smallest].priority ||
                         (eq->events[right].priority == eq->events[smallest].priority &&
                          eq->events[right].timestamp < eq->events[smallest].timestamp);
            if (better) smallest = right;
        }

        if (smallest != idx) {
            Event temp = eq->events[idx];
            eq->events[idx] = eq->events[smallest];
            eq->events[smallest] = temp;
            idx = smallest;
        } else {
            break;
        }
    }
}

bool event_queue_push(EventQueue *eq, EventType type, uint8_t priority,
                      uint32_t timestamp, uint32_t data) {
    if (eq->size >= MAX_EVENTS) return false;

    eq->events[eq->size].type = type;
    eq->events[eq->size].priority = priority;
    eq->events[eq->size].timestamp = timestamp;
    eq->events[eq->size].data = data;

    event_queue_sift_up(eq, eq->size);
    eq->size++;

    return true;
}

bool event_queue_pop(EventQueue *eq, Event *event) {
    if (eq->size == 0) return false;

    *event = eq->events[0];
    eq->size--;
    eq->events[0] = eq->events[eq->size];
    event_queue_sift_down(eq, 0);

    return true;
}


/* ============================================
 * INDEXED PRIORITY QUEUE
 * Supports update/decrease key operations
 * ============================================ */

#define IPQ_MAX_SIZE    32
#define IPQ_INVALID     0xFF

typedef struct {
    int32_t keys[IPQ_MAX_SIZE];     // Key values
    uint8_t heap[IPQ_MAX_SIZE];     // Heap of indices
    uint8_t pos[IPQ_MAX_SIZE];      // Position in heap for each index
    uint16_t size;
} IndexedMinHeap;

void iheap_init(IndexedMinHeap *h) {
    h->size = 0;
    for (int i = 0; i < IPQ_MAX_SIZE; i++) {
        h->pos[i] = IPQ_INVALID;
    }
}

bool iheap_contains(IndexedMinHeap *h, uint8_t idx) {
    return idx < IPQ_MAX_SIZE && h->pos[idx] != IPQ_INVALID;
}

static void iheap_swap(IndexedMinHeap *h, uint16_t i, uint16_t j) {
    uint8_t idx_i = h->heap[i];
    uint8_t idx_j = h->heap[j];

    h->heap[i] = idx_j;
    h->heap[j] = idx_i;
    h->pos[idx_i] = j;
    h->pos[idx_j] = i;
}

static void iheap_sift_up(IndexedMinHeap *h, uint16_t i) {
    while (i > 0) {
        uint16_t parent = (i - 1) / 2;
        if (h->keys[h->heap[i]] < h->keys[h->heap[parent]]) {
            iheap_swap(h, i, parent);
            i = parent;
        } else {
            break;
        }
    }
}

static void iheap_sift_down(IndexedMinHeap *h, uint16_t i) {
    while (true) {
        uint16_t smallest = i;
        uint16_t left = 2 * i + 1;
        uint16_t right = 2 * i + 2;

        if (left < h->size && h->keys[h->heap[left]] < h->keys[h->heap[smallest]]) {
            smallest = left;
        }
        if (right < h->size && h->keys[h->heap[right]] < h->keys[h->heap[smallest]]) {
            smallest = right;
        }

        if (smallest != i) {
            iheap_swap(h, i, smallest);
            i = smallest;
        } else {
            break;
        }
    }
}

bool iheap_insert(IndexedMinHeap *h, uint8_t idx, int32_t key) {
    if (idx >= IPQ_MAX_SIZE || h->size >= IPQ_MAX_SIZE) return false;
    if (iheap_contains(h, idx)) return false;

    h->keys[idx] = key;
    h->heap[h->size] = idx;
    h->pos[idx] = h->size;
    iheap_sift_up(h, h->size);
    h->size++;

    return true;
}

bool iheap_decrease_key(IndexedMinHeap *h, uint8_t idx, int32_t new_key) {
    if (!iheap_contains(h, idx)) return false;
    if (new_key >= h->keys[idx]) return false;

    h->keys[idx] = new_key;
    iheap_sift_up(h, h->pos[idx]);
    return true;
}

bool iheap_update(IndexedMinHeap *h, uint8_t idx, int32_t new_key) {
    if (!iheap_contains(h, idx)) return false;

    int32_t old_key = h->keys[idx];
    h->keys[idx] = new_key;

    if (new_key < old_key) {
        iheap_sift_up(h, h->pos[idx]);
    } else {
        iheap_sift_down(h, h->pos[idx]);
    }

    return true;
}

bool iheap_pop(IndexedMinHeap *h, uint8_t *idx, int32_t *key) {
    if (h->size == 0) return false;

    *idx = h->heap[0];
    *key = h->keys[*idx];

    h->size--;
    if (h->size > 0) {
        h->heap[0] = h->heap[h->size];
        h->pos[h->heap[0]] = 0;
        iheap_sift_down(h, 0);
    }
    h->pos[*idx] = IPQ_INVALID;

    return true;
}

