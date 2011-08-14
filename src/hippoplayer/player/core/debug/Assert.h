///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HIPPOASSERT_H 
#define HIPPOASSERT_H 

#include "../Types.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ZENIC_ASSERT(exp) ZENIC_ASSERT_DESC(exp, #exp)
#define ZENIC_ASSERT_DESC(exp, ...) \
do\
{\
	if (ZENIC_UNLIKELY(!(exp)))\
	{\
	}\
}\
while (0);

#define ZENIC_ASSERT_CONCAT_(a, b) a##b
#define ZENIC_ASSERT_CONCAT(a, b) ZENIC_ASSERT_CONCAT_(a, b)
// These can't be used after statements in c89
#ifdef __COUNTER__
	#define ZENIC_STATIC_ASSERT(e,m) \
		enum { ZENIC_ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
	// This can't be used twice on the same line so ensure if using in headers
	// that the headers are not included twice (by wrapping in #ifndef...#endif)
	// Note it doesn't cause an issue when used on same line of separate modules
	// compiled with gcc -combine -fwhole-program.  
	#define ZENIC_STATIC_ASSERT(e,m) \
		enum { ZENIC_ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif

#endif

