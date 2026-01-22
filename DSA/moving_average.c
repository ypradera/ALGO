/*
 * MOVING AVERAGE FILTER
 * Use: Signal smoothing, noise reduction, sensor data filtering
 *
 * Features:
 * - Simple Moving Average (SMA)
 * - Exponential Moving Average (EMA)
 * - Weighted Moving Average (WMA)
 * - Median filter
 *
 * Time: O(1) for update, O(n) for median
 * Space: O(window_size)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * SIMPLE MOVING AVERAGE (SMA)
 * ============================================ */

#define SMA_WINDOW_SIZE     16
#define SMA_WINDOW_MASK     (SMA_WINDOW_SIZE - 1)

typedef struct {
    int32_t buffer[SMA_WINDOW_SIZE];
    int32_t sum;
    uint8_t index;
    uint8_t count;
} SMAFilter;

void sma_init(SMAFilter *filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->sum = 0;
    filter->index = 0;
    filter->count = 0;
}

int32_t sma_update(SMAFilter *filter, int32_t new_value) {
    // Remove oldest value from sum
    filter->sum -= filter->buffer[filter->index];

    // Add new value
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;

    // Move index
    filter->index = (filter->index + 1) & SMA_WINDOW_MASK;

    // Track count for startup
    if (filter->count < SMA_WINDOW_SIZE) {
        filter->count++;
    }

    // Return average
    return filter->sum / filter->count;
}

int32_t sma_get_average(SMAFilter *filter) {
    if (filter->count == 0) return 0;
    return filter->sum / filter->count;
}

void sma_reset(SMAFilter *filter) {
    sma_init(filter);
}


/* ============================================
 * EXPONENTIAL MOVING AVERAGE (EMA)
 * Formula: EMA = alpha * new_value + (1 - alpha) * prev_EMA
 * Using fixed-point: alpha = ALPHA_NUM / ALPHA_DEN
 * ============================================ */

#define EMA_ALPHA_BITS      8
#define EMA_ALPHA_SCALE     (1 << EMA_ALPHA_BITS)

typedef struct {
    int32_t value;          // Current EMA value (scaled)
    uint8_t alpha;          // Smoothing factor (0-255)
    bool initialized;
} EMAFilter;

void ema_init(EMAFilter *filter, uint8_t alpha) {
    filter->value = 0;
    filter->alpha = alpha;
    filter->initialized = false;
}

// alpha: 0-255 where 255 = 100% new value (no smoothing)
// Typical values: 10-50 for heavy smoothing, 100-200 for light smoothing
int32_t ema_update(EMAFilter *filter, int32_t new_value) {
    if (!filter->initialized) {
        filter->value = new_value << EMA_ALPHA_BITS;
        filter->initialized = true;
        return new_value;
    }

    // EMA = alpha * new + (256 - alpha) * old, all in fixed point
    int32_t scaled_new = (int32_t)filter->alpha * new_value;
    int32_t scaled_old = (int32_t)(EMA_ALPHA_SCALE - filter->alpha) *
                         (filter->value >> EMA_ALPHA_BITS);

    filter->value = scaled_new + scaled_old;

    return filter->value >> EMA_ALPHA_BITS;
}

int32_t ema_get_value(EMAFilter *filter) {
    return filter->value >> EMA_ALPHA_BITS;
}

void ema_set_alpha(EMAFilter *filter, uint8_t alpha) {
    filter->alpha = alpha;
}

void ema_reset(EMAFilter *filter) {
    filter->initialized = false;
    filter->value = 0;
}


/* ============================================
 * WEIGHTED MOVING AVERAGE (WMA)
 * More recent samples have higher weight
 * ============================================ */

#define WMA_WINDOW_SIZE     8

typedef struct {
    int32_t buffer[WMA_WINDOW_SIZE];
    uint8_t weights[WMA_WINDOW_SIZE];
    uint16_t weight_sum;
    uint8_t index;
    uint8_t count;
} WMAFilter;

void wma_init(WMAFilter *filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));

    // Linear weights: 1, 2, 3, ..., WMA_WINDOW_SIZE
    filter->weight_sum = 0;
    for (int i = 0; i < WMA_WINDOW_SIZE; i++) {
        filter->weights[i] = i + 1;
        filter->weight_sum += filter->weights[i];
    }

    filter->index = 0;
    filter->count = 0;
}

int32_t wma_update(WMAFilter *filter, int32_t new_value) {
    filter->buffer[filter->index] = new_value;
    filter->index = (filter->index + 1) % WMA_WINDOW_SIZE;

    if (filter->count < WMA_WINDOW_SIZE) {
        filter->count++;
    }

    // Calculate weighted sum
    int64_t weighted_sum = 0;
    uint16_t actual_weight_sum = 0;

    for (int i = 0; i < filter->count; i++) {
        int idx = (filter->index - 1 - i + WMA_WINDOW_SIZE) % WMA_WINDOW_SIZE;
        int weight = filter->count - i;  // Most recent = highest weight
        weighted_sum += (int64_t)filter->buffer[idx] * weight;
        actual_weight_sum += weight;
    }

    return (int32_t)(weighted_sum / actual_weight_sum);
}


/* ============================================
 * MEDIAN FILTER (removes outliers)
 * ============================================ */

#define MEDIAN_WINDOW_SIZE  5

typedef struct {
    int32_t buffer[MEDIAN_WINDOW_SIZE];
    int32_t sorted[MEDIAN_WINDOW_SIZE];
    uint8_t index;
    uint8_t count;
} MedianFilter;

void median_init(MedianFilter *filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));
    memset(filter->sorted, 0, sizeof(filter->sorted));
    filter->index = 0;
    filter->count = 0;
}

// Insertion sort for small arrays
static void insertion_sort(int32_t *arr, uint8_t len) {
    for (int i = 1; i < len; i++) {
        int32_t key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int32_t median_update(MedianFilter *filter, int32_t new_value) {
    filter->buffer[filter->index] = new_value;
    filter->index = (filter->index + 1) % MEDIAN_WINDOW_SIZE;

    if (filter->count < MEDIAN_WINDOW_SIZE) {
        filter->count++;
    }

    // Copy and sort
    memcpy(filter->sorted, filter->buffer, sizeof(int32_t) * filter->count);
    insertion_sort(filter->sorted, filter->count);

    // Return median
    return filter->sorted[filter->count / 2];
}


/* ============================================
 * COMBINED FILTER (Median + EMA)
 * Removes spikes then smooths
 * ============================================ */

typedef struct {
    MedianFilter median;
    EMAFilter ema;
} CombinedFilter;

void combined_init(CombinedFilter *filter, uint8_t ema_alpha) {
    median_init(&filter->median);
    ema_init(&filter->ema, ema_alpha);
}

int32_t combined_update(CombinedFilter *filter, int32_t new_value) {
    // First remove spikes with median
    int32_t median_value = median_update(&filter->median, new_value);

    // Then smooth with EMA
    return ema_update(&filter->ema, median_value);
}


/* ============================================
 * RATE LIMITER (slew rate limiting)
 * ============================================ */

typedef struct {
    int32_t current_value;
    int32_t max_rate;       // Max change per update
    bool initialized;
} RateLimiter;

void rate_limiter_init(RateLimiter *limiter, int32_t max_rate) {
    limiter->current_value = 0;
    limiter->max_rate = max_rate;
    limiter->initialized = false;
}

int32_t rate_limiter_update(RateLimiter *limiter, int32_t target) {
    if (!limiter->initialized) {
        limiter->current_value = target;
        limiter->initialized = true;
        return target;
    }

    int32_t diff = target - limiter->current_value;

    if (diff > limiter->max_rate) {
        limiter->current_value += limiter->max_rate;
    } else if (diff < -limiter->max_rate) {
        limiter->current_value -= limiter->max_rate;
    } else {
        limiter->current_value = target;
    }

    return limiter->current_value;
}


/* ============================================
 * LOW-PASS FILTER (IIR, single pole)
 * ============================================ */

typedef struct {
    int32_t output;
    uint8_t coefficient;    // 0-255, higher = more smoothing
    bool initialized;
} LowPassFilter;

void lowpass_init(LowPassFilter *filter, uint8_t coefficient) {
    filter->output = 0;
    filter->coefficient = coefficient;
    filter->initialized = false;
}

int32_t lowpass_update(LowPassFilter *filter, int32_t input) {
    if (!filter->initialized) {
        filter->output = input << 8;
        filter->initialized = true;
        return input;
    }

    // output = output + (input - output) / coefficient
    int32_t scaled_input = input << 8;
    int32_t diff = scaled_input - filter->output;

    filter->output += diff / filter->coefficient;

    return filter->output >> 8;
}


/* ============================================
 * HIGH-PASS FILTER (removes DC offset)
 * ============================================ */

typedef struct {
    int32_t prev_input;
    int32_t prev_output;
    uint8_t alpha;          // 0-255, higher = less filtering
    bool initialized;
} HighPassFilter;

void highpass_init(HighPassFilter *filter, uint8_t alpha) {
    filter->prev_input = 0;
    filter->prev_output = 0;
    filter->alpha = alpha;
    filter->initialized = false;
}

int32_t highpass_update(HighPassFilter *filter, int32_t input) {
    if (!filter->initialized) {
        filter->prev_input = input;
        filter->prev_output = 0;
        filter->initialized = true;
        return 0;
    }

    // y[n] = alpha * (y[n-1] + x[n] - x[n-1])
    int32_t diff = input - filter->prev_input;
    int32_t output = ((int32_t)filter->alpha * (filter->prev_output + diff)) >> 8;

    filter->prev_input = input;
    filter->prev_output = output;

    return output;
}


/* ============================================
 * FLOAT VERSIONS (if FPU available)
 * ============================================ */

#ifdef USE_FLOAT_FILTERS

typedef struct {
    float value;
    float alpha;
    bool initialized;
} EMAFilterFloat;

void ema_float_init(EMAFilterFloat *filter, float alpha) {
    filter->value = 0.0f;
    filter->alpha = alpha;  // 0.0 to 1.0
    filter->initialized = false;
}

float ema_float_update(EMAFilterFloat *filter, float new_value) {
    if (!filter->initialized) {
        filter->value = new_value;
        filter->initialized = true;
        return new_value;
    }

    filter->value = filter->alpha * new_value + (1.0f - filter->alpha) * filter->value;
    return filter->value;
}

#endif // USE_FLOAT_FILTERS
