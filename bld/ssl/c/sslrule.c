/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include "ssl.h"
#include "sslint.h"


static void Action( symbol *, instruction *lbl );

static void Cycle( symbol *rule )
{
    instruction      *lbl_top;
    instruction      *lbl_bot;

    Scan();
    lbl_top = NewLabel();
    GenLabel( lbl_top );
    lbl_bot = NewLabel();
    do {
        Action( rule, lbl_bot );
    } while( CurrToken != T_RITE_BRACE );
    GenJump( lbl_top );
    GenLabel( lbl_bot );
    Scan();
}


static void SemCall( symbol *call )
{
    symbol      *parm;

    Scan();
    if( call->v.sem.parm != NULL ) {
        if( CurrToken != T_LEFT_PAREN )
            Error( "expecting '('" );
        Scan();
        parm = Lookup( CLASS_ENUMS );
        if( call->v.sem.parm != parm->v.enums.type ) {
            Error( "improper type for semantic parm" );
        }
        GenSetParm( parm->v.enums.value );
        Scan();
        if( CurrToken != T_RITE_PAREN )
            Error( "expecting ')'" );
        Scan();
    }
    GenSemCall( call->v.sem.value );
}


static void Choice( symbol *rule, instruction *lbl_exit )
{
    instruction *choice = NULL;
    instruction *lbl_def;
    bool        def;
    ssl_class   typ = CLASS_INPUT;
    int         first_value = 0;
    instruction *lbl_first;
    instruction *lbl_bot;
    instruction *lbl_curr;
    symbol      *ret = NULL;
    symbol      *call = NULL;
    symbol      *sym;

    Scan();
    switch( CurrToken ) {
    case T_OR:
        choice = GenInpChoice();
        ret = NULL;
        typ = CLASS_INPUT;
        break;
    case T_AT:
        Scan();
        call = Lookup( CLASS_RULE );
        ret = call->v.rule.ret;
        if( call->v.rule.defined && ( ret == NULL ) ) {
            Error( "rule has no return type" );
        }
        Scan();
        GenLblCall( call->v.rule.lbl );
        choice = GenChoice();
        typ = CLASS_ENUMS;
        break;
    case T_NAME:
        call = Lookup( CLASS_SEM );
        ret = call->v.sem.ret;
        if( ret == NULL )
            Error( "semantic action has no return type" );
        SemCall( call );
        choice = GenChoice();
        typ = CLASS_ENUMS;
        break;
    }
    lbl_bot = NewLabel();
    lbl_def = NewLabel();
    GenJump( lbl_def );
    def = false;
    lbl_first = NULL;
    while( CurrToken == T_OR ) {
        lbl_curr = NewLabel();
        Scan();
        for( ;; ) {
            if( CurrToken == T_STAR ) {
                if( def )
                    Error( "default case already processed" );
                def = true;
                GenLabel( lbl_def );
            } else {
                sym = Lookup( typ );
                if( typ == CLASS_INPUT ) {
                    GenTblLabel( choice, lbl_curr, sym->v.token );
                    if( lbl_first == NULL ) {
                        lbl_first = lbl_curr;
                        first_value = sym->v.token;
                    }
                } else if( sym->v.enums.type == ret ) {
                    GenTblLabel( choice, lbl_curr, sym->v.enums.value );
                    if( lbl_first == NULL ) {
                        lbl_first = lbl_curr;
                        first_value = sym->v.enums.value;
                    }
                } else if( ret == NULL ) {
                    /* forward referenced rule */
                    ret = sym->v.enums.type;
                    call->v.rule.ret = ret;
                } else {
                    Error( "choice label is improper type" );
                }
            }
            Scan();
            if( CurrToken != T_COMMA )
                break;
            Scan();
        }
        WantColon();
        GenLabel( lbl_curr );
        while( CurrToken != T_OR && CurrToken != T_RITE_BRACKET ) {
            Action( rule, lbl_exit );
        }
        if( CurrToken == T_RITE_BRACKET )
            break;
        GenJump( lbl_bot );
    }
    if( !def ) {
        GenJump( lbl_bot );
        GenLabel( lbl_def );
        if( typ == CLASS_INPUT ) {
            GenInput( first_value );
            GenJump( lbl_first );
        } else {
            GenKill();
        }
    }
    GenLabel( lbl_bot );
    Scan();
}

static void Action( symbol *rule, instruction *lbl_exit )
{
    symbol      *sym;
    symbol      *call;
    symbol      *ret;

    switch( CurrToken ) {
    case T_NAME:
    case T_LITERAL:
        sym = Lookup( CLASS_ANY );
        if( sym->typ == CLASS_INPUT ) {
            GenInput( sym->v.token );
            Scan();
        } else if( sym->typ == CLASS_SEM ) {
            SemCall( sym );
        } else {
            Error( "expecting token or semantic action" );
        }
        break;
    case T_QUESTION:
        GenInputAny();
        Scan();
        break;
    case T_DOT:
        Scan();
        sym = Lookup( CLASS_OUTPUT );
        GenOutput( sym->v.token );
        Scan();
        break;
    case T_POUND:
        Scan();
        sym = Lookup( CLASS_ERROR );
        GenError( sym->v.token );
        Scan();
        break;
    case T_LEFT_BRACE:
        Cycle( rule );
        break;
    case T_GT:
        if( lbl_exit == NULL )
            Error( "no cycle to exit from" );
        GenJump( lbl_exit );
        Scan();
        break;
    case T_LEFT_BRACKET:
        Choice( rule, lbl_exit );
        break;
    case T_AT:
        Scan();
        call = Lookup( CLASS_RULE );
        GenLblCall( call->v.rule.lbl );
        Scan();
        break;
    case T_GT_GT:
        Scan();
        if( rule->v.rule.ret != NULL ) {
            if( CurrToken != T_NAME )
                Error( "expecting name" );
            ret = Lookup( CLASS_ENUMS );
            if( ret->v.enums.type != rule->v.rule.ret ) {
                Error( "improper return type" );
            }
            GenSetResult( ret->v.enums.value );
            Scan();
        }
        GenReturn();
        break;
    default:
        Error( "unknown action" );
        break;
    }
}

void Rules( void )
{
    symbol      *sym;
    symbol      *ret;
    bool        exported;

    exported = true;       /* first rule always exported */
    do {
        sym = Lookup( CLASS_RULE );
        if( sym->v.rule.defined ) {
            Error( "multiple definitions" );
        }
        sym->v.rule.defined = true;
        Scan();
        if( CurrToken == T_GT_GT ) {
            Scan();
            ret = Lookup( CLASS_TYPE );
            if( sym->v.rule.ret != NULL && sym->v.rule.ret != ret ) {
                Error( "return type does not match previous usage" );
            }
            sym->v.rule.ret = ret;
            Scan();
        } else if( sym->v.rule.ret != NULL ) {
            Error( "expecting return type" );
        }
        WantColon();
        if( CurrToken == T_COLON ) {
            Scan();
            sym->v.rule.exported = true;
            exported = true;
        }
        if( exported ) {
            GenExportLabel( sym->v.rule.lbl );
        } else {
            GenLabel( sym->v.rule.lbl );
        }
        while( CurrToken != T_SEMI ) {
            Action( sym, NULL );
        }
        Scan();
        if( sym->v.rule.ret == NULL ) {
            GenReturn();
        } else {
            GenKill();
        }
        exported = false;
    } while( CurrToken != T_EOF );
}
