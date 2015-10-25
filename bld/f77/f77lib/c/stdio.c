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
* Description:  standard i/o support
*
****************************************************************************/


#include "ftnstd.h"
#include "fapptype.h"
#include "posio.h"
#include "ftextfun.h"

#include <string.h>
#include <errno.h>

#if defined( __OS2__ ) && defined( __386__ )
  #define INCL_WINDIALOGS
  #include <wos2.h>
#elif defined( __WINDOWS__ ) || defined( __NT__ )
  #include <windows.h>
#endif

#if defined( __UNIX__ )
static  char            NLSequence[] = { "\n" };
#else
static  char            NLSequence[] = { "\r\n" };
#endif

#if defined( __IS_WINDOWED__ )
  extern        char            __FAppType;

  #define       BUFFLEN         1024
  static        char            *BuffCursor = { NULL };
  static        char            Buffer[BUFFLEN] = { NULLCHAR };
#endif


void    StdBuffer( void ) {
//===================

// Start buffering lines of output.

#if defined( __IS_WINDOWED__ )
    if( __FAppType == FAPP_GUI ) {
        BuffCursor = Buffer;
    }
#endif
}


void    StdWrite( char *buff, int len ) {
//=======================================

// Write to STDOUT_FILENO.
    int         rc;
    static int  console_flag;

#if defined( __IS_WINDOWED__ )
    if( BuffCursor ) {
        if( BuffCursor - Buffer + len > BUFFLEN - 1 ) return;
        for( ; len > 0; --len ) {
            if( *buff != '\r' ) {
                *BuffCursor = *buff;
                ++BuffCursor;
            }
            ++buff;
        }
        *BuffCursor = NULLCHAR;
    } else {
        if( __FAppType == FAPP_GUI ) {
#if defined( __OS2__ ) && defined( __386__ )
            WinMessageBox( HWND_DESKTOP, NULLHANDLE, Buffer, "", 0, MB_SYSTEMMODAL | MB_OK );
#elif defined( __WINDOWS__ ) || defined( __NT__ )
            MessageBox( (HWND)NULL, Buffer,"", MB_SYSTEMMODAL | MB_OK );
#endif
        } else {
#endif
#if !defined( __UNIX__ )
            setmode( fileno( stdout ), O_BINARY );
#endif
            rc = write( fileno( stdout ), buff, len );
            if( ( rc < 0 ) && !console_flag) {
#ifdef __NT__
                // Since we could not write to stdout the first time
                // we guess that this is a NT GUI
                // hence we try to issue the output via the message box
                // if the app is not default windowed
                // also the stdout handle must be bad
                if( (__FAppType == FAPP_CHARACTER_MODE) && (errno == EBADF) ) {
                    // Change app type to GUI and initiate display process
                    __FAppType = FAPP_GUI;
                    StdBuffer();
                    StdWrite( buff, len );
                }
#endif
            }
#if defined( __IS_WINDOWED__ )
        }
    }
#endif
    console_flag = 1;
}


void    StdFlush( void ) {
//==================

// Flush buffered lines of output.

#if defined( __IS_WINDOWED__ )
    uint        len;

    if( __FAppType == FAPP_GUI ) {
        len = BuffCursor - Buffer;
        BuffCursor = NULL;
        StdWrite( Buffer, len );
    }
#endif
}


void    StdWriteNL( char *buff, int len ) {
//=========================================

// Write to STDOUT_FILENO.

    StdWrite( buff, len );
    StdWrite( NLSequence, strlen( NLSequence ) );
}
