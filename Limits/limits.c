// overflow_underflow_demo.c
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>

#if defined(__GNUC__) || defined(__clang__)
  #define HAS_BUILTIN_OVERFLOW 1
#else
  #define HAS_BUILTIN_OVERFLOW 0
#endif

// ---------- Printing helpers ----------
static void print_u32(const char *label, uint32_t v) {
    printf("%-30s = %10" PRIu32 " (0x%08" PRIX32 ")\n", label, v, v);
}

static void print_i32(const char *label, int32_t v) {
    printf("%-30s = %10" PRId32 " (0x%08" PRIX32 ")\n", label, v, (uint32_t)v);
}

static void print_u8(const char *label, unsigned char v) {
    printf("%-30s = %4u (0x%02X)\n", label, (unsigned)v, (unsigned)v);
}

static void print_s8(const char *label, signed char v) {
    printf("%-30s = %4d (0x%02X)\n", label, (int)v, (unsigned)(unsigned char)v);
}

// ---------- Safe overflow checks ----------
static bool add_i32_overflow(int32_t a, int32_t b, int32_t *out) {
#if HAS_BUILTIN_OVERFLOW
    return __builtin_add_overflow(a, b, out);
#else
    int64_t r = (int64_t)a + (int64_t)b;
    if (r > INT32_MAX || r < INT32_MIN) return true;
    *out = (int32_t)r;
    return false;
#endif
}

static bool sub_i32_overflow(int32_t a, int32_t b, int32_t *out) {
#if HAS_BUILTIN_OVERFLOW
    return __builtin_sub_overflow(a, b, out);
#else
    int64_t r = (int64_t)a - (int64_t)b;
    if (r > INT32_MAX || r < INT32_MIN) return true;
    *out = (int32_t)r;
    return false;
#endif
}

// ---------- Safe casting rules ----------
static bool can_cast_u32_to_i32(uint32_t u) {
    return u <= (uint32_t)INT32_MAX;
}

static bool can_cast_i32_to_u32_preserve_value(int32_t s) {
    return s >= 0; // only non-negative preserves numeric value
}

static bool can_cast_u8_to_s8(uint8_t u) {
    return u <= (uint8_t)SCHAR_MAX;
}

static bool can_cast_s8_to_u8_preserve_value(int8_t s) {
    return s >= 0;
}

// ---------- Demo sections ----------
static void demo_u32_wrap(void) {
    puts("\n=== uint32_t: overflow/underflow are WELL-DEFINED (wrap modulo 2^32) ===");

    uint32_t a = UINT32_MAX;
    print_u32("a", a);
    print_u32("a + 1", (uint32_t)(a + 1u));
    print_u32("a + 2", (uint32_t)(a + 2u));

    uint32_t b = 0u;
    print_u32("b", b);
    print_u32("b - 1", (uint32_t)(b - 1u));
    print_u32("b - 2", (uint32_t)(b - 2u));
}

static void demo_u8_wrap(void) {
    puts("\n=== unsigned char (8-bit typical): overflow/underflow are WELL-DEFINED (wrap modulo 2^8) ===");

    unsigned char a = (unsigned char)UCHAR_MAX;
    print_u8("a", a);
    print_u8("a + 1", (unsigned char)(a + 1u));
    print_u8("a + 2", (unsigned char)(a + 2u));

    unsigned char b = 0u;
    print_u8("b", b);
    print_u8("b - 1", (unsigned char)(b - 1u));
    print_u8("b - 2", (unsigned char)(b - 2u));
}

static void demo_i32_overflow_rules(void) {
    puts("\n=== int32_t: signed overflow is UNDEFINED BEHAVIOR in C ===");
    puts("We will NOT execute signed overflow. We'll detect it safely.");

    int32_t r;

    // Addition near INT32_MAX
    int32_t x = INT32_MAX;
    int32_t y = 1;
    print_i32("x", x);
    print_i32("y", y);

    bool ov_add = add_i32_overflow(x, y, &r);
    printf("%-30s = %s\n", "x + y overflows?", ov_add ? "YES" : "NO");
    if (!ov_add) print_i32("x + y", r);
    else {
        // Show what it *looks like* on typical two's complement machines (NOT guaranteed by C)
        int32_t typical = (int32_t)((uint32_t)x + (uint32_t)y);
        print_i32("typical 2's comp result", typical);
        puts("NOTE: That 'typical' value is not guaranteed by the C standard.");
    }

    // Subtraction near INT32_MIN
    int32_t p = INT32_MIN;
    int32_t q = 1;
    print_i32("\np", p);
    print_i32("q", q);

    bool ov_sub = sub_i32_overflow(p, q, &r);
    printf("%-30s = %s\n", "p - q overflows?", ov_sub ? "YES" : "NO");
    if (!ov_sub) print_i32("p - q", r);
    else {
        int32_t typical = (int32_t)((uint32_t)p - (uint32_t)q);
        print_i32("typical 2's comp result", typical);
        puts("NOTE: That 'typical' value is not guaranteed by the C standard.");
    }
}

static void demo_s8_overflow_rules(void) {
    puts("\n=== signed char: signed overflow is also UNDEFINED BEHAVIOR ===");
    puts("We'll show safe detection using a wider type.");

    signed char a = SCHAR_MAX;
    signed char b = 1;

    print_s8("a", a);
    print_s8("b", b);

    int16_t wide = (int16_t)a + (int16_t)b;
    bool ov = (wide > SCHAR_MAX || wide < SCHAR_MIN);
    printf("%-30s = %s\n", "a + b overflows?", ov ? "YES" : "NO");

    if (!ov) {
        signed char r = (signed char)wide;
        print_s8("a + b", r);
    } else {
        signed char typical = (signed char)((unsigned char)a + (unsigned char)b);
        print_s8("typical 2's comp result", typical);
        puts("NOTE: That 'typical' value is not guaranteed by the C standard.");
    }
}

static void demo_shifts(void) {
    puts("\n=== Shifts (>> and <<): what is defined and what is not ===");

    // Unsigned: shifts are well-defined (if shift count < width)
    uint32_t u = 1u;
    print_u32("u", u);
    print_u32("u << 1", (uint32_t)(u << 1));
    print_u32("u << 31", (uint32_t)(u << 31));
    print_u32("0x80000000 >> 1", (uint32_t)(0x80000000u >> 1)); // logical shift (zeros)
    puts("NOTE: For unsigned types, shifts are well-defined as long as shift amount < bit-width.");

    // Signed right shift of negative is implementation-defined (usually arithmetic shift)
    int32_t s = -1;
    print_i32("\ns", s);
    print_i32("s >> 1 (impl-defined)", (int32_t)(s >> 1));
    puts("NOTE: Right-shifting negative signed ints is implementation-defined (often stays -1).");
    puts("Because s is -1, and on almost all real machines int32_t is two’s complement and the CPU does an arithmetic right shift for signed integers.");
    print_i32("s << 1 (potential UB)", (int32_t)(s << 1));
    // Signed left shift can be UB if it overflows or if value is negative.
    int32_t t = 1;
    print_i32("\nt", t);
    print_i32("t << 1 (safe here)", (int32_t)(t << 1));

    // Avoid doing (INT32_MAX << 1) or (-1 << 1) here: those can be UB.
    puts("NOTE: Left-shifting signed values into overflow (or shifting a negative) is Undefined Behavior.");
    puts("Rule: For shifts, always ensure shift amount < bit-width, and for signed left shift, ensure it won't overflow.");
}

static void demo_casting_rules(void) {
    puts("\n=== Safe casting rules (preserve numeric value) ===");

    uint32_t u1 = 123u;
    uint32_t u2 = (uint32_t)INT32_MAX;
    uint32_t u3 = (uint32_t)INT32_MAX + 1u;

    print_u32("u1", u1);
    printf("%-30s = %s\n", "u1 -> int32 safe?", can_cast_u32_to_i32(u1) ? "YES" : "NO");

    print_u32("u2", u2);
    printf("%-30s = %s\n", "u2 -> int32 safe?", can_cast_u32_to_i32(u2) ? "YES" : "NO");

    print_u32("u3", u3);
    printf("%-30s = %s\n", "u3 -> int32 safe?", can_cast_u32_to_i32(u3) ? "YES" : "NO");
    if (!can_cast_u32_to_i32(u3)) {
        puts("Casting u3 to int32_t would be implementation-defined (and likely negative on 2's complement).");
    }

    int32_t s1 = 100;
    int32_t s2 = -5;
    print_i32("\ns1", s1);
    printf("%-30s = %s\n", "s1 -> uint32 preserves?", can_cast_i32_to_u32_preserve_value(s1) ? "YES" : "NO");
    print_i32("s2", s2);
    printf("%-30s = %s\n", "s2 -> uint32 preserves?", can_cast_i32_to_u32_preserve_value(s2) ? "YES" : "NO");
    if (!can_cast_i32_to_u32_preserve_value(s2)) {
        print_u32("s2 cast to uint32_t", (uint32_t)s2);
        puts("NOTE: Negative -> unsigned wraps modulo 2^32; value is NOT preserved.");
    }

    uint8_t ub = 200;
    uint8_t uc = 100;
    printf("\nuint8_t ub = %u\n", ub);
    printf("%-30s = %s\n", "ub -> signed char safe?", can_cast_u8_to_s8(ub) ? "YES" : "NO");
    printf("uint8_t uc = %u\n", uc);
    printf("%-30s = %s\n", "uc -> signed char safe?", can_cast_u8_to_s8(uc) ? "YES" : "NO");

    int8_t sb = -10;
    int8_t sc = 10;
    printf("\nint8_t sb = %d\n", sb);
    printf("%-30s = %s\n", "sb -> unsigned char preserves?", can_cast_s8_to_u8_preserve_value(sb) ? "YES" : "NO");
    printf("int8_t sc = %d\n", sc);
    printf("%-30s = %s\n", "sc -> unsigned char preserves?", can_cast_s8_to_u8_preserve_value(sc) ? "YES" : "NO");
}

int main(void) {
    demo_u32_wrap();
    demo_u8_wrap();
    demo_i32_overflow_rules();
    demo_s8_overflow_rules();
    demo_shifts();
    demo_casting_rules();
    return 0;
}
