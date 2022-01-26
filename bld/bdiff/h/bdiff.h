/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal definitions for bdiff tool.
*
****************************************************************************/


#ifndef __UNIX__
    #include <conio.h>
    #include <process.h>
#endif
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "banner.h"
#include "machtype.h"
#include "patchsig.h"


typedef unsigned_32     foff;
typedef signed_32       foff_diff;
typedef unsigned_32     hole;

#define IsHoleSize( x ) ( ( (x) & (sizeof( hole ) - 1) ) == 0 )

#define WORD_MAX        65535

#define END_SIG_CHAR    '\r'
#define EOF_CHAR        0x1a

typedef enum {
        CMD_DIFFS,
        CMD_SAMES,
        CMD_DONE,
        CMD_HOLES,
        CMD_ITER_HOLES
} patch_cmd;

typedef enum {
    ALG_NOTHING,        /* find differences without extra info */
    ALG_ONLY_NEW,       /* only the new exe's debugging info is available */
    ALG_BOTH,           /* both exe's have debugging info available */
} algorithm;

typedef struct region {
    struct region       *next;
    foff                old_start;
    foff                new_start;
    foff                size;
    hole                diff;
    int                 dependants;
} region;

#define s(x) #x

/*
 * for BPATCH only
 */

#define BUFFER_SIZE     16*1024
#define SECTOR_SIZE     512

typedef struct {
        foff            start;
        size_t          len;
        FILE            *fd;
        const char      *name;
        bool            dirty;
        char            *buff;
} MY_FILE;

#include "bpatch.h"
#include "dopatch.h"

// Memory management

#define bdiff_malloc    malloc
#define bdiff_realloc   realloc
#define bdiff_free      free
