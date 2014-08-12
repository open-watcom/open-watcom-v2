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


#include "asmglob.h"
#ifdef __WATCOMC__
  #include <conio.h>
  #include <process.h>
#endif
#include "wio.h"
#include "clibext.h"

#if defined( USE_TEXT_MSGS )

static const char *txtmsgs[] = {
    #define pick(num,etext,jtext) {etext},
    #include "../h/asmshare.msg"
    #include "../h/womp.msg"
    #include "../h/wasm.msg"
    #include "usage.gh"
    #undef pick
};

#else

#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#define STDOUT_HANDLE   ((int)1)

#define NO_RES_MESSAGE  "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE     (sizeof( NO_RES_MESSAGE ) - 1)

extern  long            FileShift;

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

#endif

extern  int             trademark( void );
#ifdef __OSI__
extern char             *_Copyright;
#endif

#if defined( __UNIX__ )

void PrintfUsage( void )
{
    char        msg_buff[MAX_MESSAGE_SIZE];
    int         first_ln;

    trademark();
    first_ln = MSG_USAGE_BASE;
    for( first_ln++; ; first_ln++ ) {
        MsgGet( first_ln, msg_buff );
        if( ( msg_buff[ 0 ] == '.' ) && ( msg_buff[ 1 ] == 0 ) )
            break;
        puts( msg_buff );
    }
}

#else

static void con_output( const char *text )
{
    char c;

    do {
        c = *text;
        putchar( c );
        ++text;
    } while( *text );
    putchar( '\n' );
}

static void Wait_for_return( const char *page_text )
{
    if( isatty( fileno(stdout) ) ) {
        con_output( page_text );
        fflush( stdout );
        getch();
    }
}

void PrintfUsage( void )
{
    char        msg_buff[MAX_MESSAGE_SIZE];
    unsigned    count;
    char        page_text[MAX_MESSAGE_SIZE];
    int         first_ln;

    count = trademark();
#ifdef __OSI__
    if( _Copyright != NULL ) {
        puts( _Copyright );
        count += 1;
    }
#endif
    first_ln = MSG_USAGE_BASE;
    MsgGet( first_ln++, page_text );
    for( ; ; first_ln++ ) {
        if( ++count >= 23 ) {
            Wait_for_return( page_text );
            count = 0;
        }
        MsgGet( first_ln, msg_buff );
        if( ( msg_buff[ 0 ] == '.' ) && ( msg_buff[ 1 ] == 0 ) )
            break;
        puts( msg_buff );
    }
}

#endif

void MsgPrintf( int resourceid )
{
    char        msgbuf[MAX_MESSAGE_SIZE];

    if( !Options.banner_printed ) {
        Options.banner_printed = TRUE;
        trademark();
    }
    MsgGet( resourceid, msgbuf );
    printf( msgbuf );
}

void MsgPrintf1( int resourceid, char *token )
{
    char        msgbuf[MAX_MESSAGE_SIZE];

    if( !Options.banner_printed ) {
        Options.banner_printed = TRUE;
        trademark();
    }
    MsgGet( resourceid, msgbuf );
    printf( msgbuf, token );
}

#if !defined( USE_TEXT_MSGS )

static off_t res_seek( int handle, off_t position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

#endif


bool MsgInit( void )
{
#if !defined( USE_TEXT_MSGS )
    bool        initerror;
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) == NULL ) {
        initerror = true;
    } else {
        initerror = OpenResFile( &hInstance, name );
        if( !initerror ) {
            initerror = FindResources( &hInstance );
            if( !initerror ) {
                initerror = InitResources( &hInstance );
            }
        }
    }
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !MsgGet( MSG_USAGE_BASE, name ) ) {
        initerror = true;
    }
    if( initerror ) {
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
        MsgFini();
        return( false );
    }
#endif
    return( true );
}

void MsgFini( void )
{
#if !defined( USE_TEXT_MSGS )
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
#endif
}

#define TXT_SHARE_BASE  0
#define TXT_WOMP_BASE   (TXT_SHARE_BASE + MSG_SHARE_LAST - MSG_SHARE_BASE)
#define TXT_WASM_BASE   (TXT_WOMP_BASE + MSG_WOMP_LAST - MSG_WOMP_BASE)
#define TXT_USAGE_BASE  (TXT_WASM_BASE + MSG_WASM_LAST - MSG_WASM_BASE)

bool MsgGet( int id, char *buffer )
{
#if defined( USE_TEXT_MSGS )
    int index;

    if( id >= MSG_SHARE_BASE && id < MSG_SHARE_LAST ) {
        index = id - MSG_SHARE_BASE + TXT_SHARE_BASE;
    } else if( id >= MSG_WOMP_BASE && id < MSG_WOMP_LAST ) {
        index = id - MSG_WOMP_BASE + TXT_WOMP_BASE;
    } else if( id >= MSG_WASM_BASE && id < MSG_WASM_LAST ) {
        index = id - MSG_WASM_BASE + TXT_WASM_BASE;
    } else if( id >= MSG_USAGE_BASE ) {
        index = id - MSG_USAGE_BASE + TXT_USAGE_BASE;
    } else {
        buffer[0] = '\0';
        return( FALSE );
    }
    strncpy( buffer, txtmsgs[index], MAX_MESSAGE_SIZE - 1 );
    buffer[MAX_MESSAGE_SIZE - 1] = '\0';
#else
    if( LoadString( &hInstance, id + MsgShift, (LPSTR)buffer, MAX_MESSAGE_SIZE ) != 0 ) {
        buffer[0] = '\0';
        return( FALSE );
    }
#endif
    return( TRUE );
}
