# ,----------------------------------------------------------------------.
# |                                                                      |
# |   io68 -- Makefile.am -- Use with automake to produce configure.in   |
# |                                                                      |
# |               Copyright (c) 1998-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I ../m4 --install

mysources=\
 io68.c mfp_io.c mfpemul.c mw_io.c mwemul.c paula_io.c paulaemul.c	\
 shifter_io.c ym_envel.c ym_blep.c ym_dump.c ym_io.c ym_puls.c		\
 ymemul.c

myheaders=\
 io68_private.h default.h io68.h io68_api.h mfp_io.h mfpemul.h	\
 mw_io.h mwemul.h paula_io.h paulaemul.h shifter_io.h ym_blep.h	\
 ym_dump.h ym_fixed_vol.h ym_io.h ym_puls.h ymemul.h ymoutorg.h	\
 ymout2k9.h

srcextra= ym_atarist_table.c ym_linear_table.c

# ,----------------------------------------------------------------------.
# | Libtool library                                                      |
# `----------------------------------------------------------------------'

if SOURCE_FILE68
file68_CPPFLAGS = -I$(top_builddir)/$(file68_builddir)
endif

noinst_LTLIBRARIES  = libio68.la
libio68_la_SOURCES  = $(mysources) $(myheaders)
libio68_la_LDFLAGS  = -avoid-version $(gb_LDFLAGS)
libio68_la_CFLAGS   = $(file68_CFLAGS) $(gb_CFLAGS)
libio68_la_CPPFLAGS = -I$(top_srcdir) $(file68_CPPFLAGS)

noinst_HEADERS      = $(srcextra)
