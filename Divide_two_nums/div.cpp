#include <cstdint>
#include <limits>
#include <iostream>
#include <array>

using namespace std;

int divide_i32(int dividend, int divisor)
{
    // Convert inputs to explicit 32-bit signed
    int32_t dvd = static_cast<int32_t>(dividend);
    int32_t dvs = static_cast<int32_t>(divisor);

    constexpr int32_t I32_MIN = numeric_limits<int32_t>::min();
    constexpr int32_t I32_MAX = numeric_limits<int32_t>::max();

    // Overflow case: INT_MIN / -1 -> INT_MAX
    if (dvd == I32_MIN && dvs == -1) {
        return static_cast<int>(I32_MAX);
    }

    // Result sign: negative if exactly one input is negative
    int32_t sign = ((dvd < 0) ^ (dvs < 0)) ? -1 : 1;

    // Work in negative space to avoid abs(INT_MIN) overflow
    int32_t a_dividend = (dvd > 0) ? -dvd : dvd; // <= 0
    int32_t a_divisor  = (dvs > 0) ? -dvs : dvs; // <= 0

    int32_t quotient = 0;

    while (a_dividend <= a_divisor) {
        int32_t temp_divisor = a_divisor;
        uint32_t count = 1u;

        // Safe doubling guard for int32
        const int32_t half_min = I32_MIN / 2; // -1073741824

        while ((temp_divisor >= half_min) && (temp_divisor + temp_divisor >= a_dividend)) {
            temp_divisor += temp_divisor; // double (still negative)
            count <<= 1;                  // safe: unsigned
        }

        quotient -= static_cast<int32_t>(count);
        a_dividend -= temp_divisor;
    }

    int32_t result = (sign == 1) ? -quotient : quotient;
    return static_cast<int>(result);
}

int main()
{
    constexpr int32_t I32_MIN = numeric_limits<int32_t>::min();
    constexpr int32_t I32_MAX = numeric_limits<int32_t>::max();

    array<int32_t, 8> dividend = { 10, 7, -10, -7, I32_MIN, I32_MAX, I32_MIN, (int32_t)(I32_MIN + 1) };
    array<int32_t, 8> divisor  = {  3, -3,   3, -3,     -1,      1,  I32_MIN, I32_MAX };

    for (size_t i = 0; i < dividend.size(); ++i) {
        int result = divide_i32((int)dividend[i], (int)divisor[i]);
        cout << (i + 1) << " - divide(" << dividend[i] << ", " << divisor[i]
             << ") = " << result << "\n";
    }
    return 0;
}
