/* Macro to make a FOURCC value */
#ifndef MAKEFOURCC
    #define MAKEFOURCC( p1, p2, p3, p4 ) \
        ((DWORD)(BYTE)(p1) | ((DWORD)(BYTE)(p2) << 8) | ((DWORD)(BYTE)(p3) << 16) | \
        ((DWORD)(BYTE)(p4) << 24))
#endif
