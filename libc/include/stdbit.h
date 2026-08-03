/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _STDBIT_H
#define _STDBIT_H

#include <limits.h>

#define __STDC_ENDIAN_LITTLE__ 1234
#define __STDC_ENDIAN_BIG__ 4321
#define __STDC_ENDIAN_NATIVE__ __STDC_ENDIAN_LITTLE__

#define __STDBIT_DEFINE(type, suffix)                                            \
static inline unsigned int __stdbit_lz_##suffix(type value)                      \
{                                                                                \
    unsigned int width = sizeof(type) * CHAR_BIT, count = 0;                     \
    while (count < width && !(value & ((type)1 << (width - count - 1))))         \
        ++count;                                                                 \
    return count;                                                                \
}                                                                                \
static inline unsigned int __stdbit_tz_##suffix(type value)                      \
{                                                                                \
    unsigned int width = sizeof(type) * CHAR_BIT, count = 0;                     \
    if (!value)                                                                  \
        return width;                                                            \
    while (!(value & 1)) { value >>= 1; ++count; }                               \
    return count;                                                                \
}                                                                                \
static inline unsigned int __stdbit_ones_##suffix(type value)                    \
{                                                                                \
    unsigned int count = 0;                                                      \
    while (value) { count += value & 1; value >>= 1; }                           \
    return count;                                                                \
}                                                                                \
static inline unsigned int __stdbit_lo_##suffix(type value)                      \
{ return __stdbit_lz_##suffix((type)~value); }                                   \
static inline unsigned int __stdbit_to_##suffix(type value)                      \
{ return __stdbit_tz_##suffix((type)~value); }                                   \
static inline unsigned int __stdbit_flo_##suffix(type value)                     \
{ return value ? __stdbit_lz_##suffix(value) + 1U : 0U; }                        \
static inline unsigned int __stdbit_flz_##suffix(type value)                     \
{ return value != (type)~0 ? __stdbit_lo_##suffix(value) + 1U : 0U; }            \
static inline unsigned int __stdbit_fto_##suffix(type value)                     \
{ return value ? __stdbit_tz_##suffix(value) + 1U : 0U; }                        \
static inline unsigned int __stdbit_ftz_##suffix(type value)                     \
{ return value != (type)~0 ? __stdbit_to_##suffix(value) + 1U : 0U; }            \
static inline unsigned int __stdbit_zeros_##suffix(type value)                   \
{ return sizeof(type) * CHAR_BIT - __stdbit_ones_##suffix(value); }              \
static inline _Bool __stdbit_single_##suffix(type value)                         \
{ return value && !(value & (type)(value - 1)); }                                \
static inline unsigned int __stdbit_width_##suffix(type value)                   \
{ return sizeof(type) * CHAR_BIT - __stdbit_lz_##suffix(value); }                \
static inline type __stdbit_floor_##suffix(type value)                           \
{                                                                                \
    if (!value) return 0;                                                        \
    return (type)1 << (sizeof(type) * CHAR_BIT - 1 - __stdbit_lz_##suffix(value)); \
}                                                                                \
static inline type __stdbit_ceil_##suffix(type value)                            \
{                                                                                \
    unsigned int shift;                                                          \
    if (value <= 1) return 1;                                                    \
    shift = sizeof(type) * CHAR_BIT - __stdbit_lz_##suffix((type)(value - 1));   \
    return shift == sizeof(type) * CHAR_BIT ? 0 : (type)1 << shift;              \
}

__STDBIT_DEFINE(unsigned char, uc)
__STDBIT_DEFINE(unsigned short, us)
__STDBIT_DEFINE(unsigned int, ui)
__STDBIT_DEFINE(unsigned long, ul)
__STDBIT_DEFINE(unsigned long long, ull)

#define __STDBIT_SELECT(value, operation) _Generic((value), \
    unsigned char: __stdbit_##operation##_uc,                \
    unsigned short: __stdbit_##operation##_us,               \
    unsigned int: __stdbit_##operation##_ui,                 \
    unsigned long: __stdbit_##operation##_ul,                \
    unsigned long long: __stdbit_##operation##_ull)(value)

#define stdc_leading_zeros(value) __STDBIT_SELECT(value, lz)
#define stdc_leading_ones(value) __STDBIT_SELECT(value, lo)
#define stdc_trailing_zeros(value) __STDBIT_SELECT(value, tz)
#define stdc_trailing_ones(value) __STDBIT_SELECT(value, to)
#define stdc_first_leading_one(value) __STDBIT_SELECT(value, flo)
#define stdc_first_leading_zero(value) __STDBIT_SELECT(value, flz)
#define stdc_first_trailing_one(value) __STDBIT_SELECT(value, fto)
#define stdc_first_trailing_zero(value) __STDBIT_SELECT(value, ftz)
#define stdc_count_ones(value) __STDBIT_SELECT(value, ones)
#define stdc_count_zeros(value) __STDBIT_SELECT(value, zeros)
#define stdc_has_single_bit(value) __STDBIT_SELECT(value, single)
#define stdc_bit_width(value) __STDBIT_SELECT(value, width)
#define stdc_bit_floor(value) __STDBIT_SELECT(value, floor)
#define stdc_bit_ceil(value) __STDBIT_SELECT(value, ceil)

#endif
