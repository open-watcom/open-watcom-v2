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
* Description:  C compiler mainline (monolithic version).
*
****************************************************************************/


#include "cvars.h"
#include <unistd.h>
#include <limits.h>
#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ )
  #include <process.h>
#endif
#include "errout.h"
#ifdef __OSI__
  #include "ostype.h"
#endif
#include "cgdefs.h"
#include "feprotos.h"

void ResetHandlers( void )
{
    CloseFiles();                       /* 09-may-89 */
}

#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ )
int main( void )
    {
        char       *argv[2];
        int        ret;
        char       *buffer;
        int        len;
#else
int main( int argc, char **argv )
    {
        int        ret;
        argc = argc; /* shut the compiler up */
#if !defined( __WATCOMC__ )
        _argv = argv;
        _argc = argc;
#endif
#endif
        FrontEndInit( FALSE );
        atexit( ResetHandlers );
#if defined( __CMS__ )
        /* skip command name at the start */
        argv[0] = strchr( argv[0], ' ' );
        ret = FrontEnd( &argv[0] );
#elif defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ )
  #ifdef __DOS__
        fclose( stdaux );                   /* 15-dec-92 */
        fclose( stdprn );
  #endif
        len = _bgetcmd( NULL, INT_MAX ) + 1;
        buffer = malloc( len );
        if( buffer != NULL ) {
            argv[0] = buffer;
            argv[1] = NULL;
            _bgetcmd( buffer, len );
            ret = FrontEnd( &argv[ 0 ] );
            free( buffer );
        } else {
            ret = -1;
        }
#else
    #ifdef __OSI__
        if( __OS == OS_DOS ) {
            fclose( stdaux );                       /* 15-dec-92 */
            fclose( stdprn );
        }
    #endif
        ret = FrontEnd( &argv[1] );
#endif
        FrontEndFini();
        return( ret );
    }



extern void ConsErrMsg( cmsg_info  *info ){
// C compiler call back to do a print to stderr
    char        pre[MAX_MSG_LEN];

    FmtCMsg( pre, info );
    fputs( pre, errout );
    fputs( info->msgtxt, errout );
    fputc( '\n', errout );
    fflush( errout );
}

extern void ConsErrMsgVerbatim( char const  *line ){
// C compiler call back to do a print to stderr
    fputs( line, errout );
    fputc( '\n', errout );
    fflush( errout );
}

extern void ConsMsg( char const  *line ){
// C compiler call back to do a print to stdout
    fputs( line, stdout );
    fputc( '\n', stdout );
    fflush( stdout );
}

extern void ConBlip( void ){
// C compiler do a blip to console
    putchar( '.' );
    fflush( stdout );
}

extern bool ConTTY( void ){
// C compiler do a blip to console
    return( isatty( fileno( stdout )  ) );
}

extern void BannerMsg( char const  *line ){
// C compiler call back to do a print a banner msg
    fputs( line, stdout );
    fputc( '\n', stdout );
    fflush( stdout );
}

extern void DebugMsg( char const  *line ){
// C compiler call back to print a banner type msg
    fputs( line, stdout );
    fputc( '\n', stdout );
    fflush( stdout );
}

extern void NoteMsg( char const  *line ){
// C compiler call back to print a banner type msg
    fputs( line, stdout );
    fputc( '\n', stdout );
    fflush( stdout );
}

extern char *FEGetEnv( char const *name ){
// get enviorment variable
    char *ret;

    ret = getenv( name );
    return( ret );
}

extern void FESetCurInc( void ){
}

extern void MyExit( int rc )
{
        exit( rc );
} /* myexit */

#if 0

void FECompile( void )
{
    DoCompile();
}
#endif
