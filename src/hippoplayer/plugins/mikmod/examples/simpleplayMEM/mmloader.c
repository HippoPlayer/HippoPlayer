/* mmloader.c
 * Example on how to implement a MLOADER that reads from
 * memory for libmikmod.
 * (C) 2004, Raphael Assenat (raph@raphnet.net)
 *
 * This example is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRENTY; without event the implied warrenty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mikmod.h>
#include "mmloader.h"

static BOOL _mm_MemReader_Eof(MREADER* reader);
static BOOL _mm_MemReader_Read(MREADER* reader,void* ptr,size_t size);
static int _mm_MemReader_Get(MREADER* reader);
static BOOL _mm_MemReader_Seek(MREADER* reader,long offset,int whence);
static long _mm_MemReader_Tell(MREADER* reader);

void _mm_delete_mem_reader(MREADER* reader)
{
	if (reader) { MikMod_free(reader); }
}

MREADER *_mm_new_mem_reader(void *buffer, int len)
{
	MMEMREADER* reader=(MMEMREADER*)MikMod_malloc(sizeof(MMEMREADER));
	if (reader)
	{
		reader->core.Eof =&_mm_MemReader_Eof;
		reader->core.Read=&_mm_MemReader_Read;
		reader->core.Get =&_mm_MemReader_Get;
		reader->core.Seek=&_mm_MemReader_Seek;
		reader->core.Tell=&_mm_MemReader_Tell;
		reader->buffer = buffer;
		reader->len = len;
		reader->pos = 0;
	}
	return (MREADER*)reader;
}

static BOOL _mm_MemReader_Eof(MREADER* reader)
{
	if (!reader) { return 1; }
	if ( ((MMEMREADER*)reader)->pos > ((MMEMREADER*)reader)->len ) { 
		return 1; 
	}
	return 0;
}

static BOOL _mm_MemReader_Read(MREADER* reader,void* ptr,size_t size)
{
	unsigned char *d=ptr, *s;

	if (!reader) { return 0; }

	if (reader->Eof(reader)) { return 0; }

	s = ((MMEMREADER*)reader)->buffer;
	s += ((MMEMREADER*)reader)->pos;

	if ( ((MMEMREADER*)reader)->pos + size >= ((MMEMREADER*)reader)->len) 
	{
		((MMEMREADER*)reader)->pos = ((MMEMREADER*)reader)->len;
		return 0; /* not enough remaining bytes */
	}

	((MMEMREADER*)reader)->pos += size;

	while (size--)
	{
		*d = *s;
		s++;
		d++;	
	}
	
	return 1;
}

static int _mm_MemReader_Get(MREADER* reader)
{
	int pos;

	if (reader->Eof(reader)) { return 0; }
	
	pos = ((MMEMREADER*)reader)->pos;
	((MMEMREADER*)reader)->pos++;

	return ((unsigned char*)(((MMEMREADER*)reader)->buffer))[pos];
}

static BOOL _mm_MemReader_Seek(MREADER* reader,long offset,int whence)
{
	if (!reader) { return -1; }
	
	switch(whence)
	{
		case SEEK_CUR:
			((MMEMREADER*)reader)->pos += offset;
			break;
		case SEEK_SET:
			((MMEMREADER*)reader)->pos = offset;
			break;
		case SEEK_END:
			((MMEMREADER*)reader)->pos = ((MMEMREADER*)reader)->len - offset - 1;
			break;
	}
	if ( ((MMEMREADER*)reader)->pos < 0) { ((MMEMREADER*)reader)->pos = 0; }
	if ( ((MMEMREADER*)reader)->pos > ((MMEMREADER*)reader)->len ) { 
		((MMEMREADER*)reader)->pos = ((MMEMREADER*)reader)->len;
	}
	return 0;
}

static long _mm_MemReader_Tell(MREADER* reader)
{
	if (reader) {
		return ((MMEMREADER*)reader)->pos;
	}
	return 0;
}
