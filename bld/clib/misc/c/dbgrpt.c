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
* Description:  Runtime debug support - error reporting.
*
****************************************************************************/


#include "variety.h"
#define _DEBUG                  /* so prototypes are included */
#include <crtdbg.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#if defined(__NT__) || defined(__WINDOWS__)
    #include <windows.h>
#endif
#ifdef __OS2__
    #define INCL_WIN
    #include <os2.h>
#endif
#ifndef __NT__
    #include <unistd.h>
#endif
#include "dbgdata.h"
#include "enterdb.h"
#include "liballoc.h"

#define MAX_MSG_LEN             512
#define TOO_LONG_MSG            "_CrtDbgReport: Message too long"
#define ASSERT_PREFIX1          "Assertion failed!"
#define ASSERT_PREFIX2          "Assertion failed: "
#define WINTITLE                "Open Watcom C/C++ Debug Library"



#if defined(__NT__) || defined(__WINDOWS__) || defined(__OS2__)

/*
 * Handle _CRTDBG_MODE_WNDW reporting.
 */
static int window_report( int reporttype, const char *filename,
                          int linenumber, const char *modulename,
                          const char *usermsg )
/***************************************************************/
{
    #if defined(__NT__) || defined(__WINDOWS__)
        int             osrc;
        UINT            flags;
    #endif
    #ifdef __OS2__
        int             usepm = 0;
        HMQ             hMessageQueue = 0;
        HAB             AnchorBlock = 0;
        USHORT          osrc;
    #endif
    char                outmsg[MAX_MSG_LEN];
    char                linestr[32];
    char                progname[_MAX_PATH];
    int                 rc;
    int                 retval = 0;
    static const char * reportnames[3] = {
        "Warning",                      /* _CRT_WARN */
        "Error",                        /* _CRT_ERROR */
        "Assertion Failure"             /* _CRT_ASSERT */
    };

    /*** Initialize some stuff ***/
    #ifdef __NT__
        if( GetModuleFileNameA( NULL, progname, _MAX_PATH )  ==  0 ) {
            strcpy( progname, "<unknown program>" );
        }
    #else
    {
        extern char **  _argv;
        if( (_argv[0])[0] != '\0' ) {
            strcpy( progname, _argv[0] );
        } else {
            strcpy( progname, "<unknown program>" );
        }
    }
    #endif
    if( linenumber != 0 ) {
        _bprintf( linestr, 32, "%d", linenumber );
    } else {
        strcpy( linestr, "" );
    }

    /*** Form the contents of the window ***/
    rc = _bprintf( outmsg, MAX_MSG_LEN,
                   "Debug %s!\n\nProgram: %s%s%s%s%s%s%s%s%s%s",
                   reportnames[reporttype],
                   progname,
                   modulename!=NULL ? "\nModule: " : "",
                   modulename!=NULL ? modulename : "",
                   filename!=NULL ? "\nFile: " : "",
                   filename!=NULL ? filename : "",
                   linestr[0]!='\0' ? "\nLine: " : "",
                   linestr[0]!='\0' ? linestr : "",
                   usermsg[0]!='\0' ? "\n\n" : "",
                   usermsg[0]!='\0' && reporttype==_CRT_ASSERT  ?  "Expression: "  :  "",
                   usermsg[0]!='\0' ? usermsg : "" );
    if( rc < 0 ) {
        strcpy( outmsg, TOO_LONG_MSG );
    }

    /*** Create the window ***/
    #if defined(__NT__) || defined(__WINDOWS__)
        flags = MB_ICONHAND | MB_TASKMODAL | MB_ABORTRETRYIGNORE;
        #ifdef __NT__
            flags |= MB_SETFOREGROUND;
        #endif
        osrc = MessageBox( (HWND)NULL, outmsg, WINTITLE, flags );
        switch( osrc ) {
            case 0:
                retval = -1;
                break;
            case IDABORT:
                raise( SIGABRT );
                _exit( 3 );
                break;
            case IDRETRY:
                retval = 1;
                break;
            default:
                retval = 0;
        }
    #elif defined(__OS2__)
        AnchorBlock = WinInitialize( 0 );
        if( AnchorBlock != 0 ) {
            hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
            if( hMessageQueue != 0 ) {
                usepm = 1;
            } else {
                int     rc;
                rc = WinGetLastError( AnchorBlock );
                if( (rc & 0xFFFF) == PMERR_MSG_QUEUE_ALREADY_EXISTS ) {
                    usepm = 1;
                }
            }
        }
        if( usepm ) {
            osrc = WinMessageBox( HWND_DESKTOP, 0, outmsg, WINTITLE, 0,
                                  MB_ICONHAND | MB_ABORTRETRYIGNORE );
            switch( osrc ) {
                case MBID_ERROR:
                    retval = -1;
                    break;
                case MBID_ABORT:
                    raise( SIGABRT );
                    _exit( 3 );
                    break;
                case MBID_RETRY:
                    retval = 1;
                    break;
                default:
                    retval = 0;
            }
        } else {
            retval = -1;
        }
        if( hMessageQueue != 0 ) {
            WinDestroyMsgQueue( hMessageQueue );
        }
        if( AnchorBlock != 0 ) {
            WinTerminate( AnchorBlock );
        }
    #endif

    return( retval );
}

#endif  /* defined(__NT__) || defined(__WINDOWS__) || defined(__OS2__) */



/*
 * Main reporting routine.
 */
_WCRTLINK int _CrtDbgReport( int reporttype, const char *filename,
                             int linenumber, const char *modulename,
                             const char *format, ... )
/******************************************************************/
{
    #ifdef __NT__
        DWORD           byteswritten;
        BOOL            osrc;
    #endif
    char                usermsg[MAX_MSG_LEN] = { 0 };
    char                linemsg[MAX_MSG_LEN] = { 0 };
    char                outmsg[MAX_MSG_LEN] = { 0 };
    va_list             args;
    size_t              len;
    int                 retval = 0;
    int                 rc;

    /*** Ensure reporttype is valid ***/
    if( reporttype < _CRT_WARN  ||  reporttype > _CRT_ASSERT ) {
        return( -1 );
    }

    /*** Create the user message if necessary ***/
    va_start( args, format );
    if( format != NULL ) {
        len = max( strlen( ASSERT_PREFIX1 ), strlen( ASSERT_PREFIX2 ) );
        if( _vbprintf( usermsg, MAX_MSG_LEN-len, format, args )  <  0 ) {
            strcpy( usermsg, TOO_LONG_MSG );
        }
    }

    /*** Create an assertion failure message if appropriate ***/
    if( reporttype == _CRT_ASSERT ) {
        if( format == NULL ) {
            strcpy( linemsg, ASSERT_PREFIX1 );
        } else {
            strcpy( linemsg, ASSERT_PREFIX2 );
        }
        strcat( linemsg, usermsg );
        strcat( linemsg, "\n" );
    } else {
        strcpy( linemsg, usermsg );
    }

    /*** Add file information if it's available ***/
    if( filename != NULL ) {
        if( _bprintf( outmsg, MAX_MSG_LEN, "%s(%d): %s", filename,
                      linenumber, linemsg )  <  0 ) {
            strcpy( outmsg, TOO_LONG_MSG );
        }
    } else {
        strcpy( outmsg, linemsg );
    }

    /*** If there's a user-installed report hook function, call it ***/
    if( __DbgReportHook != NULL ) {
        /* MS documents this as ...==0 but really ...!=0 is what they do */
        if( (*__DbgReportHook)( reporttype, usermsg, &retval )  !=  0 ) {
            return( retval );
        }
    }

    /*** Handle _CRTDBG_MODE_FILE ***/
    if( __DbgReportModes[reporttype] & _CRTDBG_MODE_FILE ) {
        if( __DbgReportFiles[reporttype] != _CRTDBG_INVALID_HFILE ) {
            #ifdef __NT__
                osrc = WriteFile( __DbgReportFiles[reporttype], outmsg,
                                  strlen( outmsg ), &byteswritten, NULL );
                if( osrc == FALSE )  retval = -1;
            #else
                rc = write( __DbgReportFiles[reporttype], outmsg,
                            strlen( outmsg ) );
                if( rc == -1 )  retval = -1;
            #endif
        }
    }

    /*** Handle _CRTDBG_MODE_DEBUG ***/
    if( __DbgReportModes[reporttype] & _CRTDBG_MODE_DEBUG ) {
        char *          buff;
        size_t          len;

        len = sizeof( DEBUGGER_MESSAGE_COMMAND ) + strlen( outmsg ) + 1;
        buff = (char*) lib_malloc( len );
        if( buff != NULL ) {
            sprintf( buff, DEBUGGER_MESSAGE_FORMAT, outmsg );
            PassDebuggerAMessage( buff );
            lib_free( buff );
        } else {
            retval = -1;
        }
    }

    /*** Handle _CRTDBG_MODE_WNDW ***/
    if( __DbgReportModes[reporttype] & _CRTDBG_MODE_WNDW ) {
        #if defined(__NT__) || defined(__WINDOWS__) || defined(__OS2__)
            rc = window_report( reporttype, filename, linenumber, modulename,
                                usermsg );
            if( rc != 0 )  retval = rc;
        #else
            retval = -1;
        #endif
    }

    return( retval );
}
