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
* Description:  Message resources access functions.
*
****************************************************************************/


#include <sys/types.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#include "make.h"
#include "mcache.h"
#include "mrcmsg.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#include "clibext.h"


#define ARRAY_SIZE(a)   (sizeof( a ) / sizeof( (a)[0] ))

#ifdef BOOTSTRAP

    static struct idstr { int id; char *s; } StringTable[] = {
        #define pick(id,e,j)    {id, e},
        #include "wmake.msg"
        #include "usage.gh"
        #undef pick
    };

    static int compar( const void *s1, const void *s2 )
    {
        return ((struct idstr *)s1)->id - ((struct idstr *)s2)->id;
    }

#endif

#ifndef BOOTSTRAP

static  HANDLE_INFO hInstance = { 0 };
static  unsigned    MsgShift;

#endif

bool MsgInit( void )
/******************/
{
#ifndef BOOTSTRAP
    static char     name[_MAX_PATH]; // static because address passed outside.

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( MsgGet( MSG_USAGE_BASE, name ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#else
    return( true );
#endif
}


bool MsgGet( int resourceid, char *buffer )
/*****************************************/
{
#ifdef BOOTSTRAP
    struct idstr *s;
    struct idstr msgid;

    msgid.id = resourceid;
    s = bsearch( &msgid, StringTable, ARRAY_SIZE( StringTable ), sizeof( *s ), compar );
    if( s == NULL ) {
        buffer[0] = NULLCHAR;
        return( false );
    }
    strcpy( buffer, s->s );
#else
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = NULLCHAR;
        return( false );
    }
#endif
    return( true );
}

void MsgGetTail( int resourceid, char *buffer )
/*********************************************/
{
    char        msg[MAX_RESOURCE_SIZE];
    char const  *p;

    MsgGet( resourceid, msg );
    for( p = msg; (p = strchr( p, '%' )) != NULL && *(++p) != 'L'; ++p ) {
    }
    if( p != NULL ) {
        strcpy( buffer, (++p) );
    }
}


void MsgFini( void )
/*************************/
{
#ifndef BOOTSTRAP
    CloseResFile( &hInstance );
#endif
}
