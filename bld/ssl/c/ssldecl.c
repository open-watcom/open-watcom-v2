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


#include <stdlib.h>

#include "ssl.h"
#include "sslint.h"


extern token    CurrToken;
extern char     TokenBuff[];



static void ProcTokens( class typ, unsigned *next )
{
    symbol      *sym;
    unsigned    value;

    WantColon();
    do {
        if( CurrToken != T_NAME ) Error( "expecting name" );
        sym = NewSym( typ );
        Scan();
        switch( typ ) {
        case CLASS_INPUT:
        case CLASS_OUTPUT:
        case CLASS_INOUT:
            if( CurrToken == T_LITERAL ) {
                NewAlias( sym );
                Scan();
            }
        }
        if( CurrToken == T_EQUALS ) {
            Scan();
            value = GetNum();
            Scan();
        } else {
            value = *next;
        }
        sym->v.token = value;
        if( value >= *next ) *next = value + 1;
    } while( CurrToken != T_SEMI );
    Scan();
}


static void ProcType(void)
{
    symbol      *type;
    symbol      *curr;
    unsigned    next_value;

    next_value = 0;
    type = NewSym( CLASS_TYPE );
    Scan();
    WantColon();
    do {
        curr = NewSym( CLASS_ENUMS );
        curr->v.enums.type = type;
        Scan();
        if( CurrToken == T_EQUALS ) {
            Scan();
            curr->v.enums.value = GetNum();
            Scan();
        } else {
            curr->v.enums.value = next_value;
        }
        if( curr->v.enums.value >= next_value ) {
            next_value = curr->v.enums.value + 1;
        }
    } while( CurrToken != T_SEMI );
    Scan();
}


static void ProcMech( unsigned *next )
{
    symbol      *sym;

    if( CurrToken != T_NAME ) Error( "expecting name" );
    Scan();
    WantColon();
    do {
        sym = NewSym( CLASS_SEM );
        sym->v.sem.parm = NULL;
        sym->v.sem.ret = NULL;
        Scan();
        if( CurrToken == T_LEFT_PAREN ) {
            Scan();
            sym->v.sem.parm = Lookup( CLASS_TYPE );
            Scan();
            if( CurrToken != T_RITE_PAREN ) Error( "expecting ')'" );
            Scan();
        }
        if( CurrToken == T_GT_GT ) {
            Scan();
            sym->v.sem.ret = Lookup( CLASS_TYPE );
            Scan();
        }
        if( CurrToken == T_EQUALS ) {
            Scan();
            sym->v.sem.value = GetNum();
            Scan();
        } else {
            sym->v.sem.value = *next;
        }
        if( sym->v.sem.value >= *next ) *next = sym->v.sem.value + 1;
    } while( CurrToken != T_SEMI );
    Scan();
}

void Decls(void)
{
    unsigned    next_inp;
    unsigned    next_out;
    unsigned    next_err;
    unsigned    next_sem;

    next_inp = 0;
    next_out = 0;
    next_err = 0;
    next_sem = 0;

    for( ;; ) {
        switch( CurrToken ) {
        case T_INPUT:
            Scan();
            if( CurrToken == T_OUTPUT ) {
                Scan();
                if( next_out > next_inp ) next_inp = next_out;
                ProcTokens( CLASS_INOUT, &next_inp );
                if( next_inp > next_out ) next_out = next_inp;
            } else {
                ProcTokens( CLASS_INPUT, &next_inp );
            }
            break;
        case T_OUTPUT:
            Scan();
            ProcTokens( CLASS_OUTPUT, &next_out );
            break;
        case T_ERROR:
            Scan();
            ProcTokens( CLASS_ERROR, &next_err );
            break;
        case T_TYPE:
             Scan();
             ProcType();
             break;
        case T_MECH:
            Scan();
            ProcMech( &next_sem );
            break;
        default:
            return;
        }
    }
}
