#ifdef _WIN32

#ifndef GETOPT_H
#define GETOPT_H

int getopt(int nargc, char * const nargv[], const char *ostr) ;

#endif

#else /* _WIN32 */

#include <fnmatch.h>

#endif /* _WIN32 */

