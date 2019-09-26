#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tfmx.h"
#include "tfmx_iface.h"

#ifdef _WIN32
#include <wtypes.h>
#define strncasecmp strnicmp
#define DIRDELIM '\\'
#else
#include <unistd.h>
#define DIRDELIM '/'
#endif

char IsTFMXFilename(char* fName) {
    int suffixPos;
    char* c;

    if (!fName)
        return 0; /* Check null string */

    if (!(c = strrchr(fName, DIRDELIM)))
        c = fName;
    else
        c++;                   /* Strip dirnames */
    suffixPos = strlen(c) - 4; /* Get filename length */

    if (strncasecmp(c, "mdat.", 5) == 0) {
        return 1;
    } else if (strncasecmp(c, "tfmx.", 5) == 0) {
        return 1;
    } else if (suffixPos >= 0 && strncasecmp(c + suffixPos, ".tfx", 4) == 0) {
        return 1;
    }

    return 0;
}

void TFMXError(char* err) {
    printf("TFMXError %s\n", err);
}
