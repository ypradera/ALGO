/*
 * RING BUFFER (CIRCULAR BUFFER)
 * Use: UART RX/TX, DMA buffers, logging, sensor data
 *
 * Features:
 * - Lock-free single producer/single consumer (SPSC)
 * - Power-of-2 size for fast modulo (masking)
 * - No dynamic allocation
 *
 * Time: O(1) all operations
 * Space: O(n) buffer size
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC RING BUFFER (byte-based)
 * ============================================ */

#define RING_BUFFER_SIZE    256  // Must be power of 2
#define RING_BUFFER_MASK    (RING_BUFFER_SIZE - 1)

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;  // write index
    volatile uint16_t tail;  // read index
} RingBuffer;

void ring_buffer_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    memset(rb->buffer, 0, RING_BUFFER_SIZE);
}

bool ring_buffer_is_empty(RingBuffer *rb) {
    return rb->head == rb->tail;
}

bool ring_buffer_is_full(RingBuffer *rb) {
    return ((rb->head + 1) & RING_BUFFER_MASK) == rb->tail;
}

uint16_t ring_buffer_count(RingBuffer *rb) {
    return (rb->head - rb->tail) & RING_BUFFER_MASK;
}

uint16_t ring_buffer_free_space(RingBuffer *rb) {
    return RING_BUFFER_SIZE - 1 - ring_buffer_count(rb);
}

bool ring_buffer_put(RingBuffer *rb, uint8_t data) {
    if (ring_buffer_is_full(rb)) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) & RING_BUFFER_MASK;
    return true;
}

bool ring_buffer_get(RingBuffer *rb, uint8_t *data) {
    if (ring_buffer_is_empty(rb)) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
    return true;
}

// Peek without removing
bool ring_buffer_peek(RingBuffer *rb, uint8_t *data) {
    if (ring_buffer_is_empty(rb)) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    return true;
}

// Peek at specific offset from tail
bool ring_buffer_peek_at(RingBuffer *rb, uint16_t offset, uint8_t *data) {
    if (offset >= ring_buffer_count(rb)) {
        return false;
    }

    *data = rb->buffer[(rb->tail + offset) & RING_BUFFER_MASK];
    return true;
}

// Bulk write
uint16_t ring_buffer_write(RingBuffer *rb, const uint8_t *data, uint16_t len) {
    uint16_t written = 0;

    while (written < len && !ring_buffer_is_full(rb)) {
        rb->buffer[rb->head] = data[written];
        rb->head = (rb->head + 1) & RING_BUFFER_MASK;
        written++;
    }

    return written;
}

// Bulk read
uint16_t ring_buffer_read(RingBuffer *rb, uint8_t *data, uint16_t len) {
    uint16_t read_count = 0;

    while (read_count < len && !ring_buffer_is_empty(rb)) {
        data[read_count] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
        read_count++;
    }

    return read_count;
}

// Discard n bytes
void ring_buffer_discard(RingBuffer *rb, uint16_t n) {
    uint16_t count = ring_buffer_count(rb);
    if (n > count) n = count;
    rb->tail = (rb->tail + n) & RING_BUFFER_MASK;
}

void ring_buffer_flush(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
}


/* ============================================
 * GENERIC RING BUFFER (any type)
 * ============================================ */

#define GENERIC_RING_SIZE   64

typedef struct {
    void *buffer;
    uint16_t element_size;
    uint16_t capacity;
    uint16_t mask;
    volatile uint16_t head;
    volatile uint16_t tail;
} GenericRingBuffer;

void generic_ring_init(GenericRingBuffer *rb, void *buffer, uint16_t element_size, uint16_t capacity) {
    rb->buffer = buffer;
    rb->element_size = element_size;
    rb->capacity = capacity;
    rb->mask = capacity - 1;  // capacity must be power of 2
    rb->head = 0;
    rb->tail = 0;
}

bool generic_ring_put(GenericRingBuffer *rb, const void *element) {
    if (((rb->head + 1) & rb->mask) == rb->tail) {
        return false;  // full
    }

    uint8_t *dest = (uint8_t *)rb->buffer + (rb->head * rb->element_size);
    memcpy(dest, element, rb->element_size);
    rb->head = (rb->head + 1) & rb->mask;
    return true;
}

bool generic_ring_get(GenericRingBuffer *rb, void *element) {
    if (rb->head == rb->tail) {
        return false;  // empty
    }

    uint8_t *src = (uint8_t *)rb->buffer + (rb->tail * rb->element_size);
    memcpy(element, src, rb->element_size);
    rb->tail = (rb->tail + 1) & rb->mask;
    return true;
}


/* ============================================
 * OVERWRITING RING BUFFER
 * (overwrites oldest data when full)
 * ============================================ */

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile bool full;
} OverwriteRingBuffer;

void overwrite_ring_init(OverwriteRingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->full = false;
}

void overwrite_ring_put(OverwriteRingBuffer *rb, uint8_t data) {
    rb->buffer[rb->head] = data;

    if (rb->full) {
        rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
    }

    rb->head = (rb->head + 1) & RING_BUFFER_MASK;
    rb->full = (rb->head == rb->tail);
}

bool overwrite_ring_get(OverwriteRingBuffer *rb, uint8_t *data) {
    if (!rb->full && rb->head == rb->tail) {
        return false;  // empty
    }

    *data = rb->buffer[rb->tail];
    rb->full = false;
    rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
    return true;
}


/* ============================================
 * LOCK-FREE SPSC RING BUFFER
 * (Single Producer Single Consumer)
 * Safe for ISR to main thread communication
 * ============================================ */

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;  // written by producer only
    volatile uint16_t tail;  // written by consumer only
} SPSCRingBuffer;

// Called from ISR (producer)
bool spsc_ring_put_isr(SPSCRingBuffer *rb, uint8_t data) {
    uint16_t next_head = (rb->head + 1) & RING_BUFFER_MASK;

    if (next_head == rb->tail) {
        return false;  // full
    }

    rb->buffer[rb->head] = data;

    // Memory barrier for ARM Cortex-M
    __asm volatile ("dmb" ::: "memory");

    rb->head = next_head;
    return true;
}

// Called from main loop (consumer)
bool spsc_ring_get(SPSCRingBuffer *rb, uint8_t *data) {
    if (rb->head == rb->tail) {
        return false;  // empty
    }

    *data = rb->buffer[rb->tail];

    // Memory barrier
    __asm volatile ("dmb" ::: "memory");

    rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
    return true;
}


/* ============================================
 * UART RX BUFFER EXAMPLE
 * ============================================ */

static RingBuffer uart_rx_buffer;

// Called from UART RX ISR
void UART_RX_ISR_Handler(uint8_t received_byte) {
    ring_buffer_put(&uart_rx_buffer, received_byte);
}

// Called from main loop
int uart_read_line(char *line, int max_len) {
    int i = 0;
    uint8_t byte;

    while (i < max_len - 1 && ring_buffer_get(&uart_rx_buffer, &byte)) {
        line[i++] = (char)byte;
        if (byte == '\n') break;
    }

    line[i] = '\0';
    return i;
}

// Check if complete line is available
bool uart_line_available(void) {
    uint16_t count = ring_buffer_count(&uart_rx_buffer);

    for (uint16_t i = 0; i < count; i++) {
        uint8_t byte;
        if (ring_buffer_peek_at(&uart_rx_buffer, i, &byte)) {
            if (byte == '\n') return true;
        }
    }

    return false;
}
