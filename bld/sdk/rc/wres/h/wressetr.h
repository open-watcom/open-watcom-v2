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


#ifndef WRESSETRTNS_INCLUDED
#define WRESSETRTNS_INCLUDED

#ifndef WRESLAYER0_INCLUDED
/* The low level I/O routines named below will be passed a WResFileID by the */
/* higher level I/O routines and which must be the WResFileID returned by one */
/* of the file opening functions which will get it from the low level open */
/* function */

typedef int             WResFileID;

struct WResRoutines {                                       /* defaults */
    /* I/O routines */
    WResFileID (*   open) (const char *, int, ...);         /* open */
    int (*          close) (WResFileID);                    /* close */
    int (*          write) (WResFileID, const void *, size_t); /* write */
    int (*          read) (WResFileID, void *, size_t);     /* read */
    off_t (*        seek) (WResFileID, off_t, int );        /* lseek */
    off_t (*        tell) (WResFileID);                     /* tell */
    /* memory routines */
    void * (*       alloc) (size_t);                        /* malloc */
    void (*         free) (void *);                         /* free */
};
#endif

#define WResSetRtns( open, close, read, write, seek, tell, alloc, free ) \
                struct WResRoutines WResRtns = {    \
                    open,                           \
                    close,                          \
                    write,                          \
                    read,                           \
                    seek,                           \
                    tell,                           \
                    alloc,                          \
                    free                            \
                }
#endif
