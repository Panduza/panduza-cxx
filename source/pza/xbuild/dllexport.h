#ifdef _WIN32

// This is required to build the shared lib in windows
#define PZA_DllExport __declspec( dllexport )

#else

#define PZA_DllExport

#endif
