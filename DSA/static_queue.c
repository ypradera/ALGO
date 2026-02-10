/*
 * STATIC QUEUE
 * Use: Task queues, message passing, event queues, command buffers
 *
 * Features:
 * - Fixed-size, no dynamic allocation
 * - FIFO (First-In-First-Out)
 * - Thread-safe variants for RTOS
 *
 * Time: O(1) all operations
 * Space: O(n) queue size
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC STATIC QUEUE (fixed element type)
 * ============================================ */

#define QUEUE_SIZE      32
#define QUEUE_MASK      (QUEUE_SIZE - 1)

typedef struct {
    int32_t data[QUEUE_SIZE];
    volatile uint8_t head;  // dequeue from here
    volatile uint8_t tail;  // enqueue here
} StaticQueue;

void queue_init(StaticQueue *q) {
    q->head = 0;
    q->tail = 0;
}

bool queue_is_empty(StaticQueue *q) {
    return q->head == q->tail;
}

bool queue_is_full(StaticQueue *q) {
    return ((q->tail + 1) & QUEUE_MASK) == q->head;
}

uint8_t queue_count(StaticQueue *q) {
    return (q->tail - q->head) & QUEUE_MASK;
}

bool queue_enqueue(StaticQueue *q, int32_t value) {
    if (queue_is_full(q)) {
        return false;
    }

    q->data[q->tail] = value;
    q->tail = (q->tail + 1) & QUEUE_MASK;
    return true;
}

bool queue_dequeue(StaticQueue *q, int32_t *value) {
    if (queue_is_empty(q)) {
        return false;
    }

    *value = q->data[q->head];
    q->head = (q->head + 1) & QUEUE_MASK;
    return true;
}

bool queue_peek(StaticQueue *q, int32_t *value) {
    if (queue_is_empty(q)) {
        return false;
    }

    *value = q->data[q->head];
    return true;
}

void queue_flush(StaticQueue *q) {
    q->head = 0;
    q->tail = 0;
}


/* ============================================
 * MESSAGE QUEUE (for embedded systems)
 * ============================================ */

#define MSG_QUEUE_SIZE      16
#define MSG_MAX_LEN         64

typedef struct {
    uint8_t type;
    uint8_t priority;
    uint16_t length;
    uint8_t data[MSG_MAX_LEN];
} Message;

typedef struct {
    Message messages[MSG_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} MessageQueue;

void msg_queue_init(MessageQueue *mq) {
    mq->head = 0;
    mq->tail = 0;
}

bool msg_queue_is_empty(MessageQueue *mq) {
    return mq->head == mq->tail;
}

bool msg_queue_is_full(MessageQueue *mq) {
    return ((mq->tail + 1) % MSG_QUEUE_SIZE) == mq->head;
}

bool msg_queue_send(MessageQueue *mq, uint8_t type, const uint8_t *data, uint16_t len) {
    if (msg_queue_is_full(mq) || len > MSG_MAX_LEN) {
        return false;
    }

    Message *msg = &mq->messages[mq->tail];
    msg->type = type;
    msg->priority = 0;
    msg->length = len;
    memcpy(msg->data, data, len);

    mq->tail = (mq->tail + 1) % MSG_QUEUE_SIZE;
    return true;
}

bool msg_queue_receive(MessageQueue *mq, Message *msg) {
    if (msg_queue_is_empty(mq)) {
        return false;
    }

    memcpy(msg, &mq->messages[mq->head], sizeof(Message));
    mq->head = (mq->head + 1) % MSG_QUEUE_SIZE;
    return true;
}


/* ============================================
 * EVENT QUEUE (lightweight)
 * ============================================ */

#define EVENT_QUEUE_SIZE    64

typedef struct {
    uint8_t event_id;
    uint32_t timestamp;
    uint32_t param;
} Event;

typedef struct {
    Event events[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} EventQueue;

void event_queue_init(EventQueue *eq) {
    eq->head = 0;
    eq->tail = 0;
}

bool event_queue_post(EventQueue *eq, uint8_t event_id, uint32_t param, uint32_t timestamp) {
    uint8_t next_tail = (eq->tail + 1) % EVENT_QUEUE_SIZE;

    if (next_tail == eq->head) {
        return false;  // full
    }

    eq->events[eq->tail].event_id = event_id;
    eq->events[eq->tail].param = param;
    eq->events[eq->tail].timestamp = timestamp;
    eq->tail = next_tail;

    return true;
}

bool event_queue_get(EventQueue *eq, Event *event) {
    if (eq->head == eq->tail) {
        return false;  // empty
    }

    *event = eq->events[eq->head];
    eq->head = (eq->head + 1) % EVENT_QUEUE_SIZE;
    return true;
}


/* ============================================
 * PRIORITY QUEUE (simple, for task scheduling)
 * ============================================ */

#define PRIO_QUEUE_SIZE     16
#define NUM_PRIORITIES      4

typedef struct {
    uint8_t task_id;
    void (*handler)(void);
    uint32_t param;
} Task;

typedef struct {
    Task tasks[NUM_PRIORITIES][PRIO_QUEUE_SIZE];
    uint8_t head[NUM_PRIORITIES];
    uint8_t tail[NUM_PRIORITIES];
} PriorityTaskQueue;

void prio_queue_init(PriorityTaskQueue *pq) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        pq->head[i] = 0;
        pq->tail[i] = 0;
    }
}

bool prio_queue_add(PriorityTaskQueue *pq, uint8_t priority, Task *task) {
    if (priority >= NUM_PRIORITIES) return false;

    uint8_t next_tail = (pq->tail[priority] + 1) % PRIO_QUEUE_SIZE;
    if (next_tail == pq->head[priority]) {
        return false;  // full
    }

    pq->tasks[priority][pq->tail[priority]] = *task;
    pq->tail[priority] = next_tail;
    return true;
}

bool prio_queue_get_highest(PriorityTaskQueue *pq, Task *task) {
    // Check from highest priority (0) to lowest
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (pq->head[i] != pq->tail[i]) {
            *task = pq->tasks[i][pq->head[i]];
            pq->head[i] = (pq->head[i] + 1) % PRIO_QUEUE_SIZE;
            return true;
        }
    }
    return false;  // all queues empty
}


/* ============================================
 * COMMAND QUEUE (for serial/protocol handlers)
 * ============================================ */

#define CMD_QUEUE_SIZE      8
#define CMD_MAX_ARGS        8

typedef struct {
    uint8_t cmd_code;
    uint8_t num_args;
    int32_t args[CMD_MAX_ARGS];
} Command;

typedef struct {
    Command commands[CMD_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} CommandQueue;

void cmd_queue_init(CommandQueue *cq) {
    cq->head = 0;
    cq->tail = 0;
}

bool cmd_queue_push(CommandQueue *cq, uint8_t cmd_code, int32_t *args, uint8_t num_args) {
    uint8_t next_tail = (cq->tail + 1) % CMD_QUEUE_SIZE;

    if (next_tail == cq->head || num_args > CMD_MAX_ARGS) {
        return false;
    }

    Command *cmd = &cq->commands[cq->tail];
    cmd->cmd_code = cmd_code;
    cmd->num_args = num_args;

    for (int i = 0; i < num_args; i++) {
        cmd->args[i] = args[i];
    }

    cq->tail = next_tail;
    return true;
}

bool cmd_queue_pop(CommandQueue *cq, Command *cmd) {
    if (cq->head == cq->tail) {
        return false;
    }

    *cmd = cq->commands[cq->head];
    cq->head = (cq->head + 1) % CMD_QUEUE_SIZE;
    return true;
}


/* ============================================
 * GENERIC MACRO-BASED QUEUE
 * ============================================ */

#define DEFINE_STATIC_QUEUE(name, type, size) \
    typedef struct { \
        type data[size]; \
        volatile uint16_t head; \
        volatile uint16_t tail; \
    } name##_Queue; \
    \
    static inline void name##_init(name##_Queue *q) { \
        q->head = 0; \
        q->tail = 0; \
    } \
    \
    static inline bool name##_is_empty(name##_Queue *q) { \
        return q->head == q->tail; \
    } \
    \
    static inline bool name##_is_full(name##_Queue *q) { \
        return ((q->tail + 1) % size) == q->head; \
    } \
    \
    static inline bool name##_enqueue(name##_Queue *q, type value) { \
        if (name##_is_full(q)) return false; \
        q->data[q->tail] = value; \
        q->tail = (q->tail + 1) % size; \
        return true; \
    } \
    \
    static inline bool name##_dequeue(name##_Queue *q, type *value) { \
        if (name##_is_empty(q)) return false; \
        *value = q->data[q->head]; \
        q->head = (q->head + 1) % size; \
        return true; \
    }

// Example usage:
// DEFINE_STATIC_QUEUE(sensor, float, 32)
// sensor_Queue my_queue;
// sensor_init(&my_queue);
// sensor_enqueue(&my_queue, 3.14f);
