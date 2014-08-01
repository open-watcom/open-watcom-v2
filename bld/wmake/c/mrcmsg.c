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
* Description:  Message resources access functions.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
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

#ifdef BOOTSTRAP


    static struct idstr { int id; char *s; } StringTable[] = {
        #define pick( id, en, jp )  {id, en},
        #include "wmake.msg"
        #include "usage.gh"
        #undef pick
    };

    static int compar( const void *s1, const void *s2 )
    {
        return ((struct idstr *)s1)->id - ((struct idstr *)s2)->id;
    }

    #ifndef _arraysize
        #define _arraysize( a ) (sizeof(a)/sizeof(a[0]))
    #endif

#endif

/* this is a table storing msg id's which need two parameters for PrtMsg.
 * keep it in order.
 */
static  TABLE_TYPE  PARA_TABLE[] = {
/*        msgid                         msgtype */
        { TARGET_ALREADY_M,             "EM" },
        { SKIPPING_AFTER_ELSE,          "12" },
        { NOT_ALLOWED_AFTER_ELSE,       "12" },
        { DELETING_ITEM,                "12" },
        { NO_DEF_CMDS_FOR_MAKE,         "sE" },
        { PTARG_IS_TYPE_M,              "EM" },
        { IMP_ENV_M,                    "EM" },
        { GETDATE_MSG,                  "sE" },
        { END_OF_RESOURCE_MSG,          NULL } 
};

#ifndef BOOTSTRAP

static  HANDLE_INFO hInstance = { 0 };
static  unsigned    MsgShift;

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)


static long res_seek( int handle, off_t position, int where )
/************************************************************
 * fool the resource compiler into thinking that the resource information
 * starts at offset 0
 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}


WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

#endif

int MsgInit( void )
/************************/
{
#ifndef BOOTSTRAP
    int         initerror;
    static char name[_MAX_PATH]; // static because address passed outside.

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) == NULL ) {
        initerror = 1;
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
        initerror = 1;
    }
    if( initerror ) {
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
        MsgFini();
        return( 0 );
    }
#endif
    return( 1 );
}


int MsgGet( int resourceid, char *buffer )
/***********************************************/
{
#ifdef BOOTSTRAP
    {
        struct idstr *s;
        struct idstr msgid;

        msgid.id = resourceid;
        s = bsearch( &msgid, StringTable, _arraysize( StringTable ), sizeof( *s ), compar );
        if( s == NULL ) {
            buffer[0] = '\0';
            return( 0 );
        }
        strcpy( buffer, s->s );
    }
#else
    if( LoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_RESOURCE_SIZE ) == -1 ) {
        buffer[0] = '\0';
        return( 0 );
    }
#endif
    return( 1 );
}

void MsgGetTail( int resourceid, char *buffer )
/****************************************************/
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
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
#endif
}


static char *msgInTable( int resourceid )
/***************************************/
{
    int i;

    for( i = 0; PARA_TABLE[i].msgid < END_OF_RESOURCE_MSG; i++ ) {
        if( resourceid == PARA_TABLE[i].msgid ) {
            return( PARA_TABLE[i].msgtype );
        }
        if( resourceid < PARA_TABLE[i].msgid ) {
            break;
        }
    }
    return( NULL );
}


int MsgReOrder( int resourceid, char *buff, char **paratype )
/******************************************************************/
{
    int rvalue = 0;

    MsgGet( resourceid, buff );
    *paratype = msgInTable( resourceid );
    if( *paratype != NULL ) {
        buff = strchr( buff, '%' );
        while( buff != NULL ) {
            if( *(buff+1) == '%' ) {
                buff++;
            } else if( *(buff+1) == **paratype ) {
                break;
            } else if( *(buff+1) == *(*paratype+1) ) {
                rvalue = 1;
                break;
            }
            buff = strchr( (buff+1), '%' );
        }
    }
    return( rvalue );
}
