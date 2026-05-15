/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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

static unsigned GetParmUInt( bool parm_long )
{
    unsigned    parm;

    /*
     * numbers in PRS file are in little-endian format
     * read it as little-endian unaligned data
     */
    if( parm_long ) {
        parm = MGET_LE_U16_UN( TblPtr );
        TblPtr++;
    } else {
        parm = MGET_U8( TblPtr );
    }
    TblPtr++;
    return( parm );
}


static int GetParmInt( bool parm_long )
{
    int         parm;

    /*
     * numbers in PRS file are in little-endian format
     * read it as little-endian unaligned data
     */
    if( parm_long ) {
        parm = SSL2INT( MGET_LE_U16_UN( TblPtr ) );
        TblPtr++;
    } else {
        parm = MGET_S8( TblPtr );
    }
    TblPtr++;
    return( parm );
}


static const char *GetTablePos( const char *table )
{
    /*
     * numbers in PRS file are in little-endian format
     * read it as little-endian unaligned data
     */
    return( table + MGET_LE_U16_UN( TblPtr ) );
}


int SSLWalk( const char *table, unsigned start, const char **stk_bot, unsigned stk_size )
{
    op_code         opcode;
    bool            parm_long;
    unsigned        num_items;
    signed int      disp;
    const char      *addr;
    const char      **stk_ptr;
    const char      **stk_end;
    unsigned        result;
    unsigned        parm;
    unsigned        wanted;
    tokens          token;

    result = 0;
    parm = 0;
    stk_ptr = stk_bot;
    stk_end = stk_bot + stk_size;
    TblPtr = table + start;
    token = SSLCurrToken();
    for( ;; ) {
        opcode = (op_code)MGET_U8( TblPtr );
        parm_long = ( (opcode & INS_LONG) != 0 );
        TblPtr++;
        switch( opcode & INS_MASK ) {
        case INS_INPUT:
            wanted = GetParmUInt( parm_long );
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
            SSLOutToken( (tokens)GetParmUInt( parm_long ) );
            break;
        case INS_ERROR:
            if( SSLError( TERM_ERROR, GetParmUInt( parm_long ) ) ) {
                return( TERM_ERROR );
            }
            break;
        case INS_JUMP:
            addr = TblPtr - 1;
            TblPtr = addr + GetParmInt( parm_long );
            break;
        case INS_CALL:
            if( stk_ptr >= stk_end ) {
                /* stack overflow */
                SSLError( TERM_STK_OVERFLOW, 0 );
                return( TERM_STK_OVERFLOW );
            }
            addr = TblPtr - 1;
            disp = GetParmInt( parm_long );
            *stk_ptr++ = TblPtr;
            TblPtr = addr + disp;
            break;
        case INS_SET_RESULT:
            result = GetParmUInt( parm_long );
            break;
        case INS_SET_PARM:
            parm = GetParmUInt( parm_long );
            break;
        case INS_SEMANTIC:
            result = SSLSemantic( GetParmUInt( parm_long ), parm );
            token = SSLCurrToken();
            break;
        case INS_KILL:
            SSLError( TERM_KILL, GetParmUInt( parm_long ) );
            return( TERM_KILL );
        case INS_RETURN:
            if( stk_ptr <= stk_bot )
                return( TERM_NORMAL );
            TblPtr = *--stk_ptr;
            break;
        case INS_IN_CHOICE:
            num_items = MGET_U8( TblPtr );
            TblPtr++;
            while( num_items-- > 0 ) {
                if( token == (tokens)GetParmUInt( parm_long ) ) {
                    token = SSLNextToken();
                    TblPtr = GetTablePos( table );
                    break;
                }
                TblPtr += 2;    /* skip position 2-bytes */
            }
            break;
        case INS_CHOICE:
            num_items = MGET_U8( TblPtr );
            TblPtr++;
            while( num_items-- > 0 ) {
                if( result == GetParmUInt( parm_long ) ) {
                    TblPtr = GetTablePos( table );
                    break;
                }
                TblPtr += 2;    /* skip position 2-bytes */
            }
            break;
        }
    }
}
