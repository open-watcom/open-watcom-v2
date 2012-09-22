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


#include <string.h>
#include <stdlib.h>
#include "disasm.h"

static  const ins_name        PrefixName[] = {
    I_LOCK,
    0,
    I_REPNE,
    I_REPE
};


static  void            PadBlanks( char * );

#ifdef O2A
#define START_INDEX     (DO_UNIX ? '(' : '[')
#define END_INDEX       (DO_UNIX ? ')' : ']')
#else
#define START_INDEX     '['
#define END_INDEX       ']'
#endif


static  bool    IsJumpIns( uint_16 opcode )
/*****************************************/
{
    switch( opcode ) {
    case I_JMP:
    case I_JA:
    case I_JAE:
    case I_JB:
    case I_JBE:
    case I_JE:
    case I_JNE:
    case I_JG:
    case I_JGE:
    case I_JL:
    case I_JLE:
    case I_JO:
    case I_JNO:
    case I_JS:
    case I_JNS:
    case I_JP:
    case I_JPO:
        return( TRUE );
    }
    return( FALSE );
}


static void FormIndex( operand * op, char * buff )
/************************************************/

{
    const char        *src;

    if( op->mode != ADDR_INDEX ) {
        src = X86RegisterName[ op->base ];
#ifdef O2A
        if( *src  &&  DO_UNIX ) {       /* FORM_ASSMEBLER is implied */
            *buff++ = '%';
        }
#endif
        while( *src ) {
            *buff++ = *src++;
        }
    }
    if( op->mode != ADDR_BASE ) {
#ifdef O2A
            if( DO_UNIX ) {
                *buff++ = ',';
            } else /* note missing '{' -- don't change */
#endif
            if ( op->mode == ADDR_BASE_INDEX ) {
                *buff++ = '+';
            }
        src = X86RegisterName[ op->index ];
#ifdef O2A
        if( *src  &&  DO_UNIX ) {       /* FORM_ASSEMBLER is implied */
            *buff++ = '%';
        }
#endif
        while( *src ) {
            *buff++ = *src++;
        }
        if( op->scale != 1 ) {
#ifdef O2A
            if( DO_UNIX ) {
                *buff++ = ',';
            } else {
#endif
                *buff++ = '*';
#ifdef O2A
            }
#endif
            *buff++ = op->scale + '0';
        }
    }
    *buff = '\0';
}

/*
 * We've got a string instruction with a segment override, have to
 * fiddle things so that the override gets shown.
 */
static void FixStringIns( instruction *ins )
{
    operand     *op;
    unsigned    size;

    size = 0;
    switch( ins->opcode )
    {
        case I_MOVSD:
            ++size;
            /* fall through */
        case I_MOVSB:
            ++size;
            /* fall through */
        case I_MOVSW:
            ins->opcode = I_MOVS;
            ins->num_oper = 2;
            ins->mem_ref_op = OP_2;
            ins->op[ OP_1 ].mode = ADDR_ES_DI;
            if( ins->pref & ADDR_LONG ) {
                ins->op[ OP_1 ].mode++;
            }
            break;
        case I_CMPSD:
            ++size;
            /* fall through */
        case I_CMPSW:
            ++size;
            /* fall through */
        case I_CMPSB:
            ins->opcode = I_CMPS;
            ins->num_oper = 2;
            ins->mem_ref_op = OP_1;
            ins->op[ OP_2 ].mode = ADDR_ES_DI;
            if( ins->pref & ADDR_LONG ) {
                ins->op[ OP_2 ].mode++;
            }
            break;
        case I_LODSD:
            ++size;
            /* fall through */
        case I_LODSW:
            ++size;
            /* fall through */
        case I_LODSB:
            ins->opcode = I_LODS;
            ins->num_oper = 1;
            ins->mem_ref_op = OP_1;
            break;
        case I_OUTSD:
            ++size;
            /* fall through */
        case I_OUTSW:
            ++size;
            /* fall through */
        case I_OUTSB:
            ins->opcode = I_OUTS;
            ins->num_oper = 2;
            ins->mem_ref_op = OP_2;
            ins->op[ OP_1 ].mode = ADDR_REG;
            ins->op[ OP_1 ].base = DX_REG;
            break;
        default:
            return;
    }
    switch( size ) {
    case 0:
        ins->modifier = MOD_BYTE;
        break;
    case 1:
        ins->modifier = MOD_WORD;
        break;
    case 2:
        ins->modifier = MOD_LINT;
        break;
    }
    op = &ins->op[ ins->mem_ref_op ];
    op->mode = ADDR_BASE;
    op->disp = 0;
    op->scale = 1;
    op->size = 0;
    if( ins->pref & ADDR_LONG ) {
        op->base = ESI_REG;
    } else {
        op->base = SI_REG;
    }
}

static char *DumpOverride( char *buff, int seg_override )
{
#ifdef O2A
    if( DO_UNIX ) {
        *buff++ = '%';
    }
#endif
    strcpy( buff, X86RegisterName[ seg_override ] );
    buff += strlen( buff );
    *buff++ = ':';
    *buff = '\0';
    return( buff );
}


void  FormatIns( char *buf, instruction *curr_ins, form_option format )
/*********************************************************************/

{
    const char          *name;
    int                 len;
    operand             *op;
    int                 i;
    int                 seg_override;
    char                prefix;
    char                tmp_buff[ 80 ];
    char                *ptr;

    if( curr_ins->opcode == I_INVALID ) {
        strcpy( buf, "?????" );
        return;
    }
    CurrIns = *curr_ins;
    prefix = FALSE;
    for( i = 0; i <= 3; ++i ) {
        if( CurrIns.pref & PrefixTab[ i ] ) {
            strcpy( tmp_buff, InsName[ PrefixName[ i ] ] );
            PadBlanks( tmp_buff );
            if( format & FORM_NAME_UPPER ) {
                ZapUpper( tmp_buff );
            }
            strcat( buf, tmp_buff );
            prefix = TRUE;
        }
    }
    seg_override = NULL_REG;
    if( CurrIns.pref & PREF_xS ) {
        seg_override = CurrIns.seg_used;
        FixStringIns( &CurrIns );
    }
    if( CurrIns.opcode < FIRST_WTK_INS ) {
        name = InsName[ CurrIns.opcode ];
    } else {
        name = GetWtkInsName( CurrIns.opcode - FIRST_WTK_INS );
    }
    strcpy( tmp_buff, name );
#ifdef O2A
    if( DO_UNIX ) {
        ToUnixInsName( tmp_buff, &CurrIns );
    }
#endif
    if( format & FORM_NAME_UPPER ) {
        ZapUpper( tmp_buff );
    }
    strcat( buf, tmp_buff );
    PadBlanks( buf );
    if( prefix ) {          /* if a prefix like 'lock', need space after name */
        strcat( buf, " " );
    }

    tmp_buff[ 0 ] = '\0';
#ifdef O2A
    if( !DO_UNIX ) {
#endif
        if( CurrIns.opcode == I_CALL_FAR || CurrIns.opcode == I_JMP_FAR ) {
            if( CurrIns.modifier == MOD_NONE ) {
                if( format & FORM_ASSEMBLER ) {
                    strcpy( tmp_buff, "far ptr " );
                } else {
                    strcpy( tmp_buff, "far " );
                }
            }
        } else if( CurrIns.opcode == I_CALL ||
                ( IsJumpIns( CurrIns.opcode ) && CurrIns.ins_size != 2 ) ) {
            if( CurrIns.modifier == MOD_NONE ) {
                if( format & FORM_ASSEMBLER ) {
                    strcpy( tmp_buff, "near ptr " );
                }
            }
        }
        if( format & FORM_NAME_UPPER ) {
            ZapUpper( tmp_buff );
        }
        strcat( buf, tmp_buff );

#ifdef O2A
    } else if( CurrIns.num_oper >= 2 ) { // swap operands
        operand tmp;
        int     last;

        last = CurrIns.num_oper - 1;
        tmp = CurrIns.op[ OP_1 ];
        CurrIns.op[ OP_1 ] = CurrIns.op[ last ];
        CurrIns.op[ last ] = tmp;
        if( CurrIns.mem_ref_op == 0 ) {
            CurrIns.mem_ref_op = last;
        } else if( CurrIns.mem_ref_op == last ) {
            CurrIns.mem_ref_op = 0;
        }
    }
#endif
    for( i = 0; i < CurrIns.num_oper; ++i ) {
        if( i > 0 ) {
            len = strlen( buf );
            buf[ len ] = ',';
            buf[ len + 1 ] = '\0';
        }
        op = &CurrIns.op[ i ];
        if( CurrIns.mem_ref_op == i ) {
            if(
#ifdef O2A
                !DO_UNIX  &&
#endif
                CurrIns.modifier != MOD_NONE ) {
                strcpy( tmp_buff, ModifierTab[ CurrIns.modifier ] );
                if( format & FORM_NAME_UPPER ) {
                    ZapUpper( tmp_buff );
                }
                strcat( buf, tmp_buff );
            }
            if( seg_override != NULL_REG ) {
                DumpOverride( tmp_buff, seg_override );
                if( format & FORM_REG_UPPER ) ZapUpper( tmp_buff );
                strcat( buf, tmp_buff );
            }
        }
        switch( op->mode ) {
        case ADDR_REG:
            ptr = tmp_buff;
#ifdef O2A
            if( DO_UNIX ) {
                *ptr++ = '%';
            }
#endif
            strcpy( ptr, X86RegisterName[ op->base ] );
            if( format & FORM_REG_UPPER ) ZapUpper( ptr );
            strcat( buf, tmp_buff );
            break;
        case ADDR_CONST:
#ifdef O2A
            if( DO_UNIX ) {
                strcat( buf, "$" );
            }
#endif
            if( op->size == 0 ) {
                len = strlen( buf );
                ltoa( op->disp, &buf[ len ], 10 );
            } else {
                strcat( buf, ToStr( op->disp, 2 * op->size, op->offset ) );
            }
            break;
        case ADDR_LABEL:
#ifdef O2A
            if( !DO_UNIX  &&
                ( format & FORM_ASSEMBLER && IsJumpIns( CurrIns.opcode ) ) ) {
                if( CurrIns.ins_size == 2 ) {
                    if( format & FORM_NAME_UPPER ) {
                        strcat( buf, "SHORT " );
                    } else {
                        strcat( buf, "short " );
                    }
                }
            }
#endif
            strcat( buf, JmpLabel( op->disp, op->offset ) );
            break;
        case ADDR_ABS:
            strcat( buf, ToBrStr( op->disp, op->offset ) );
            break;
        case ADDR_BASE:
        case ADDR_INDEX:
        case ADDR_BASE_INDEX:
            if(
#ifdef O2A
                DO_UNIX  ||
#endif
                ( format & FORM_INDEX_IN ) == 0 ) {
                if( op->size != 0 ) {
                    strcat( buf, ToIndex( op->disp, op->offset ) );
                }
            }
            len = strlen( buf );
            buf[ len ] = START_INDEX;
            FormIndex( op, tmp_buff );
            if( format & FORM_REG_UPPER ) {
                ZapUpper( tmp_buff );
            }
            strcpy( &buf[ len + 1 ], tmp_buff );
            if(
#ifdef O2A
                !DO_UNIX  &&
#endif
                ( format & FORM_INDEX_IN ) ) {
                if( op->size != 0 ) {
                    name = ToIndex( op->disp, op->offset );
                    if( *name != '+' && *name != '-' ) {
                        strcat( buf, "+" );
                    }
                    strcat( buf, name );
                }
            }
            len = strlen( buf );
            buf[ len ] = END_INDEX;
            buf[ len + 1 ] = '\0';
            break;
        case ADDR_SEG_OFFSET:
            /* modifier or seg override */
            strcat( buf, ToSegStr( op->disp, (op+1)->disp, op->offset ) );
            break;
        case ADDR_ES_DI:
        case ADDR_ES_EDI:
            /* we have an overridden string instruction */
            ptr = DumpOverride( tmp_buff, ES_REG );
            *ptr++ = START_INDEX;
            strcpy( ptr,
                X86RegisterName[ op->mode == ADDR_ES_DI ? DI_REG : EDI_REG ] );
            ptr += strlen( ptr );
            *ptr++ = END_INDEX;
            *ptr = '\0';
            if( format & FORM_REG_UPPER ) ZapUpper( tmp_buff );
            strcat( buf, tmp_buff );
            break;
        case ADDR_WTK:
            {
                char * tmpstr;
                wop_type tmptype = (wop_type)op->size;
                if( tmptype == WST  ||  tmptype == ANY_SINGLE ) {
                    tmpstr = "ws";
                } else {
                    tmpstr = "wd";
                }
                strcat( buf, tmpstr );
                itoa( (int)op->disp, &buf[ strlen( buf ) ], 10 );
            }
            break;
        case ADDR_WTK_OPCODE:
            strcat( buf, InsName[ op->disp ] );
            break;
        }
    }
    if( CurrIns.pref & EMU_INTERRUPT ) {
        strcat( buf, "; int " );
        strcat( buf, ToStr( CurrIns.op[ OP_3 ].disp, 2, -1 ) );
    }
}


#ifndef O2A
static
#endif
void  ZapUpper( char *str )
/*************************/

{
    while( *str != '\0' ) {
        if( *str >= 'a' ) {
            *str &= 0xdf;
        }
        ++str;
    }
}


static  void  PadBlanks( char *str )
/**********************************/

{
    int                 oldlen;
    int                 padlen;

    oldlen = strlen( str );
    padlen = OPCODE_LEN - oldlen % OPCODE_LEN;
    memset( &str[ oldlen ], ' ', padlen );
    str[ oldlen + padlen ] = '\0';
}
