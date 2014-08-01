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


#include "vi.h"
#include <malloc.h>
#include <setjmp.h>
#include "source.h"
#include "ddedef.h"
#include "expr.h"
#include "srcwin.h"

#define TIME_OUT        10 * 1000L

/*
 * getVarName - extract a variable name from a command
 */
static bool getVarName( char *str, char *tmp1, vlist *vl )
{

    if( NextWord1( str, tmp1 ) <= 0 ) {
        return( FALSE );
    }
    if( !VarName( tmp1, vl ) ) {
        return( FALSE );
    }
    return( TRUE );

} /* getVarName */

/*
 * RunDDECommand - try to run a Windows specific command
 */
bool RunDDECommand( int token, char *str, char *tmp1, vi_rc *result, vlist *vl )
{
    vi_rc       rc;
    char        *tmp2;
    HSZ         hdl;
    HSZ         serverhdl, topichdl;
    DWORD       dword;
    HCONV       hconv;
    HDDEDATA    data;
    char        *ptr;
    int         len;
    jmp_buf     jmpaddr;
    int         jmprc;

    tmp2 = alloca( MAX_INPUT_LINE );
    if( tmp2 == NULL ) {
        return( FALSE );
    }

    if( token == T_DDEINIT ) {
        if( !DDEInit() ) {
            *result = ERR_DDE_FAIL;
        } else {
            *result = ERR_NO_ERR;
        }
        return( TRUE );
    }

    if( !UseDDE ) {
        *result = ERR_INVALID_DDE;
        return( TRUE );
    }

    rc = ERR_NO_ERR;
    switch( token ) {
    case T_DDEQUERYSTRING:
        /*
         * syntax: ddequerystring <resvar> handle
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( str, vl );
        if( !GetDWORD( str, &hdl  ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        len = DdeQueryString( DDEInstId, hdl, NULL, 0, CP_WINANSI ) + 1;
        ptr = MemAlloc( len  );
        DdeQueryString( DDEInstId, hdl, ptr, len, CP_WINANSI );
        VarAddStr( tmp1, ptr, vl );
        MemFree( ptr );
        break;

    case T_DDERET:
        /*
         * syntax: dderet retval
         */
        Expand( str, vl );
        jmprc = setjmp( jmpaddr );
        if( jmprc == 0 ) {
            StartExprParse( str, jmpaddr );
            DDERet = GetConstExpr();
        } else {
            rc = ERR_INVALID_DDE;
        }
        break;

    case T_DDESERVER:
        /*
         * syntax: ddeserver <serverhandle>
         */
        Expand( str, vl );
        if( !GetDWORD( str, &hdl  ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            if( !DdeNameService( DDEInstId, hdl, (HSZ)NULL, DNS_REGISTER ) ) {
                rc = ERR_DDE_FAIL;
            } else {
                ServerCount++;
            }
        }
        break;

    case T_CREATEDDESTRING:
        /*
         * syntax: createddestring <handlevar> "<string>"
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( GetStringWithPossibleQuote( str, tmp2 ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( tmp2, vl );
        if( !CreateStringHandle( tmp2, &hdl ) ) {
            rc = ERR_DDE_FAIL;
        } else {
            sprintf( tmp2, "%ld", (long)hdl );
            VarAddStr( tmp1, tmp2, vl );
        }
        break;

    case T_DELETEDDESTRING:
        /*
         * syntax: deleteddestring <handle>
         */
        Expand( str, vl );
        if( !GetDWORD( str, &hdl ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            DeleteStringHandle( hdl );
        }
        break;

    case T_DDEGETDATA:
        /*
         * syntax: ddegetdata <strvar> <datahandle>
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( str, vl );
        if( !GetDWORD( str, &data ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        len = DdeGetData( data, NULL, 0, 0 );
        ptr = MemAlloc( len );
        DdeGetData( data, (LPBYTE)ptr, len, 0 );
        VarAddStr( tmp1, ptr,  vl );
        MemFree( ptr );
//      DdeFreeDataHandle( data );
        break;


    case T_DDECREATEDATAHANDLE:
        /*
         * syntax: ddecreatedatahandle <handlevar> <itemhandle> "<string>"
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( str, vl );
        if( !GetDWORD( str, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( GetStringWithPossibleQuote( str, tmp2 ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = DdeCreateDataHandle( DDEInstId, (LPBYTE)tmp2, strlen( tmp2 ) + 1,
                                    0, hdl, ClipboardFormat, 0 );
        if( data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAIL;
        } else {
            sprintf( tmp2, "%ld", (long)data );
            VarAddStr( tmp1, tmp2, vl );
        }
        break;

    case T_DDECONNECT:
        /*
         * syntax: ddeconnect <convvar> <serverhandle> <topichandle>
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( str, vl );
        if( !GetDWORD( str, &serverhdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetDWORD( str, &topichdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        hconv = DdeConnect( DDEInstId, serverhdl, topichdl, NULL );
        if( hconv == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAIL;
        } else {
            sprintf( tmp2, "%ld", (long)hconv );
            VarAddStr( tmp1, tmp2, vl );
        }
        break;
    case T_DDEDISCONNECT:
        /*
         * syntax: ddedisconnect <hconv>
         */
        Expand( str, vl );
        if( !GetDWORD( str, &hconv ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            DdeDisconnect( hconv );
        }
        break;

    case T_DDEREQUEST:
        /*
         * syntax: dderequest <datavar> <conv> <strhandle>
         */
        if( !getVarName( str, tmp1, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        Expand( str, vl );
        if( !GetDWORD( str, &hconv ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetDWORD( str, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = DdeClientTransaction( NULL, 0, hconv, hdl, ClipboardFormat,
                                     XTYP_REQUEST, TIME_OUT, &dword );
        if( data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAIL;
        } else {
            len = DdeGetData( data, NULL, 0, 0 ) + 1;
            ptr = MemAlloc( len );
            DdeGetData( data, (LPBYTE)ptr, len, 0 );
            VarAddStr( tmp1, ptr,  vl );
            MemFree( ptr );
            DdeFreeDataHandle( data );
        }
        break;

    case T_DDEPOKE:
        /*
         * syntax: ddepoke "<data>" <conv> <strhandle>
         */
        Expand( str, vl );
        if( GetStringWithPossibleQuote( str, tmp1 ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetDWORD( str, &hconv ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetDWORD( str, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = DdeCreateDataHandle( DDEInstId, (LPBYTE)tmp1, strlen( tmp1 ) + 1,
                                    0L, hdl, ClipboardFormat, 0 );
        if( data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAIL;
        } else {
            DdeClientTransaction( (LPBYTE) data, -1, hconv, hdl,
                                  ClipboardFormat, XTYP_POKE, TIME_OUT, NULL );
        }
        break;
    }

    *result = rc;
    return( TRUE );

} /* RunDDECommand */
