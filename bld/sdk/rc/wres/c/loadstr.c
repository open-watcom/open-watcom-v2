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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h>
#ifdef UNIX
    #include <alloca.h>
    #ifdef _AIX
        #define alloca __alloca
    #endif
#endif


/*
   This next set of lines is a temp fix until the 11.0 headers are
   in universal usage.
   The net result is that for 16bit Intel platforms _WCI86FAR will be __far
   for all other compilation targets it will be nothing.
*/
#ifndef _WCI86FAR
    #include <errno.h>
    #ifndef _WCI86FAR
        #ifdef M_I86
            #define _WCI86FAR __far
        #else
            #define _WCI86FAR
        #endif
    #endif
#endif

#ifdef WIN_GUI
#include <windows.h>
#else
#define WINAPI
typedef unsigned int UINT;
typedef char _WCI86FAR *   LPSTR;
#endif

#include <unistd.h>
#include "wresall.h"
#include "loadstr.h"

#define GET_STR_BUF_LEN 128

#include "phandle.h"

WResDir    MainDir;

static int GetString(   WResLangInfo    *res,
                        PHANDLE_INFO    hInstance,
                        UINT            idResource,
                        LPSTR           lpszBuffer,
                        int             nBufferMax )
/*************************************************/
{
    off_t               prevpos;
    int                 length;
    int                 stringbufflen;
    char *              stringbuff;
    int                 stringnum;
    int                 stringlen;
    int                 numread;
    int                 ix1, ix2;

    prevpos = WRESSEEK( hInstance->handle, res->Offset, SEEK_SET );
    if ( prevpos == -1L ) return( -1 );
    length = res->Length;
    stringbufflen = min( GET_STR_BUF_LEN, length );
//  stringbuff = (char *) WRESALLOC( stringbufflen );       JBS 92/10/02
    stringbuff = (char *) alloca( stringbufflen );
    if ( stringbuff == NULL ) return( -1 );
    stringnum = idResource & 0x0f;

    numread = 0;
    stringlen = 0;
    ix2 = 0;
    ix1 = 0;
    do {
        if ( numread == 0 ) {
            if ( length > stringbufflen ) {
                length -= stringbufflen;
            } else {
                stringbufflen = length;
                length = 0;
            }
            numread = WRESREAD( hInstance->handle, stringbuff, stringbufflen );
            if( numread == 0 ) return( -1 );    // 15-sep-93 AFS
            if( numread == -1 ) return( -1 );
            ix1 = 0;
        }
        if ( ix1 < numread ) {
            if ( stringlen == 0 ) {
                stringlen = ((unsigned char *)stringbuff)[ ix1++ ];
                stringnum--;
            }
            for( ; stringlen > 0; --stringlen ) {
                if ( ix1 >= numread ) break;
                if ( stringnum < 0 ) {
                    if( ix2 < nBufferMax - 1 ) {
                        lpszBuffer[ ix2++ ] = stringbuff[ ix1 ];
                    }
                }
                ix1++;
            }
        } else {
            numread = 0;
        }
    } while( !( (stringlen == 0) && (stringnum < 0) ) );
    lpszBuffer[ ix2 ] = '\0';

//  WRESFREE( stringbuff );     JBS 92/10/02

    return( 0 );
}

#if 0   // moved to loadres.c
static int GetResource( WResLangInfo    *res,
                        PHANDLE_INFO    hInstance,
                        LPSTR           lpszBuffer )
/**************************************************/
{
    off_t               prevpos;
    unsigned            numread;

    prevpos = WRESSEEK( hInstance->handle, res->Offset, SEEK_SET );
    if ( prevpos == -1L ) return( -1 );
    numread = WRESREAD( hInstance->handle, (void *)lpszBuffer, (int)res->Length );

    return( 0 );
}
#endif


extern int WINAPI WResLoadString2(  WResDir             dir,
                                    PHANDLE_INFO        hInstance,
                                    UINT                idResource,
                                    LPSTR               lpszBuffer,
                                    int                 nBufferMax )
/******************************************************************/
{
    int                 retcode;
    int                 block_num;
    WResID              string_type;
    WResID              block_id;
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;


    block_num = (idResource >> 4) + 1;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    WResInitIDFromNum( block_num, &block_id );
    WResInitIDFromNum( (long)RT_STRING, &string_type );

    if( dir == NULL ) {
        InitResources2( &dir, hInstance );
    }
    wind = WResFindResource( &string_type, &block_id, dir, &lang );

    if ( WResIsEmptyWindow( wind ) ) {
        retcode = -1;
    } else {
        res = WResGetLangInfo( wind );
        retcode = GetString( res, hInstance, idResource,
                             lpszBuffer, nBufferMax );
    }
    return( retcode );
}

extern int WINAPI WResLoadString(   PHANDLE_INFO        hInstance,
                                    UINT                idResource,
                                    LPSTR               lpszBuffer,
                                    int                 nBufferMax )
/******************************************************************/
{
    return( WResLoadString2( MainDir, hInstance, idResource, lpszBuffer, nBufferMax ) );
}

extern int OpenResFile( PHANDLE_INFO hInstance )
/*******************************************/
{
    return( hInstance->handle = ResOpenFileRO( hInstance->filename ) );
}

extern int InitResources2( WResDir *dir, PHANDLE_INFO hInstance )
/*********************************************/
{
    *dir = WResInitDir();
    if( *dir == NULL ) return( -1 );
    return( WResReadDir( hInstance->handle, *dir, NULL ) );
}

extern int InitResources( PHANDLE_INFO hInstance )
/*********************************************/
{
    return( InitResources2( &MainDir, hInstance ) );
}

extern int CloseResFile2( WResDir dir, PHANDLE_INFO hInstance )
/**************************************************************/
{
    WResFreeDir( dir );
    return( ResCloseFile( hInstance->handle ) );
}

extern int CloseResFile( PHANDLE_INFO hInstance )
/********************************************/
{
    return( CloseResFile2( MainDir, hInstance ) );
}

#if !defined( NATURAL_PACK )
#include "pshpk1.h"
#endif

typedef struct dbgheader {
    uint_16     signature;
    uint_8      exe_major_ver;
    uint_8      exe_minor_ver;
    uint_8      obj_major_ver;
    uint_8      obj_minor_ver;
    uint_16     lang_size;
    uint_16     seg_size;
    uint_32     debug_size;
} dbgheader;

#if !defined( NATURAL_PACK )
#include "poppk.h"
#endif

#define VALID_SIGNATURE 0x8386
#define FOX_SIGNATURE1  0x8300
#define FOX_SIGNATURE2  0x8301
#define WAT_RES_SIG     0x8302

/* The following define for LEVEL was borrowed from definition in BPATCH */
#define LEVEL           "WATCOM patch level .xxx"
#define LEVEL_HEAD_SIZE (sizeof(LEVEL)-5)       /* w/o ".xxx\0" */

long                    FileShift;

extern int FindResources( PHANDLE_INFO hInstance )
/* look for the resource information in a debugger record at the end of file */
{
    off_t       currpos;
    off_t       offset;
    dbgheader   header;
    int         notfound;
    char        buffer[ sizeof( LEVEL ) ];

    #define     __handle        hInstance->handle
    notfound = 1;
    FileShift = 0;
    offset = sizeof(dbgheader);
    if( WRESSEEK( __handle, -(long)sizeof(LEVEL), SEEK_END ) != -1L ){
        if( WRESREAD( __handle, buffer, sizeof(LEVEL) ) == sizeof(LEVEL) ) {
            if( memcmp( buffer, LEVEL, LEVEL_HEAD_SIZE ) == 0 ) {
                offset += sizeof(LEVEL);
            }
        }
    }
    currpos = WRESSEEK( __handle, - offset, SEEK_END );
    for(;;) {
        WRESREAD( __handle, &header, sizeof(dbgheader) );
        if( header.signature == WAT_RES_SIG ) {
            notfound = 0;
            FileShift = currpos - header.debug_size + sizeof(dbgheader);
            break;
        } else if( header.signature == VALID_SIGNATURE ||
                   header.signature == FOX_SIGNATURE1 ||
                   header.signature == FOX_SIGNATURE2 ) {
            currpos -= header.debug_size;
            WRESSEEK( __handle, currpos, SEEK_SET );
        } else {        /* did not find the resource information */
            break;
        }
    }
    return( notfound );
    #undef __handle
}

extern void LoadstrInitStatics( void )
/************************************/
{
    MainDir = NULL;
}

