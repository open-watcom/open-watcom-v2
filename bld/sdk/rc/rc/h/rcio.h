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


#ifndef RCIO_INCLUDED
#define RCIO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "watcom.h"
#include "wresall.h"
#include "types.h"
#include "semstr.h"
#include "semsingl.h"
#include "sharedio.h"
#ifdef UNIX
    #include "clibext.h"
#endif

typedef struct RcResFileID {
    char                *filename;
    int                 IsWatcomRes     : 1;
    int                 IsOpen          : 1;
    FullStringTable *   StringTable;
    FullStringTable *   ErrorTable;
    uint_16             NextCurOrIcon;
    WResDir             dir;                    /* don't write this if !IsWatcomRes */
    int                 handle;                 /* posix level I/O handle */
    FullFontDir *       FontDir;
    char                namebuf[ _MAX_PATH ];   /* filename usually points here */
} RcResFileID;

/**** Text file input ****/
typedef struct LogicalFileInfo {
    char        Filename[ _MAX_PATH ];
    int         LineNum;
    int         IsCOrHFile;
} LogicalFileInfo;

#define RC_EOF (-1)

extern void RcTmpFileName( char * tmpfilename );
extern int RcPass1IoInit( void );
extern void RcPass1IoShutdown( void );
extern int RcPass2IoInit( void );
extern void RcPass2IoShutdown( int noerror );
extern void RcIoPrintBanner( void );
extern void RcIoPrintHelp( const char * );
extern void RcIoTextInputInit( void );
extern int RcIoTextInputShutdown( void );
extern int RcIoPushInputFile( char * filename );
extern int RcIoPopInputFile( void );
extern int RcIoGetChar( void );
extern void RcIoOverrideIsCOrHFlag( void );
extern void RcIoSetIsCOrHFlag( void );
extern const LogicalFileInfo * RcIoGetLogicalFileInfo( void );
extern int RcIoIsCOrHFile( void );
extern void RcIoSetLogicalFileInfo( int linenum, char * filename );
extern int RcIoOpenInput( char * filename, int flags, ... );
extern void RcFindResource( char *name, char *fullpath );
extern char *RcGetEnv( const char *name );

#endif
