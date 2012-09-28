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
* Description:  Message resource handling.
*
****************************************************************************/


#include "as.h"
#ifdef _STANDALONE_
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include <fcntl.h>
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#else
// No res file to use. Just compile in the messages...
// Use English message unless compiled with "-dJAPANESE_MSG"
#if !defined( JAPANESE_MSG )
#define PICK( id, e_msg, j_msg )    e_msg,
#else
#define PICK( id, e_msg, j_msg )    j_msg,
#endif
static char *asMessages[] = {
    "IMPOSSIBLE",
    #include "as.msg"
};
#undef PICK
#endif

#ifdef _STANDALONE_
#define NIL_HANDLE      ((int)-1)

#define NO_RES_MESSAGE "Error: could not open message resource file\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)

static HANDLE_INFO      hInstance = {0};
static unsigned         msgShift;
extern long             FileShift;

static off_t resSeek( int handle, off_t position, int where )
//***********************************************************
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position+FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, resSeek, tell, MemAlloc, MemFree );
#endif

extern int AsMsgInit() {
//**********************

#ifdef _STANDALONE_
    int         error;
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) == NULL ) {
        error = 1;
    } else {
        hInstance.filename = name;
        OpenResFile( &hInstance );
        if( hInstance.handle == NIL_HANDLE ) {
            error = 1;
        } else {
            error = FindResources( &hInstance );
            if( !error ) {
                error = InitResources( &hInstance );
            }
        }
    }
    msgShift = WResLanguage() * MSG_LANG_SPACING;
    if( !error && !AsMsgGet( USAGE_1, name ) ) {
        error = 1;
    }
    if( error ) {
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
        AsMsgFini();
        return 0;
    }
#endif
    return 1;
}

extern int AsMsgGet( int resourceid, char *buffer ) {
//***************************************************

#ifdef _STANDALONE_
    if( LoadString( &hInstance, resourceid + msgShift,
                (LPSTR) buffer, MAX_RESOURCE_SIZE ) == -1 ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
#else
    strcpy( buffer, asMessages[ resourceid ] );
    return( 1 );
#endif
}

extern void AsMsgFini() {
//***********************

#ifdef _STANDALONE_
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
#endif
}
