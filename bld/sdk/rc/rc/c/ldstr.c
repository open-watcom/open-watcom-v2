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


#include <string.h>
#include "wresall.h"
#include "layer0.h"
#include "errors.h"
#include "global.h"
#include "fcntl.h"
#include "ldstr.h"
#include "process.h"
#include "wreslang.h"
#include "iortns.h"

static unsigned MsgShift;

#ifdef UNIX
    #undef BOOTSTRAP_RC
    #define BOOTSTRAP_RC
#endif

#ifdef BOOTSTRAP_RC
    #define pick( id, en, jp )  en,

    static char *StringTable[] = {
        #include "rc.msg"
    };

    static char *UsageTable[] = {
        #include "usage.h"
    };

    #ifndef _arraysize
        #define _arraysize( a ) (sizeof(a)/sizeof(a[0]))
    #endif

#endif


int InitRcMsgs( char *dllname ) {

#ifndef BOOTSTRAP_RC
    int         error;
    char        fname[_MAX_PATH];
    WResFileID (*oldopen) (const char *, int, ...);

    error = FALSE;
#ifdef DLL_COMPILE
    if( dllname == NULL ) {
#else
    dllname = dllname; // get rid of unref'd warning
    if( _cmdname( fname ) == NULL ) {
#endif
        error = TRUE;
    } else {
#ifdef DLL_COMPILE
        strcpy( fname, dllname );
#endif
        Instance.filename = fname;

        /* swap open functions so this file handle is not buffered.
         * This makes it easier for layer0 to fool WRES into thinking
         * that the resource information starts at offset 0 */
        oldopen = WResRtns.open;
        WResRtns.open = open;
        OpenResFile( &Instance );
        WResRtns.open = oldopen;

        if( Instance.handle == -1 ) error = TRUE;
        if( !error ) {
            RegisterOpenFile( Instance.handle );
            error = FindResources( &Instance );
        }
        if( !error ) {
            error = InitResources( &Instance );
        }
        MsgShift = WResLanguage() * MSG_LANG_SPACING;
        if( !error && !GetRcMsg( USAGE_MSG_BASE, fname, sizeof( fname ) ) ) {
            error = TRUE;
        }
    }
    if( error ) {
        if( Instance.handle != -1 ) CloseResFile( &Instance );
        RcFatalError( ERR_RCSTR_NOT_FOUND );
//      return( 0 );
    }
#endif // BOOTSTRAP_RC
    return( 1 );
}

int GetRcMsg( unsigned resid, char *buff, unsigned buff_len ) {
#ifdef BOOTSTRAP_RC
    {
        *buff = '\0';
        if( resid >= USAGE_MSG_BASE ) {
            resid -= USAGE_MSG_BASE;
            if( resid >= _arraysize( UsageTable ) ) {
                return( 0 );
            }
            strcpy( buff, UsageTable[resid] );
            return( 1 );
        }

        if( resid >= _arraysize( StringTable ) ) {
            return( 0 );
        }
        strcpy( buff, StringTable[resid] );
    }
#else
    if( WResLoadString( &Instance, resid + MsgShift, buff, buff_len ) != 0 ) {
        buff[0] = '\0';
        return( 0 );
    }
#endif
    return( 1 );
}

void FiniRcMsgs( void ) {
#ifndef BOOTSTRAP_RC
    CloseResFile( &Instance );
#endif
}
