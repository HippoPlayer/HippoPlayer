/* soundeffects.c
 * An example on how to use libmikmod to play
 * sound effects.
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

SAMPLE *Load(char *fn)
{	
	char *file_data;
	long file_length;
	FILE *fptr;

	// return Sample_Load(fn);

	/* open the file */
	fptr = fopen(fn, "rb");
	if (fptr==NULL) { 
		perror("fopen");
		return 0;
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
		return 0;
	}
	if (fread(file_data, file_length, 1, fptr) != 1)
	{
		perror("fread");
		fclose(fptr);
		MikMod_free(file_data);
		return 0;
	}
	fclose(fptr);

	return Sample_LoadMem(file_data, file_length);
}

main()
{
    /* sound effects */
    SAMPLE *sfx1, *sfx2;
    /* voices */
    int v1, v2;
	int i;

    /* register all the drivers */
    MikMod_RegisterAllDrivers();

    /* initialize the library */
    md_mode |= DMODE_SOFT_SNDFX;
    if (MikMod_Init("")) {
        fprintf(stderr, "Could not initialize sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }

    /* load samples */
    sfx1 = Load("first.wav");
    if (!sfx1) {
        MikMod_Exit();
        fprintf(stderr, "Could not load the first sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }
    sfx2 = Load("second.wav");
    if (!sfx2) {
        Sample_Free(sfx1);
        MikMod_Exit();
        fprintf(stderr, "Could not load the second sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }

    /* reserve 2 voices for sound effects */
    MikMod_SetNumVoices(-1, 2);

    /* get ready to play */
    MikMod_EnableOutput();

    /* play first sample */
    v1 = Sample_Play(sfx1, 0, 0);
    do {
        MikMod_Update();
        MikMod_Sleep(100000);
    } while (!Voice_Stopped(v1));

	for (i=0;i<10;i++) {
        MikMod_Update();
        MikMod_Sleep(100000);
    }


    /* half a second later, play second sample */
    v2 = Sample_Play(sfx2, 0, 0);
    do {
        MikMod_Update();
        MikMod_Sleep(100000);
    } while (!Voice_Stopped(v2));

	for (i=0;i<10;i++) {
        MikMod_Update();
        MikMod_Sleep(100000);
    }

    MikMod_DisableOutput();


    Sample_Free(sfx2);
    Sample_Free(sfx1);

    MikMod_Exit();
}
