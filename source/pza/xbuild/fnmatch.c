#ifdef _WIN32

#include <windows.h>
#include <stdbool.h>
#include <shlwapi.h>

bool fnmatch(const char *pattern, const char *string, int flags)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    bool match = false;
    
    hFind = FindFirstFileA(string, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (PathMatchSpecA(findData.cFileName, pattern)) {
                match = true;
                break;
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    
    return match;
}


#endif // _WIN32