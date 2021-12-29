/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "wreslang.h"
#ifndef INCL_MSGTEXT
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include "wressetr.h"
#include "wresset2.h"

#include "clibext.h"
#endif


#ifdef INCL_MSGTEXT

// No res file to use. Just compile in the messages...
static char *asMessages[] = {
    #define pick( id, e_msg, j_msg )    e_msg,
    #include "as.msg"
#if defined( _STANDALONE_ )
    #include "usage.gh"
#endif
    #undef pick
#if 0
//#if defined( JAPANESE )
    #define pick( id, e_msg, j_msg )    j_msg,
    #include "as.msg"
#if defined( _STANDALONE_ )
    #include "usage.gh"
#endif
    #undef pick
#endif
};

#else

static HANDLE_INFO      hInstance = {0};

#endif


static unsigned         msgShift = 0;

#if defined( _STANDALONE_ )
bool AsMsgInit( void )
//********************
{
#ifdef INCL_MSGTEXT
  #if 0
    enum {
        MSG_LANG_SPACING = 0
        #define pick(c,e,j) + 1
        #include "as.msg"
    #if defined( _STANDALONE_ )
        #include "usage.gh"
    #endif
        #undef pick
    };
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
  #endif
    return( true );
#else
    char        name[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( AsMsgGet( MSG_AS_BASE, AsResBuffer ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#endif
}
#endif

bool AsMsgGet( int resourceid, char *buffer )
//*******************************************
{
#ifdef INCL_MSGTEXT
    strcpy( buffer, asMessages[resourceid + msgShift] );
#else
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + msgShift, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

#if defined( _STANDALONE_ )
void AsMsgFini( void )
//********************
{
#ifdef INCL_MSGTEXT
#else
    CloseResFile( &hInstance );
#endif
}
#endif
