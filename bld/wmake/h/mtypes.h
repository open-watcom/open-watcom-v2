/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  wmake types and its version of <ctypes.h>
*
****************************************************************************/


#ifndef _MTYPES_H
#define _MTYPES_H   1

#include <limits.h>
#include <stddef.h>     /* for things like size_t and such */
#include "bool.h"


/*
 * NOTE:
 *  STRM_PLACEHOLDERS macro depends on STRM_T definition in mstream.h
 *
 *  hold this macro in sync with STRM_T definition
 */
#define STRM_PLACEHOLDERS   4

/*
 * provide machine independant definitions for different quantities
 */
#ifndef _WINDOWS_H
typedef int_32          INT32;
typedef uint_32         UINT32;
#endif

typedef int_16          INT16;
typedef uint_16         UINT16;
typedef int_8           INT8;
typedef uint_8          UINT8;

typedef uint_8          byte;

typedef struct Node {
    struct Node *next;
    char        *name;
} NODE;                     /* for singly linked lists */

typedef enum {              /* our return type */
    RET_SUCCESS,            /* successful return from a function */
    RET_WARN,               /* warning return from a function    */
    RET_ERROR               /* error return from a function      */
} RET_T;

enum {
    IS_WS       =   0x01,
    IS_PRINT    =   0x02,
    IS_ALPHA    =   0x04,
    IS_WILDC    =   0x08,
    IS_DIRC     =   0x10,
    IS_FILEC    =   0x20,
    IS_MACC     =   0x40,
    IS_BARF     =   0x80
};

#define sisascii(__s)       ((__s) >= CHAR_MIN && (__s) <= CHAR_MAX)

extern const UINT8 IsArray[256 + STRM_PLACEHOLDERS];

#define sisws(__s)          (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_WS)
#define sisprint(__s)       (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_PRINT)
#define sisalpha(__s)       (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_ALPHA)
#define sisextc(__s)        (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_FILEC)
#define sisdirc(__s)        (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_DIRC)
#define sisfilec(__s)       (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_FILEC)
#define sismacc(__s)        (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_MACC)
#define sisbarf(__s)        (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_BARF)
#define siswildc(__s)       (IsArray[(__s)+STRM_PLACEHOLDERS] & IS_WILDC)
#define sisdotc(__s)        ((__s) == '.')

#define cisws(__c)          sisws((byte)(__c))
#define cisprint(__c)       sisprint((byte)(__c))
#define cisalpha(__c)       sisalpha((byte)(__c))
#define cisextc(__c)        sisextc((byte)(__c))
#define cisdirc(__c)        sisdirc((byte)(__c))
#define cisfilec(__c)       sisfilec((byte)(__c))
#define cismacc(__c)        sismacc((byte)(__c))
#define cisbarf(__c)        sisbarf((byte)(__c))
#define ciswildc(__c)       siswildc((byte)(__c))
#define cisdotc(__s)        sisdotc((byte)(__c))

#define ctolower(__c)       (byte)tolower((byte)(__c))
#define ctoupper(__c)       (byte)toupper((byte)(__c))

#endif  /* !_MTYPES_H */
