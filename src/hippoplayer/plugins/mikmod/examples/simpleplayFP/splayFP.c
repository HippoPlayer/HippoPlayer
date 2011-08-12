/* splayFP.c
 * An example on how to use libmikmod to play
 * a module from a FILE*
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

int main(int argc, char **argv)
{
	MODULE *module;
	FILE *fptr;

	if (argc<2) {
		fprintf(stderr, "Usage: ./splayFP file\n");
		return 1;
	}

	/* register all the drivers */
	MikMod_RegisterAllDrivers();

	/* register all the module loaders */
	MikMod_RegisterAllLoaders();

	/* init the library */
	md_mode |= DMODE_SOFT_MUSIC;
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

	/* load module */
	module = Player_LoadFP(fptr, 64, 0);	
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

	fclose(fptr);
	MikMod_Exit();

	return 0;
}

