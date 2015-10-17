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
* Description:  Internal definitions for bdiff tool.
*
****************************************************************************/


#ifndef __UNIX__
#include <conio.h>
#include <process.h>
#endif
#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define s(x) #x

/*
 * for BPATCH only
 */

#define BUFFER_SIZE     16*1024
#define SECTOR_SIZE     512

typedef struct {
        foff            start;
        unsigned        len;
        int             fd;
        const char      *name;
        bool            dirty;
        char            buff[BUFFER_SIZE];
} MY_FILE;

#include "bpatch.h"
#include "dopatch.h"

// Memory management

extern void             *bdiff_malloc( size_t );
extern void             *bdiff_realloc( void *, size_t );
extern void             bdiff_free( void * );
