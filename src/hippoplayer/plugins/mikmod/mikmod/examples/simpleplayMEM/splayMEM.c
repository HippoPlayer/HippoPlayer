/* splay.c
 * An example on how to use libmikmod to play
 * a module, but to load it with a custom MLOADER.
 *
 * (C) 2004, Raphael Assenat (raph@raphnet.net)
 *
 * This example is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRENTY; without event the implied warrenty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include <mikmod.h>

#if !defined _WIN32 && !defined _WIN64
#include <unistd.h>  /* for usleep() */
#define MikMod_Sleep(ns) usleep(ns)
#else
#define MikMod_Sleep(ns) Sleep(ns / 1000)
#endif

#include "mmloader.h"

int main(int argc, char **argv)
{
	MODULE *module;
	unsigned char *file_data;
	long file_length;
	FILE *fptr;
	MREADER *mem_reader;

	if (argc<2) {
		fprintf(stderr, "Usage: ./splay1 file\n");
		return 1;
	}

	/* register all the drivers */
	MikMod_RegisterAllDrivers();

	/* register all the module loaders */
	MikMod_RegisterAllLoaders();

	/* init the library */
	md_mode |= DMODE_SOFT_MUSIC | DMODE_NOISEREDUCTION | DMODE_HQMIXER | DMODE_SIMDMIXER; 
	
	if (MikMod_Init("")) {
		fprintf(stderr, "Could not initialize sound, reason: %s\n",
				MikMod_strerror(MikMod_errno));
		return 2;
	}

	/* open the file */
	fptr = fopen(argv[1], "rb");
	if (fptr==NULL) { 
		perror("fopen");
		MikMod_Exit();
		return 1;
	}

	/* calculate the file size */
	fseek(fptr, 0, SEEK_END);
	file_length = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);

	/* allocate a buffer and load the file into it */
	file_data = MikMod_malloc(file_length);
	if (file_data == NULL) {
		perror("MikMod_malloc");
		fclose(fptr);
		MikMod_Exit();
		return 1;
	}
	if (fread(file_data, file_length, 1, fptr) != 1)
	{
		perror("fread");
		fclose(fptr);
		MikMod_free(file_data);
		MikMod_Exit();
		return 1;
	}
	fclose(fptr);

	/* Create the memory reader */
	mem_reader = _mm_new_mem_reader(file_data, file_length);
	if (mem_reader == NULL) {
		fprintf(stderr, "failed to create mem reader\n");
		MikMod_free(file_data);
		MikMod_Exit();
		return 1;
	}

	/* load module */
	module = Player_LoadGeneric(mem_reader, 64, 0);	
	if (module) {
		/* start module */
		printf("Playing %s\n", module->songname);
		Player_Start(module);

		while (Player_Active()) {
			MikMod_Sleep(10000);
			MikMod_Update();
		}

		Player_Stop();
		Player_Free(module);
	} else
		fprintf(stderr, "Could not load module, reason: %s\n",
				MikMod_strerror(MikMod_errno));

	MikMod_free(file_data);
	_mm_delete_mem_reader(mem_reader);
	MikMod_Exit();

	return 0;
}

