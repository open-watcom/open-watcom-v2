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


// definitions used in all ms2wlink files.

typedef enum {
    FALSE = 0,
    TRUE
} bool;

typedef int         f_handle;

#define NIL_HANDLE      ((f_handle)-1)
#define STDIN_HANDLE    ((f_handle)0)
#define STDOUT_HANDLE   ((f_handle)1)
#define STDERR_HANDLE   ((f_handle)2)
#define STDAUX_HANDLE   ((f_handle)3)
#define STDPRN_HANDLE   ((f_handle)4)

#define MAX_LINE (256)
#define FNMAX  80             // maximum file name length.

/*  File Extension formats */
// see DefExt array in utils.c
enum {
    E_OBJECT = 0,
    E_LOAD,
    E_MAP,
    E_LIBRARY,
    E_DEF,
    E_COMMAND
};

enum {
    OBJECT_SLOT = 0,
    RUN_SLOT,
    MAP_SLOT,
    LIBRARY_SLOT,
    DEF_SLOT,
    OPTION_SLOT,
    OVERLAY_SLOT
};

typedef enum {
    FMT_DEFAULT         = 0,
    FMT_DOS             = 1,
    FMT_COM             = 2,
    FMT_OS2             = 3,
    FMT_WINDOWS         = 4
} format_type;

typedef enum {
    NO_EXTRA            = 0,
    DLL_INITGLOBAL      = 1,
    DLL_INITINSTANCE    = 2,
    APP_PMCOMPATIBLE    = 3,
    APP_PM              = 4,
    APP_FULLSCREEN      = 5,
    APP_VIRTDEVICE      = 6
} extra_type;

// structures used in MS2WLINK files.

typedef struct cmdentry {
    struct cmdentry *   next;
    char *              command;
    bool                asis;       // true iff entry should be printed "as is".
} cmdentry;
