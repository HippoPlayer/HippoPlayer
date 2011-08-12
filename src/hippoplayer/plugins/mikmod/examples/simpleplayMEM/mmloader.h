#ifndef _mmloader_h__
#define _mmloader_h__

typedef struct MMEMREADER {
	MREADER core;
	void *buffer;
	long len;
	long pos;
} MMEMREADER;

MREADER *_mm_new_mem_reader(void *buffer, int len);
void _mm_delete_mem_reader(MREADER* reader);

#endif

