#ifndef _UADE_COMPILER_SUPPORT_H_
#define _UADE_COMPILER_SUPPORT_H_

#ifdef _WIN32
#define likely(x) x	
#define unlikely(x) x	
#else
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#endif

#endif
