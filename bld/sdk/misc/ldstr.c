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


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mem.h"
#include "ldstr.h"

// No string to be loaded can be more than LDSTR_MAX_STR_LEN bytes long
#define LDSTR_MAX_STR_LEN       500

#define ALLOC_STRING( x )       AllocRCString( x )
#define FREE_STRING( x )        FreeRCString( x )

#ifndef SPECIAL_STRING_LOADING

static char     getStringBuffer[ LDSTR_MAX_STR_LEN ];
static char     tmpBuf[ LDSTR_MAX_STR_LEN ];
static HANDLE   curInst;

/*
 * GetRCString - return a pointer to a string from the resource file.
 *              NB the pointer is only valid until the next call to
 *              GetString
 */
char *GetRCString( DWORD msgid ) {

    LoadString( curInst, msgid, getStringBuffer, LDSTR_MAX_STR_LEN );
    return( getStringBuffer );
}

char *AllocRCString( DWORD id ) {

    char        *ret;
    int         len;

    len = LoadString( curInst, id, tmpBuf, LDSTR_MAX_STR_LEN );
    ret = MemAlloc( len + 1 );
    if( ret != NULL ) {
        strcpy( ret, tmpBuf );
    }
    return( ret );
}

DWORD CopyRCString( DWORD id, char *buf, DWORD bufsize ) {
    DWORD       len;

    len = LoadString( curInst, id, buf, bufsize );
    return( len );
}

void FreeRCString( char *str ) {
    MemFree( str );
}

void SetInstance( HANDLE inst ) {
    curInst = inst;
}
#endif

int RCsprintf( char *buf, DWORD fmtid, ... ) {

    va_list     al;
    char        *fmtstr;
    int         ret;

    va_start( al, fmtid );
    fmtstr = ALLOC_STRING( fmtid );
    ret = vsprintf( buf, fmtstr, al );
    FREE_STRING( fmtstr );
    va_end( al );
    return( ret );
}

#ifndef WR_NO_RCFPRINTF
/*
 * RCfprintf
 */
void RCfprintf( FILE *fp, DWORD strid, ...  ) {

    va_list     al;
    char        *str;

    va_start( al, strid );
    str = ALLOC_STRING( strid );
    vfprintf( fp, str, al );
    FREE_STRING( str );
    va_end( al );
}
#endif

/*
 * RCvfprintf
 */
void RCvfprintf( FILE *fp, DWORD strid, va_list al  ) {

    char        *str;

    str = ALLOC_STRING( strid );
    vfprintf( fp, str, al );
    FREE_STRING( str );
}

/*
 *MyMessageBox-
 */
int RCMessageBox( HWND hwnd , DWORD msgid, char *title, UINT type ) {

    char        *msg;
    int         ret;

    msg = ALLOC_STRING( msgid );
    ret = MessageBox( hwnd, msg, title, type );
    FREE_STRING( msg );
    return( ret );
}

