#include <cstdint>
#include <iostream>
#include <vector>

class Solution {
public:
    bool validUtf8(const std::vector<int>& data) {
        int i = 0;
        const int dataSize = static_cast<int>(data.size());

        while (i < dataSize) {
            uint32_t b0 = static_cast<uint32_t>(data[i]) & 0xFFu;

            // 1-byte (ASCII): 0xxxxxxx
            if (b0 < 0x80u) {
                ++i;
                continue;
            }

            int numBytes;
            if ((b0 & 0xE0u) == 0xC0u)      numBytes = 2; // 110xxxxx
            else if ((b0 & 0xF0u) == 0xE0u) numBytes = 3; // 1110xxxx
            else if ((b0 & 0xF8u) == 0xF0u) numBytes = 4; // 11110xxx
            else return false;

            int end = i + numBytes;
            if (end > dataSize) return false;

            // Continuation bytes must be 10xxxxxx
            for (int k = i + 1; k < end; ++k) {
                uint32_t bk = static_cast<uint32_t>(data[k]) & 0xFFu;
                if ((bk & 0xC0u) != 0x80u) return false;
            }

            i = end;
        }

        return true;
    }
};

int main() {
    Solution sol;

    std::vector<int> data1 = {197, 130, 1};
    std::vector<int> data2 = {235, 140, 4};

    bool result1 = sol.validUtf8(data1); // expected true
    bool result2 = sol.validUtf8(data2); // expected false

    std::cout << "Result 1: " << (result1 ? "true" : "false") << "\n";
    std::cout << "Result 2: " << (result2 ? "true" : "false") << "\n";
    return 0;
}
