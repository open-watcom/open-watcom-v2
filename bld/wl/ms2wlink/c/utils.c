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
 *  UTILS : utility routines for the MS linker file translator
 *
*/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>
#include "ms2wlink.h"

static  char *  DefExt[] = {
    ".obj",
    ".exe",
    ".map",
    ".lib",
    ".def",
    ".lnk",
    ".obj"              // for overlay object files.
};

extern char *   PromptText[] = {
    "object modules ",
    "run file ",
    "list file ",
    "libraries ",
    "definitions file "
};

static bool     WritePrompt;

extern void *   MemAlloc( unsigned );
extern void     MemFree( void * );
extern void     Error( char * );
extern unsigned QWrite( f_handle, void *, unsigned, char * );
extern bool     QIsConIn( f_handle );
extern void     QSetBinary( f_handle );

extern char *   Msg2Splice( char *, char * );
extern char *   Msg3Splice( char *, char *, char * );

extern format_type  FmtType;
extern cmdentry *   Commands[];
extern bool         MapOption;

extern void UtilsInit( void )
/***************************/
// check to see if STDIN is the console. if not, don't write prompt.
{
    QSetBinary( STDIN_HANDLE );
    WritePrompt = QIsConIn( STDIN_HANDLE );
}

void ImplyFormat( format_type typ )
/*********************************/
{
    if( FmtType == FMT_DEFAULT ) FmtType = typ;
}

extern char *FileName( char *buff, int len, char etype, bool force )
/******************************************************************/
{
    char                *namptr;
    char                *ptr;
    int                 cnt;

    namptr = buff + len;
    cnt = 0;
    while( cnt != len ) {
        cnt++;
        if( *--namptr == '\\' ) break;
    }
    if( *namptr == '\\' ) {
        namptr++;
    }
    cnt = len - ( (int) namptr - (int) buff );
    namptr = buff + len;
    while( *namptr != '.' && --cnt != 0 ) {
        namptr--;
    }
    if( *namptr != '.' || force ) {
        if( cnt != 0 ) {
            len = cnt;
        }
        ptr = MemAlloc( len + strlen( DefExt[ etype ] ) + 1 );
        memcpy( ptr, buff, len );
        strcpy( ptr + len, DefExt[ etype ] );
    } else {
        ptr = MemAlloc( len + 1 );
        memcpy( ptr, buff, len );
        ptr[ len ] = '\0';
    }
    return( ptr );
}

extern void Warning( char *msg, int prompt )
/******************************************/
// print a warning to the linker command file in the form of a linker comment.
{
    AddCommand( Msg2Splice( "# ", msg ), prompt, TRUE );
}

extern void AddNumOption( char *msg, unsigned value )
/****************************************************/
{
    char    buffer[ 7 ];
    char *  msg2;

    utoa( value, buffer, 10 );
    msg2 = Msg3Splice( msg, "=", buffer );
    AddOption( msg2 );
    MemFree( msg2 );
}

extern void AddStringOption( char *msg, char *string, int len )
/*************************************************************/
{
    char *  cmd;
    int     msglen;
    char *  tmp;

    msglen = strlen( msg );
    cmd = alloca( len + msglen + 2 );
    memcpy( cmd, msg, msglen );
    tmp = cmd + msglen;
    *tmp++ = '=';
    memcpy( tmp, string, len );
    tmp += len;
    *tmp = '\0';
    AddOption( cmd );
}

extern void NotSupported( char *msg )
/***********************************/
{
    char * msg2;

    msg2 = Msg2Splice( msg, " option is not supported by WLINK" );
    Warning( msg2, OPTION_SLOT );
    MemFree( msg2 );
}

extern void NotNecessary( char *msg )
/***********************************/
{
    char * msg2;

    msg2 = Msg2Splice( msg, " option is not necessary when using WLINK" );
    Warning( msg2, OPTION_SLOT );
    MemFree( msg2 );
}

extern void AddOption( char *msg )
/********************************/
{
    AddCommand( Msg2Splice( "option ", msg ), OPTION_SLOT, TRUE );
}

extern char * Msg2Splice( char *msg1, char *msg2 )
/************************************************/
// splice 2 messages together
{
    int     len1;
    int     len2;
    char *  both;

    len1 = strlen( msg1 );
    len2 = strlen( msg2 );
    both = MemAlloc( len1 + len2 + 1 );
    memcpy( both, msg1, len1 );
    memcpy( both+len1, msg2, len2 );
    *(both + len1 + len2) = '\0';
    return( both );
}

extern char * Msg3Splice( char *msg1, char *msg2, char *msg3 )
/************************************************************/
{
    int     len1;
    int     len2;
    int     len3;
    char *  all;

    len1 = strlen( msg1 );
    len2 = strlen( msg2 );
    len3 = strlen( msg3 );
    all = MemAlloc( len1 + len2 + len3 + 1 );
    memcpy( all, msg1, len1 );
    memcpy( all+len1, msg2, len2 );
    memcpy( all+len1+len2, msg3, len3 );
    *(all+len1+len2+len3) = '\0';
    return( all );
}

extern void AddCommand( char *msg, int prompt, bool verbatim )
/************************************************************/
{
    cmdentry *  cmd;
    cmdentry *  list;

    cmd = MemAlloc( sizeof( cmdentry ) );
    cmd->command = msg;
    cmd->asis = verbatim;
    cmd->next = NULL;
    list = Commands[ prompt ];
    if( list == NULL ) {
        Commands[ prompt ] = cmd;
    } else {                         // always add at the end of the list.
        while( list->next != NULL ) {
            list = list->next;
        }
        list->next = cmd;
    }
}

extern char * FindNotAsIs( int slot )
/***********************************/
// search through the given slot for a command which isn't marked "asis"
// since comments are "asis", this can be used to determine if a filename is
// specified, and what that filename is.
{
    cmdentry *  cmd;

    for( cmd = Commands[ slot ]; cmd != NULL; cmd = cmd->next ) {
        if( !cmd->asis ) {
            return( cmd->command );
        }
    }
    return( NULL );
}

static char * FindObjectName( void )
/**********************************/
{
    char *  msg;

    msg = FindNotAsIs( OBJECT_SLOT );
    if( msg == NULL ) {
        msg = FindNotAsIs( OVERLAY_SLOT );
        if( msg == NULL ) {
            Error( "no object files specified" );
        }
    }
    return( msg );
}

extern void OutPutPrompt( int prompt )
/************************************/
{
    char *  msg;

    if( !WritePrompt ) return;
    msg = NULL;
    switch( prompt ) {
    case RUN_SLOT:
        msg = FindObjectName();
        msg = FileName( msg, strlen( msg ), E_LOAD, TRUE );
        PromptStart( msg, prompt );
        MemFree( msg );
        break;
    case MAP_SLOT:
        if( MapOption ) {
            msg = FindNotAsIs( RUN_SLOT );
            if( msg == NULL ) {
                msg = FindObjectName();
            }
            msg = FileName( msg, strlen( msg ), E_MAP, TRUE );
            PromptStart( msg, prompt );
            MemFree( msg );
            break;
        }         // note the possible fall through.
    case DEF_SLOT:
        msg = "nul";
    default:         // note the fall through
        PromptStart( msg, prompt );
        QWrite( STDERR_HANDLE, DefExt[ prompt ], 4, "console" );
    }
    QWrite( STDERR_HANDLE, "]:", 2, "console" );
}

static void PromptStart( char * msg, int prompt )
/***********************************************/
{
    char *  text;

    text = PromptText[ prompt ];
    QWrite( STDERR_HANDLE, text, strlen( text ), "console" );
    QWrite( STDERR_HANDLE, "[", 1, "console" );
    if( msg != NULL ) {
        QWrite( STDERR_HANDLE, msg, strlen( msg ), "console" );
    }
}

// spawn/suicide support.

static void *SpawnStack;

extern int Spawn( void (*fn)() )
/******************************/
{
    void *save_env;
    jmp_buf env;
    int status;

    save_env = SpawnStack;
    SpawnStack = env;
    status = setjmp( env );
    if( status == 0 ) {
        (*fn)();
    }
    SpawnStack = save_env;  /* unwind */
    return( status );
}


extern void Suicide( void )
/*************************/
{
    if( SpawnStack != NULL ) {
        longjmp( SpawnStack, 1 );
    }
}
