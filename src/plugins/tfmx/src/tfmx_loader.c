/*
 * tfmx.c
 * File loader and UI for TFMX player.
 * jhp 29Feb96
 */

#include <glib.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include "tfmx.h"
#include "tfmx_player.h"
#include "tfmx_iface.h"
#include "unsqsh.h"
#include "tfmx_audio.h"
#include "xmms_tfmx.h"

extern struct Hdb hdb[8];
extern struct Pdblk pdb;
extern struct Mdb mdb;
extern char act[8];

U32 outRate=44100;
unsigned int mlen = 0;
U8 *smplbuf = 0;
U8 *smplbuf_end = 0;
int *macros = 0;
int *patterns = 0;
short ts[512][8];

struct TFMXHeader mdat_header;

U32 mdat_editbuf[MDAT_EDITBUF_LONGS];

int num_ts;
int num_pat;
int num_mac;
int gubed=0;
int printinfo=0;
int loops=1;

/* structure of of cyb tfmx module (mdat and smpl in one file) */
/* format by my weird friend Alexis 'Cyb' Nasr, back in 1995, yeah ! */
/* FYI, he stopped coding (used asm m68k back then), and is now a doctor ! */
/* values are stored in big endian in the file */
struct CybHeader {
    guint32 TFhd_head;    /* dc.l "TFHD" for recognition */
    guint32 TFhd_offset;  /* dc.l	TFhd_sizeof */
    guint8  TFhd_type;    /* module type :0/1/2/3 (bit 7=FORCED) */
    guint8  TFhd_version; /* currently 0 */
    guint32 _TFhd_mdatsize;  /* compiler may align them, use offsets */
    guint32 _TFhd_smplsize;
    guint16 _TFhd_sizeof;
};


static int tfmx_loader(char *mfn,char *sfn);


/* loading of a single Cyb' TFMX file */
/* return 0 on success */
static int 
tfmx_cyb_file_load (char *fn)
{
    char *tmp_mdat = NULL;
    char *tmp_smpl = NULL;
    FILE *cybf = NULL;
    char *radix = NULL;
    guint8 *cybmem = NULL;
    long fileSize;
    FILE *mdatf = NULL;
    FILE *smplf = NULL;
    struct CybHeader *cybh = NULL;
    int retval = 1;
    int ulen;
    int mdatsize;
    int smplsize;
    guint32 offset;

    /* get radix from filename */
    if (!(radix = strrchr(fn,'/')))
	radix = fn;
    else
	radix++;

    /* open the single file */
    cybf = fopen(fn, "rb");

    if (!cybf)
    {
	return retval;
    }

    /* get length */
    fseek(cybf,0, SEEK_END);
    fileSize = ftell(cybf);
    rewind(cybf);

    /* alloc mem */
    cybmem = (char *)g_malloc(fileSize);
    if (!cybmem)
	goto cleanup;

    /* read it */
    if (fread(cybmem, fileSize, 1, cybf) < 1)
	goto cleanup;
    fclose(cybf);
    cybf = NULL;

    ulen = tfmx_sqsh_get_ulen(cybmem, fileSize);
    if (ulen)
    {
	guint8 *dest;

	dest = (guint8 *)g_malloc(ulen+100);
	if (!dest)
	    goto cleanup;

	tfmx_sqsh_unpack(cybmem+16, dest, ulen);

	g_free(cybmem);
	cybmem = dest;
    }

    if (strncmp(cybmem, "TFHD", 4))
	goto cleanup;

    cybh = (struct CybHeader *)cybmem;

    offset = GINT32_FROM_BE(cybh->TFhd_offset);

    mdatsize = 0;
    mdatsize  = cybmem[10]; mdatsize <<= 8;
    mdatsize |= cybmem[11]; mdatsize <<= 8;
    mdatsize |= cybmem[12]; mdatsize <<= 8; 
    mdatsize |= cybmem[13];

    smplsize = 0;
    smplsize  = cybmem[14]; smplsize <<= 8;
    smplsize |= cybmem[15]; smplsize <<= 8;
    smplsize |= cybmem[16]; smplsize <<= 8; 
    smplsize |= cybmem[17];

    /* create temp file names from radix */
    tmp_mdat = g_strdup_printf("/tmp/__mdat_%s__", radix);
    tmp_smpl = g_strdup_printf("/tmp/__smpl_%s__", radix);

    /* open and write temp files */
    mdatf = fopen(tmp_mdat, "wb");
    if (!mdatf)
	goto cleanup;
    fwrite(cybmem + offset, mdatsize, 1, mdatf);
    fclose(mdatf);

    smplf = fopen(tmp_smpl, "wb");
    if (!smplf)
	goto cleanup;
    fwrite(cybmem + offset + mdatsize, smplsize, 1, mdatf);
    fclose(smplf);

    /* tfmx loading */
    if (tfmx_loader(tmp_mdat, tmp_smpl) == 1) {
	goto cleanup;
    }
    retval = 0;

/* a kind of poor man exception handling :-/ */
  cleanup:
    /* if value for tmpfile => remove it */
    if (mdatf)
	remove(tmp_mdat);
    if (smplf)
	remove(tmp_smpl);
    if (tmp_mdat)
	g_free(tmp_mdat);
    if (tmp_smpl)
	g_free(tmp_smpl);
    if (cybmem)
	g_free(cybmem);
    if (cybf)
	fclose(cybf);
    return retval;
}

char LoadTFMXFile(char *fName)
{
    int suffixPos, status;
    char *mfn = fName, *sfn, *c;
	
    if(!fName) return 1;
    if(!(sfn = strdup(mfn))) return 1;
	
    if (!(c = strrchr(sfn,'/')))
	c = sfn;
    else
	c++;
    suffixPos = strlen(c) - 4;	/* Get filename length */

    if (strncasecmp(c,"mdat.",5) == 0) {
	/* Case-preserving conversion of "mdat" to "smpl" */ 
	(*c++)^='m'^'s'; (*c++)^='d'^'m'; (*c++)^='a'^'p'; (*c++)^='t'^'l';
	c-=4;
    }
    else if (strncasecmp(c,"tfmx.",5) == 0) { /* Single Cyb' TFMX file */
	free(sfn);
	return tfmx_cyb_file_load(fName);
    }
    else if (suffixPos >= 0 && strncasecmp(c + suffixPos,".tfx", 4) == 0) {
	/* Case-preserving conversion of ".tfx" to ".sam" */
	*(c+suffixPos+1)^='t'^'s'; *(c+suffixPos+2)^='f'^'a'; *(c+suffixPos+3)^='x'^'m';
    }
    else  {
	TFMXERR("LoadTFMX: Song name prefix / suffix missing ?!");
	free(sfn); return 1;
    }
	
    if ((status=tfmx_loader(mfn,sfn))==1) {
	/* TFMXERR("LoadTFMXFile: Loading of module failed"); */
	free(sfn); return 1;
    }
    else if (status==2) {
	/* TFMXERR("LoadTFMXFile: Not an MDAT file"); */
	free(sfn); return 1;
    }

    free(sfn);
    return 0;
}

static int tfmx_loader (char *mfn,char *sfn)
{
    FILE *gfd, *smplFile;
    /* struct stat s; */
    int x, y, z = 0;
    U16 *sh, *lg;

    if ((gfd = fopen(mfn,"rb"))<=0) {
	TFMXERR("LoadTFMX: Failed to open song");
	return(1);
    }
	
    if (!fread(&mdat_header, sizeof(mdat_header), 1, gfd)) {
	TFMXERR("LoadTFMX: Failed to read TFMX header");
	fclose(gfd);
	return(1);
    }
    if (strncmp("TFMX-SONG", mdat_header.magic, 9)
	&& strncmp("TFMX_SONG", mdat_header.magic, 9)
	&& strncasecmp("TFMXSONG", mdat_header.magic, 8)
	&& strncasecmp("TFMX ", mdat_header.magic, 5))
    {
	TFMXERR("LoadTFMX: Not a TFMX module");
	fclose(gfd);
	return(2);
    }
		
    if (!(x = fread(&mdat_editbuf, sizeof(U32), MDAT_EDITBUF_LONGS, gfd)))
    {
	TFMXERR("LoadTFMX: Read error in MDAT file");
	fclose(gfd);
	return(1);
    }

    fclose(gfd);

    mlen = x;

    mdat_editbuf[x] = -1;
    if (!mdat_header.trackstart)
	mdat_header.trackstart = 0x180;
    else
	mdat_header.trackstart = (g_ntohl(mdat_header.trackstart) - 0x200L) >> 2;
    if (!mdat_header.pattstart)
	mdat_header.pattstart = 0x80;
    else mdat_header.pattstart = (g_ntohl(mdat_header.pattstart) - 0x200L) >> 2;
    if (!mdat_header.macrostart) mdat_header.macrostart=0x100;
    else mdat_header.macrostart=(g_ntohl(mdat_header.macrostart)-0x200L)>>2;
    if (x<136) {
	return(2);
    }

    for (x=0;x<32;x++) {
	mdat_header.start[x]=g_ntohs(mdat_header.start[x]);
	mdat_header.end[x]=g_ntohs(mdat_header.end[x]);
	mdat_header.tempo[x]=g_ntohs(mdat_header.tempo[x]);
    }

    /* Calc the # of subsongs */
    nSongs = 0;
    for (x = 0; x < 31; x++)
    {
	if ((mdat_header.start[x] <= mdat_header.end[x])
	    && !(x > 0 && mdat_header.end[x] == 0L))
	{
	    nSongs++;
	}
    }
/* Now that we have pointers to most everything, this would be a good time to
   fix everything we can... ntohs tracksteps, convert pointers to array
   indices, ntohl patterns and macros.  We fix the macros first, then the
   patterns, and then the tracksteps (because we have to know when the
   patterns begin to know when the tracksteps end...) */
    z = mdat_header.macrostart;
    macros = &(mdat_editbuf[z]);

    for (x = 0; x < 128; x++) {
	y=(g_ntohl(mdat_editbuf[z])-0x200);
	if ((y&3) || ((y>>2) > mlen)) /* probably not strictly right */
	    break;
	mdat_editbuf[z++]=y >> 2;
    }
    num_mac = x;

    z=mdat_header.pattstart;
    patterns = &mdat_editbuf[z];
    for (x = 0; x < 128; x++) {
	y=(g_ntohl(mdat_editbuf[z])-0x200);
	if ((y&3) || ((y>>2) > mlen))
	    break;
	mdat_editbuf[z++] = y>>2;
    }
    num_pat = x;

    lg = (U16 *)&mdat_editbuf[patterns[0]];
    sh = (U16 *)&mdat_editbuf[mdat_header.trackstart];
    num_ts = (patterns[0] - mdat_header.trackstart) >> 2;
    y=0;
    while (sh<lg) {
	x=g_ntohs(*sh);
	*sh++=x;
    }	

/* Now at long last we calc the size of and load the sample file. */
    {
	long fileSize = 0;

	if ((smplFile=fopen(sfn,"rb")) == NULL) {
	    TFMXERR("LoadTFMX: Error opening SMPL file");
	    return(1);
	}
	if (fseek(smplFile,0, SEEK_END)) {
	    TFMXERR("LoadTFMX: fseek failed in SMPL file"); fclose(smplFile);
	    return(1);
	}
	if ((fileSize = ftell(smplFile)) < 0) {
	    TFMXERR("LoadTFMX: ftell failed in SMPL file"); fclose(smplFile);
	    return(1);
	}

	if(smplbuf) {	// Dealloc any left behind samplebuffer
	    free(smplbuf);
	    smplbuf=0;
	}
		
	if (!(smplbuf=(void *)malloc(fileSize))) {
	    TFMXERR("LoadTFMX: Error allocating samplebuffer"); fclose(smplFile);
	    return(1);
	}

	smplbuf_end = smplbuf + fileSize - 1;
	rewind(smplFile);

	if (!fread(smplbuf,1,fileSize,smplFile)) {
	    TFMXERR("LoadTFMX: Error reading SMPL file");
	    fclose(smplFile);
	    free(smplbuf);
	    return(1);
	}
	fclose(smplFile);
    }

    if (plugin_cfg.blend)
	output_chans = 2;
    plugin_cfg.blend &= 1;

    tfmx_calc_sizes();
    TFMXRewind();

    return 0;

/* Now the song is fully loaded.  Everything is done but ntohl'ing the actual
   pattern and macro data. The routines that use the data do it for themselves.*/
}

void tfmx_fill_module_info(char *t)
{
    int x;

    /* Don't print info if there's no song... */
    if(!smplbuf) {
	sprintf(t, "No song loaded!");
	return;
    }

    t += sprintf(t, "Module text section:\n\n");
    for (x = 0;x < 6; x++)
	t += sprintf(t, ">%40.40s\n", mdat_header.text[x]);

    t += sprintf(t, "\n%d tracksteps at 0x%04lx\n", num_ts, (mdat_header.trackstart<<2)+0x200);
    t += sprintf(t, "%d patterns at 0x%04lx\n", num_pat, (mdat_header.pattstart<<2)+0x200);
    t += sprintf(t, "%d macros at 0x%04lx\n", num_mac, (mdat_header.macrostart<<2)+0x200);

    t += sprintf(t, "\nSubsongs:\n\n");
    for (x = 0; x < 31; x++)
    {
	if ((mdat_header.start[x] <= mdat_header.end[x])
	    && !(x > 0 && mdat_header.end[x] == 0L))
	{
	    t += sprintf(t,"Song %2d: start %3x end %3x tempo %d\n", x,
			 g_ntohs(mdat_header.start[x]), g_ntohs(mdat_header.end[x]),
			 mdat_header.tempo[x]);
	}
    }
}
