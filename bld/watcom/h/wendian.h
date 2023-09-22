/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2023 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Include POSIX endian.h header file or appropriate macros.
*
****************************************************************************/


#ifdef __BSD__
    #include <sys/endian.h>
#elif defined( __LINUX__ ) && !defined( __WATCOMC__ )
    #include <endian.h>
#elif defined( __WATCOMC__ ) && ( __WATCOMC__ > 1290 )
    #include <endian.h>
#else
/*
 * Intel systems without endian.h
 * OW 1.9, MSVC, OSX
 * implementation of endian.h content
 */
#define LITTLE_ENDIAN   1234
#define BIG_ENDIAN      4321

#define _SWAPNC_16(w)   \
(\
    (((w) & 0x00FFU) << 8)\
    | (((w) & 0xFF00U) >> 8)\
)
#define _SWAPNC_32(w)   \
(\
    (((w) & 0x000000FFUL) << 24)\
    | (((w) & 0x0000FF00UL) << 8)\
    | (((w) & 0x00FF0000UL) >> 8)\
    | (((w) & 0xFF000000UL) >> 24)\
)
#define _SWAPNC_64(w)   \
(\
    (((w) & 0x00000000000000FFULL) << 56)\
    | (((w) & 0x000000000000FF00ULL) << 40)\
    | (((w) & 0x0000000000FF0000ULL) << 24)\
    | (((w) & 0x00000000FF000000ULL) << 8)\
    | (((w) & 0x000000FF00000000ULL) >> 8)\
    | (((w) & 0x0000FF0000000000ULL) >> 24)\
    | (((w) & 0x00FF000000000000ULL) >> 40)\
    | (((w) & 0xFF00000000000000ULL) >> 56)\
)

#define BYTE_ORDER      LITTLE_ENDIAN

#define htobe16(x) _SWAPNC_16(x)
#define htole16(x) (x)
#define be16toh(x) _SWAPNC_16(x)
#define le16toh(x) (x)

#define htobe32(x) _SWAPNC_32(x)
#define htole32(x) (x)
#define be32toh(x) _SWAPNC_32(x)
#define le32toh(x) (x)

#define htobe64(x) _SWAPNC_64(x)
#define htole64(x) (x)
#define be64toh(x) _SWAPNC_64(x)
#define le64toh(x) (x)
#endif
