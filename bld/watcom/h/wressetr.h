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

/* The low level I/O routines named below will be passed a file handle by the */
/* higher level I/O routines and which must be the file handle returned by one */
/* of the file opening functions which will get it from the low level open */
/* function */

#if !defined( __WATCOMC__ ) && defined( __UNIX__ )
#define WResFileSSize   ssize_t
#define WResFileSize    size_t
#define WResFileOffset  off_t
#else
#define WResFileSSize   int
#define WResFileSize    unsigned
#define WResFileOffset  long
#endif

typedef int             WResFileID;

#define NIL_HANDLE      ((WResFileID)-1)

typedef struct WResRoutines {                                               /* defaults */
    /* I/O routines */
    WResFileID      (*cli_open)(const char *, int, ...);                    /* open */
    int             (*cli_close)(WResFileID);                               /* close */
    WResFileSSize   (*cli_read)(WResFileID, void *, WResFileSize);          /* read */
    WResFileSSize   (*cli_write)(WResFileID, const void *, WResFileSize);   /* write */
    WResFileOffset  (*cli_seek)(WResFileID, WResFileOffset, int );          /* lseek */
    WResFileOffset  (*cli_tell)(WResFileID);                                /* tell */
    /* memory routines */
    void            *(*cli_alloc)(size_t);                                  /* malloc */
    void            (*cli_free)(void *);                                    /* free */
} WResRoutines;

#define WResSetRtns( __open, __close, __read, __write, __seek, __tell, __alloc, __free ) \
    struct WResRoutines WResRtns = {    \
        __open,                         \
        __close,                        \
        __read,                         \
        __write,                        \
        __seek,                         \
        __tell,                         \
        __alloc,                        \
        __free                          \
    }

/* This is a global variable exported by function FindResources */
extern WResFileOffset   FileShift;

#endif
