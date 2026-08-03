/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _STDCKDINT_H
#define _STDCKDINT_H

#include <limits.h>

struct __ckd_value {
    unsigned long long magnitude;
    _Bool negative;
    _Bool wide;
};

#define __CKD_SIGNED_VALUE(type, unsigned_type, suffix)                           \
static inline struct __ckd_value __ckd_value_##suffix(type value)                 \
{                                                                                \
    struct __ckd_value result;                                                    \
    result.negative = value < 0;                                                  \
    result.magnitude = result.negative                                            \
        ? 0ULL - (unsigned long long)(unsigned_type)value                         \
        : (unsigned long long)value;                                              \
    result.wide = 0;                                                             \
    return result;                                                               \
}

#define __CKD_UNSIGNED_VALUE(type, suffix)                                       \
static inline struct __ckd_value __ckd_value_##suffix(type value)                 \
{ return (struct __ckd_value){ (unsigned long long)value, 0, 0 }; }

__CKD_SIGNED_VALUE(char, unsigned char, c)
__CKD_SIGNED_VALUE(signed char, unsigned char, sc)
__CKD_UNSIGNED_VALUE(unsigned char, uc)
__CKD_SIGNED_VALUE(short, unsigned short, s)
__CKD_UNSIGNED_VALUE(unsigned short, us)
__CKD_SIGNED_VALUE(int, unsigned int, i)
__CKD_UNSIGNED_VALUE(unsigned int, ui)
__CKD_SIGNED_VALUE(long, unsigned long, l)
__CKD_UNSIGNED_VALUE(unsigned long, ul)
__CKD_SIGNED_VALUE(long long, unsigned long long, ll)
__CKD_UNSIGNED_VALUE(unsigned long long, ull)

#define __CKD_VALUE(value) _Generic((value),               \
    char: __ckd_value_c,                                   \
    signed char: __ckd_value_sc,                           \
    unsigned char: __ckd_value_uc,                         \
    short: __ckd_value_s,                                  \
    unsigned short: __ckd_value_us,                        \
    int: __ckd_value_i,                                    \
    unsigned int: __ckd_value_ui,                          \
    long: __ckd_value_l,                                   \
    unsigned long: __ckd_value_ul,                         \
    long long: __ckd_value_ll,                             \
    unsigned long long: __ckd_value_ull)(value)

static inline struct __ckd_value
__ckd_add_values(struct __ckd_value a, struct __ckd_value b)
{
    struct __ckd_value result = { 0, 0, 0 };
    if (a.negative == b.negative) {
        result.magnitude = a.magnitude + b.magnitude;
        result.wide = result.magnitude < a.magnitude;
        result.negative = a.negative;
    } else if (a.magnitude >= b.magnitude) {
        result.magnitude = a.magnitude - b.magnitude;
        result.negative = a.negative && result.magnitude != 0;
    } else {
        result.magnitude = b.magnitude - a.magnitude;
        result.negative = b.negative;
    }
    return result;
}

static inline struct __ckd_value
__ckd_sub_values(struct __ckd_value a, struct __ckd_value b)
{
    if (b.magnitude)
        b.negative = !b.negative;
    return __ckd_add_values(a, b);
}

static inline struct __ckd_value
__ckd_mul_values(struct __ckd_value a, struct __ckd_value b)
{
    struct __ckd_value result;
    result.wide = b.magnitude && a.magnitude > ULLONG_MAX / b.magnitude;
    result.magnitude = a.magnitude * b.magnitude;
    result.negative = (a.negative != b.negative) && result.magnitude != 0;
    return result;
}

#define __CKD_STORE_SIGNED(type, unsigned_type, suffix, maximum)                  \
static inline _Bool __ckd_store_##suffix(struct __ckd_value value, type *result)  \
{                                                                                \
    unsigned long long negative_limit = (unsigned long long)(maximum) + 1ULL;     \
    unsigned long long bits = value.negative ? 0ULL - value.magnitude             \
                                              : value.magnitude;                  \
    *result = (type)(unsigned_type)bits;                                          \
    return value.wide || (value.negative ? value.magnitude > negative_limit       \
                                        : value.magnitude > (maximum));          \
}

#define __CKD_STORE_UNSIGNED(type, suffix, maximum)                              \
static inline _Bool __ckd_store_##suffix(struct __ckd_value value, type *result)  \
{                                                                                \
    unsigned long long bits = value.negative ? 0ULL - value.magnitude             \
                                              : value.magnitude;                  \
    *result = (type)bits;                                                        \
    return value.wide || value.negative || value.magnitude > (maximum);          \
}

__CKD_STORE_SIGNED(char, unsigned char, c, CHAR_MAX)
__CKD_STORE_SIGNED(signed char, unsigned char, sc, SCHAR_MAX)
__CKD_STORE_UNSIGNED(unsigned char, uc, UCHAR_MAX)
__CKD_STORE_SIGNED(short, unsigned short, s, SHRT_MAX)
__CKD_STORE_UNSIGNED(unsigned short, us, USHRT_MAX)
__CKD_STORE_SIGNED(int, unsigned int, i, INT_MAX)
__CKD_STORE_UNSIGNED(unsigned int, ui, UINT_MAX)
__CKD_STORE_SIGNED(long, unsigned long, l, LONG_MAX)
__CKD_STORE_UNSIGNED(unsigned long, ul, ULONG_MAX)
__CKD_STORE_SIGNED(long long, unsigned long long, ll, LLONG_MAX)
__CKD_STORE_UNSIGNED(unsigned long long, ull, ULLONG_MAX)

#define __CKD_STORE(result) _Generic(*(result), \
    char: __ckd_store_c,                         \
    signed char: __ckd_store_sc,                 \
    unsigned char: __ckd_store_uc,               \
    short: __ckd_store_s,                        \
    unsigned short: __ckd_store_us,              \
    int: __ckd_store_i,                          \
    unsigned int: __ckd_store_ui,                \
    long: __ckd_store_l,                         \
    unsigned long: __ckd_store_ul,               \
    long long: __ckd_store_ll,                   \
    unsigned long long: __ckd_store_ull)

#define ckd_add(result, a, b) \
    (__CKD_STORE(result)(__ckd_add_values(__CKD_VALUE(a), __CKD_VALUE(b)), result))
#define ckd_sub(result, a, b) \
    (__CKD_STORE(result)(__ckd_sub_values(__CKD_VALUE(a), __CKD_VALUE(b)), result))
#define ckd_mul(result, a, b) \
    (__CKD_STORE(result)(__ckd_mul_values(__CKD_VALUE(a), __CKD_VALUE(b)), result))

#endif
