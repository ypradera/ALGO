"""
BIT MANIPULATION PATTERN
Use: Flags, masks, XOR tricks, optimization
Time: O(1) for basic ops, Space: O(1)
"""

from typing import List


# === BASIC BIT OPERATIONS ===
def get_bit(num: int, i: int) -> int:
    """Get bit at position i (0-indexed from right)"""
    return (num >> i) & 1


def set_bit(num: int, i: int) -> int:
    """Set bit at position i to 1"""
    return num | (1 << i)


def clear_bit(num: int, i: int) -> int:
    """Clear bit at position i (set to 0)"""
    return num & ~(1 << i)


def toggle_bit(num: int, i: int) -> int:
    """Toggle bit at position i"""
    return num ^ (1 << i)


def update_bit(num: int, i: int, val: int) -> int:
    """Set bit at position i to val (0 or 1)"""
    mask = ~(1 << i)
    return (num & mask) | (val << i)


# === COUNT BITS ===
def count_ones(num: int) -> int:
    """Count number of 1 bits (Brian Kernighan's algorithm)"""
    count = 0
    while num:
        num &= num - 1  # clear lowest set bit
        count += 1
    return count


def count_ones_builtin(num: int) -> int:
    """Count using Python builtin"""
    return bin(num).count('1')


# === POWER OF TWO ===
def is_power_of_two(num: int) -> bool:
    """Check if num is power of 2"""
    return num > 0 and (num & (num - 1)) == 0


def next_power_of_two(num: int) -> int:
    """Find next power of 2 >= num"""
    if num <= 1:
        return 1
    num -= 1
    num |= num >> 1
    num |= num >> 2
    num |= num >> 4
    num |= num >> 8
    num |= num >> 16
    return num + 1


# === LOWEST/HIGHEST BITS ===
def lowest_set_bit(num: int) -> int:
    """Isolate the lowest set bit"""
    return num & (-num)


def clear_lowest_set_bit(num: int) -> int:
    """Clear the lowest set bit"""
    return num & (num - 1)


def highest_set_bit_pos(num: int) -> int:
    """Position of highest set bit (0-indexed)"""
    if num == 0:
        return -1
    pos = 0
    while num > 1:
        num >>= 1
        pos += 1
    return pos


# === XOR TRICKS ===
def single_number(arr: List[int]) -> int:
    """Find single number (all others appear twice)"""
    result = 0
    for num in arr:
        result ^= num
    return result


def single_number_iii(arr: List[int]) -> List[int]:
    """Find two numbers that appear once (others appear twice)"""
    xor_all = 0
    for num in arr:
        xor_all ^= num

    # find rightmost set bit (differentiating bit)
    diff_bit = xor_all & (-xor_all)

    a, b = 0, 0
    for num in arr:
        if num & diff_bit:
            a ^= num
        else:
            b ^= num

    return [a, b]


def missing_number(arr: List[int]) -> int:
    """Find missing number in [0, n]"""
    n = len(arr)
    result = n

    for i, num in enumerate(arr):
        result ^= i ^ num

    return result


def swap_without_temp(a: int, b: int) -> tuple:
    """Swap two integers without temporary variable"""
    a ^= b
    b ^= a
    a ^= b
    return a, b


# === BIT MASKS ===
def create_mask(start: int, end: int) -> int:
    """Create mask with 1s from position start to end (inclusive)"""
    return ((1 << (end - start + 1)) - 1) << start


def extract_bits(num: int, start: int, length: int) -> int:
    """Extract 'length' bits starting from position 'start'"""
    mask = (1 << length) - 1
    return (num >> start) & mask


def insert_bits(num: int, insert: int, start: int, length: int) -> int:
    """Insert 'insert' into 'num' at position 'start'"""
    mask = ((1 << length) - 1) << start
    num &= ~mask  # clear bits
    return num | (insert << start)


# === REVERSE BITS ===
def reverse_bits(num: int, bits: int = 32) -> int:
    """Reverse bits of a number"""
    result = 0
    for _ in range(bits):
        result = (result << 1) | (num & 1)
        num >>= 1
    return result


# === SUBSET ENUMERATION ===
def enumerate_subsets(mask: int):
    """Generate all subsets of a bitmask"""
    subsets = []
    subset = mask
    while subset:
        subsets.append(subset)
        subset = (subset - 1) & mask
    subsets.append(0)
    return subsets


def count_subsets(mask: int) -> int:
    """Count number of subsets (2^popcount)"""
    return 1 << count_ones(mask)


# === GRAY CODE ===
def binary_to_gray(num: int) -> int:
    """Convert binary to Gray code"""
    return num ^ (num >> 1)


def gray_to_binary(gray: int) -> int:
    """Convert Gray code to binary"""
    binary = 0
    while gray:
        binary ^= gray
        gray >>= 1
    return binary


# === EMBEDDED: REGISTER OPERATIONS ===
def read_modify_write(reg: int, mask: int, value: int) -> int:
    """Read-modify-write pattern for hardware registers"""
    return (reg & ~mask) | (value & mask)


def set_field(reg: int, field_mask: int, field_pos: int, value: int) -> int:
    """Set a field in a register"""
    reg &= ~field_mask  # clear field
    reg |= (value << field_pos) & field_mask  # set new value
    return reg


def get_field(reg: int, field_mask: int, field_pos: int) -> int:
    """Get a field from a register"""
    return (reg & field_mask) >> field_pos


# === PARITY ===
def parity(num: int) -> int:
    """Compute parity (1 if odd number of 1s, 0 if even)"""
    num ^= num >> 16
    num ^= num >> 8
    num ^= num >> 4
    num ^= num >> 2
    num ^= num >> 1
    return num & 1


# === ADD WITHOUT ARITHMETIC ===
def add_binary(a: int, b: int) -> int:
    """Add two numbers using only bit operations"""
    while b:
        carry = a & b
        a = a ^ b
        b = carry << 1
    return a
