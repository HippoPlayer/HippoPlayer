///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HIPPOTYPES_H
#define HIPPOTYPES_H

// HACK
//

#define HIPPO_MACOSX

#include <stddef.h>

typedef unsigned int uint;

#if defined(HIPPO_WIN32)

#include <xmmintrin.h> // __m128

#if defined(__GNUC__)
#include <stdint.h>
#else

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned int uint32_t;
typedef signed int int32_t;

typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;

typedef __m128 uint128_t;
typedef __m128 int128_t;

typedef unsigned char bool;

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

#pragma warning(disable: 4100 4127 4201)
#endif

#define HIPPO_LIKELY(exp) exp
#define HIPPO_UNLIKELY(exp) exp
#define HIPPO_INLINE __forceinline
#define HIPPO_RESTRICT __restrict
#define HIPPO_ALIGN(x) __declspec(align(x))
#define HIPPO_UNCACHEDAC_PTR(x)	x
#define HIPPO_UNCACHED_PTR(x) x
#define HIPPO_CACHED_PTR(x) x
#define HIPPO_ALIGNOF(t) __alignof(t)
#define HIPPO_BREAK __debugbreak()

#if defined(_WIN64)
#define HIPPO_X64
#endif

#elif defined(HIPPO_UNIX) || defined(HIPPO_MACOSX)

#include <stdint.h>

#if defined(__SSE__)
#include <xmmintrin.h>

typedef __m128 uint128_t;
typedef __m128 int128_t;
#else
#error No SSE support detected
#endif

#define HIPPO_LIKELY(exp) __builtin_expect(exp, 1)
#define HIPPO_UNLIKELY(exp) __builtin_expect(exp, 0)
#define HIPPO_INLINE inline
#define HIPPO_RESTRICT __restrict
#define HIPPO_ALIGN(x) __attribute__((aligned(x)))
#define HIPPO_UNCACHEDAC_PTR(x)	x
#define HIPPO_UNCACHED_PTR(x) x
#define HIPPO_CACHED_PTR(x) x
#define HIPPO_ALIGNOF(t) __alignof__(t)
#define HIPPO_BREAK ((*(volatile uint32_t *)(0)) = 0x666)

#include <stdbool.h>

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

#if defined(__LP64__)
#define HIPPO_X64
#endif

#else
#error Platform not supported
#endif

#define sizeof_array(array) (int)(sizeof(array) / sizeof(array[0]))

#endif
