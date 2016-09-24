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
* Description:  Definitions used in all ms2wlink files.
*
****************************************************************************/


#include "bool.h"

typedef int         f_handle;

#define NIL_HANDLE      ((f_handle)-1)
#define STDIN_HANDLE    STDIN_FILENO
#define STDOUT_HANDLE   STDOUT_FILENO
#define STDERR_HANDLE   STDERR_FILENO

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

typedef enum {
    OBJECT_SLOT = 0,
    RUN_SLOT,
    MAP_SLOT,
    LIBRARY_SLOT,
    DEF_SLOT,
    OPTION_SLOT,
    OVERLAY_SLOT
} prompt_slot;

typedef enum {
    FMT_DEFAULT         = 0,
    FMT_DOS             = 1,
    FMT_COM             = 2,
    FMT_OS2             = 3,
    FMT_WINDOWS         = 4,
    FMT_WINVXD          = 5,
    FMT_WINVXDDYN       = 6
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
    struct cmdentry     *next;
    char                *command;
    bool                asis;       // true iff entry should be printed "as is".
} cmdentry;

extern bool         MapOption;
extern format_type  FmtType;
extern extra_type   FmtInfo;
extern bool         DebugInfo;

extern cmdentry     *Commands[];
extern const char   *PromptText[];
extern bool         HaveDefFile;

extern format_type  FmtType;
extern bool         MapOption;

// mem.c
extern void     MemInit( void );
extern void     MemFini( void );
extern void     MemFree( void * );
extern void     *MemAlloc( size_t );

// fileio.h
extern f_handle QOpenR( const char *name );
extern size_t   QRead( f_handle file, void *buffer, size_t len, const char *name );
extern size_t   QWrite( f_handle file, const void *buffer, size_t len, const char *name );
extern void     QWriteNL( f_handle file, const char *name );
extern void     QClose( f_handle file, const char *name );
extern unsigned long QFileSize( f_handle file );
extern bool     QReadStr( f_handle file, char *dest, size_t size, const char *name );
extern bool     QIsConIn( f_handle file );
extern void     ErrorOut( const char *msg );
extern void     ErrorExit( const char *msg );
extern void     CommandOut( const char *command );
extern void     QSetBinary( f_handle file );

// keyword.c
extern bool     GetNumber( unsigned long *val );
extern void     ProcessDefCommand( void );
extern void     ProcessOption( const char *opt );

// ms2wlink.c
extern void     WriteHelp( void );

// parsems.c
extern void     EatWhite( void );
extern bool     InitParsing( void );
extern void     FreeParserMem( void );
extern void     ParseDefFile( void );
extern void     StartNewFile( char *fname );
extern void     DirectiveError( void );
extern char     *ToString( void );
extern void     ParseMicrosoft( void );

// utils.c
extern void     UtilsInit( void );
extern void     ImplyFormat( format_type typ );
extern char     *FileName( const char *buff, int etype, bool force );
extern void     AddCommand( char *msg, int prompt, bool verbatim );
extern void     Warning( const char *msg, int prompt );
extern void     AddOption( const char *msg );
extern void     AddNumOption( const char *msg, unsigned value );
extern void     AddStringOption( const char *msg, const char *string, size_t len );
extern void     NotSupported( const char *msg );
extern void     NotNecessary( const char *msg );
extern void     NotRecognized( const char *msg );
extern char     *Msg2Splice( const char *msg1, const char *msg2 );
extern char     *Msg3Splice( const char *msg1, const char *msg2, const char *msg3 );
extern char     *FindNotAsIs( int slot );
extern char     *FindObjectName( void );
extern void     OutPutPrompt( int prompt );
extern int      Spawn( void (*fn)( void ) );
extern void     Suicide( void );
