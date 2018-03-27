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

typedef enum {
    WRES_OPEN_RO,
    WRES_OPEN_RW,
    WRES_OPEN_NEW,
    WRES_OPEN_TMP
} wres_open_mode;

typedef struct WResRoutines {                                       /* defaults */
    /* I/O routines */
    FILE            *(*cli_open)(const char *, wres_open_mode);     /* fopen */
    bool            (*cli_close)(FILE *);                           /* fclose */
    size_t          (*cli_read)(FILE *, void *, size_t);            /* fread */
    size_t          (*cli_write)(FILE *, const void *, size_t);     /* fwrite */
    bool            (*cli_seek)(FILE *, long, int );                /* fseek */
    long            (*cli_tell)(FILE *);                            /* ftell */
    bool            (*cli_ioerr)(FILE *,size_t);                    /* ioerr */
    /* memory routines */
    void            *(*cli_alloc)(size_t);                          /* malloc */
    void            (*cli_free)(void *);                            /* free */
} WResRoutines;

#define WResSetRtns( __open, __close, __read, __write, __seek, __tell, __ioerr, __alloc, __free ) \
    WResRoutines WResRtns = { __open, __close, __read, __write, __seek, __tell, __ioerr, __alloc, __free }

/* This is a global variable exported by function FindResources */
extern long     WResFileShift;

#endif
