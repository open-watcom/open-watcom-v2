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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _MTYPES_H
#define _MTYPES_H   1
#include <limits.h>
#include <stddef.h>     /* for things like size_t and such */

#ifndef NDEBUG
#include "massert.h"
#endif


/*
 * provide machine independant definitions for different quantities
 */
typedef   signed long   INT32;
typedef unsigned long   UINT32;
typedef   signed short  INT16;
typedef unsigned short  UINT16;
typedef   signed char   INT8;
typedef unsigned char   UINT8;

typedef unsigned        BIT;        /* for bit fields in structures */

typedef unsigned char   BOOLEAN;
enum {
    FALSE = (0==1),
    TRUE = (0==0)
};


typedef struct Node NODE;       /* for singly linked lists */
struct Node {
    NODE        *next;
    char        *name;
};


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


#define isascii(__c)    ((__c) >= CHAR_MIN && (__c) <= CHAR_MAX)

extern const UINT8 IsArray[258];

#define isws(__c)           (IsArray[(__c)+2] & IS_WS)
#define isprint(__c)        (IsArray[(__c)+2] & IS_PRINT)
#define isalpha(__c)        (IsArray[(__c)+2] & IS_ALPHA)
#define isextc(__c)         (IsArray[(__c)+2] & IS_EXTC)
#define isdirc(__c)         (IsArray[(__c)+2] & IS_DIRC)
#define isfilec(__c)        (IsArray[(__c)+2] & IS_FILEC)
#define ismacc(__c)         (IsArray[(__c)+2] & IS_MACC)
#define isbarf(__c)         (IsArray[(__c)+2] & IS_BARF)

/*
 * Since we have redifined a bunch of things from ctype.h, we can't
 * include ctype.h for toupper() and tolower().  BUT... this is ANSI
 * so we SHOULD be able to do this...
 */
extern int toupper( int );

#endif  /* !_MTYPES_H */
