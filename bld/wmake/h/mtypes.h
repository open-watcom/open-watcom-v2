/****************************************************************************
*
*                            Open Watcom Project
*
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

typedef unsigned        BIT;        /* for bit fields in structures */

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
    IS_EXTC     =   0x08,
    IS_DIRC     =   0x10,
    IS_FILEC    =   0x20,
    IS_MACC     =   0x40,
    IS_BARF     =   0x80
};

#define sisascii(__s)       ((__s) >= CHAR_MIN && (__s) <= CHAR_MAX)

extern const UINT8 IsArray[258];

#define sisws(__s)          (IsArray[(__s)+2] & IS_WS)
#define sisprint(__s)       (IsArray[(__s)+2] & IS_PRINT)
#define sisalpha(__s)       (IsArray[(__s)+2] & IS_ALPHA)
#define sisextc(__s)        (IsArray[(__s)+2] & IS_EXTC)
#define sisdirc(__s)        (IsArray[(__s)+2] & IS_DIRC)
#define sisfilec(__s)       (IsArray[(__s)+2] & IS_FILEC)
#define sismacc(__s)        (IsArray[(__s)+2] & IS_MACC)
#define sisbarf(__s)        (IsArray[(__s)+2] & IS_BARF)

#define cisws(__c)          sisws((byte)(__c))
#define cisprint(__c)       sisprint((byte)(__c))
#define cisalpha(__c)       sisalpha((byte)(__c))
#define cisextc(__c)        sisextc((byte)(__c))
#define cisdirc(__c)        sisdirc((byte)(__c))
#define cisfilec(__c)       sisfilec((byte)(__c))
#define cismacc(__c)        sismacc((byte)(__c))
#define cisbarf(__c)        sisbarf((byte)(__c))

#define ctolower(__c)       tolower((byte)(__c))
#define ctoupper(__c)       toupper((byte)(__c))

#endif  /* !_MTYPES_H */
