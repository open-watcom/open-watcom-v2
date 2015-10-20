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


#include "dbgdefn.h"
#include "sslops.h"
#include "dbgsem.h"


static unsigned char    *TblPtr;

enum {
        TERM_NORMAL,
        TERM_SYNTAX,
        TERM_ERROR,
        TERM_STK_OVERFLOW,
        TERM_KILL
    };


static int GetParm( op_code operation )
{
    unsigned_16  parm;

    parm = *TblPtr++;
    if( operation & INS_LONG ) {
        parm |= *TblPtr++ << 8;
    }
    return( parm );
}


static int GetSignedParm( op_code operation )
{
    signed_16  parm;

    parm = *((signed char *)TblPtr++);
    if( operation & INS_LONG ) {
        parm &= 0xff;
        parm |= *TblPtr++ << 8;
    }
    return( parm );
}


int SSLWalk( unsigned char *table, unsigned start, void **stk_bot, unsigned stk_size )
{
    op_code         operation;
    unsigned        num_items;
    signed int      disp;
    unsigned char   *addr;
    void            **stk_ptr;
    void            **stk_end;
    unsigned        result = 0;
    unsigned        parm = 0;
    unsigned        wanted;
    unsigned        token;

    stk_ptr = stk_bot;
    stk_end = stk_bot + stk_size;
    TblPtr = &table[start];
    token = SSLCurrToken();
    for( ;; ) {
        operation = *TblPtr++;
        switch( operation & INS_MASK ) {
        case INS_INPUT:
            wanted = GetParm( operation );
            if( token != wanted ) {
                if( SSLError( TERM_SYNTAX, wanted ) ) return( TERM_SYNTAX );
            }
            token = SSLNextToken();
            break;
        case INS_IN_ANY:
            token = SSLNextToken();
            break;
        case INS_OUTPUT:
            SSLOutToken( GetParm( operation ) );
            break;
        case INS_ERROR:
            if( SSLError( TERM_ERROR, GetParm( operation ) ) ) {
                return( TERM_ERROR );
            }
            break;
        case INS_JUMP:
            addr = TblPtr - 1;
            TblPtr = addr + GetSignedParm( operation );
            break;
        case INS_CALL:
            if( stk_ptr >= stk_end ) {
                /* stack overflow */
                SSLError( TERM_STK_OVERFLOW, 0 );
                return( TERM_STK_OVERFLOW );
            }
            addr = TblPtr - 1;
            disp = GetSignedParm( operation );
            *stk_ptr++ = TblPtr;
            TblPtr = addr + disp;
            break;
        case INS_SET_RESULT:
            result = GetParm( operation );
            break;
        case INS_SET_PARM:
            parm = (signed_16)GetParm( operation );
            break;
        case INS_SEMANTIC:
            result = SSLSemantic( GetParm( operation ), parm );
            token = SSLCurrToken();
            break;
        case INS_KILL:
            SSLError( TERM_KILL, GetParm( operation ) );
            return( TERM_KILL );
        case INS_RETURN:
            if( stk_ptr <= stk_bot ) return( TERM_NORMAL );
            TblPtr = *--stk_ptr;
            break;
        case INS_IN_CHOICE:
            for( num_items = *TblPtr++; num_items > 0; num_items-- ) {
                if( token == GetParm( operation ) ) {
                    token = SSLNextToken();
                    TblPtr = &table[ TblPtr[0] | (TblPtr[1] << 8) ];
                    break;
                }
                TblPtr += 2;
            }
            break;
        case INS_CHOICE:
            for( num_items = *TblPtr++; num_items > 0; num_items-- ) {
                if( result == GetParm( operation ) ) {
                    TblPtr = &table[ TblPtr[0] | (TblPtr[1] << 8) ];
                    break;
                }
                TblPtr += 2;
            }
            break;
        }
    }
}
