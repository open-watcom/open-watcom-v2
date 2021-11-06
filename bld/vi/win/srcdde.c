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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "vi.h"
#include <setjmp.h>
#include "ddedef.h"
#include "expr.h"
#include "srcwin.h"


#define TIME_OUT        10 * 1000L

/*
 * GetHSZ - get a HSZ from a string
 */
static bool GetHSZ( const char **str, HSZ *res )
{
    char        hdlstr[MAX_STR];

    *str = GetNextWord1( *str, hdlstr );
    if( *hdlstr == '\0'  ) {
        return( false );
    }
    *res = (HSZ)strtoul( hdlstr, NULL, 10 );
    return( true );

} /* GetHSZ */

/*
 * GetHCONV - get a HCONV from a string
 */
static bool GetHCONV( const char **str, HCONV *res )
{
    char        hdlstr[MAX_STR];

    *str = GetNextWord1( *str, hdlstr );
    if( *hdlstr == '\0'  ) {
        return( false );
    }
    *res = (HCONV)strtoul( hdlstr, NULL, 10 );
    return( true );

} /* GetHCONV */

/*
 * GetHDDEDATA - get a HDDEDATA from a string
 */
static bool GetHDDEDATA( const char **str, HDDEDATA *res )
{
    char        hdlstr[MAX_STR];

    *str = GetNextWord1( *str, hdlstr );
    if( *hdlstr == '\0'  ) {
        return( false );
    }
    *res = (HDDEDATA)strtoul( hdlstr, NULL, 10 );
    return( true );

} /* GetHDDEDATA */

/*
 * RunDDECommand - try to run a Windows specific command
 */
bool RunDDECommand( int token, const char *data, char *buffer, vi_rc *result, vars_list *vl )
{
    vi_rc       rc;
    char        tmp1[MAX_INPUT_LINE];
    char        tmp2[MAX_INPUT_LINE];
    HSZ         hdl;
    HSZ         serverhdl, topichdl;
    DWORD       dword;
    HCONV       hconv;
    HDDEDATA    dde_data;
    char        *ptr;
    int         len;
    jmp_buf     jmpaddr;
    int         jmprc;

    if( token == T_DDEINIT ) {
        if( !DDEInit() ) {
            *result = ERR_DDE_FAILED;
        } else {
            *result = ERR_NO_ERR;
        }
        return( true );
    }

    if( !UseDDE ) {
        *result = ERR_INVALID_DDE;
        return( true );
    }

    rc = ERR_NO_ERR;
    switch( token ) {
    case T_DDEQUERYSTRING:
        /*
         * syntax: ddequerystring <resvar> handle
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp1, data, vl );
        if( !GetHSZ( &data, &hdl  ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        len = DdeQueryString( DDEInstId, hdl, NULL, 0, CP_WINANSI ) + 1;
        ptr = MemAlloc( len  );
        DdeQueryString( DDEInstId, hdl, ptr, len, CP_WINANSI );
        VarAddStr( buffer, ptr, vl );
        MemFree( ptr );
        break;

    case T_DDERET:
        /*
         * syntax: dderet retval
         */
        data = Expand( buffer, data, vl );
        jmprc = setjmp( jmpaddr );
        if( jmprc == 0 ) {
            StartExprParse( data, jmpaddr );
            DDERet = (HDDEDATA)GetConstExpr();
        } else {
            rc = ERR_INVALID_DDE;
        }
        break;

    case T_DDESERVER:
        /*
         * syntax: ddeserver <serverhandle>
         */
        data = Expand( buffer, data, vl );
        if( !GetHSZ( &data, &hdl  ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            if( !DdeNameService( DDEInstId, hdl, (HSZ)NULL, DNS_REGISTER ) ) {
                rc = ERR_DDE_FAILED;
            } else {
                ServerCount++;
            }
        }
        break;

    case T_CREATEDDESTRING:
        /*
         * syntax: createddestring <handlevar> "<string>"
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( GetNextWordOrString( &data, tmp1 ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp2, tmp1, vl );
        if( !CreateStringHandle( data, &hdl ) ) {
            rc = ERR_DDE_FAILED;
        } else {
            sprintf( tmp1, "%ld", (long)hdl );
            VarAddStr( buffer, tmp1, vl );
        }
        break;

    case T_DELETEDDESTRING:
        /*
         * syntax: deleteddestring <handle>
         */
        data = Expand( buffer, data, vl );
        if( !GetHSZ( &data, &hdl ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            DeleteStringHandle( hdl );
        }
        break;

    case T_DDEGETDATA:
        /*
         * syntax: ddegetdata <strvar> <datahandle>
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp1, data, vl );
        if( !GetHDDEDATA( &data, &dde_data ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        len = DdeGetData( dde_data, NULL, 0, 0 );
        ptr = MemAlloc( len );
        DdeGetData( dde_data, (LPBYTE)ptr, len, 0 );
        VarAddStr( buffer, ptr,  vl );
        MemFree( ptr );
//      DdeFreeDataHandle( dde_data );
        break;


    case T_DDECREATEDATAHANDLE:
        /*
         * syntax: ddecreatedatahandle <handlevar> <itemhandle> "<string>"
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp2, data, vl );
        if( !GetHSZ( &data, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( GetNextWordOrString( &data, tmp1 ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        dde_data = DdeCreateDataHandle( DDEInstId, (LPBYTE)tmp1, strlen( tmp1 ) + 1,
                                    0, hdl, ClipboardFormat, 0 );
        if( dde_data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAILED;
        } else {
            sprintf( tmp1, "%ld", (long)dde_data );
            VarAddStr( buffer, tmp1, vl );
        }
        break;

    case T_DDECONNECT:
        /*
         * syntax: ddeconnect <convvar> <serverhandle> <topichandle>
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp1, data, vl );
        if( !GetHSZ( &data, &serverhdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetHSZ( &data, &topichdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        hconv = DdeConnect( DDEInstId, serverhdl, topichdl, NULL );
        if( hconv == (HCONV)NULL ) {
            rc = ERR_DDE_FAILED;
        } else {
            sprintf( tmp1, "%ld", (long)hconv );
            VarAddStr( buffer, tmp1, vl );
        }
        break;
    case T_DDEDISCONNECT:
        /*
         * syntax: ddedisconnect <hconv>
         */
        data = Expand( buffer, data, vl );
        if( !GetHCONV( &data, &hconv ) ) {
            rc = ERR_INVALID_DDE;
        } else {
            DdeDisconnect( hconv );
        }
        break;

    case T_DDEREQUEST:
        /*
         * syntax: dderequest <datavar> <conv> <strhandle>
         */
        if( !ReadVarName( &data, buffer, vl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        data = Expand( tmp1, data, vl );
        if( !GetHCONV( &data, &hconv ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetHSZ( &data, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        dde_data = DdeClientTransaction( NULL, 0, hconv, hdl, ClipboardFormat,
                                     XTYP_REQUEST, TIME_OUT, &dword );
        if( dde_data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAILED;
        } else {
            len = DdeGetData( dde_data, NULL, 0, 0 ) + 1;
            ptr = MemAlloc( len );
            DdeGetData( dde_data, (LPBYTE)ptr, len, 0 );
            VarAddStr( buffer, ptr,  vl );
            MemFree( ptr );
            DdeFreeDataHandle( dde_data );
        }
        break;

    case T_DDEPOKE:
        /*
         * syntax: ddepoke "<data>" <conv> <strhandle>
         */
        data = Expand( tmp1, data, vl );
        if( GetNextWordOrString( &data, buffer ) != ERR_NO_ERR ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetHCONV( &data, &hconv ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        if( !GetHSZ( &data, &hdl ) ) {
            rc = ERR_INVALID_DDE;
            break;
        }
        dde_data = DdeCreateDataHandle( DDEInstId, (LPBYTE)buffer, strlen( buffer ) + 1,
                                    0L, hdl, ClipboardFormat, 0 );
        if( dde_data == (HDDEDATA)NULL ) {
            rc = ERR_DDE_FAILED;
        } else {
            DdeClientTransaction( (LPBYTE)dde_data, (DWORD)-1L, hconv, hdl,
                                  ClipboardFormat, XTYP_POKE, TIME_OUT, NULL );
        }
        break;
    }

    *result = rc;
    return( true );

} /* RunDDECommand */
