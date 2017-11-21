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
* Description:  Client routines setup for wres library.
*
****************************************************************************/


#ifndef WRESSETR_INCLUDED
#define WRESSETR_INCLUDED

#if !defined( _WIN64 ) && !defined( __WATCOMC__ ) && defined( __UNIX__ )
#include <sys/types.h>
#endif
#include <stdio.h>

/* The low level I/O routines named below will be passed a file handle by the */
/* higher level I/O routines and which must be the file handle returned by one */
/* of the file opening functions which will get it from the low level open */
/* function */

typedef FILE                *WResFileID;
#if defined( _WIN64 )
typedef long                WResFileOffset;
#elif !defined( __WATCOMC__ ) && defined( __UNIX__ )
typedef off_t               WResFileOffset;
#else
typedef long                WResFileOffset;
#endif

typedef enum {
    WRES_OPEN_RO,
    WRES_OPEN_RW,
    WRES_OPEN_NEW,
} wres_open_mode;

typedef struct WResRoutines {                                           /* defaults */
    /* I/O routines */
    WResFileID      (*cli_open)(const char *, wres_open_mode);          /* open */
    bool            (*cli_close)(WResFileID);                           /* close */
    size_t          (*cli_read)(WResFileID, void *, size_t);            /* read */
    size_t          (*cli_write)(WResFileID, const void *, size_t);     /* write */
    bool            (*cli_seek)(WResFileID, WResFileOffset, int );      /* lseek */
    WResFileOffset  (*cli_tell)(WResFileID);                            /* tell */
    bool            (*cli_ioerr)(WResFileID,size_t);                    /* ioerr */
    /* memory routines */
    void            *(*cli_alloc)(size_t);                              /* malloc */
    void            (*cli_free)(void *);                                /* free */
} WResRoutines;

#define WResSetRtns( __open, __close, __read, __write, __seek, __tell, __ioerr, __alloc, __free ) \
    WResRoutines WResRtns = { __open, __close, __read, __write, __seek, __tell, __ioerr, __alloc, __free }

/* This is a global variable exported by function FindResources */
extern WResFileOffset   WResFileShift;

#endif
