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
* Description:  Constants for string processing on RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <string.h>
#include "riscstr.h"


#if USE_INT64

struct __RISC_StrData   __RISC_StringData = {
    /* unsigned_64      byteMasks[8]; */
    {   { 0x000000FF, 0x00000000 },
        { 0x0000FF00, 0x00000000 },
        { 0x00FF0000, 0x00000000 },
        { 0xFF000000, 0x00000000 },
        { 0x00000000, 0x000000FF },
        { 0x00000000, 0x0000FF00 },
        { 0x00000000, 0x00FF0000 },
        { 0x00000000, 0xFF000000 }
    },
    /* unsigned_64      frontCharsMasks[8]; */
    {   { 0x00000000, 0x00000000 },
        { 0x000000FF, 0x00000000 },
        { 0x0000FFFF, 0x00000000 },
        { 0x00FFFFFF, 0x00000000 },
        { 0xFFFFFFFF, 0x00000000 },
        { 0xFFFFFFFF, 0x000000FF },
        { 0xFFFFFFFF, 0x0000FFFF },
        { 0xFFFFFFFF, 0x00FFFFFF },
    },
    /* unsigned_64      skipCharsMasks[8]; */
    {   { 0xFFFFFFFF, 0xFFFFFFFF },
        { 0xFFFFFF00, 0xFFFFFFFF },
        { 0xFFFF0000, 0xFFFFFFFF },
        { 0xFF000000, 0xFFFFFFFF },
        { 0x00000000, 0xFFFFFFFF },
        { 0x00000000, 0xFFFFFF00 },
        { 0x00000000, 0xFFFF0000 },
        { 0x00000000, 0xFF000000 },
    },
    /* unsigned_64      _01Mask; */
    { 0x01010101, 0x01010101 },
    /* unsigned_64      _80Mask; */
    { 0x80808080, 0x80808080 },
    /* unsigned_64      subMask[8]; */
    {   { 0x01010101, 0x01010101 },
        { 0x01010100, 0x01010101 },
        { 0x01010000, 0x01010101 },
        { 0x01000000, 0x01010101 },
        { 0x00000000, 0x01010101 },
        { 0x00000000, 0x01010100 },
        { 0x00000000, 0x01010000 },
        { 0x00000000, 0x01000000 },
    }
};


#else


#ifdef __WIDECHAR__

    struct __wRISC_StrData      __wRISC_StringData = {
        /* uint_32      frontCharsMasks[2]; */
        {
            0x00000000,         /* 0: ignore all chars */
            0x0000FFFF          /* 1: include second char */
        },
        /* uint_32      skipCharsMasks[2]; */
        {
            0xFFFFFFFF,         /* 0: include all chars */
            0xFFFF0000          /* 1: ignore starting char */
        },
        /* uint_32      subMasks[2]; */
        {
            0x00010001,         /* 0: test all characters */
            0x00010000          /* 1: ignore low order char */
        }
    };

#else

    struct __RISC_StrData       __RISC_StringData = {
        /* uint_32      frontCharsMasks[4]; */
        {
            0x00000000,         /* 0: ignore all bytes */
            0x000000FF,         /* 1: include first byte */
            0x0000FFFF,         /* 1: include second byte */
            0x00FFFFFF          /* 1: include third byte */
        },
        /* uint_32      skipCharsMasks[4]; */
        {
            0xFFFFFFFF,         /* 0: include all bytes */
            0xFFFFFF00,         /* 1: ignore first byte */
            0xFFFF0000,         /* 2: ignore 2 starting bytes */
            0xFF000000          /* 3: ignore 3 starting bytes */
        },
        /* uint_32      subMasks[4]; */
        {
            0x01010101,         /* 0: test all characters */
            0x01010100,         /* 1: ignore low order byte */
            0x01010000,         /* 2: ignore 2 low order bytes */
            0x01000000          /* 3: ignore all but high byte */
        }
    };

#endif


//extern UINT __FRONT_BYTES[] = {
//    0x00000000,               /* 0: ignore all bytes */
//    0x000000FF,               /* 1: include first byte */
//    0x0000FFFF,               /* 1: include second byte */
//    0x00FFFFFF                /* 1: include third byte */
//};
//
//extern UINT __SKIP_BYTES[] = {
//    0xFFFFFFFF,               /* 0: include all bytes */
//    0xFFFFFF00,               /* 1: ignore first byte */
//    0xFFFF0000,               /* 2: ignore 2 starting bytes */
//    0xFF000000                /* 3: ignore 3 starting bytes */
//};
//
//extern UINT __SubMask[] = {
//    0x01010101,               /* 0: test all characters */
//    0x01010100,               /* 1: ignore low order byte */
//    0x01010000,               /* 2: ignore 2 low order bytes */
//    0x01000000                /* 3: ignore all but high byte */
//};

#endif
