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


#include <wlib.h>

int main( int argc, char *argv[] )
{
    int retcode;
    char    *cmd_line;
    int     cmd_len;

    InitSubSystems( argv[0] );
    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = MemAllocGlobal( cmd_len );
    _bgetcmd( cmd_line, cmd_len );
    retcode = WlibMainLine( argv[0], cmd_line );
    MemFreeGlobal( cmd_line );
    FiniSubSystems();
    return( retcode );
}

void FatalResError()
{
    write( fileno(stderr), NO_RES_MESSAGE, NO_RES_SIZE );
    longjmp( Env, 1 );
}

void FatalError( int str, ... )
{
    va_list             arglist;
    char                buff[MAX_ERROR_SIZE];

    va_start( arglist, str );
    MsgGet( str, buff );
    vfprintf( stderr, buff, arglist );
    fprintf( stderr, "\n" );
    va_end( arglist );
    longjmp( Env, 1 );
}

void Warning( int str, ... )
{
    va_list             arglist;
    char                buff[MAX_ERROR_SIZE];

    if( Options.quiet ) return;
    MsgGet( str, buff );
    va_start( arglist, str );
    vfprintf( stderr, buff, arglist );
    fprintf( stderr, "\n" );
    va_end( arglist );
}

void Usage( void )
{
    char                buff[MAX_ERROR_SIZE];
    int                 str;
    int                 count=3;

    for( str = USAGE_FIRST; str <= USAGE_LAST; ++str ) {
        MsgGet( str, buff );
        if( (count>20 && (!buff || buff[0]=='\0')) || count==24 ) {
            fprintf( stderr, "    (Press Return to continue)" );
            fflush( stderr );
            getch();
            count=0;
            fprintf( stderr, "\r                               \r");
        }
        fprintf( stderr, "%s\n", buff );
        count++;
    }
    longjmp( Env, 1 );
    //exit( EXIT_FAILURE );
}

char *WlibGetEnv( char *name)
{
    return( getenv( name ) );
}

void Banner()
{
    static char *bannerText[] = {
#ifdef __DEBUG__
banner1w( "Library Manager", _WLIB_VERSION_ ) " [Internal Development]",
#else
banner1w( "Library Manager", _WLIB_VERSION_ ),
#endif
banner2a(),
banner3,
    NULL
    };
    static int alreadyDone=0;
    char **text;

    if( Options.quiet || alreadyDone || Options.terse_listing ) return;

    alreadyDone = 1;
    text = bannerText;
    while( *text ) {
        fprintf( stderr, "%s\n", *text++ );
    }
}
