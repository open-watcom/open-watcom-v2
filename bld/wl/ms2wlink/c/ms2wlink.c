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


/*
 *  MS2WLINK : translator between microsoft linker commmands and WATCOM
 *             linker commands.
*/

#include <string.h>
#include "ms2wlink.h"
#include "banner.h"

// this array contains linked lists of the commands which are to be put into the
// wlink command file. The data stored in the indices is as followes:
// 0 == object file names
// 1 == run file name
// 2 == map file name
// 3 == library file names
// 4 == definitions file name  (later definition file commands
// 5 == linker options.
// 6 == overlays.

cmdentry *  Commands[ 7 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
char *      FormatNames[] = {
    "",
    "dos",
    "com",
    "os2 ",
    "windows ",
    "os2v2 ",
    "nt "
};

char *      ExtraNames[] = {
    "dll initglobal",
    "dll initinstance",
    "pmcompatible",
    "pm",
    "fullscreen",
    "virtdevice"
};

bool            DebugInfo = FALSE;
bool            MapOption = FALSE;
format_type     FmtType = FMT_DEFAULT;
extra_type      FmtInfo = NO_EXTRA;
bool            HaveDefFile = FALSE;

extern void         MemInit( void );
extern void         MemFini( void );
extern void         MemFree( void * );
extern void         Error( char * );
extern void         CommandOut( char * );
extern int          Spawn( void (*fn)() );
extern void         ParseMicrosoft( void );
extern bool         InitParsing( void );
extern void         FreeParserMem( void );
extern void         UtilsInit( void );
extern char *       Msg2Splice( char *, char * );
extern char *       Msg3Splice( char *, char *, char * );
extern void         ImplyFormat( format_type );

static void         DoConvert( void );

extern void main( void )
/**********************/
{
    MemInit();
    UtilsInit();
    Spawn( DoConvert );
    FreeMemory();
    MemFini();
}

static void DoConvert( void )
/***************************/
{
    if( !InitParsing() ) {
        WriteHelp();
    } else {
        ParseMicrosoft();      // most of the work is done here.
        BuildWATCOM();
    }
}

static void BuildWATCOM( void )
/*****************************/
// generate a watcom linker command file & write it to the screen.
{
    char *  form;

    CommandOut( "# This file produced by MS2WLINK" );
    if( DebugInfo ) {
        CommandOut( "debug all" );
    }
    ImplyFormat( HaveDefFile ? FMT_OS2 : FMT_DOS );
    if( FmtInfo != NO_EXTRA ) {
        form = Msg3Splice( "system ", FormatNames[ FmtType ],
                                                   ExtraNames[ FmtInfo - 1 ] );
    } else {
        form = Msg2Splice( "system ", FormatNames[ FmtType ] );
    }
    CommandOut( form );
    MemFree( form );
    PrefixWrite( Commands[ OBJECT_SLOT ], "file ", 5 );
    if( Commands[ OVERLAY_SLOT ] != NULL ) {
        CommandOut( "begin" );
        PrefixWrite( Commands[ OVERLAY_SLOT ], "    file ", 9 );
        CommandOut( "end" );
    }
    PrefixWrite( Commands[ RUN_SLOT ], "name ", 5 );
    if( Commands[ MAP_SLOT ] != NULL ) {
        PrefixWrite( Commands[ MAP_SLOT ], "option map=", 11 );
        MapOption = FALSE;     // make sure it isn't generated a second time.
    }
    PrefixWrite( Commands[ LIBRARY_SLOT ], "library ", 8 );
    if( MapOption ) {
        CommandOut( "option map" );
    }
    ListWrite( Commands[ OPTION_SLOT ] );
}

static void PrefixWrite( cmdentry *cmdlist, char *prefix, int len )
/*****************************************************************/
{
    char        buffer[ FNMAX + 14 ];
    char *      after;

    if( cmdlist != NULL ) {
        memcpy( buffer, prefix, len );
        after = buffer + len;       // the spot after the prefix.
        for(; cmdlist != NULL; cmdlist = cmdlist->next ) {
            if( cmdlist->asis ) {
                CommandOut( cmdlist->command );
            } else {
                memcpy( after, cmdlist->command, strlen(cmdlist->command) + 1);
                CommandOut( buffer );
            }
        }
    }
}

static void ListWrite( cmdentry *cmdlist )
/****************************************/
// write out a list of commands without a prefix.
{
    for(;cmdlist != NULL ; cmdlist = cmdlist->next ) {
        CommandOut( cmdlist->command );
    }
}

static void FreeMemory( void )
/****************************/
{
    int         index;
    cmdentry *  cmd;
    cmdentry *  nextone;

    FreeParserMem();
    for( index = 0; index < 7; index++ ) {
        cmd = Commands[ index ];
        while( cmd != NULL ) {
            MemFree( cmd->command );
            nextone = cmd->next;
            MemFree( cmd );
            cmd = nextone;
        }
    }
}

#define NL "\r\n"
static char TheHelp[] = {
    banner1( "Microsoft to watcom linker command translation utility ",
                _MS2WLINK_VERSION_ ) NL
    banner2( "1990" ) NL
    banner3 NL
    "usage: pass this program the same arguments that would be passed to the" NL
    "       Microsoft linker, and an equivalent set of WATCOM linker commands" NL
    "       will be written to standard output." NL
};

extern void WriteHelp( void )
/***************************/
{
    CommandOut( TheHelp );
}
