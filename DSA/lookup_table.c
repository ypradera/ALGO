/*
 * LOOKUP TABLE IMPLEMENTATIONS
 * Use: Fast math, linearization, calibration, gamma correction, sin/cos
 *
 * Features:
 * - Trigonometric tables (sin, cos)
 * - Linearization tables
 * - Gamma/brightness correction
 * - Temperature calibration
 * - Interpolation methods
 *
 * Time: O(1) lookup
 * Space: O(n) table size
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * SINE/COSINE LOOKUP TABLE
 * 256 entries for 0-90 degrees, Q15 format
 * ============================================ */

// Q15 format: 32767 = 1.0, -32768 = -1.0
#define Q15_ONE     32767
#define Q15_HALF    16384

// First quadrant sine table (0-90 degrees, 256 entries)
static const int16_t sine_table_q15[257] = {
    0, 402, 804, 1206, 1608, 2009, 2410, 2811,
    3212, 3612, 4011, 4410, 4808, 5205, 5602, 5998,
    6393, 6786, 7179, 7571, 7962, 8351, 8739, 9126,
    9512, 9896, 10278, 10659, 11039, 11417, 11793, 12167,
    12539, 12910, 13279, 13645, 14010, 14372, 14732, 15090,
    15446, 15800, 16151, 16499, 16846, 17189, 17530, 17869,
    18204, 18537, 18868, 19195, 19519, 19841, 20159, 20475,
    20787, 21096, 21403, 21706, 22005, 22301, 22594, 22884,
    23170, 23452, 23731, 24007, 24279, 24547, 24811, 25072,
    25329, 25582, 25832, 26077, 26319, 26556, 26790, 27019,
    27245, 27466, 27683, 27896, 28105, 28310, 28510, 28706,
    28898, 29085, 29268, 29447, 29621, 29791, 29956, 30117,
    30273, 30424, 30571, 30714, 30852, 30985, 31113, 31237,
    31356, 31470, 31580, 31685, 31785, 31880, 31971, 32057,
    32137, 32213, 32285, 32351, 32412, 32469, 32521, 32567,
    32609, 32646, 32678, 32705, 32728, 32745, 32757, 32765,
    32767, 32765, 32757, 32745, 32728, 32705, 32678, 32646,
    32609, 32567, 32521, 32469, 32412, 32351, 32285, 32213,
    32137, 32057, 31971, 31880, 31785, 31685, 31580, 31470,
    31356, 31237, 31113, 30985, 30852, 30714, 30571, 30424,
    30273, 30117, 29956, 29791, 29621, 29447, 29268, 29085,
    28898, 28706, 28510, 28310, 28105, 27896, 27683, 27466,
    27245, 27019, 26790, 26556, 26319, 26077, 25832, 25582,
    25329, 25072, 24811, 24547, 24279, 24007, 23731, 23452,
    23170, 22884, 22594, 22301, 22005, 21706, 21403, 21096,
    20787, 20475, 20159, 19841, 19519, 19195, 18868, 18537,
    18204, 17869, 17530, 17189, 16846, 16499, 16151, 15800,
    15446, 15090, 14732, 14372, 14010, 13645, 13279, 12910,
    12539, 12167, 11793, 11417, 11039, 10659, 10278, 9896,
    9512, 9126, 8739, 8351, 7962, 7571, 7179, 6786,
    6393, 5998, 5602, 5205, 4808, 4410, 4011, 3612,
    3212, 2811, 2410, 2009, 1608, 1206, 804, 402,
    0
};

// Input: 0-1023 (0-360 degrees mapped to 10-bit)
// Output: Q15 sine value
int16_t sin_lut(uint16_t angle_1024) {
    uint16_t idx = angle_1024 & 0x3FF;  // Mod 1024
    uint8_t quadrant = idx >> 8;
    uint8_t offset = idx & 0xFF;

    switch (quadrant) {
        case 0: return sine_table_q15[offset];
        case 1: return sine_table_q15[256 - offset];
        case 2: return -sine_table_q15[offset];
        case 3: return -sine_table_q15[256 - offset];
    }
    return 0;
}

int16_t cos_lut(uint16_t angle_1024) {
    return sin_lut(angle_1024 + 256);  // cos = sin(x + 90)
}

// With linear interpolation for higher accuracy
int16_t sin_lut_interp(uint16_t angle_1024) {
    uint16_t idx = angle_1024 & 0x3FF;
    uint8_t quadrant = idx >> 8;
    uint8_t coarse = (idx & 0xFF) >> 0;  // Could shift for sub-sampling
    uint8_t frac = idx & 0x00;  // Fractional part (if sub-sampling)

    int16_t y0 = sine_table_q15[coarse];
    int16_t y1 = sine_table_q15[coarse + 1];

    // Linear interpolation
    int16_t result = y0 + (((int32_t)(y1 - y0) * frac) >> 8);

    // Apply quadrant
    if (quadrant == 1) result = sine_table_q15[256 - (idx & 0xFF)];
    else if (quadrant == 2) result = -sine_table_q15[idx & 0xFF];
    else if (quadrant == 3) result = -sine_table_q15[256 - (idx & 0xFF)];

    return result;
}


/* ============================================
 * GAMMA CORRECTION TABLE
 * For LED brightness / display
 * ============================================ */

// 8-bit input to 8-bit output, gamma 2.2
static const uint8_t gamma_2_2[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    178, 180, 182, 184, 187, 189, 191, 194, 196, 199, 201, 204, 206, 209, 211, 214,
    216, 219, 221, 224, 227, 229, 232, 235, 237, 240, 243, 246, 249, 251, 254, 255
};

uint8_t gamma_correct(uint8_t input) {
    return gamma_2_2[input];
}

// Inverse gamma (for converting sRGB to linear)
static const uint8_t gamma_inv_2_2[256] = {
    0, 21, 28, 34, 39, 43, 46, 50, 53, 56, 59, 61, 64, 66, 68, 70,
    72, 74, 76, 78, 80, 82, 84, 85, 87, 89, 90, 92, 93, 95, 96, 98,
    99, 101, 102, 103, 105, 106, 107, 109, 110, 111, 112, 114, 115, 116, 117, 118,
    119, 120, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 149, 150,
    150, 151, 152, 153, 154, 155, 155, 156, 157, 158, 159, 159, 160, 161, 162, 162,
    163, 164, 165, 165, 166, 167, 168, 168, 169, 170, 170, 171, 172, 173, 173, 174,
    175, 175, 176, 177, 177, 178, 179, 179, 180, 181, 181, 182, 183, 183, 184, 185,
    185, 186, 187, 187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 193, 194, 195,
    195, 196, 196, 197, 198, 198, 199, 199, 200, 201, 201, 202, 202, 203, 203, 204,
    205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 210, 211, 211, 212, 212, 213,
    214, 214, 215, 215, 216, 216, 217, 217, 218, 218, 219, 220, 220, 221, 221, 222,
    222, 223, 223, 224, 224, 225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230,
    230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238,
    238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246,
    246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 251, 251, 252, 252, 253, 255
};

uint8_t gamma_uncorrect(uint8_t input) {
    return gamma_inv_2_2[input];
}


/* ============================================
 * LINEARIZATION TABLE
 * For sensor calibration
 * ============================================ */

#define LIN_TABLE_SIZE      16

typedef struct {
    int16_t input[LIN_TABLE_SIZE];      // Raw ADC values
    int16_t output[LIN_TABLE_SIZE];     // Calibrated values
    uint8_t size;
} LinearizationTable;

void lin_table_init(LinearizationTable *lt) {
    lt->size = 0;
}

void lin_table_add_point(LinearizationTable *lt, int16_t input, int16_t output) {
    if (lt->size >= LIN_TABLE_SIZE) return;

    // Insert in sorted order
    int i;
    for (i = lt->size; i > 0 && lt->input[i-1] > input; i--) {
        lt->input[i] = lt->input[i-1];
        lt->output[i] = lt->output[i-1];
    }
    lt->input[i] = input;
    lt->output[i] = output;
    lt->size++;
}

// Linear interpolation lookup
int16_t lin_table_lookup(LinearizationTable *lt, int16_t input) {
    if (lt->size == 0) return input;
    if (lt->size == 1) return lt->output[0];

    // Clamp to table range
    if (input <= lt->input[0]) return lt->output[0];
    if (input >= lt->input[lt->size - 1]) return lt->output[lt->size - 1];

    // Find segment
    uint8_t i;
    for (i = 1; i < lt->size; i++) {
        if (input < lt->input[i]) break;
    }

    // Linear interpolation
    int32_t x0 = lt->input[i - 1];
    int32_t x1 = lt->input[i];
    int32_t y0 = lt->output[i - 1];
    int32_t y1 = lt->output[i];

    return y0 + ((y1 - y0) * (input - x0)) / (x1 - x0);
}


/* ============================================
 * THERMISTOR LOOKUP TABLE
 * NTC temperature conversion
 * ============================================ */

#define THERM_TABLE_SIZE    21

// ADC value to temperature (in 0.1°C)
// Example: 10K NTC with 10K pullup, 10-bit ADC
static const uint16_t therm_adc[THERM_TABLE_SIZE] = {
    1008, 994, 969, 930, 875, 806, 724, 636, 548, 464,
    388, 321, 264, 216, 177, 145, 119, 98, 81, 67, 56
};

static const int16_t therm_temp[THERM_TABLE_SIZE] = {
    -200, -100, 0, 100, 200, 300, 400, 500, 600, 700,
    800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800
};

// ADC reading to temperature in 0.1°C
int16_t thermistor_lookup(uint16_t adc_value) {
    // Table is in descending ADC order
    if (adc_value >= therm_adc[0]) return therm_temp[0];
    if (adc_value <= therm_adc[THERM_TABLE_SIZE - 1]) return therm_temp[THERM_TABLE_SIZE - 1];

    // Find segment
    uint8_t i;
    for (i = 1; i < THERM_TABLE_SIZE; i++) {
        if (adc_value > therm_adc[i]) break;
    }

    // Linear interpolation
    int32_t x0 = therm_adc[i - 1];
    int32_t x1 = therm_adc[i];
    int32_t y0 = therm_temp[i - 1];
    int32_t y1 = therm_temp[i];

    return y0 + ((y1 - y0) * (int32_t)(adc_value - x0)) / (x1 - x0);
}


/* ============================================
 * PWM DIMMING CURVE
 * Perceptually linear brightness
 * ============================================ */

// CIE 1931 lightness curve (8-bit input to 8-bit PWM)
static const uint8_t cie_lut[256] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4,
    4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7,
    7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10, 11, 11,
    11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17,
    17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25,
    25, 26, 26, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 34, 34,
    35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47,
    47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 61,
    62, 63, 64, 65, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 79,
    80, 81, 82, 83, 85, 86, 87, 88, 90, 91, 92, 94, 95, 96, 98, 99,
    100, 102, 103, 105, 106, 108, 109, 110, 112, 113, 115, 116, 118, 120, 121, 123,
    124, 126, 128, 129, 131, 132, 134, 136, 138, 139, 141, 143, 145, 146, 148, 150,
    152, 154, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181,
    183, 185, 187, 189, 191, 193, 196, 198, 200, 202, 204, 207, 209, 211, 214, 216,
    218, 220, 223, 225, 228, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255
};

uint8_t brightness_to_pwm(uint8_t brightness) {
    return cie_lut[brightness];
}


/* ============================================
 * INTERPOLATION UTILITIES
 * ============================================ */

// Linear interpolation between two values
// frac is 0-255 (0 = a, 255 = b)
int16_t lerp_i16(int16_t a, int16_t b, uint8_t frac) {
    return a + (((int32_t)(b - a) * frac) >> 8);
}

uint16_t lerp_u16(uint16_t a, uint16_t b, uint8_t frac) {
    return a + (((uint32_t)(b - a) * frac) >> 8);
}

// 2D bilinear interpolation
// Values at corners: v00, v10, v01, v11
// Position: fx, fy (0-255 each)
int16_t bilerp_i16(int16_t v00, int16_t v10, int16_t v01, int16_t v11,
                   uint8_t fx, uint8_t fy) {
    int16_t v0 = lerp_i16(v00, v10, fx);  // Top edge
    int16_t v1 = lerp_i16(v01, v11, fx);  // Bottom edge
    return lerp_i16(v0, v1, fy);
}


/* ============================================
 * GENERIC LOOKUP TABLE STRUCTURE
 * ============================================ */

#define GENERIC_LUT_MAX     32

typedef struct {
    int32_t x[GENERIC_LUT_MAX];
    int32_t y[GENERIC_LUT_MAX];
    uint8_t size;
    bool extrapolate;           // Allow extrapolation outside range
} GenericLUT;

void generic_lut_init(GenericLUT *lut, bool extrapolate) {
    lut->size = 0;
    lut->extrapolate = extrapolate;
}

bool generic_lut_add(GenericLUT *lut, int32_t x, int32_t y) {
    if (lut->size >= GENERIC_LUT_MAX) return false;

    // Insert sorted by x
    int i;
    for (i = lut->size; i > 0 && lut->x[i-1] > x; i--) {
        lut->x[i] = lut->x[i-1];
        lut->y[i] = lut->y[i-1];
    }
    lut->x[i] = x;
    lut->y[i] = y;
    lut->size++;

    return true;
}

int32_t generic_lut_lookup(GenericLUT *lut, int32_t x) {
    if (lut->size == 0) return 0;
    if (lut->size == 1) return lut->y[0];

    // Handle out of range
    if (x <= lut->x[0]) {
        if (lut->extrapolate && lut->size >= 2) {
            // Linear extrapolation
            int32_t dx = lut->x[1] - lut->x[0];
            int32_t dy = lut->y[1] - lut->y[0];
            return lut->y[0] + (dy * (x - lut->x[0])) / dx;
        }
        return lut->y[0];
    }

    if (x >= lut->x[lut->size - 1]) {
        if (lut->extrapolate && lut->size >= 2) {
            int32_t dx = lut->x[lut->size - 1] - lut->x[lut->size - 2];
            int32_t dy = lut->y[lut->size - 1] - lut->y[lut->size - 2];
            return lut->y[lut->size - 1] + (dy * (x - lut->x[lut->size - 1])) / dx;
        }
        return lut->y[lut->size - 1];
    }

    // Binary search for segment
    uint8_t lo = 0, hi = lut->size - 1;
    while (lo < hi - 1) {
        uint8_t mid = (lo + hi) / 2;
        if (x < lut->x[mid]) {
            hi = mid;
        } else {
            lo = mid;
        }
    }

    // Linear interpolation
    int32_t x0 = lut->x[lo];
    int32_t x1 = lut->x[hi];
    int32_t y0 = lut->y[lo];
    int32_t y1 = lut->y[hi];

    return y0 + ((y1 - y0) * (x - x0)) / (x1 - x0);
}


/* ============================================
 * FAST INVERSE SQUARE ROOT (Quake)
 * ============================================ */

float fast_inv_sqrt(float x) {
    union {
        float f;
        uint32_t i;
    } conv;

    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5f - (x * 0.5f * conv.f * conv.f);
    return conv.f;
}


/* ============================================
 * BIT REVERSAL TABLE (for FFT)
 * ============================================ */

// 8-bit reversal table
static const uint8_t bit_reverse_8[256] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

uint8_t reverse_bits_8(uint8_t x) {
    return bit_reverse_8[x];
}

uint16_t reverse_bits_16(uint16_t x) {
    return (bit_reverse_8[x & 0xFF] << 8) | bit_reverse_8[x >> 8];
}

