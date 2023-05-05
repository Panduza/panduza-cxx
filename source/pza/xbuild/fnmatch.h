#pragma once
#ifdef _WIN32

#ifndef FNMATCH_H
#define FNMATCH_H
#ifdef __cplusplus
extern "C" {
#endif
bool fnmatch(const char *pattern, const char *string, int flags);
#ifdef __cplusplus
}
#endif
#endif FNMATCH_H


#else /* _WIN32 */

#include <fnmatch.h>

#endif /* _WIN32 */
