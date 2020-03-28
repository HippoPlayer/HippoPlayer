#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "support.h"
#include <uade/ossupport.h>

#ifdef _WIN32
#include <uade/dirent_win32.h>
#endif

/*
 * Skip whitespace characters in string starting from offset i. Returns offset
 * j >= i as the next non-whitespace character offset, or -1 if non-whitespace
 * are not found.
 */
static int skipws(const char *s, int i)
{
	while (isspace(s[i]))
		i++;

	if (s[i] == 0)
		return -1;

	return i;
}

/*
 * Skip non-whitespace characters in string starting from offset i. Returns
 * offset j >= i as the next whitespace character offset, or -1 if no
 * whitespace if found.
 */
static int skipnws(const char *s, int i)
{
	while (!isspace(s[i]) && s[i] != 0)
		i++;

	if (s[i] == 0)
		return -1;

	return i;
}

/* Zero terminate the current word. Returns -1 is *s == 0 or the next word
   does not exist. Otherwise returns offset to the beginning of next word. */
int uade_skip_and_terminate_word(char *s, int i)
{
	i = skipnws(s, i);
	if (i < 0)
		return -1;

	/* Zero terminate word */
	s[i] = 0;

	i = skipws(s, i + 1);
	if (i < 0)
		return -1;

	return i;
}

char *uade_xbasename(const char *s)
{
  char *t = strrchr(s, (int) '/');
  if (t == NULL) {
	  t = (char *) s;
  } else {
	  t++;
  }
  return t;
}

/*
 * Split a string into 2 whitespace separated fields returned in "key" and
 * "value". If more than 2 fields are found, they are cut off by zero
 * terminating "key" and "value" inside the string. If "value" is not found,
 * *value is set to NULL. If "key" is not found, *key is set to NULL.
 * If something is found, both *key and *value become pointers inside the
 * string s.
 *
 * Return values:
 *  - 0 if neither "key" nor "value" is found
 *  - 1 if only "key" is found
 *  - 2 if both "key" and "value" are found
 */
int uade_get_two_ws_separated_fields(char **key, char **value, char *s)
{
	int i;

	*key = NULL;
	*value = NULL;

	i = skipws(s, 0);   /* Skip initial whitespace */

	if (i < 0)
		return 0;   /* We got nothing */

	*key = s + i;

	i = uade_skip_and_terminate_word(s, i);

	if (i < 0)
		return 1;   /* We got a "key", but not a "value" */

	*value = s + i;

	uade_skip_and_terminate_word(s, i);

	return 2;           /* We got both a "key" and a "value" */
}

/* Split line with respect to white space. */
char **uade_read_and_split_lines(size_t *nitems, size_t *lineno, FILE *f,
				 const char *delim)
{
	char line[UADE_LINESIZE], templine[UADE_LINESIZE];
	char **items = NULL;
	size_t pos;
	char *sp;
	char *s;

	*nitems = 0;

	while (uade_xfgets(line, sizeof line, f) != NULL) {

		if (lineno != NULL)
			(*lineno)++;

		/* Skip, if a comment line */
		if (line[0] == '#')
			continue;

		/*
		 * strsep() modifies line that it touches, so we make a copy
		 * of it, and then count the number of items on the line
		 */
		strlcpy(templine, line, sizeof(templine));
		sp = templine;
		while ((s = strsep(&sp, delim)) != NULL) {
			if (*s == 0)
				continue;
			(*nitems)++;
		}

		if (*nitems > 0)
			break;
	}

	if (*nitems == 0)
		return NULL;

	items = calloc(*nitems + 1, sizeof items[0]);
	if (items == NULL) {
		uade_warning("No memory for nws items.\n");
		return NULL;
	}

	sp = line;
	pos = 0;
	while ((s = strsep(&sp, delim)) != NULL) {
		if (*s == 0)
			continue;

		items[pos] = strdup(s);
		if (items[pos] == NULL) {
			uade_warning("No memory for an nws item.\n");
			goto error;
		}

		pos++;
	}
	items[pos] = NULL;
	assert(pos == *nitems);
	return items;

error:
	for (pos = 0; pos < *nitems && items[pos] != NULL; pos++)
		free_and_null(items[pos]);
	free_and_null(items);
	return NULL;
}

char *uade_xfgets(char *s, int size, FILE *stream)
{
	char *ret;
	while (1) {
		ret = fgets(s, size, stream);
		/*
		 * Believe it or not, but it is possible that ret == NULL but
		 * EOF did not happened.
		 */
		if (ret != NULL || feof(stream))
			break;
	}
	return ret;
}
