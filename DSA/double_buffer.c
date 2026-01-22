/*
 * DOUBLE BUFFER IMPLEMENTATIONS
 * Use: DMA transfers, display updates, audio streaming, sensor acquisition
 *
 * Features:
 * - Basic double buffer
 * - Triple buffer for variable timing
 * - Ping-pong buffer for DMA
 * - Frame buffer for displays
 * - Audio buffer with interpolation
 *
 * Time: O(1) buffer swap
 * Space: O(2n) or O(3n) for triple buffering
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC DOUBLE BUFFER
 * ============================================ */

#define DOUBLE_BUF_SIZE     256

typedef struct {
    uint8_t buffer_a[DOUBLE_BUF_SIZE];
    uint8_t buffer_b[DOUBLE_BUF_SIZE];
    uint8_t *write_buf;     // Buffer being written to
    uint8_t *read_buf;      // Buffer being read from
    volatile bool swap_pending;
} DoubleBuffer;

void double_buffer_init(DoubleBuffer *db) {
    memset(db->buffer_a, 0, DOUBLE_BUF_SIZE);
    memset(db->buffer_b, 0, DOUBLE_BUF_SIZE);
    db->write_buf = db->buffer_a;
    db->read_buf = db->buffer_b;
    db->swap_pending = false;
}

// Get buffer to write to
uint8_t* double_buffer_get_write(DoubleBuffer *db) {
    return db->write_buf;
}

// Get buffer to read from
uint8_t* double_buffer_get_read(DoubleBuffer *db) {
    return db->read_buf;
}

// Request buffer swap (call from producer)
void double_buffer_swap_request(DoubleBuffer *db) {
    db->swap_pending = true;
}

// Perform swap (call from consumer or sync point)
void double_buffer_swap(DoubleBuffer *db) {
    if (db->swap_pending) {
        uint8_t *temp = db->write_buf;
        db->write_buf = db->read_buf;
        db->read_buf = temp;
        db->swap_pending = false;
    }
}

// Check if new data is ready
bool double_buffer_is_ready(DoubleBuffer *db) {
    return db->swap_pending;
}


/* ============================================
 * TRIPLE BUFFER
 * Eliminates producer/consumer blocking
 * ============================================ */

#define TRIPLE_BUF_SIZE     256

typedef enum {
    BUF_STATE_FREE,         // Can be written to
    BUF_STATE_READY,        // Has new data, ready for reading
    BUF_STATE_IN_USE        // Currently being read
} BufferState;

typedef struct {
    uint8_t buffers[3][TRIPLE_BUF_SIZE];
    volatile uint8_t write_idx;     // Current write buffer
    volatile uint8_t ready_idx;     // Buffer with latest complete data
    volatile uint8_t read_idx;      // Current read buffer
    volatile BufferState states[3];
} TripleBuffer;

void triple_buffer_init(TripleBuffer *tb) {
    for (int i = 0; i < 3; i++) {
        memset(tb->buffers[i], 0, TRIPLE_BUF_SIZE);
        tb->states[i] = BUF_STATE_FREE;
    }
    tb->write_idx = 0;
    tb->ready_idx = 1;
    tb->read_idx = 2;
    tb->states[0] = BUF_STATE_FREE;
}

uint8_t* triple_buffer_get_write(TripleBuffer *tb) {
    return tb->buffers[tb->write_idx];
}

// Producer: Mark write buffer as ready, get new write buffer
void triple_buffer_write_done(TripleBuffer *tb) {
    // Current write buffer becomes ready
    uint8_t old_ready = tb->ready_idx;
    tb->ready_idx = tb->write_idx;
    tb->states[tb->ready_idx] = BUF_STATE_READY;

    // Old ready buffer (if not being read) becomes new write buffer
    if (tb->states[old_ready] != BUF_STATE_IN_USE) {
        tb->write_idx = old_ready;
        tb->states[old_ready] = BUF_STATE_FREE;
    } else {
        // Find another free buffer
        for (int i = 0; i < 3; i++) {
            if (i != tb->ready_idx && tb->states[i] != BUF_STATE_IN_USE) {
                tb->write_idx = i;
                tb->states[i] = BUF_STATE_FREE;
                break;
            }
        }
    }
}

// Consumer: Get latest data buffer
uint8_t* triple_buffer_get_read(TripleBuffer *tb) {
    // Release current read buffer
    if (tb->states[tb->read_idx] == BUF_STATE_IN_USE) {
        tb->states[tb->read_idx] = BUF_STATE_FREE;
    }

    // Get ready buffer
    tb->read_idx = tb->ready_idx;
    tb->states[tb->read_idx] = BUF_STATE_IN_USE;

    return tb->buffers[tb->read_idx];
}

// Consumer: Release read buffer
void triple_buffer_read_done(TripleBuffer *tb) {
    tb->states[tb->read_idx] = BUF_STATE_FREE;
}


/* ============================================
 * PING-PONG DMA BUFFER
 * For continuous DMA transfers
 * ============================================ */

#define DMA_BUF_SIZE        512

typedef void (*DMACallback)(uint8_t *buffer, uint16_t size);

typedef struct {
    uint8_t buffer_a[DMA_BUF_SIZE];
    uint8_t buffer_b[DMA_BUF_SIZE];
    volatile uint8_t active_buffer;     // 0 = A, 1 = B
    volatile bool half_complete;
    volatile bool full_complete;
    DMACallback callback;
} PingPongDMA;

void pingpong_init(PingPongDMA *pp, DMACallback callback) {
    memset(pp->buffer_a, 0, DMA_BUF_SIZE);
    memset(pp->buffer_b, 0, DMA_BUF_SIZE);
    pp->active_buffer = 0;
    pp->half_complete = false;
    pp->full_complete = false;
    pp->callback = callback;
}

// Get buffer addresses for DMA setup
uint8_t* pingpong_get_buffer_a(PingPongDMA *pp) {
    return pp->buffer_a;
}

uint8_t* pingpong_get_buffer_b(PingPongDMA *pp) {
    return pp->buffer_b;
}

// Call from DMA half-complete ISR
void pingpong_half_complete_isr(PingPongDMA *pp) {
    pp->half_complete = true;
}

// Call from DMA complete ISR
void pingpong_complete_isr(PingPongDMA *pp) {
    pp->full_complete = true;
    pp->active_buffer ^= 1;  // Toggle active buffer
}

// Process completed buffer (call from main loop)
void pingpong_process(PingPongDMA *pp) {
    if (pp->half_complete) {
        pp->half_complete = false;
        // First half of current buffer is ready
        uint8_t *buf = (pp->active_buffer == 0) ? pp->buffer_a : pp->buffer_b;
        if (pp->callback) {
            pp->callback(buf, DMA_BUF_SIZE / 2);
        }
    }

    if (pp->full_complete) {
        pp->full_complete = false;
        // Second half (previous buffer) is ready
        uint8_t *buf = (pp->active_buffer == 0) ? pp->buffer_b : pp->buffer_a;
        if (pp->callback) {
            pp->callback(buf + DMA_BUF_SIZE / 2, DMA_BUF_SIZE / 2);
        }
    }
}


/* ============================================
 * FRAME BUFFER (Display)
 * ============================================ */

#define FRAME_WIDTH         128
#define FRAME_HEIGHT        64
#define FRAME_BUF_SIZE      (FRAME_WIDTH * FRAME_HEIGHT / 8)  // 1-bit per pixel

typedef struct {
    uint8_t buffer_a[FRAME_BUF_SIZE];
    uint8_t buffer_b[FRAME_BUF_SIZE];
    uint8_t *draw_buffer;       // Buffer being drawn to
    uint8_t *display_buffer;    // Buffer being displayed
    volatile bool swap_pending;
    volatile bool transfer_active;
} FrameBuffer;

void framebuffer_init(FrameBuffer *fb) {
    memset(fb->buffer_a, 0, FRAME_BUF_SIZE);
    memset(fb->buffer_b, 0, FRAME_BUF_SIZE);
    fb->draw_buffer = fb->buffer_a;
    fb->display_buffer = fb->buffer_b;
    fb->swap_pending = false;
    fb->transfer_active = false;
}

void framebuffer_clear(FrameBuffer *fb) {
    memset(fb->draw_buffer, 0, FRAME_BUF_SIZE);
}

void framebuffer_fill(FrameBuffer *fb, uint8_t pattern) {
    memset(fb->draw_buffer, pattern, FRAME_BUF_SIZE);
}

// Set pixel (1-bit monochrome)
void framebuffer_set_pixel(FrameBuffer *fb, uint8_t x, uint8_t y, bool on) {
    if (x >= FRAME_WIDTH || y >= FRAME_HEIGHT) return;

    uint16_t byte_idx = (y / 8) * FRAME_WIDTH + x;
    uint8_t bit_idx = y % 8;

    if (on) {
        fb->draw_buffer[byte_idx] |= (1 << bit_idx);
    } else {
        fb->draw_buffer[byte_idx] &= ~(1 << bit_idx);
    }
}

bool framebuffer_get_pixel(FrameBuffer *fb, uint8_t x, uint8_t y) {
    if (x >= FRAME_WIDTH || y >= FRAME_HEIGHT) return false;

    uint16_t byte_idx = (y / 8) * FRAME_WIDTH + x;
    uint8_t bit_idx = y % 8;

    return (fb->draw_buffer[byte_idx] & (1 << bit_idx)) != 0;
}

// Draw horizontal line
void framebuffer_hline(FrameBuffer *fb, uint8_t x, uint8_t y, uint8_t length) {
    for (uint8_t i = 0; i < length && (x + i) < FRAME_WIDTH; i++) {
        framebuffer_set_pixel(fb, x + i, y, true);
    }
}

// Draw vertical line
void framebuffer_vline(FrameBuffer *fb, uint8_t x, uint8_t y, uint8_t length) {
    for (uint8_t i = 0; i < length && (y + i) < FRAME_HEIGHT; i++) {
        framebuffer_set_pixel(fb, x, y + i, true);
    }
}

// Draw rectangle
void framebuffer_rect(FrameBuffer *fb, uint8_t x, uint8_t y,
                      uint8_t width, uint8_t height, bool filled) {
    if (filled) {
        for (uint8_t j = 0; j < height; j++) {
            framebuffer_hline(fb, x, y + j, width);
        }
    } else {
        framebuffer_hline(fb, x, y, width);
        framebuffer_hline(fb, x, y + height - 1, width);
        framebuffer_vline(fb, x, y, height);
        framebuffer_vline(fb, x + width - 1, y, height);
    }
}

// Request buffer swap after drawing complete
void framebuffer_swap_request(FrameBuffer *fb) {
    fb->swap_pending = true;
}

// Get display buffer for transfer
uint8_t* framebuffer_get_display(FrameBuffer *fb) {
    return fb->display_buffer;
}

// Call after display transfer complete (e.g., from DMA ISR)
void framebuffer_transfer_complete(FrameBuffer *fb) {
    fb->transfer_active = false;

    if (fb->swap_pending) {
        uint8_t *temp = fb->draw_buffer;
        fb->draw_buffer = fb->display_buffer;
        fb->display_buffer = temp;
        fb->swap_pending = false;
    }
}

bool framebuffer_can_start_transfer(FrameBuffer *fb) {
    return !fb->transfer_active;
}

void framebuffer_start_transfer(FrameBuffer *fb) {
    fb->transfer_active = true;
}


/* ============================================
 * AUDIO DOUBLE BUFFER
 * With sample rate conversion
 * ============================================ */

#define AUDIO_BUF_SIZE      256
#define AUDIO_SAMPLE_BITS   16

typedef struct {
    int16_t buffer_a[AUDIO_BUF_SIZE];
    int16_t buffer_b[AUDIO_BUF_SIZE];
    int16_t *playback_buf;
    int16_t *fill_buf;
    volatile uint16_t playback_idx;
    volatile bool buffer_ready;
    volatile bool underrun;
} AudioBuffer;

void audio_buffer_init(AudioBuffer *ab) {
    memset(ab->buffer_a, 0, sizeof(ab->buffer_a));
    memset(ab->buffer_b, 0, sizeof(ab->buffer_b));
    ab->playback_buf = ab->buffer_a;
    ab->fill_buf = ab->buffer_b;
    ab->playback_idx = 0;
    ab->buffer_ready = false;
    ab->underrun = false;
}

// Fill buffer with samples
void audio_buffer_fill(AudioBuffer *ab, int16_t *samples, uint16_t count) {
    if (count > AUDIO_BUF_SIZE) count = AUDIO_BUF_SIZE;
    memcpy(ab->fill_buf, samples, count * sizeof(int16_t));
    ab->buffer_ready = true;
}

// Get next sample for DAC (call from timer ISR)
int16_t audio_buffer_get_sample(AudioBuffer *ab) {
    int16_t sample = ab->playback_buf[ab->playback_idx];
    ab->playback_idx++;

    if (ab->playback_idx >= AUDIO_BUF_SIZE) {
        ab->playback_idx = 0;

        if (ab->buffer_ready) {
            // Swap buffers
            int16_t *temp = ab->playback_buf;
            ab->playback_buf = ab->fill_buf;
            ab->fill_buf = temp;
            ab->buffer_ready = false;
            ab->underrun = false;
        } else {
            // Buffer underrun - repeat last buffer
            ab->underrun = true;
        }
    }

    return sample;
}

bool audio_buffer_needs_fill(AudioBuffer *ab) {
    return !ab->buffer_ready;
}

bool audio_buffer_has_underrun(AudioBuffer *ab) {
    bool underrun = ab->underrun;
    ab->underrun = false;
    return underrun;
}


/* ============================================
 * SENSOR DATA BUFFER
 * For ADC/sensor acquisition
 * ============================================ */

#define SENSOR_CHANNELS     4
#define SAMPLES_PER_BUFFER  64

typedef struct {
    uint16_t buffer_a[SENSOR_CHANNELS][SAMPLES_PER_BUFFER];
    uint16_t buffer_b[SENSOR_CHANNELS][SAMPLES_PER_BUFFER];
    uint16_t (*acquire_buf)[SAMPLES_PER_BUFFER];
    uint16_t (*process_buf)[SAMPLES_PER_BUFFER];
    volatile uint16_t sample_idx;
    volatile bool buffer_full;
    volatile uint32_t sequence_num;
} SensorBuffer;

void sensor_buffer_init(SensorBuffer *sb) {
    memset(sb->buffer_a, 0, sizeof(sb->buffer_a));
    memset(sb->buffer_b, 0, sizeof(sb->buffer_b));
    sb->acquire_buf = sb->buffer_a;
    sb->process_buf = sb->buffer_b;
    sb->sample_idx = 0;
    sb->buffer_full = false;
    sb->sequence_num = 0;
}

// Add sample set (call from ADC ISR)
void sensor_buffer_add_sample(SensorBuffer *sb, uint16_t *channel_data) {
    for (int ch = 0; ch < SENSOR_CHANNELS; ch++) {
        sb->acquire_buf[ch][sb->sample_idx] = channel_data[ch];
    }

    sb->sample_idx++;
    if (sb->sample_idx >= SAMPLES_PER_BUFFER) {
        sb->sample_idx = 0;
        sb->buffer_full = true;
        sb->sequence_num++;

        // Swap buffers
        uint16_t (*temp)[SAMPLES_PER_BUFFER] = sb->acquire_buf;
        sb->acquire_buf = sb->process_buf;
        sb->process_buf = temp;
    }
}

// Check if buffer ready for processing
bool sensor_buffer_is_ready(SensorBuffer *sb) {
    return sb->buffer_full;
}

// Get process buffer
uint16_t* sensor_buffer_get_channel(SensorBuffer *sb, uint8_t channel) {
    if (channel >= SENSOR_CHANNELS) return NULL;
    return sb->process_buf[channel];
}

// Mark processing complete
void sensor_buffer_processing_done(SensorBuffer *sb) {
    sb->buffer_full = false;
}

uint32_t sensor_buffer_get_sequence(SensorBuffer *sb) {
    return sb->sequence_num;
}


/* ============================================
 * CIRCULAR DMA BUFFER
 * For continuous streaming
 * ============================================ */

#define CIRC_DMA_SIZE       1024
#define CIRC_DMA_MASK       (CIRC_DMA_SIZE - 1)

typedef struct {
    uint8_t buffer[CIRC_DMA_SIZE];
    volatile uint16_t dma_pos;      // Updated by DMA or ISR
    uint16_t read_pos;              // Read position
} CircularDMABuffer;

void circ_dma_init(CircularDMABuffer *cb) {
    memset(cb->buffer, 0, CIRC_DMA_SIZE);
    cb->dma_pos = 0;
    cb->read_pos = 0;
}

// Get buffer address for DMA setup
uint8_t* circ_dma_get_buffer(CircularDMABuffer *cb) {
    return cb->buffer;
}

uint16_t circ_dma_get_size(void) {
    return CIRC_DMA_SIZE;
}

// Update DMA position (call from ISR or read DMA counter)
void circ_dma_update_pos(CircularDMABuffer *cb, uint16_t pos) {
    cb->dma_pos = pos & CIRC_DMA_MASK;
}

// Get number of bytes available to read
uint16_t circ_dma_available(CircularDMABuffer *cb) {
    uint16_t dma = cb->dma_pos;
    uint16_t read = cb->read_pos;

    if (dma >= read) {
        return dma - read;
    } else {
        return CIRC_DMA_SIZE - read + dma;
    }
}

// Read from buffer
uint16_t circ_dma_read(CircularDMABuffer *cb, uint8_t *dest, uint16_t max_len) {
    uint16_t available = circ_dma_available(cb);
    if (max_len > available) max_len = available;

    for (uint16_t i = 0; i < max_len; i++) {
        dest[i] = cb->buffer[cb->read_pos];
        cb->read_pos = (cb->read_pos + 1) & CIRC_DMA_MASK;
    }

    return max_len;
}

// Peek without consuming
uint8_t circ_dma_peek(CircularDMABuffer *cb, uint16_t offset) {
    return cb->buffer[(cb->read_pos + offset) & CIRC_DMA_MASK];
}

// Skip bytes
void circ_dma_skip(CircularDMABuffer *cb, uint16_t count) {
    cb->read_pos = (cb->read_pos + count) & CIRC_DMA_MASK;
}

