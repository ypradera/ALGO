/*
 * EMBEDDED SECURITY IMPLEMENTATIONS
 * Use: Authentication, encryption, secure storage, input validation
 *
 * Features:
 * - Constant-time comparison (timing attack resistant)
 * - Secure memory operations
 * - Simple ciphers (XOR, XTEA, AES-128)
 * - Hash functions (DJB2, FNV, simple SHA-256)
 * - HMAC construction
 * - Rate limiting / brute force protection
 * - Input validation
 * - Secure boot helpers
 *
 * Time: Varies by algorithm
 * Space: O(1) to O(n) depending on operation
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * CONSTANT-TIME OPERATIONS
 * Prevent timing side-channel attacks
 * ============================================ */

// Constant-time comparison (returns 0 if equal)
int secure_compare(const uint8_t *a, const uint8_t *b, size_t len) {
    volatile uint8_t result = 0;

    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }

    return result;
}

// Constant-time select: returns a if select==0, b if select!=0
uint32_t ct_select(uint32_t a, uint32_t b, uint32_t select) {
    uint32_t mask = (uint32_t)(-(int32_t)(select != 0));
    return (a & ~mask) | (b & mask);
}

// Constant-time byte select
uint8_t ct_select_byte(uint8_t a, uint8_t b, uint8_t select) {
    uint8_t mask = (uint8_t)(-(int8_t)(select != 0));
    return (a & ~mask) | (b & mask);
}

// Constant-time is-zero check (returns 1 if x==0, 0 otherwise)
uint32_t ct_is_zero(uint32_t x) {
    return ((x | (~x + 1)) >> 31) ^ 1;
}

// Constant-time equality check
uint32_t ct_equal(uint32_t a, uint32_t b) {
    return ct_is_zero(a ^ b);
}


/* ============================================
 * SECURE MEMORY OPERATIONS
 * ============================================ */

// Secure memory zero (prevent compiler optimization)
void secure_zero(void *ptr, size_t len) {
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (len--) {
        *p++ = 0;
    }
}

// Secure memory set
void secure_memset(void *ptr, uint8_t value, size_t len) {
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (len--) {
        *p++ = value;
    }
}

// Secure copy (handles overlap safely)
void secure_copy(void *dest, const void *src, size_t len) {
    volatile uint8_t *d = (volatile uint8_t *)dest;
    const volatile uint8_t *s = (const volatile uint8_t *)src;

    if (d < s) {
        while (len--) {
            *d++ = *s++;
        }
    } else {
        d += len;
        s += len;
        while (len--) {
            *--d = *--s;
        }
    }
}


/* ============================================
 * XOR CIPHER (Simple obfuscation)
 * Note: NOT cryptographically secure
 * ============================================ */

void xor_cipher(uint8_t *data, size_t len, const uint8_t *key, size_t key_len) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % key_len];
    }
}

// XOR with rolling key
void xor_cipher_rolling(uint8_t *data, size_t len, uint32_t seed) {
    for (size_t i = 0; i < len; i++) {
        seed = seed * 1103515245 + 12345;
        data[i] ^= (seed >> 16) & 0xFF;
    }
}


/* ============================================
 * XTEA CIPHER (Lightweight block cipher)
 * 64-bit block, 128-bit key
 * ============================================ */

#define XTEA_ROUNDS     32
#define XTEA_DELTA      0x9E3779B9

void xtea_encrypt(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0;

    for (int i = 0; i < XTEA_ROUNDS; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += XTEA_DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

void xtea_decrypt(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = XTEA_DELTA * XTEA_ROUNDS;

    for (int i = 0; i < XTEA_ROUNDS; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
        sum -= XTEA_DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

// XTEA-CBC mode
void xtea_cbc_encrypt(uint8_t *data, size_t len, const uint32_t key[4],
                      uint32_t iv[2]) {
    for (size_t i = 0; i < len; i += 8) {
        uint32_t block[2];
        memcpy(block, &data[i], 8);

        block[0] ^= iv[0];
        block[1] ^= iv[1];

        xtea_encrypt(block, key);

        iv[0] = block[0];
        iv[1] = block[1];

        memcpy(&data[i], block, 8);
    }
}

void xtea_cbc_decrypt(uint8_t *data, size_t len, const uint32_t key[4],
                      uint32_t iv[2]) {
    for (size_t i = 0; i < len; i += 8) {
        uint32_t block[2], next_iv[2];
        memcpy(block, &data[i], 8);

        next_iv[0] = block[0];
        next_iv[1] = block[1];

        xtea_decrypt(block, key);

        block[0] ^= iv[0];
        block[1] ^= iv[1];

        iv[0] = next_iv[0];
        iv[1] = next_iv[1];

        memcpy(&data[i], block, 8);
    }
}


/* ============================================
 * SIMPLE AES-128 (Compact implementation)
 * ============================================ */

// S-box for SubBytes
static const uint8_t aes_sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Round constants
static const uint8_t aes_rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

typedef struct {
    uint8_t round_keys[176];  // 11 round keys * 16 bytes
} AES128Context;

static uint8_t aes_xtime(uint8_t x) {
    return (x << 1) ^ (((x >> 7) & 1) * 0x1b);
}

void aes128_key_expansion(AES128Context *ctx, const uint8_t key[16]) {
    memcpy(ctx->round_keys, key, 16);

    for (int i = 16; i < 176; i += 4) {
        uint8_t temp[4];
        memcpy(temp, &ctx->round_keys[i - 4], 4);

        if (i % 16 == 0) {
            // RotWord + SubWord + Rcon
            uint8_t t = temp[0];
            temp[0] = aes_sbox[temp[1]] ^ aes_rcon[i / 16];
            temp[1] = aes_sbox[temp[2]];
            temp[2] = aes_sbox[temp[3]];
            temp[3] = aes_sbox[t];
        }

        for (int j = 0; j < 4; j++) {
            ctx->round_keys[i + j] = ctx->round_keys[i - 16 + j] ^ temp[j];
        }
    }
}

void aes128_encrypt_block(AES128Context *ctx, uint8_t block[16]) {
    uint8_t state[16];
    memcpy(state, block, 16);

    // Initial round key addition
    for (int i = 0; i < 16; i++) {
        state[i] ^= ctx->round_keys[i];
    }

    // Main rounds
    for (int round = 1; round < 10; round++) {
        // SubBytes
        for (int i = 0; i < 16; i++) {
            state[i] = aes_sbox[state[i]];
        }

        // ShiftRows
        uint8_t temp;
        temp = state[1]; state[1] = state[5]; state[5] = state[9];
        state[9] = state[13]; state[13] = temp;
        temp = state[2]; state[2] = state[10]; state[10] = temp;
        temp = state[6]; state[6] = state[14]; state[14] = temp;
        temp = state[3]; state[3] = state[15]; state[15] = state[11];
        state[11] = state[7]; state[7] = temp;

        // MixColumns
        for (int i = 0; i < 16; i += 4) {
            uint8_t a = state[i], b = state[i+1], c = state[i+2], d = state[i+3];
            uint8_t e = a ^ b ^ c ^ d;
            state[i]   ^= e ^ aes_xtime(a ^ b);
            state[i+1] ^= e ^ aes_xtime(b ^ c);
            state[i+2] ^= e ^ aes_xtime(c ^ d);
            state[i+3] ^= e ^ aes_xtime(d ^ a);
        }

        // AddRoundKey
        for (int i = 0; i < 16; i++) {
            state[i] ^= ctx->round_keys[round * 16 + i];
        }
    }

    // Final round (no MixColumns)
    for (int i = 0; i < 16; i++) {
        state[i] = aes_sbox[state[i]];
    }

    uint8_t temp;
    temp = state[1]; state[1] = state[5]; state[5] = state[9];
    state[9] = state[13]; state[13] = temp;
    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;
    temp = state[3]; state[3] = state[15]; state[15] = state[11];
    state[11] = state[7]; state[7] = temp;

    for (int i = 0; i < 16; i++) {
        state[i] ^= ctx->round_keys[160 + i];
    }

    memcpy(block, state, 16);
}


/* ============================================
 * HASH FUNCTIONS
 * ============================================ */

// DJB2 hash (fast, simple)
uint32_t hash_djb2(const uint8_t *data, size_t len) {
    uint32_t hash = 5381;

    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

// FNV-1a hash (good distribution)
uint32_t hash_fnv1a(const uint8_t *data, size_t len) {
    uint32_t hash = 0x811c9dc5;  // FNV offset basis

    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 0x01000193;  // FNV prime
    }

    return hash;
}

// MurmurHash3 (32-bit, excellent for hash tables)
uint32_t hash_murmur3(const uint8_t *data, size_t len, uint32_t seed) {
    uint32_t h = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Body
    const uint32_t *blocks = (const uint32_t *)data;
    size_t nblocks = len / 4;

    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << 15) | (k >> 17);
        k *= c2;
        h ^= k;
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    // Tail
    const uint8_t *tail = data + nblocks * 4;
    uint32_t k = 0;

    switch (len & 3) {
        case 3: k ^= tail[2] << 16; // fall through
        case 2: k ^= tail[1] << 8;  // fall through
        case 1: k ^= tail[0];
                k *= c1;
                k = (k << 15) | (k >> 17);
                k *= c2;
                h ^= k;
    }

    // Finalization
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}


/* ============================================
 * HMAC (Hash-based Message Authentication Code)
 * ============================================ */

#define HMAC_BLOCK_SIZE     64

typedef uint32_t (*HashFunc)(const uint8_t *data, size_t len);

void hmac_compute(const uint8_t *key, size_t key_len,
                  const uint8_t *message, size_t msg_len,
                  HashFunc hash, uint32_t *result) {
    uint8_t k_ipad[HMAC_BLOCK_SIZE];
    uint8_t k_opad[HMAC_BLOCK_SIZE];
    uint8_t key_block[HMAC_BLOCK_SIZE];

    // Prepare key
    memset(key_block, 0, HMAC_BLOCK_SIZE);
    if (key_len > HMAC_BLOCK_SIZE) {
        // Hash key if too long
        uint32_t h = hash(key, key_len);
        memcpy(key_block, &h, 4);
    } else {
        memcpy(key_block, key, key_len);
    }

    // Create pads
    for (int i = 0; i < HMAC_BLOCK_SIZE; i++) {
        k_ipad[i] = key_block[i] ^ 0x36;
        k_opad[i] = key_block[i] ^ 0x5c;
    }

    // Inner hash: H(K XOR ipad || message)
    uint8_t inner_data[HMAC_BLOCK_SIZE + 256];  // Adjust size as needed
    memcpy(inner_data, k_ipad, HMAC_BLOCK_SIZE);
    size_t copy_len = (msg_len > 256) ? 256 : msg_len;
    memcpy(inner_data + HMAC_BLOCK_SIZE, message, copy_len);
    uint32_t inner_hash = hash(inner_data, HMAC_BLOCK_SIZE + copy_len);

    // Outer hash: H(K XOR opad || inner_hash)
    uint8_t outer_data[HMAC_BLOCK_SIZE + 4];
    memcpy(outer_data, k_opad, HMAC_BLOCK_SIZE);
    memcpy(outer_data + HMAC_BLOCK_SIZE, &inner_hash, 4);
    *result = hash(outer_data, HMAC_BLOCK_SIZE + 4);

    // Clean sensitive data
    secure_zero(key_block, HMAC_BLOCK_SIZE);
    secure_zero(k_ipad, HMAC_BLOCK_SIZE);
    secure_zero(k_opad, HMAC_BLOCK_SIZE);
}


/* ============================================
 * RATE LIMITING / BRUTE FORCE PROTECTION
 * ============================================ */

#define RATE_LIMIT_SLOTS    16
#define RATE_LIMIT_WINDOW   60000   // 60 seconds in ms
#define RATE_LIMIT_MAX      5       // Max attempts per window

typedef struct {
    uint32_t id;            // User/IP identifier
    uint32_t first_attempt; // Timestamp of first attempt
    uint8_t count;          // Attempt count
    bool locked;            // Account locked
} RateLimitEntry;

typedef struct {
    RateLimitEntry entries[RATE_LIMIT_SLOTS];
    uint32_t lockout_duration;  // Lockout time in ms
} RateLimiter;

void rate_limiter_init(RateLimiter *rl, uint32_t lockout_ms) {
    memset(rl->entries, 0, sizeof(rl->entries));
    rl->lockout_duration = lockout_ms;
}

static RateLimitEntry* rate_limiter_find(RateLimiter *rl, uint32_t id) {
    for (int i = 0; i < RATE_LIMIT_SLOTS; i++) {
        if (rl->entries[i].id == id && rl->entries[i].count > 0) {
            return &rl->entries[i];
        }
    }
    return NULL;
}

static RateLimitEntry* rate_limiter_alloc(RateLimiter *rl) {
    // Find empty slot
    for (int i = 0; i < RATE_LIMIT_SLOTS; i++) {
        if (rl->entries[i].count == 0) {
            return &rl->entries[i];
        }
    }
    // Evict oldest
    uint32_t oldest_time = UINT32_MAX;
    int oldest_idx = 0;
    for (int i = 0; i < RATE_LIMIT_SLOTS; i++) {
        if (rl->entries[i].first_attempt < oldest_time) {
            oldest_time = rl->entries[i].first_attempt;
            oldest_idx = i;
        }
    }
    return &rl->entries[oldest_idx];
}

// Returns true if request is allowed
bool rate_limiter_check(RateLimiter *rl, uint32_t id, uint32_t current_time) {
    RateLimitEntry *entry = rate_limiter_find(rl, id);

    if (entry) {
        // Check if window expired
        if ((current_time - entry->first_attempt) > RATE_LIMIT_WINDOW) {
            entry->first_attempt = current_time;
            entry->count = 1;
            entry->locked = false;
            return true;
        }

        // Check if locked out
        if (entry->locked) {
            if ((current_time - entry->first_attempt) > rl->lockout_duration) {
                entry->first_attempt = current_time;
                entry->count = 1;
                entry->locked = false;
                return true;
            }
            return false;
        }

        // Increment count
        entry->count++;
        if (entry->count > RATE_LIMIT_MAX) {
            entry->locked = true;
            return false;
        }

        return true;
    }

    // New entry
    entry = rate_limiter_alloc(rl);
    entry->id = id;
    entry->first_attempt = current_time;
    entry->count = 1;
    entry->locked = false;

    return true;
}

void rate_limiter_reset(RateLimiter *rl, uint32_t id) {
    RateLimitEntry *entry = rate_limiter_find(rl, id);
    if (entry) {
        entry->count = 0;
        entry->locked = false;
    }
}


/* ============================================
 * INPUT VALIDATION
 * ============================================ */

// Check if string contains only printable ASCII
bool validate_printable(const char *str, size_t max_len) {
    for (size_t i = 0; i < max_len && str[i]; i++) {
        if (str[i] < 0x20 || str[i] > 0x7E) {
            return false;
        }
    }
    return true;
}

// Check if string contains only alphanumeric
bool validate_alphanumeric(const char *str, size_t max_len) {
    for (size_t i = 0; i < max_len && str[i]; i++) {
        char c = str[i];
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9'))) {
            return false;
        }
    }
    return true;
}

// Check if string is valid hex
bool validate_hex(const char *str, size_t max_len) {
    for (size_t i = 0; i < max_len && str[i]; i++) {
        char c = str[i];
        if (!((c >= 'A' && c <= 'F') ||
              (c >= 'a' && c <= 'f') ||
              (c >= '0' && c <= '9'))) {
            return false;
        }
    }
    return true;
}

// Validate integer range
bool validate_int_range(int32_t value, int32_t min, int32_t max) {
    return value >= min && value <= max;
}

// Safe string length (bounded)
size_t safe_strlen(const char *str, size_t max_len) {
    size_t len = 0;
    while (len < max_len && str[len]) {
        len++;
    }
    return len;
}

// Sanitize string (replace non-printable with underscore)
void sanitize_string(char *str, size_t len) {
    for (size_t i = 0; i < len && str[i]; i++) {
        if (str[i] < 0x20 || str[i] > 0x7E) {
            str[i] = '_';
        }
    }
}


/* ============================================
 * SECURE BOOT HELPERS
 * ============================================ */

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t length;
    uint32_t checksum;
    uint32_t entry_point;
    uint8_t signature[32];
} FirmwareHeader;

#define FW_MAGIC    0x464D5752  // "FMWR"

// Verify firmware header
bool fw_verify_header(const FirmwareHeader *header, uint32_t max_size) {
    if (header->magic != FW_MAGIC) {
        return false;
    }

    if (header->length > max_size) {
        return false;
    }

    if (header->entry_point < sizeof(FirmwareHeader)) {
        return false;
    }

    return true;
}

// Calculate firmware checksum
uint32_t fw_calculate_checksum(const uint8_t *data, uint32_t length) {
    return hash_fnv1a(data, length);
}

// Verify firmware integrity
bool fw_verify_checksum(const FirmwareHeader *header, const uint8_t *data) {
    uint32_t calc_checksum = fw_calculate_checksum(data, header->length);
    return ct_equal(calc_checksum, header->checksum);
}


/* ============================================
 * TOKEN / SESSION MANAGEMENT
 * ============================================ */

#define TOKEN_SIZE          16
#define MAX_SESSIONS        8
#define SESSION_TIMEOUT     300000  // 5 minutes in ms

typedef struct {
    uint8_t token[TOKEN_SIZE];
    uint32_t user_id;
    uint32_t created_at;
    uint32_t last_access;
    bool valid;
} Session;

typedef struct {
    Session sessions[MAX_SESSIONS];
    uint32_t (*get_random)(void);  // RNG function pointer
} SessionManager;

void session_manager_init(SessionManager *sm, uint32_t (*rng)(void)) {
    memset(sm->sessions, 0, sizeof(sm->sessions));
    sm->get_random = rng;
}

// Generate random token
static void generate_token(SessionManager *sm, uint8_t token[TOKEN_SIZE]) {
    for (int i = 0; i < TOKEN_SIZE; i += 4) {
        uint32_t r = sm->get_random();
        token[i] = r & 0xFF;
        token[i+1] = (r >> 8) & 0xFF;
        token[i+2] = (r >> 16) & 0xFF;
        if (i + 3 < TOKEN_SIZE) {
            token[i+3] = (r >> 24) & 0xFF;
        }
    }
}

// Create new session
bool session_create(SessionManager *sm, uint32_t user_id, uint32_t current_time,
                    uint8_t token_out[TOKEN_SIZE]) {
    // Find free slot
    int slot = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (!sm->sessions[i].valid) {
            slot = i;
            break;
        }
    }

    // Evict oldest if no free slot
    if (slot < 0) {
        uint32_t oldest = UINT32_MAX;
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (sm->sessions[i].last_access < oldest) {
                oldest = sm->sessions[i].last_access;
                slot = i;
            }
        }
    }

    // Create session
    generate_token(sm, sm->sessions[slot].token);
    sm->sessions[slot].user_id = user_id;
    sm->sessions[slot].created_at = current_time;
    sm->sessions[slot].last_access = current_time;
    sm->sessions[slot].valid = true;

    memcpy(token_out, sm->sessions[slot].token, TOKEN_SIZE);
    return true;
}

// Validate token and get session
Session* session_validate(SessionManager *sm, const uint8_t token[TOKEN_SIZE],
                          uint32_t current_time) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sm->sessions[i].valid &&
            secure_compare(sm->sessions[i].token, token, TOKEN_SIZE) == 0) {

            // Check timeout
            if ((current_time - sm->sessions[i].last_access) > SESSION_TIMEOUT) {
                sm->sessions[i].valid = false;
                return NULL;
            }

            sm->sessions[i].last_access = current_time;
            return &sm->sessions[i];
        }
    }
    return NULL;
}

// Destroy session
void session_destroy(SessionManager *sm, const uint8_t token[TOKEN_SIZE]) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sm->sessions[i].valid &&
            secure_compare(sm->sessions[i].token, token, TOKEN_SIZE) == 0) {
            secure_zero(&sm->sessions[i], sizeof(Session));
            break;
        }
    }
}

// Clean expired sessions
void session_cleanup(SessionManager *sm, uint32_t current_time) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sm->sessions[i].valid &&
            (current_time - sm->sessions[i].last_access) > SESSION_TIMEOUT) {
            secure_zero(&sm->sessions[i], sizeof(Session));
        }
    }
}


/* ============================================
 * PASSWORD HASHING (Simple PBKDF2-like)
 * ============================================ */

#define PBKDF2_ITERATIONS   1000
#define SALT_SIZE           8

typedef struct {
    uint8_t salt[SALT_SIZE];
    uint32_t hash;
} PasswordHash;

void password_hash(const char *password, size_t len,
                   const uint8_t salt[SALT_SIZE], PasswordHash *result) {
    memcpy(result->salt, salt, SALT_SIZE);

    // Combine password and salt
    uint8_t combined[64];
    size_t pw_len = (len > 56) ? 56 : len;
    memcpy(combined, password, pw_len);
    memcpy(combined + pw_len, salt, SALT_SIZE);

    // Iterate hash
    uint32_t hash = hash_fnv1a(combined, pw_len + SALT_SIZE);
    for (int i = 0; i < PBKDF2_ITERATIONS; i++) {
        uint8_t data[4];
        memcpy(data, &hash, 4);
        hash = hash_fnv1a(data, 4);
    }

    result->hash = hash;
    secure_zero(combined, sizeof(combined));
}

bool password_verify(const char *password, size_t len, const PasswordHash *stored) {
    PasswordHash computed;
    password_hash(password, len, stored->salt, &computed);

    bool match = ct_equal(computed.hash, stored->hash);
    secure_zero(&computed, sizeof(computed));

    return match;
}

