#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool validUtf8(int* data, int dataSize) {
    int i = 0;

    while (i < dataSize) {
        unsigned b0 = (unsigned)data[i] & 0xFFu;

        //For a 1-byte character, the first bit is a 0, followed by its Unicode code.
        if (b0 < 0x80u) {
            ++i;
            continue;
        }

        int numBytes;
        if ((b0 & 0xE0u) == 0xC0u)      numBytes = 2;
        else if ((b0 & 0xF0u) == 0xE0u) numBytes = 3;
        else if ((b0 & 0xF8u) == 0xF0u) numBytes = 4;
        else return false;

        int end = i + numBytes;          // Micro-opt: compute once
        if (end > dataSize) return false;

        // Micro-opt: avoid i+j each time; walk a pointer index
        for (int k = i + 1; k < end; ++k) {
            unsigned bk = (unsigned)data[k] & 0xFFu;
            //check 0x10 so Most significant 2 bits.
            if ((bk & 0xC0u) != 0x80u) return false;
        }

        i = end;
    }

    return true;
}


int main() {
    // Example usage
    int data1[] = {197, 130, 1};
    int data2[] = {235, 140, 4};

    bool result1 = validUtf8(data1, 3); // Expected: true
    bool result2 = validUtf8(data2, 3); // Expected: false

    printf("Result 1: %s\n", result1 ? "true" : "false");
    printf("Result 2: %s\n", result2 ? "true" : "false");

    return 0;
}