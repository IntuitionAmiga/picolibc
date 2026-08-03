/* SPDX-License-Identifier: BSD-3-Clause */

int
__ie64_clz32(unsigned int value)
{
    int count = 0;
    unsigned int bit = 1U << 31;
    while (bit && !(value & bit)) {
        ++count;
        bit >>= 1;
    }
    return count;
}

int
__ie64_clz64(unsigned long value)
{
    int count = 0;
    unsigned long bit = 1UL << 63;
    while (bit && !(value & bit)) {
        ++count;
        bit >>= 1;
    }
    return count;
}
