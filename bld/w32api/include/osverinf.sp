/* Operating system version information */
typedef struct _OSVERSIONINFOA {
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    CHAR    szCSDVersion[128];
} OSVERSIONINFOA;
typedef OSVERSIONINFOA      *POSVERSIONINFOA;
typedef OSVERSIONINFOA      *LPOSVERSIONINFOA;
typedef struct _OSVERSIONINFOW {
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    WCHAR   szCSDVersion[128];
} OSVERSIONINFOW;
typedef OSVERSIONINFOW      *POSVERSIONINFOW;
typedef OSVERSIONINFOW      *LPOSVERSIONINFOW;
typedef OSVERSIONINFOW      RTL_OSVERSIONINFOW;
typedef OSVERSIONINFOW      *PRTL_OSVERSIONINFOW;
#ifdef UNICODE
typedef OSVERSIONINFOW      OSVERSIONINFO;
typedef POSVERSIONINFOW     POSVERSIONINFO;
typedef LPOSVERSIONINFOW    LPOSVERSIONINFO;
#else
typedef OSVERSIONINFOA      OSVERSIONINFO;
typedef POSVERSIONINFOA     POSVERSIONINFO;
typedef LPOSVERSIONINFOA    LPOSVERSIONINFO;
#endif

/* Operating system version information (extended version) */
typedef struct _OSVERSIONINFOEXA {
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    CHAR    szCSDVersion[128];
    USHORT  wServicePackMajor;
    USHORT  wServicePackMinor;
    USHORT  wSuiteMask;
    UCHAR   wProductType;
    UCHAR   wReserved;
} OSVERSIONINFOEXA;
typedef OSVERSIONINFOEXA    *POSVERSIONINFOEXA;
typedef OSVERSIONINFOEXA    *LPOSVERSIONINFOEXA;
typedef struct _OSVERSIONINFOEXW {
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    WCHAR   szCSDVersion[128];
    USHORT  wServicePackMajor;
    USHORT  wServicePackMinor;
    USHORT  wSuiteMask;
    UCHAR   wProductType;
    UCHAR   wReserved;
} OSVERSIONINFOEXW;
typedef OSVERSIONINFOEXW    *POSVERSIONINFOEXW;
typedef OSVERSIONINFOEXW    *LPOSVERSIONINFOEXW;
typedef OSVERSIONINFOEXW    RTL_OSVERSIONINFOEXW;
typedef OSVERSIONINFOEXW    *PRTL_OSVERSIONINFOEXW;
#ifdef UNICODE
typedef OSVERSIONINFOEXW    OSVERSIONINFOEX;
typedef POSVERSIONINFOEXW   POSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXW  LPOSVERSIONINFOEX;
#else
typedef OSVERSIONINFOEXA    OSVERSIONINFOEX;
typedef POSVERSIONINFOEXA   POSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXA  LPOSVERSIONINFOEX;
#endif
