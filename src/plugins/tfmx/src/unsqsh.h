#ifndef __UNSQSH_H__
#define __UNSQSH_H__

#include <glib.h>

guint32 tfmx_sqsh_get_ulen (guint8 *src, guint32 srclen);
void tfmx_sqsh_unpack(guint8 *src, guint8 *dst, gint32 dstlen);

#endif /* __UNSQSH_H__ */
