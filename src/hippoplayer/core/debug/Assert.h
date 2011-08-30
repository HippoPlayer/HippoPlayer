///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HIPPOASSERT_H 
#define HIPPOASSERT_H 

#include "../Types.h"

void Hippo_displayAssert();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HIPPO_ASSERT(exp) HIPPO_ASSERT_DESC(exp, #exp)
#define HIPPO_ASSERT_DESC(exp, ...) \
do\
{\
	if (HIPPO_UNLIKELY(!(exp)))\
	{\
		Hippo_displayAssert(); \
	}\
}\
while (0);

#define HIPPO_ASSERT_CONCAT_(a, b) a##b
#define HIPPO_ASSERT_CONCAT(a, b) HIPPO_ASSERT_CONCAT_(a, b)
// These can't be used after statements in c89
#ifdef __COUNTER__
	#define HIPPO_STATIC_ASSERT(e,m) \
		enum { HIPPO_ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
	// This can't be used twice on the same line so ensure if using in headers
	// that the headers are not included twice (by wrapping in #ifndef...#endif)
	// Note it doesn't cause an issue when used on same line of separate modules
	// compiled with gcc -combine -fwhole-program.  
	#define HIPPO_STATIC_ASSERT(e,m) \
		enum { HIPPO_ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif

#endif

