/*
 * BIT MANIPULATION PATTERN
 * Use: Flags, masks, XOR tricks, optimization
 * Time: O(1) for basic ops, Space: O(1)
 */

#include <vector>
#include <cstdint>

using namespace std;

// === BASIC BIT OPERATIONS ===
inline int getBit(int num, int i) {
    return (num >> i) & 1;
}

inline int setBit(int num, int i) {
    return num | (1 << i);
}

inline int clearBit(int num, int i) {
    return num & ~(1 << i);
}

inline int toggleBit(int num, int i) {
    return num ^ (1 << i);
}

inline int updateBit(int num, int i, int val) {
    int mask = ~(1 << i);
    return (num & mask) | (val << i);
}

// === COUNT BITS ===
int countOnes(int num) {
    int count = 0;
    while (num) {
        num &= num - 1;  // clear lowest set bit
        count++;
    }
    return count;
}

int countOnesBuiltin(int num) {
    return __builtin_popcount(num);
}

// === POWER OF TWO ===
bool isPowerOfTwo(int num) {
    return num > 0 && (num & (num - 1)) == 0;
}

int nextPowerOfTwo(int num) {
    if (num <= 1) return 1;
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    return num + 1;
}

// === LOWEST/HIGHEST BITS ===
inline int lowestSetBit(int num) {
    return num & (-num);
}

inline int clearLowestSetBit(int num) {
    return num & (num - 1);
}

int highestSetBitPos(int num) {
    if (num == 0) return -1;
    return 31 - __builtin_clz(num);
}

// === XOR TRICKS ===
int singleNumber(vector<int>& arr) {
    int result = 0;
    for (int num : arr) {
        result ^= num;
    }
    return result;
}

vector<int> singleNumberIII(vector<int>& arr) {
    unsigned int xorAll = 0;
    for (int num : arr) {
        xorAll ^= num;
    }

    // find rightmost set bit
    unsigned int diffBit = xorAll & (-xorAll);

    int a = 0, b = 0;
    for (int num : arr) {
        if (num & diffBit) {
            a ^= num;
        } else {
            b ^= num;
        }
    }

    return {a, b};
}

int missingNumber(vector<int>& arr) {
    int n = arr.size();
    int result = n;

    for (int i = 0; i < n; i++) {
        result ^= i ^ arr[i];
    }

    return result;
}

void swapWithoutTemp(int& a, int& b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

// === BIT MASKS ===
inline int createMask(int start, int end) {
    return ((1 << (end - start + 1)) - 1) << start;
}

inline int extractBits(int num, int start, int length) {
    int mask = (1 << length) - 1;
    return (num >> start) & mask;
}

inline int insertBits(int num, int insert, int start, int length) {
    int mask = ((1 << length) - 1) << start;
    num &= ~mask;
    return num | (insert << start);
}

// === REVERSE BITS ===
uint32_t reverseBits(uint32_t num) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (num & 1);
        num >>= 1;
    }
    return result;
}

// === SUBSET ENUMERATION ===
vector<int> enumerateSubsets(int mask) {
    vector<int> subsets;
    int subset = mask;
    do {
        subsets.push_back(subset);
        subset = (subset - 1) & mask;
    } while (subset != mask);
    return subsets;
}

int countSubsets(int mask) {
    return 1 << __builtin_popcount(mask);
}

// === GRAY CODE ===
inline int binaryToGray(int num) {
    return num ^ (num >> 1);
}

int grayToBinary(int gray) {
    int binary = 0;
    while (gray) {
        binary ^= gray;
        gray >>= 1;
    }
    return binary;
}

vector<int> grayCodeSequence(int n) {
    vector<int> result;
    for (int i = 0; i < (1 << n); i++) {
        result.push_back(i ^ (i >> 1));
    }
    return result;
}

// === EMBEDDED: REGISTER OPERATIONS ===
inline uint32_t readModifyWrite(uint32_t reg, uint32_t mask, uint32_t value) {
    return (reg & ~mask) | (value & mask);
}

inline uint32_t setField(uint32_t reg, uint32_t fieldMask, int fieldPos, uint32_t value) {
    reg &= ~fieldMask;
    reg |= (value << fieldPos) & fieldMask;
    return reg;
}

inline uint32_t getField(uint32_t reg, uint32_t fieldMask, int fieldPos) {
    return (reg & fieldMask) >> fieldPos;
}

// === PARITY ===
int parity(int num) {
    num ^= num >> 16;
    num ^= num >> 8;
    num ^= num >> 4;
    num ^= num >> 2;
    num ^= num >> 1;
    return num & 1;
}

// === ADD WITHOUT ARITHMETIC ===
int addBinary(int a, int b) {
    while (b) {
        int carry = a & b;
        a = a ^ b;
        b = carry << 1;
    }
    return a;
}

// === HAMMING DISTANCE ===
int hammingDistance(int x, int y) {
    return __builtin_popcount(x ^ y);
}

// === TOTAL HAMMING DISTANCE ===
int totalHammingDistance(vector<int>& nums) {
    int total = 0;
    int n = nums.size();

    for (int i = 0; i < 32; i++) {
        int ones = 0;
        for (int num : nums) {
            ones += (num >> i) & 1;
        }
        total += ones * (n - ones);
    }

    return total;
}

// === COUNTING BITS (0 to n) ===
vector<int> countBits(int n) {
    vector<int> dp(n + 1);
    for (int i = 1; i <= n; i++) {
        dp[i] = dp[i >> 1] + (i & 1);
    }
    return dp;
}

// === UTF-8 VALIDATION ===
bool validUtf8(vector<int>& data) {
    int remaining = 0;

    for (int byte : data) {
        byte &= 0xFF;

        if (remaining == 0) {
            if ((byte >> 7) == 0) {
                remaining = 0;
            } else if ((byte >> 5) == 0b110) {
                remaining = 1;
            } else if ((byte >> 4) == 0b1110) {
                remaining = 2;
            } else if ((byte >> 3) == 0b11110) {
                remaining = 3;
            } else {
                return false;
            }
        } else {
            if ((byte >> 6) != 0b10) {
                return false;
            }
            remaining--;
        }
    }

    return remaining == 0;
}
