#include <stdint.h>
#include "xxh3.h"

uint64_t xx3h_hash_bits(const uint8_t* data, int len) {
	return XXH3_64bits(data, len);
}



