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


/*
 * MSG : linker message output
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <process.h>

#include "linkstd.h"
#include "dbginfo.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "mapio.h"
#include "msg.h"
#include "fileio.h"

#include "wressetr.h"
#include "wreslang.h"

#if _LINKER != _WATFOR77
static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
static  int             Res_Flag;
#endif

extern char *   _LpDllName;
int WLinkItself;   // file handle

#define NO_RES_MESSAGE "could not open message resource file"

extern int InitMsg( void )
{
#if _LINKER == _WATFOR77
    BannerPrinted = FALSE;
    return( EXIT_SUCCESS );
#else
    char        buff[_MAX_PATH];
    int         initerror;
#if _LINKER == _DLLHOST
    char *      fname;

    fname = _LpDllName;
    initerror = fname == NULL;
#else
    char        fname[_MAX_PATH];

    initerror = _cmdname( fname ) == NULL;
#endif
    Res_Flag = EXIT_SUCCESS;
    BannerPrinted = FALSE;
    if( !initerror ) {
        hInstance.filename = fname;
        OpenResFile( &hInstance );
        WLinkItself = hInstance.handle;
        if( hInstance.handle == NIL_HANDLE ) {
            initerror = TRUE;
        } else {
            initerror = FindResources( &hInstance );
        }
    }
    if( !initerror ) {
        initerror = InitResources( &hInstance );
    }
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !Msg_Get( MSG_PRODUCT, buff ) ) {
        initerror = 1;
    }
    if( initerror ) {
        Res_Flag = EXIT_FAILURE;
        WriteInfoStdOut( NO_RES_MESSAGE, ERR, NULL );
    } else {
        Res_Flag = EXIT_SUCCESS;
    }
    return Res_Flag;
#endif
}

#if _LINKER != _WATFOR77
extern int Msg_Get( int resourceid, char *buffer )
{
    if( Res_Flag != EXIT_SUCCESS || LoadString( &hInstance, resourceid + MsgShift,
                (LPSTR) buffer, RESOURCE_MAX_SIZE ) != 0 ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
}
#else
// value of F77_MSG_BASE must correspond to value of MSG_BASE in "errmsg.rc"
#define F77_MSG_BASE    20000
extern  int     LoadMsg(int,char *,int);

extern int Msg_Get( int resourceid, char *buffer )
{
    if( !LoadMsg( F77_MSG_BASE + resourceid, buffer, RESOURCE_MAX_SIZE ) ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
}
#endif

extern void Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info,
                        char *types, ... )
{
    va_list     args;

    va_start( args, types );
    Msg_Put_Args( rc_buff, arg_info, types, &args );
    va_end( args );
}

extern void Msg_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info, char *types,
                        va_list *args )
{
    int         argnum = 0;
    int         j;
    int         order[3];
    char        *findpct;
    char        types_buff[4];
    char        ch;

    if( types != NULL ) {
        strcpy( types_buff, types );
    }
    findpct = strchr( rc_buff, '%' );
    while( findpct != NULL ) {
        ch = *( findpct + 1 );
        for( j = 0; types_buff[j] != '\0'; j++ ) {
            if( types_buff[j] == ch ) {
                order[j] = argnum;
                argnum++;
                if( isdigit( types_buff[j] ) ) {
                    *( findpct + 1 ) = 's';
                    types_buff[j] = 's';
                }
                break;
            }
        }
        findpct = strchr( findpct + 2, '%' );
    }
    for( j = 0; j < argnum; j++ ) {
        Msg_Add_Args( &(arg_info->arg[order[j]]), types_buff[j], args );
        arg_info->index = 0;
    }
}

static void Msg_Add_Args( MSG_ARG *arginfo, char typech, va_list *args )
{
    switch( typech ) {
        case 's':
            arginfo->string = va_arg( *args, char * );
            break;
        case 'x':
        case 'd':
            arginfo->int_16 = va_arg( *args, unsigned_16 );
            break;
        case 'l':
            arginfo->int_32 = va_arg( *args, unsigned_32 );
            break;
        case 'a':
            arginfo->address = va_arg( *args, targ_addr * );
            break;
        case 'S':
            arginfo->symb = va_arg( *args, symbol * );
            break;
    }
}

extern void Msg_Write_Map( int resourceid, ... )
{
    char        msg_buff[RESOURCE_MAX_SIZE];
    va_list     arglist;

    Msg_Get( resourceid, msg_buff );
    va_start( arglist, resourceid );
    DoWriteMap( msg_buff, &arglist );
    va_end( arglist );
}

extern int FiniMsg()
{
    int     retcode = EXIT_SUCCESS;

#if _LINKER != _WATFOR77
    if( Res_Flag == EXIT_SUCCESS ) {
        if ( CloseResFile( &hInstance ) != -1 ) {
            Res_Flag = EXIT_FAILURE;
        } else {
            retcode = EXIT_FAILURE;
        }
    }
#endif
    return retcode;
}
