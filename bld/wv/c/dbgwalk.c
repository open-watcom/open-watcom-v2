/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "sslops.h"
#include "dbgsem.h"
#include "dbgwalk.h"


static const char   *TblPtr;

static ssl_value GetParm( op_code operation )
{
    ssl_value   parm;

    parm = MGET_U8( TblPtr );
    TblPtr++;
    if( operation & INS_LONG ) {
        parm |= MGET_U8( TblPtr ) << 8;
        TblPtr++;
    }
    return( parm );
}


static int GetParmInt( op_code operation )
{
    int     parm;

    parm = MGET_S8( TblPtr );
    TblPtr++;
    if( operation & INS_LONG ) {
        parm &= 0xff;
        parm |= MGET_S8( TblPtr ) << 8;
        TblPtr++;
    }
    return( parm );
}


static const char *GetTablePos( const char *table )
{
    return( table + GETWORD( TblPtr ) );
}


int SSLWalk( const char *table, unsigned start, const char **stk_bot, unsigned stk_size )
{
    op_code         operation;
    unsigned        num_items;
    signed int      disp;
    const char      *addr;
    const char      **stk_ptr;
    const char      **stk_end;
    ssl_value       result;
    ssl_value       parm;
    ssl_value       wanted;
    tokens          token;

    result = 0;
    parm = 0;
    stk_ptr = stk_bot;
    stk_end = stk_bot + stk_size;
    TblPtr = table + start;
    token = SSLCurrToken();
    for( ;; ) {
        operation = (op_code)MGET_U8( TblPtr );
        TblPtr++;
        switch( operation & INS_MASK ) {
        case INS_INPUT:
            wanted = GetParm( operation );
            if( token != (tokens)wanted ) {
                if( SSLError( TERM_SYNTAX, wanted ) ) {
                    return( TERM_SYNTAX );
                }
            }
            token = SSLNextToken();
            break;
        case INS_IN_ANY:
            token = SSLNextToken();
            break;
        case INS_OUTPUT:
            SSLOutToken( (tokens)GetParm( operation ) );
            break;
        case INS_ERROR:
            if( SSLError( TERM_ERROR, GetParm( operation ) ) ) {
                return( TERM_ERROR );
            }
            break;
        case INS_JUMP:
            addr = TblPtr - 1;
            TblPtr = addr + GetParmInt( operation );
            break;
        case INS_CALL:
            if( stk_ptr >= stk_end ) {
                /* stack overflow */
                SSLError( TERM_STK_OVERFLOW, 0 );
                return( TERM_STK_OVERFLOW );
            }
            addr = TblPtr - 1;
            disp = GetParmInt( operation );
            *stk_ptr++ = TblPtr;
            TblPtr = addr + disp;
            break;
        case INS_SET_RESULT:
            result = GetParm( operation );
            break;
        case INS_SET_PARM:
            parm = GetParm( operation );
            break;
        case INS_SEMANTIC:
            result = SSLSemantic( GetParm( operation ), parm );
            token = SSLCurrToken();
            break;
        case INS_KILL:
            SSLError( TERM_KILL, GetParm( operation ) );
            return( TERM_KILL );
        case INS_RETURN:
            if( stk_ptr <= stk_bot )
                return( TERM_NORMAL );
            TblPtr = *--stk_ptr;
            break;
        case INS_IN_CHOICE:
            for( num_items = MGET_U8( TblPtr ), TblPtr++; num_items > 0; num_items-- ) {
                if( token == (tokens)GetParm( operation ) ) {
                    token = SSLNextToken();
                    TblPtr = GetTablePos( table );
                    break;
                }
                TblPtr += 2;
            }
            break;
        case INS_CHOICE:
            for( num_items = MGET_U8( TblPtr ), TblPtr++; num_items > 0; num_items-- ) {
                if( result == GetParm( operation ) ) {
                    TblPtr = GetTablePos( table );
                    break;
                }
                TblPtr += 2;
            }
            break;
        }
    }
}
