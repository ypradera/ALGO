/*
 * STATIC HASHMAP PATTERN
 * Use: Two sum, prefix sum lookups, frequency counting
 * Time: O(1) avg, O(n) worst case
 * Space: O(HASHMAP_SIZE) static allocation
 *
 * Embedded-friendly: no malloc, fixed size, open addressing
 */

#include <string.h>

#define HASHMAP_SIZE 100003  // prime number for better distribution

// === BASIC HASHMAP (long long keys, int values) ===

typedef struct {
    long long key;
    int value;
    int occupied;
} HashEntry;

typedef struct {
    HashEntry entries[HASHMAP_SIZE];
    int size;
} HashMap;

void hashmap_init(HashMap *map) {
    map->size = 0;
    memset(map->entries, 0, sizeof(map->entries));
}

static unsigned int hash_func(long long key) {
    // handle negative keys
    unsigned long long k = (unsigned long long)key;
    return (unsigned int)(k % HASHMAP_SIZE);
}

int hashmap_get(HashMap *map, long long key, int default_val) {
    unsigned int idx = hash_func(key);
    int probes = 0;

    while (map->entries[idx].occupied && probes < HASHMAP_SIZE) {
        if (map->entries[idx].key == key) {
            return map->entries[idx].value;
        }
        idx = (idx + 1) % HASHMAP_SIZE;
        probes++;
    }

    return default_val;
}

int hashmap_contains(HashMap *map, long long key) {
    unsigned int idx = hash_func(key);
    int probes = 0;

    while (map->entries[idx].occupied && probes < HASHMAP_SIZE) {
        if (map->entries[idx].key == key) {
            return 1;
        }
        idx = (idx + 1) % HASHMAP_SIZE;
        probes++;
    }

    return 0;
}

void hashmap_put(HashMap *map, long long key, int value) {
    unsigned int idx = hash_func(key);

    while (map->entries[idx].occupied) {
        if (map->entries[idx].key == key) {
            map->entries[idx].value = value;
            return;
        }
        idx = (idx + 1) % HASHMAP_SIZE;
    }

    map->entries[idx].key = key;
    map->entries[idx].value = value;
    map->entries[idx].occupied = 1;
    map->size++;
}

void hashmap_increment(HashMap *map, long long key) {
    int val = hashmap_get(map, key, 0);
    hashmap_put(map, key, val + 1);
}

void hashmap_decrement(HashMap *map, long long key) {
    int val = hashmap_get(map, key, 0);
    hashmap_put(map, key, val - 1);
}

// === INTEGER HASHMAP (int keys, int values) ===
// Smaller footprint for problems with smaller key range

typedef struct {
    int key;
    int value;
    int occupied;
} IntHashEntry;

typedef struct {
    IntHashEntry entries[HASHMAP_SIZE];
    int size;
} IntHashMap;

void int_hashmap_init(IntHashMap *map) {
    map->size = 0;
    memset(map->entries, 0, sizeof(map->entries));
}

static unsigned int int_hash_func(int key) {
    unsigned int k = (unsigned int)key;
    return k % HASHMAP_SIZE;
}

int int_hashmap_get(IntHashMap *map, int key, int default_val) {
    unsigned int idx = int_hash_func(key);
    int probes = 0;

    while (map->entries[idx].occupied && probes < HASHMAP_SIZE) {
        if (map->entries[idx].key == key) {
            return map->entries[idx].value;
        }
        idx = (idx + 1) % HASHMAP_SIZE;
        probes++;
    }

    return default_val;
}

void int_hashmap_put(IntHashMap *map, int key, int value) {
    unsigned int idx = int_hash_func(key);

    while (map->entries[idx].occupied) {
        if (map->entries[idx].key == key) {
            map->entries[idx].value = value;
            return;
        }
        idx = (idx + 1) % HASHMAP_SIZE;
    }

    map->entries[idx].key = key;
    map->entries[idx].value = value;
    map->entries[idx].occupied = 1;
    map->size++;
}

void int_hashmap_increment(IntHashMap *map, int key) {
    int val = int_hashmap_get(map, key, 0);
    int_hashmap_put(map, key, val + 1);
}

// === TWO SUM USING HASHMAP ===
// Find indices i, j where arr[i] + arr[j] = target
int two_sum(int *arr, int n, int target, int *idx1, int *idx2) {
    static IntHashMap map;
    int_hashmap_init(&map);

    for (int i = 0; i < n; i++) {
        int complement = target - arr[i];
        int j = int_hashmap_get(&map, complement, -1);

        if (j != -1) {
            *idx1 = j;
            *idx2 = i;
            return 1;  // found
        }

        int_hashmap_put(&map, arr[i], i);
    }

    return 0;  // not found
}

// === COUNT PAIRS WITH SUM ===
int count_pairs_with_sum(int *arr, int n, int target) {
    static IntHashMap map;
    int_hashmap_init(&map);

    int count = 0;

    for (int i = 0; i < n; i++) {
        int complement = target - arr[i];
        count += int_hashmap_get(&map, complement, 0);
        int_hashmap_increment(&map, arr[i]);
    }

    return count;
}

// === SUBARRAY SUM EQUALS K ===
// Count subarrays with sum exactly k
int count_subarrays_sum_k(int *arr, int n, int k) {
    static HashMap map;
    hashmap_init(&map);

    int count = 0;
    long long prefix_sum = 0;

    // prefix_sum[0] = 0 occurs once (empty prefix)
    hashmap_put(&map, 0, 1);

    for (int i = 0; i < n; i++) {
        prefix_sum += arr[i];

        // if (prefix_sum - k) exists, we found subarrays ending at i
        count += hashmap_get(&map, prefix_sum - k, 0);

        // record current prefix sum
        hashmap_increment(&map, prefix_sum);
    }

    return count;
}

// === LONGEST SUBARRAY WITH SUM K ===
int longest_subarray_sum_k(int *arr, int n, int k) {
    static HashMap map;
    hashmap_init(&map);

    int max_len = 0;
    long long prefix_sum = 0;

    for (int i = 0; i < n; i++) {
        prefix_sum += arr[i];

        if (prefix_sum == k) {
            max_len = i + 1;
        } else if (hashmap_contains(&map, prefix_sum - k)) {
            int len = i - hashmap_get(&map, prefix_sum - k, 0);
            if (len > max_len) max_len = len;
        }

        // only store first occurrence (for longest subarray)
        if (!hashmap_contains(&map, prefix_sum)) {
            hashmap_put(&map, prefix_sum, i);
        }
    }

    return max_len;
}

// === CONTIGUOUS ARRAY (equal 0s and 1s) ===
// Treat 0 as -1, find longest subarray with sum 0
int find_max_equal_01(int *arr, int n) {
    static HashMap map;
    hashmap_init(&map);

    int max_len = 0;
    int sum = 0;

    // sum 0 at index -1 (before array starts)
    hashmap_put(&map, 0, -1);

    for (int i = 0; i < n; i++) {
        sum += (arr[i] == 0) ? -1 : 1;

        if (hashmap_contains(&map, sum)) {
            int len = i - hashmap_get(&map, sum, 0);
            if (len > max_len) max_len = len;
        } else {
            hashmap_put(&map, sum, i);
        }
    }

    return max_len;
}

// === FREQUENCY MAP ===
// Count occurrences of each element
void build_frequency_map(int *arr, int n, IntHashMap *map) {
    int_hashmap_init(map);
    for (int i = 0; i < n; i++) {
        int_hashmap_increment(map, arr[i]);
    }
}
