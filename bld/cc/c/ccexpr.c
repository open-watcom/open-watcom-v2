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


#include "cvars.h"
#include "ctokens.h"
#include "cerrs.h"
#include <string.h>


extern  TYPEPTR TypeName();
extern  TYPEPTR TypeOf();                       /* cnode */
extern  int     EnumLookup();                   /* cenum */
extern  FMEPTR  MacroLookup();
extern  TOKEN   NextToken();
extern  TREEPTR Expr13();
extern  void    MustRecog();
extern  unsigned long SizeOfArg();              /* csizeof */

long int CExpr1(), CExpr2(), CExpr3(), CExpr4(), CExpr5(), CExpr6(),
         CExpr7(), CExpr8(), CExpr9(), CExpr10(), CExpr11(), ConstId();


long int ConstExpr()
    {
        register long int value, value1, value2;

        value = CExpr1();
        if( CurToken == T_QUESTION ) {
            NextToken();
            value1 = ConstExpr();
            MustRecog( T_COLON );
            value2 = ConstExpr();
/*          value = value ? value1 : value2;  */
            if( value != 0 ) {
                value = value1;
            } else {
                value = value2;
            }
        }
        return( value );
    }


local long int CExpr1()
    {
        register long int value;

        value = CExpr2();
        while( CurToken == T_OR_OR ) {
            NextToken();
            value |= CExpr2();
        }
        return( value );
    }


local long int CExpr2()
    {
        register long int value;

        value = CExpr3();
        while( CurToken == T_AND_AND ) {
            NextToken();
            value &= CExpr3();
        }
        return( value );
    }


local long int CExpr3()
    {
        register long int value;

        value = CExpr4();
        while( CurToken == T_OR ) {
            NextToken();
            value |= CExpr4();
        }
        return( value );
    }


local long int CExpr4()
    {
        register long int value;

        value = CExpr5();
        while( CurToken == T_XOR ) {
            NextToken();
            value = value ^ CExpr5();
        }
        return( value );
    }


local long int CExpr5()
    {
        register long int value;

        value = CExpr6();
        while( CurToken == T_AND ) {
            NextToken();
            value &= CExpr6();
        }
        return( value );
    }


local long int CExpr6()
    {
        register long int value;

        value = CExpr7();
        for( ;; ) {
            if( CurToken == T_EQ ) {
                NextToken();
                value = value == CExpr7();
            } else if( CurToken == T_NE ) {
                NextToken();
                value = value != CExpr7();
            } else {
                break;
            }
        }
        return( value );
    }


local long int CExpr7()
    {
        register long int value;

        value = CExpr8();
        for( ;; ) {

/*      have to know if expressions are signed or unsigned */

            if( CurToken == T_LT ) {
                NextToken();
                value = value < CExpr8();
            } else if( CurToken == T_LE ) {
                NextToken();
                value = value <= CExpr8();
            } else if( CurToken == T_GT ) {
                NextToken();
                value = value > CExpr8();
            } else if( CurToken == T_GE ) {
                NextToken();
                value = value >= CExpr8();
            } else {
                break;
            }
        }
        return( value );
    }


local long int CExpr8()
    {
        register long int value;

        value = CExpr9();
        for( ;; ) {
            if( CurToken == T_RSHIFT ) {
                NextToken();
                value >>= CExpr9();
            } else if( CurToken == T_LSHIFT ) {
                NextToken();
                value <<= CExpr9();
            } else {
                break;
            }
        }
        return( value );
    }


local long int CExpr9()
    {
        register long int value;

        value = CExpr10();
        for( ;; ) {
            if( CurToken == T_PLUS ) {
                NextToken();
                value += CExpr10();
            } else if( CurToken == T_MINUS ) {
                NextToken();
                value -= CExpr10();
            } else {
                break;
            }
        }
        return( value );
    }


local long int CExpr10()
    {
        register long int value;

        value = CExpr11();
        for( ;; ) {
            if( CurToken == T_TIMES ) {
                NextToken();
                value *= CExpr11();
            } else if( CurToken == T_DIVIDE ) {
                NextToken();
                value /= CExpr11();
            } else if( CurToken == T_PERCENT ) {
                NextToken();
                value %= CExpr11();
            } else {
                break;
            }
        }
        return( value );
    }


local long int CExpr11()
    {
        register long int value;
        register TYPEPTR typ;

        value = 0;
        switch( CurToken ) {
        case T_PLUS:
            NextToken();
            value = CExpr11();
            break;
        case T_MINUS:
            NextToken();
            value = - CExpr11();
            break;
        case T_EXCLAMATION:
            NextToken();
            value = ! CExpr11();
            break;
        case T_TILDE:
            NextToken();
            value = ~ CExpr11();
            break;
        case T_SIZEOF:
            NextToken();
            ++SizeOfCount;
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                typ = TypeName();
                if( typ == NULL ) {
                    typ = TypeOf( Expr13() );
                }
                MustRecog( T_RIGHT_PAREN );
            } else {
                typ = TypeOf( Expr13() );
            }
            value = SizeOfArg( typ );
            --SizeOfCount;
            break;
        case T_LEFT_PAREN:
            NextToken();
            typ = TypeName();
            if( typ != NULL ) {
                MustRecog( T_RIGHT_PAREN );
                value = ConstExpr();
            } else {
                value = ConstExpr();
                MustRecog( T_RIGHT_PAREN );
            }
            break;
        case T_ID:
            value = ConstId();
            break;
        case T_CONSTANT:
            value = Constant;
            NextToken();
            break;
        }
        return( value );
    }


local long int ConstId()
    {
        register long int value;
        register int    enum_var;
        auto     struct enum_info ei;

        if( CompFlags.pre_processing ) {
            if( strcmp( "defined", Buffer ) == 0 ) {
                CompFlags.pre_processing = 2;      /* don't want macro expanded */
                NextToken();
                if( CurToken == T_LEFT_PAREN ) {
                    NextToken();
                    value = MacroLookup() != NULL;
                    CompFlags.pre_processing = 1;
                    NextToken();
                    MustRecog( T_RIGHT_PAREN );
                } else {
                    value = MacroLookup() != NULL;
                    CompFlags.pre_processing = 1;
                    NextToken();
                }
            } else {
                NextToken();                    /* 16-mar-88 */
                value = 0;
            }
        } else {
            enum_var = EnumLookup( HashValue, Buffer, &ei );
            NextToken();
            if( enum_var ) {
                value = ei.value;
            } else {
                CErr2p( ERR_NOT_A_CONSTANT_EXPR, Buffer );
                value = 0;
            }
        }
        return( value );
    }

