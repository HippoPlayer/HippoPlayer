/*
 * @file    info68.c
 * @brief   program to retrieve information from sc68 files
 * @author  http://sourceforge.net/users/benjihan
 *
 * Copyright (c) 1998-2016 Benjamin Gerard
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* file68 includes */
#include <sc68/file68.h>
#include <sc68/file68_err.h>
#include <sc68/file68_str.h>
#include <sc68/file68_msg.h>
#include <sc68/file68_uri.h>
#include <sc68/file68_opt.h>

/* Standard includes */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif


#define BUILD_DATE  __DATE__

enum {
  XCODE_OK  = 0,
  XCODE_ERR = 1,                        /* generic error */
  XCODE_CLI = 2,                        /* command line error */
  XCODE_INP = 4,                        /* input error */
  XCODE_OUT = 8,                        /* output error */
  XCODE_TRK = 16,                       /* tracklist error */
};

static int ioerror = 0;                 /* global flag for I/O error */
static int fmt_z = ' ';                 /* alignmnent ('-',' ','0') */
static int fmt_l = 0;                   /* format length */
static char fmt_b[2048];                /* format temp buffer */
#define fmt_max (sizeof(fmt_b)-1)

typedef struct diskinfo {
  disk68_t * d;
  const char * uri;
  int has_time,has_loop,from,to;
  int cnt;
} disk_t;

static char prg[] = "info68";
static int error(const char *fmt, ...)
{
  va_list list;
  va_start(list, fmt);
  fprintf(stderr, "%s: ", prg);
  vfprintf(stderr, fmt, list);
  fflush(stderr);
  va_end(list);
  return XCODE_ERR;
}

static void message_cb(int bit, void* user, const char * fmt, va_list list)
{
  switch (bit) {
  case msg68_NEVER: break;

  case msg68_CRITICAL: case msg68_ERROR: case msg68_WARNING:
    fprintf(stderr,"%s: ",prg);
  default:
    vfprintf(stderr,fmt,list);
    fflush(stderr);
    break;
  }

}

static int output_error(const char * filename)
{
  error ("output error -- \"%s\"\n", filename);
  return XCODE_OUT;
}

static int input_error(const char *filename)
{
  error ("not an sc68 file -- \"%s\"\n", filename);
  return XCODE_INP;
}

static int tracklist_error(const char *command)
{
  error("invalid track-list -- \"%s\"\n", command);
  return XCODE_TRK;
}

static void print_option(void * data,
                         const char * option,
                         const char * envvar,
                         const char * values,
                         const char * desc)
{
  fprintf(data,"  %s", option);
  if (values && *values)
    fprintf(data,"=%s", values);
  fprintf(data,"\n"
          "    %-16s %c%s\n", envvar,
          (desc[0]>='a' && desc[0]<='z') ? desc[0]-'a'+'A' : desc[0],
          desc+1);
}

static int print_help(int more)
{
  puts
    ("Usage: info68 [OPTION] [--] <URI> [track-list] format ...\n"
     " or    info68 [OPTION] -f format -- <URI> [<URI> ...]\n"
     "\n"
     "Get and format information on sc68 files.\n");

  puts
    ("Options:\n"
     "\n"
     "  --             Break option parsing\n"
     "  -h --help      Display this message and exit (incremental)\n"
     "  -V --version   Display version and exit\n"
     "  -v --verbose   Verbosity level (incremental)\n"
     "  -o --output=   Change output to file (- is stdout)\n"
     "  -A --all       Display all information and tags\n"
     "\n"
     "  -t --tracks=   Set default track-list.\n"
     "  -f --format=   Set the format string.\n"
     "\n"
     "  If the format string is set (-f or --format) info68 interprets the\n"
     "  command line in a more convenient way to work with common utils\n"
     "  such as xargs.\n"
      );

  if (more > 1) {
    option68_help(stdout,print_option,more > 2);
    puts("");
  }

  puts
    ("Track-list:\n"
     "\n"
     "  Syntax:\n"
     "\n"
     "    %DIGIT[-DIGIT][,DIGIT[-DIGIT]...]\n"
     "\n"
     "    track-list executes the next format string for all listed tracks\n"
     "    one after another  in the given order.  First track is number 1.\n"
     "    0 is replaced by the last track number (number of track).\n"
     "\n"
     "Format:\n"
     "\n"
     "    format is a string with special commands sequence. Each of these\n"
     "    commands  start with  a percent '%' char  followed by  a command\n"
     "    identifer. Printf-like format modifiers are partially supported.\n");

  if (more <= 1) {
    puts("    Use help option one more time for more details on commands\n");
  } else {
    puts
      ("  disk-commands:\n"
       "\n"
       "    `%#'         number of tracks\n"
       "    `%?'         default track\n"
       "    `%N'         disk name\n"
       "    `%A'         default track author name\n"
       "    `%C'         default track composer name\n"
       "    `%P'         default track ripper name\n"
       "    `%V'         default track converter name\n"
       "    `%T'         disk time in sec\n"
       "    `%Y'         formated disk time. Format \"TT MM:SS\"\n"
       "    `%H'         all tracks ORed hardware flags (see %h)\n"
       "    `%F'         file format (sc68 or sndh)\n"
       "    `%~'         file hash code (unic-id)\n"
       "    `%U'         input URI\n"
       "    `%I'         input number (1-based)\n"
        );
    puts
      ("  track-commands:\n"
       "\n"
       "    `%&'         track number\n"
       "    `%n'         track name\n"
       "    `%a'         author name\n"
       "    `%c'         composer name\n"
       "    `%p'         ripper name\n"
       "    `%v'         converter name\n"
       "    `%r'         replay name\n"
       "    `%t'         time in sec\n"
       "    `%y'         formated time. Format \"TT MM:SS\"\n"
       "    `%f'         replay frequency (frame length)\n"
       "    `%m'         duration in frames\n"
       "    `%@'         load address (in hexdecimal)\n"
       "    `%h'         hardware flags [YSAT] uppercase means activated\n"
       "                 Y:YM-2149 S:STE A:Amiga T:Timers\n");

    puts
      ("  misc-commands:\n"
       "\n"
       "    `%%'         display %\n"
       "    `%0'         display null char (deprecated)\n"
       "    `%Z'         display null char (prefered)\n"
       "    `%L'         display a newline character (deprecated)\n"
       "    `%.'         display a newline character (prefered)\n");

    puts
      ("  tag-commands:\n"
       "\n"
       "    `%{tag}'     display named-tag.\n"
       "                 Use uppercase first letter to address disk tag\n"
       "                 and lowercase to address current track tag\n");
  }

  puts
    ("URI:\n"
     "\n"
     "  $ - or\n"
     "  $ stdin://           read file fron standard input.\n"
     "  $ path or\n"
     "  $ file://path or\n"
     "  $ local://path       local file.\n"
     "  $ null://            null file.\n"
     "  $ http://path or\n"
     "  $ ftp://path or\n"
     "  $ ...                load file via given protocol (see curl)\n"
     "  $ sc68://author/hw/title[/track:loop\n"
     "  Access sc68 music database. The music file is first searched in\n"
     "  local music path and if not found in remote music path.\n");

#if 0 /* DEPRECATED NEED TO BE UPODATED */
  puts
    ("Examples:\n"
     "\n"
     "  $ info68 'sc68://music/Jochen Hippel (Mad Max)/0/Wings Of Death/' \\"
     "           '%1,5,4-6' '> %&/%# %N - %a - %n%L'\n"
     "\n"
     "  $ info68 -t1,5,4-6 -f'> %&/%# %N - %a - %n%L' -- \\"
     "           'sc68://music/Jochen Hippel (Mad Max)/0/Wings Of Death/'"
     "\n"
     "  > 1/10 Wings Of Death - Jochen Hippel (Mad Max) - Level #1\n"
     "  > 5/10 Wings Of Death - Jochen Hippel (Mad Max) - Level #5\n"
     "  > 4/10 Wings Of Death - Jochen Hippel (Mad Max) - Level #4\n"
     "  > 5/10 Wings Of Death - Jochen Hippel (Mad Max) - Level #5\n"
     "  > 6/10 Wings Of Death - Jochen Hippel (Mad Max) - Level #6\n");
#endif

  puts
    ("\n"
     "Copyright (c) 1998-2016 Benjamin Gerard.\n"
     "\n"
     "Visit <" PACKAGE_URL ">\n"
     "Report bugs to <" PACKAGE_BUGREPORT ">");

  return 0;
}

/* Display version number. */
static int print_version(void)
{
  puts
    (PACKAGE_STRING "\n"
     "\n"
     "Copyright (c) 1998-2016 Benjamin Gerard.\n"
     "License GPLv3+ or later <http://gnu.org/licenses/gpl.html>\n"
     "This is free software: you are free to change and redistribute it.\n"
     "There is NO WARRANTY, to the extent permitted by law.\n"
     "\n"
     "Written by Benjamin Gerard"
      );
  return 0;
}

static const char * HWflags(const hwflags68_t f)
{
  static const char hex[] = "0123456789ABCDEF";
  static char flags[] = "YSA0";
  flags[0] = "yY"[!!(f & SC68_PSG)];
  flags[1] = "sS"[!!(f & (SC68_DMA|SC68_LMC))];
  flags[2] = "aA"[!!(f & SC68_AGA)];
  flags[3] = (f & SC68_XTD) ? hex[15 & (f>>SC68_MFP_BIT)] : '.';
  return flags;
}

static const char * Xflags(const hwflags68_t f)
{
  static char flags[32];
  static const struct {
    int letter, mask;
  } xf[] = {
    { 'y', SC68_PSG },
    { 'e', SC68_DMA },
    { 'a', SC68_MFP_TA },
    { 'b', SC68_MFP_TB },
    { 'c', SC68_MFP_TC },
    { 'd', SC68_MFP_TD },
    { 'p', SC68_AGA },
    { 'l', SC68_LMC },
    { 's', SC68_DSP },
    { 't', SC68_BLT },
    { 'h', SC68_HBL },
    { 'x', 0 },                        /* reserved for SFX */
    { 'g', 0 },                        /* reserved for digital only */
    { 'j', 0 },                        /* reserved for jingles */
    { 0,0 }
  };

  int i,j;
  flags[0] = '~';
  for (i=0, j=1; xf[i].letter; ++i)
    if (f & xf[i].mask)
      flags[j++] = xf[i].letter;
  flags[j] = 0;
  return flags;
}

/* Reset format */
static void NoFmt() {
  fmt_l = 0;
  fmt_z = ' ';
}


/* Print a char (byte) */
static void PutC(vfs68_t *out, const int c)
{
  ioerror |= ( vfs68_putc(out, c) < 0 );
}

/* Print string without formating */
static void PutR(vfs68_t *out, const char *s)
{
  if (s) {
    ioerror |= ( vfs68_puts(out, s) < 0 );
  }
}

/* Print a string (honor alignment or length format modifier) */
static void PutS(vfs68_t *out, const char *s)
{
  if (s) {
    int i, len = strlen(s);

    if (fmt_z != '-')
      for (i=0; i+len < fmt_l; ++i)
        ioerror |= ( vfs68_putc(out, fmt_z) < 0 );
    ioerror |= ( vfs68_write(out, s, len) < 0 );
    if (fmt_z == '-')
      for (i=0; i+len < fmt_l; ++len)
        ioerror |= ( vfs68_putc(out, ' ') < 0 );
  }
  NoFmt();
}

/* Print a integer number in given format */
static void PutN(vfs68_t *out, int v, int i)
{
  char fmt[8], *f = fmt;
  int len = fmt_l;

  /* build printf format string */
  *f++ = '%';
  if (len > 0) {
    if (fmt_z != ' ') *f++ = fmt_z;
    *f++ = '*';
  }
  *f++ = i; *f++ = 0;

  len = len > 0
    ? snprintf(fmt_b, fmt_max, fmt, len, v)
    : snprintf(fmt_b, fmt_max, fmt, v)
    ;

  ioerror |= ( vfs68_write(out, fmt_b, len) < 0 );
  NoFmt();
}

static void PutI(vfs68_t *out, int v) {
  PutN(out,v,'u');
}

static void PutX(vfs68_t *out, unsigned int v) {
  PutN(out,v,'x');
}

static void PutX32(vfs68_t *out, unsigned int v)
{
  fmt_z = '0'; fmt_l = 8; PutN(out, v, 'x');
}


static char * get_tag(const disk68_t * dsk, int trk, const char * key)
{
  return file68_tag(dsk, trk, key);
}

/* Output a summary of everything we have on that file/disk. */
static int output_all(vfs68_t * out, disk_t * disk)
{
  disk68_t * d = disk->d;
  const char * inpname = disk->uri;
  int i, j;
  const char * key, * val;

  PutR(out,"file: ");     PutR(out,inpname);     PutC(out,'\n');
  PutR(out,"hash: ");     PutX32(out,d->hash);  PutC(out,'\n');
  PutR(out,"tracks: ");   PutI(out,d->nb_mus);  PutC(out,'\n');
  PutR(out,"default: ");  PutI(out,d->def_mus); PutC(out,'\n');
  if (disk->has_time) {
    PutR(out,"time-ms: ");  PutI(out,d->time_ms); PutC(out,'\n');
  }
  PutR(out,"hardware: "); PutR(out,HWflags(d->hwflags)); PutC(out,'\n');
  for (j=0; !file68_tag_enum(d, 0, j, &key, &val); ++j) {
    PutR(out,key); PutR(out,": "); PutR(out,val); PutC(out,'\n');
  }

  for (i=1; i<=d->nb_mus; ++i) {
    const music68_t *m = d->mus+(i-1);

    PutR(out,"track: ");    PutI(out,i);           PutC(out,'\n');
    /* PutR(out,"remap: ");    PutI(out,m->track);    PutC(out,'\n'); */
    if (m->has.time) {
      PutR(out,"time-ms: ");  PutI(out,m->first_ms); PutC(out,'\n');
      PutR(out,"time-fr: ");  PutI(out,m->first_fr); PutC(out,'\n');
    }
    if (m->has.loop) {
      PutR(out,"loops: ");    PutI(out,m->loops);    PutC(out,'\n');
      PutR(out,"loop-ms: ");  PutI(out,m->loops_ms); PutC(out,'\n');
      PutR(out,"loop-fr: ");  PutI(out,m->loops_fr); PutC(out,'\n');
    }

    if (m->replay) {
      PutR(out,"replay: ");
      PutR(out, m->replay);
      PutC(out,'\n');
    }

    PutR(out,"rate: "); PutI(out,m->frq); PutC(out,'\n');

    PutR(out,"address: ");
    if (m->has.pic)
      PutR(out,"PIC");
    else
      PutX(out,m->a0);
    PutC(out,'\n');

    PutR(out,"hardware: "); PutR(out,HWflags(m->hwflags)); PutC(out,'\n');
    if (m->hwflags & SC68_XTD) {
      PutR(out,"x-hardware: ");
      PutR(out,Xflags(m->hwflags)); PutC(out,'\n');
    }

    for (j=0; !file68_tag_enum(d, i, j, &key, &val); ++j) {
      PutR(out,key); PutR(out,": "); PutR(out,val); PutC(out,'\n');
    }
  }
  return ioerror
    ? output_error(vfs68_filename(out))
    : XCODE_OK
    ;
}

static int load_new_disk(disk_t *disk, const char * inpname)
{
  int has_time,has_loop,j;
  file68_free(disk->d);
  disk->cnt++;
  disk->uri = 0;
  disk->d = file68_load_uri(inpname);
  if (!disk->d)
    return input_error(inpname);
  disk->uri = inpname;

  /* Determine if disk has time and loop for all its tracks */
  for (has_loop = has_time = j = 0; j < disk->d->nb_mus; ++j) {
    has_time += disk->d->mus[j].has.time;
    has_loop += disk->d->mus[j].has.loop;
  }
  disk->has_time = has_time == disk->d->nb_mus;
  disk->has_loop = has_loop == disk->d->nb_mus;
  disk->from = disk->to = disk->d->def_mus;

  return XCODE_OK;
}


static int format_track(vfs68_t * out, char * fmt,
                        const disk_t * disk, int trk)
{
  int esc = 0, c;
  char * s;
  const disk68_t  * d = disk->d;
  const music68_t * m = d->mus + trk;

  /* Parse format string for this track */
  for (s=fmt, esc=0, ioerror=0; !ioerror && (c = *s, c); ++s) {

    if (!esc) {
      /* Not escaped */
      if(esc = (c=='%'), !esc) {
        PutC(out,c);
      } else {
        int d;

        /* Read printf-like format modifiers */
        fmt_l = 0;
        d = s[1];
        if (d == '0' || d == '-') {
          fmt_z = d;
          d = (++s)[1];
        } else fmt_z = ' ';

        while (d >= '0' && d <= '9') {
          ++s;
          fmt_l = fmt_l*10 + d - '0';
          d = s[1];
        }
      }
    } else {
      /* Escaped */
      esc = 0;
      switch (c) {

        /* SPECIAL commands */
      case 'L': case '.':
        PutC(out,'\n');
        break;
      case '%':
        PutC(out,'%');
        break;
      case '0': case 'Z':
        PutC(out,'\0');
        break;

        /* DISK commands */
      case 'I':
        PutI(out,disk->cnt);
        break;
      case 'U':
        PutS(out,disk->uri);
        break;
      case '~':
        PutX32(out,d->hash);
        break;
      case '#':
        PutI(out,d->nb_mus);
        break;
      case '?':
        PutI(out,d->def_mus+1);
        break;
      case 'N':
        PutS(out,d->tags.tag.title.val);
        break;
      case 'A':
        PutS(out,d->tags.tag.artist.val);
        break;
      case 'C':
        PutS(out,file68_tag_get(d,0,TAG68_COMPOSER));
        break;
      case 'P':
        PutS(out,file68_tag_get(d,0,TAG68_RIPPER));
        break;
      case 'V':
        PutS(out,file68_tag_get(d,0,TAG68_CONVERTER));
        break;
      case 'T':
        PutI(out, disk->has_time*d->time_ms/1000u);
        break;
      case 'Y':
        PutS(out,strtime68(0, d->nb_mus, disk->has_time*d->time_ms/1000u));
        break;
      case 'H':
        PutS(out,HWflags(d->hwflags));
        break;
      case 'F':
        PutS(out,file68_tag_get(d,0,TAG68_FORMAT));
        break;

        /* TRACK Commands */
      case '&':
        PutI(out,trk+1);
        break;
      case 'n':
        PutS(out,m->tags.tag.title.val);
        break;
      case 'a':
        PutS(out,m->tags.tag.artist.val);
        break;
      case 'c':
        PutS(out,file68_tag_get(d,trk+1,TAG68_COMPOSER));
        break;
      case 'p':
        PutS(out,file68_tag_get(d,trk+1,TAG68_RIPPER));
        break;
      case 'v':
        PutS(out,file68_tag_get(d,trk+1,TAG68_CONVERTER));
        break;
      case 'r':
        PutS(out, m->replay ? m->replay : "built-in");
        break;
      case 't':
        PutI(out,m->has.time*m->first_ms/1000u);
        break;
      case 'y':
        PutS(out,strtime68(0, trk+1, m->has.time*m->first_ms/1000u));
        break;
      case 'm':
        PutI(out,m->has.time*m->first_fr);
        break;
      case 'h':
        PutS(out, HWflags(m->hwflags));
        break;
      case 'f':
        PutI(out,m->frq);
        break;
      case '@':
        PutX(out,m->a0);
        break;
      case '{': {
        char * key = s+1, * end, * val;
        if (end = strchr(key,'}'), end) {
          int trk = isupper((int)*key) ? 0 : trk+1;
          *end = 0;
          val = get_tag(d, trk, key);
          if (val) PutS(out,val);
          free(val);
          *end = '}';
          s = end;
        } break;
      }
      } /* switch */
    } /* else */
  } /* for */
  return ioerror
    ? output_error(vfs68_filename(out))
    : XCODE_OK
    ;
}


static int do_tracks(vfs68_t * out, char * fmt, const disk_t * disk)
{
  int trk, endtrk;
  for (trk=disk->from, endtrk=disk->to; trk <= endtrk; ++trk) {
    int code = format_track(out, fmt, disk, trk);
    if (code) return code;
  }
  return XCODE_OK;
}

/* Parse a track number
 * return >=0 track number [0..max-1]
 * return <0  on error
 */
static int parse_tracknum(const char **ps, int max)
{
  int n;
  if (!isdigit((int)**ps))
    return -1;
  n = strtol(*ps, (char **)ps, 10);
  if (n < 0 || n > max)
    return -1;
  else if (!n)
    n = max;                            /* 0 is substitued by max */
  return n - 1;
}

/* Process track-list
 * return  0  on end (no interval is available)
 * return <0  on error
 * return  1  on continue (have an interval)
 */
static int tracklist_parse(const char **tracklist, int max,
                           int *from, int *to)
{
  int ft, tt;
  const char *t = *tracklist;

  /* Skip comma ',' */
  if (t) while(*t == ',') ++t;

  /* Done with this list. */
  if (!t || !*t) {
    *tracklist = 0;
    return 0;
  }

  /* Parse a track numbe as from-track */
  *tracklist = t;
  if (ft = parse_tracknum(tracklist, max), ft < 0)
    return -1;

  /* Next char must be a delimiter */
  switch(**tracklist) {
  case ',': case 0:
    /* next is new interval or the end: this interval is a single track */
    tt = ft;
    break;
  case '-':
    /* interval delimiter: parse a 2nd track number for this interval */
    (*tracklist)++;
    if (tt = parse_tracknum(tracklist, max), tt < 0)
      return -2;
    break;
  default:
    /* Anything else is an error. */
    return -1;
  }
  *from = ft;
  *to   = tt;

  return 1;
}

/* return  0 on success
 *        -1 on error
 */
static int tracklist_verify(const char * tracklist)
{
  const char * tl = tracklist; int a,b,c;
  while (c = tracklist_parse(&tl, SC68_MAX_TRACK, &a, &b), c > 0);
  return c;
}

static int do_tracklist(vfs68_t * out, char * fmt, disk_t * disk,
                        const char * tracklist)
{
  int res = XCODE_OK;
  const char * tl = tracklist;

  if (!tl) {
      disk->from = disk->to = disk->d->def_mus;
      res = do_tracks(out, fmt, disk);
  } else {
    while(res = tracklist_parse(&tl, disk->d->nb_mus, &disk->from, &disk->to),
          res > 0) {
      res = do_tracks(out, fmt, disk);
      if (res) break;
    }
  }
  return (res < 0)
    ? tracklist_error(tracklist)
    : res
    ;
}

int main(int argc, char ** argv)
{
  int  i, j, opt_all = 0, code = XCODE_OK;
  int opt_usage = 0, opt_version = 0, opt_verbose = 0;
  char * opt_fmt = 0, * opt_tracks = 0;

  disk_t disk;
  vfs68_t * out = 0;
  const char * inpname = 0;
  const char * outname = "stdout:info68";

  static const struct option longopts[] = {
    { "version",    0, 0, 'V' },
    { "help",       0, 0, 'h' },
    { "usage",      0, 0, 'h' },
    { "verbose",    0, 0, 'v' },
    { "all",        0, 0, 'A' },
    { "output",     1, 0, 'o' },
    { "format",     1, 0, 'f' },
    { "tracks",     1, 0, 't' },
    { 0,0,0,0 }
  };
  static char shortopts[(sizeof(longopts)/sizeof(*longopts))*3+1];

  memset(&disk,0,sizeof(disk));

  /* Create short option list from longs */
  for (j=i=0; longopts[i].name ; ++i) {
    shortopts[j++] = longopts[i].val;
    if (longopts[i].has_arg >= 1)
      shortopts[j++] = ':';
    if (longopts[i].has_arg >= 2)
      shortopts[j++] = ':';
  }
  shortopts[j++] = 0;

  /* Init file68 library */
  msg68_cat_filter(-1, (1<<msg68_INFO)-1);

  argv[0] = prg;
  argc = file68_init(argc, argv);
  if (argc < 0) {
    code = error("file68 initialisation failed.\n");
    goto exit;
  }
  msg68_set_cookie(&opt_verbose);
  msg68_set_handler(message_cb);


  /* Parsing command line with getopt() */
  opterr = 0;                           /* getopt() doesn't print error */
  for (;;) {
    int longindex = 0;
    int val =
      getopt_long(argc, argv, shortopts, longopts, &longindex);

    switch (val) {
    case  -1: break;                /* Scan finish */

    case ':': case '?':             /* Unknown or missing parameter */
      val = optopt;
      if (val)
        error("unknown option -- `%c'\n", val);
      else {
        val = optind - 1;
        error("unknown option -- `%s'\n",
              (val>0 && val<argc) ?
              argv[val] + (argv[val][0]=='-') + (argv[val][1]=='-') : "?");
      }
      code = XCODE_CLI;
      goto finish;

    case 'h':                           /* --help */
      ++opt_usage;
      break;

    case 'V':                           /* --version */
      opt_version = 1;
      break;

    case 'v':                           /* --verbose */
      ++opt_verbose;
      break;

    case 'A':                           /* --all */
      opt_all = 1;
      break;

    case 'o':                           /* --output= */
      outname = optarg;
      break;

    case 'f':                           /* --format= */
      opt_fmt = optarg;
      break;

    case 't':
      opt_tracks = optarg;
      break;

    default:
      error("(internal) unhandled option -- `%c' (%d)\n",
            isgraph(val) ? val:'-', val);
      code = XCODE_CLI;
      goto finish;
    }
    if (val == -1) break;
  }

  /* Where option parsing left. */
  i = optind;

  /* Ask for a bit of help ? */
  if (opt_usage) {
    print_help(opt_usage);
    goto finish;
  }

  /* Ask for version ? */
  if (opt_version) {
    print_version();
    goto finish;
  }

  /* Setup libfile68 verbose level */
  if (opt_verbose > msg68_TRACE)
    opt_verbose = msg68_TRACE;
  msg68_cat_filter(-1,(1<<(opt_verbose+1))-1);


  /* all and format are incompatible. Ignoring format. */
  if (opt_fmt && opt_all)
    opt_fmt = 0;

  /* Verify tracklist format (empty is valid) */
  if (opt_tracks) {
    if (tracklist_verify(opt_tracks) < 0) {
      code |= tracklist_error(opt_tracks);
      goto finish;
    }
  }

  /* Single input mode require at least an input argument */
  if (!opt_fmt) {
    if (i >= argc) {
      error("missing input argument. Try --help.\n");
      code = XCODE_CLI;
      goto finish;
    }
    code = load_new_disk(&disk, inpname = argv[i++]);
    if (code)
      goto finish;
  }

  /* Open output */
  out = uri68_vfs(outname, 2, 0);
  if (vfs68_open(out)) {
    error ("output open error -- `%s'.\n",
           out ? vfs68_filename(out) : outname);
    code = XCODE_OUT;
    goto finish;
  }

  if (opt_fmt) {
    /* Using the multi-input syntax. */

    for ( ; i < argc; ++i ) {
      int newcode = load_new_disk(&disk, inpname = argv[i]);
      if (newcode) {
        code |= newcode;
        continue;                       /* continue on input error */
      }

      newcode = opt_all
        ? output_all(out, &disk)
        : do_tracklist(out, opt_fmt, &disk, opt_tracks)
        ;
      if (newcode) {
        code |= newcode;
        goto finish;                    /* break on anything else */
      }
    }
  }

  else if (opt_all) {
    /* Using all-mode on single input. */
    assert(disk.d && disk.uri);
    code = output_all(out, &disk);
  }

  else {
    /* Using the single input syntax. */

    assert(disk.d && disk.uri);
    if (opt_tracks) {
      /* If tracklist was given don't try to interprete tracklist in
       * the remaining argurments. */
      for (; i<argc; ++i) {
        code = do_tracklist(out, argv[i], &disk, opt_tracks);
        if (code) goto finish;
      }
    } else {
      /* Here is the source of the problem: for backward compatibility
       * we allow tracklist to start tracklist by either `-' or `%'
       * followed by a number. For this reason format string can not
       * be "%0" not "-0" that is a tracklist pointing the last
       * track. However this should not be too inconvenient as these
       * format strings do not really make sense.
       */

      for (; i<argc; ++i) {
        char * arg = argv[i];
        /* Is tracklist ? */
        if ( i+1 < argc /* not last argument */ &&
             (arg[0] == '%' || arg[0] == '-') /* start with % or - */ &&
             ((arg[1] == '0' && !arg[2]) /* "%0" or "-0" */ ||
              (arg[1] >= '1' && arg[1] <= '9')) &&
             tracklist_verify(arg+1) == 0 /* proper format */ ) {
          opt_tracks = arg+1;
          arg = argv[++i];
        }
        code = do_tracklist(out, arg, &disk, opt_tracks);
        if (code) goto finish;
      }
    }
  }

finish:
  vfs68_destroy(out);
  file68_free(disk.d);
  file68_shutdown();

exit:
  return code;
}
