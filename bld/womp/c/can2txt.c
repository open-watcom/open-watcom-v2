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
#include "womp.h"
#include "can2txt.h"
#include "genutil.h"
#include "cantype.h"
#include "namemgr.h"
#include "cansymb.h"
#include "canaddr.h"
#include "canmisc.h"
#include "myassert.h"
#include "fixup.h"
#include "objrec.h"
#include "objprs.h"

STATIC const char empty[] = "";
STATIC const char farStr[] = "FAR";
STATIC const char nearStr[] = "NEAR";
STATIC const char _386Str[] = " 386";

STATIC const char *locMethods[] = {
/*FIX_LO_BYTE       */  "LOBYTE",
/*FIX_OFFSET        */  "OFFSET",
/*FIX_BASE          */  "BASE",
/*FIX_POINTER       */  "POINTER",
/*FIX_HI_BYTE       */  "HIBYTE",
/*FIX_OFFSET386     */  "OFFSET386",
/*FIX_POINTER386    */  "POINTER386"
};

STATIC const char *frameMethods[] = {
/*F_SEG,    T_SEGWD */  "SI",
/*F_GRP,    T_GRPWD */  "GI",
/*F_EXT,    T_EXTWD */  "EI",
/*F_ABS,    T_ABSWD */  "",
/*F_LOC             */  "LOCATION",
/*F_TARG            */  "TARGET",
/*F_NONE            */  "NONE"
};

STATIC void printAddrHdl( const char *prefix, addr_handle hdl ) {

    addr_info   *addr;
    fixup       *fix;
    int         i;
    size_t      data_len;

    addr = CanAFind( hdl );
    if( addr == NULL ) {
        PrtFmt( "%s", prefix );
        PrtMsg( MSG_TXT_NO_ADDR );
        return;
    }
    PrtFmt( "%sdata", prefix );
    data_len = addr->data_len;
    for( i = 0; i < data_len; ++i ) {
        PrtFmt( " %t", addr->data[ i ] );
    }
    PrtFmt( "\n" );
    fix = addr->fixup;
    if( fix != NULL ) {
/**/    myassert( fix->loc_method <= FIX_POINTER386 );
        PrtFmt( "%sfixup %s_REL  %s%s  Frame: %s", prefix,
            fix->self_relative ? "SELF" : "SEG",
            fix->loader_resolved ? "LR" : empty,
            locMethods[ fix->loc_method ],
            frameMethods[ fix->lr.frame ] );
/**/    myassert( fix->lr.frame <= F_NONE && fix->lr.target <= T_ABSWD );
        if( fix->lr.frame < F_LOC ) {
            PrtFmt( "(%x)", fix->lr.frame_datum );
        }
        PrtFmt( "  Target: %s(%x)",
                frameMethods[ fix->lr.target ], fix->lr.target_datum );
        if( fix->lr.target_offset != 0 ) {
            PrtFmt( ",%X\n", fix->lr.target_offset );
        } else {
            PrtFmt( "\n" );
        }
    }
}

STATIC const struct {
    const char  *name;
    uint_8      size;
} typeData[] = {
/*CANT_RESERVED     */  { "RESERVED",       0 },
/*CANT_INTEGER      */  { "INTEGER",        1 },
/*CANT_REAL         */  { "REAL",           1 },
/*CANT_VOID         */  { "VOID",           0 },
/*CANT_COMPLEX      */  { "COMPLEX",        1 },
/*CANT_TYPEDEF      */  { "TYPEDEF",        1 },
/*CANT_SUBRANGE     */  { "SUBRANGE",       1 },
/*CANT_ARRAY        */  { "ARRAY",          1 },
/*CANT_ARRAY_ZERO   */  { "ARRAY_ZERO",     1 },
/*CANT_ARRAY_DESC   */  { "ARRAY_DESC",     0 },
/*CANT_POINTER      */  { "POINTER",        1 },
/*CANT_ENUM         */  { "ENUM",           1 },
/*CANT_STRUCT       */  { "STRUCT",         1 },
/*CANT_PROCEDURE    */  { "PROCEDURE",      0 },
/*CANT_CHARBLOCK    */  { "CHARBLOCK",      1 },
/*CANT_CHARBLOCK_IND*/  { "CHARBLOCK_IND",  0 }
};

STATIC const char *scopeTxt[] = {
/*CANT_SCOPE_NULL   */  "none",
/*CANT_SCOPE_STRUCT */  "struct",
/*CANT_SCOPE_UNION  */  "union",
/*CANT_SCOPE_ENUM   */  "enum"
};

#define INDENT  "    "
#define END "\n"

STATIC int doPrintType( cantype *type, void *parm ) {

    uint            tmp;

    parm = parm;
    if( type == NULL ) {
        return( 0 );
    }
/**/myassert( type->class < CANT_CHARBLOCK_IND );
    if( typeData[ type->class ].size ) {
        PrtFmt( "%x: %s size %X bits", type->hdl,
            typeData[ type->class ].name, type->size );
    } else {
        PrtFmt( "%x: %s", type->hdl, typeData[ type->class ].name );
    }
    switch( type->class ) {
    case CANT_RESERVED:
    case CANT_REAL:
    case CANT_VOID:
    case CANT_COMPLEX:
        PrtFmt( "\n" END );
        break;

    case CANT_INTEGER:
        PrtFmt( " %sSIGNED\n" END, type->sgned ? empty : "UN" );
        break;

    case CANT_TYPEDEF:
        PrtFmt( "\n" INDENT "scope %s type %x name \"%s\"\n" END,
            scopeTxt[ type->d.typdef.scope ], type->d.typdef.type,
            NameGet( type->d.typdef.name ) );
        break;

    case CANT_SUBRANGE:
        PrtFmt( "\n" INDENT "base_type %x(%sSIGNED) low %X high %X\n" END,
            type->d.subrng.base_type,
            type->sgned ? empty : "UN",
            type->d.subrng.low,
            type->d.subrng.high );
        break;

    case CANT_ARRAY:
        PrtFmt( "\n" INDENT "base_type %x index_type %x\n" END,
            type->d.array.base_type, type->d.array.index_type );
        break;

    case CANT_ARRAY_ZERO:
        PrtFmt( "\n" INDENT "base_type %x high_bound %X\n" END,
            type->d.arrayz.base_type, type->d.arrayz.high );
        break;

    case CANT_ARRAY_DESC:
        PrtFmt( "\n" INDENT "base_type lo_type %x hi_type %x\n",
            type->d.arrayd.base_type, type->d.arrayd.lo_type,
            type->d.arrayd.hi_type );
        printAddrHdl( INDENT, type->d.arrayd.bounds );
        PrtFmt( END );
        break;

    case CANT_POINTER:
        tmp = type->d.pointr.class;
        PrtFmt( " base_type %x %s%s%s%s\n" END, type->d.pointr.base_type,
            tmp & CANT_PTR_FAR  ? farStr    : nearStr,
            tmp & CANT_PTR_HUGE ? " HUGE"   : empty,
            tmp & CANT_PTR_DEREF? " DEREF"  : empty,
            tmp & CANT_PTR_386  ? _386Str   : empty
        );
        break;

    case CANT_ENUM:
        {
            enum_const *enum_c;
            enum_const *enum_cstop;

            PrtFmt( " base_type %x(%sSIGNED) num_consts %x\n",
                type->d.enumr.base_type,
                type->sgned ? empty : "UN",
                type->d.enumr.num_consts );
            enum_c = type->d.enumr.consts;
            if( enum_c != NULL ) {
                enum_cstop = enum_c + type->d.enumr.num_consts;
                while( enum_c < enum_cstop ) {
                    PrtFmt( INDENT "value %X name \"%s\"\n",
                        enum_c->value, NameGet( enum_c->name ) );
                    ++enum_c;
                }
            }
            PrtFmt( END );
        }
        break;

    case CANT_STRUCT:
        {
            struct_field    *field;
            struct_field    *field_stop;

            PrtFmt( " num_entries %x\n", type->d.strct.num_fields );
            field = type->d.strct.fields;
            if( field != NULL ) {
                field_stop = field + type->d.strct.num_fields;
                while( field < field_stop ) {
                    PrtFmt( INDENT "bit_offset %X type %x name \"%s\"\n",
                        field->bit_offset, field->type, NameGet( field->name ));
                    ++field;
                }
            }
            PrtFmt( END );
        }
        break;

    case CANT_PROCEDURE:
        {
            proc_parm   *parm;
            proc_parm   *parm_stop;
            uint_8      parm_num;

            tmp = type->d.proc.class;
            PrtFmt( "\n" INDENT "%s%s ret_type %x num_parms %t\n",
                tmp & CANT_PROC_FAR ? farStr : nearStr,
                tmp & CANT_PROC_386 ? _386Str : empty,
                type->d.proc.ret_type,
                type->d.proc.num_parms
            );
            parm = type->d.proc.parms;
            if( parm != NULL ) {
                parm_stop = parm + type->d.proc.num_parms;
                parm_num = 0;
                while( parm < parm_stop ) {
                    PrtFmt( INDENT "parm_num %t parm_type %x\n",
                        parm_num, parm->type );
                    ++parm_num;
                    ++parm;
                }
            }
            PrtFmt( END );
        }
        break;

    case CANT_CHARBLOCK:
        PrtFmt( " length %X\n" END, type->d.charb.length );
        break;

    case CANT_CHARBLOCK_IND:
        PrtFmt( " length_type %x\n", type->d.charbi.length_type );
        printAddrHdl( INDENT "length ", type->d.charbi.length );
        PrtFmt( END );
        break;

    default:
/**/    never_reach();
    }
    return( 0 );
}

STATIC const char *regNames[] = {
/*CANS_REG_AL   */  "AL",
/*CANS_REG_AH   */  "AH",
/*CANS_REG_BL   */  "BL",
/*CANS_REG_BH   */  "BH",
/*CANS_REG_CL   */  "CL",
/*CANS_REG_CH   */  "CH",
/*CANS_REG_DL   */  "DL",
/*CANS_REG_DH   */  "DH",
/*CANS_REG_AX   */  "AX",
/*CANS_REG_BX   */  "BX",
/*CANS_REG_CX   */  "CX",
/*CANS_REG_DX   */  "DX",
/*CANS_REG_SI   */  "SI",
/*CANS_REG_DI   */  "DI",
/*CANS_REG_BP   */  "BP",
/*CANS_REG_SP   */  "SP",
/*CANS_REG_CS   */  "CS",
/*CANS_REG_SS   */  "SS",
/*CANS_REG_DS   */  "DS",
/*CANS_REG_ES   */  "ES",
/*CANS_REG_ST0  */  "ST0",
/*CANS_REG_ST1  */  "ST1",
/*CANS_REG_ST2  */  "ST2",
/*CANS_REG_ST3  */  "ST3",
/*CANS_REG_ST4  */  "ST4",
/*CANS_REG_ST5  */  "ST5",
/*CANS_REG_ST6  */  "ST6",
/*CANS_REG_ST7  */  "ST7",
/*CANS_REG_EAX  */  "EAX",
/*CANS_REG_EBX  */  "EBX",
/*CANS_REG_ECX  */  "ECX",
/*CANS_REG_EDX  */  "EDX",
/*CANS_REG_ESI  */  "ESI",
/*CANS_REG_EDI  */  "EDI",
/*CANS_REG_EBP  */  "EBP",
/*CANS_REG_ESP  */  "ESP",
/*CANS_REG_FS   */  "FS",
/*CANS_REG_GS   */  "GS",
/*CANS_REG_FLAGS*/  "FLAGS"
};

#define IND_WIDTH   4
#define MAX_SPACES  160

#pragma on (check_stack);
STATIC void doPrintSymbs( void ) {

    symb_handle hdl;
    symb_handle head;
    char        spaces[ MAX_SPACES + 1 ];
    char        *indent;
    char        *stg_class;
    unsigned    i;
    enum {
        NORMAL_LOCALS,
        WAITING_FOR_PARM,
        IN_PARMS,
        NEXT_IS_RET_VAL
    }           locals_state;

    spaces[ MAX_SPACES ] = 0;
    indent = spaces + MAX_SPACES;
    memset( spaces, ' ', MAX_SPACES );

    locals_state = NORMAL_LOCALS;
    head = CanSGetHead();
    if( head == NULL ) {
        return;
    }
    hdl = head;
    do {
        switch( hdl->class ) {
        case CANS_CHANGE_SEG:
            PrtFmt( "%sCHANGE_SEG\n", indent );
            indent -= IND_WIDTH;
            printAddrHdl( indent, hdl->d.cseg.seg );
            indent += IND_WIDTH;
            break;
        case CANS_BLOCK:
            PrtFmt( "%sBLOCK name \"%s\" type %x start_offset %X size %X\n",
                indent, NameGet( hdl->d.nat.name_hdl ), hdl->d.nat.type_hdl,
                hdl->d.block.start_offset, hdl->d.block.size );
            locals_state = WAITING_FOR_PARM;
            indent -= IND_WIDTH;
            break;
        case CANS_BP_OFFSET:
        case CANS_MEM_LOC:
        case CANS_REGISTER:
        case CANS_IND_REG:
            switch( locals_state ) {
            case WAITING_FOR_PARM:
                locals_state = IN_PARMS;
                PrtFmt( "%sParms:\n", indent );
                indent -= IND_WIDTH;
                break;
            case NEXT_IS_RET_VAL:
                PrtFmt( "%sReturn Value:\n", indent );
                indent -= IND_WIDTH;
                break;
            }
            switch( hdl->class ) {
            case CANS_BP_OFFSET:
                PrtFmt( "%sBP_OFFSET %X name \"%s\" type %x\n", indent,
                    hdl->d.bpoff.offset, NameGet( hdl->d.nat.name_hdl ),
                    hdl->d.nat.type_hdl );
                break;
            case CANS_MEM_LOC:
                if( hdl->d.memloc.is_static ) {
                    stg_class = "static";
                } else {
                    stg_class = "extern";
                }
                PrtFmt( "%sMEM_LOC (%s) name \"%s\" type %x\n",indent,stg_class,
                    NameGet( hdl->d.nat.name_hdl ), hdl->d.nat.type_hdl );
                indent -= IND_WIDTH;
                printAddrHdl( indent, hdl->d.memloc.mem_hdl );
                indent += IND_WIDTH;
                break;
            case CANS_REGISTER:
                PrtFmt( "%sREGISTER name \"%s\" type %x: ", indent,
                    NameGet( hdl->d.nat.name_hdl ), hdl->d.nat.type_hdl );
                for( i = hdl->d.reg.num_regs; i > 0; --i ) {
                    PrtFmt( "%s%c", regNames[ hdl->d.reg.reg[i-1] ],
                        ( i > 1 ) ? ':' : '\n' );
                }
                break;
            case CANS_IND_REG:
                PrtFmt( "%sIND_REG name \"%s\" type %x %s ", indent,
                    NameGet( hdl->d.nat.name_hdl ), hdl->d.nat.type_hdl,
                    hdl->d.indr.calloc ? "CALLOC" : "RALLOC" );
                if( hdl->d.indr.far_ptr ) {
                    PrtFmt( "%s:%s\n", regNames[ hdl->d.indr.seg ],
                        regNames[ hdl->d.indr.off ] );
                } else {
                    PrtFmt( "%s\n", regNames[ hdl->d.indr.off ] );
                }
                break;
            }
            if( locals_state == NEXT_IS_RET_VAL ) {
                locals_state = NORMAL_LOCALS;
                indent += IND_WIDTH;
            }
            break;
        case CANS_PROLOG:
            switch( locals_state ) {
            case WAITING_FOR_PARM:
                locals_state = NORMAL_LOCALS;
                break;
            case IN_PARMS:
                indent += IND_WIDTH;
                locals_state = NORMAL_LOCALS;
                break;
            }
            if( hdl->d.prolog.has_ret_val ) {
                locals_state = NEXT_IS_RET_VAL;
            }
            if( hdl->d.prolog.has_ret_addr ) {
                PrtFmt( "%sPROLOG size %t ret_addr_offset %X\n", indent,
                    hdl->d.prolog.size, hdl->d.prolog.ret_addr_offset );
            } else {
                PrtFmt( "%sPROLOG size %t\n", indent, hdl->d.prolog.size );
            }
            break;
        case CANS_EPILOG:
            PrtFmt( "%sEPILOG size %t\n", indent, hdl->d.epilog.size );
            break;
        case CANS_BLOCK_END:
            indent += IND_WIDTH;
            PrtFmt( "%sBLOCK_END\n", indent );
            break;
        default:
/**/        never_reach();
        }
        if( indent < spaces + IND_WIDTH ) {
            indent = spaces + IND_WIDTH;
        }
        hdl = CanSFwd( hdl );
    } while( hdl != head );
}
#pragma off(check_stack);

STATIC void doPrintMisc( void ) {

    if( CanMisc.compiler != NULL ) {
              /* 12345678901234567 */
        PrtMsg( MSG_TXT_COMPILER, CanMisc.compiler );
    }
    if( CanMisc.processor == CAN_PROC_8086 ) {
        PrtMsg( MSG_TXT_8086_PRCSR );
    } else {
        PrtMsg( MSG_TXT_80X86_PRCSR, CanMisc.processor );
    }
/**/myassert( CanMisc.memory_model <= CAN_MODEL_TINY );
          /* 12345678901234567 */
    PrtMsg( MSG_TXT_MEM_MODEL, MSG_TXT_MODEL_0+CanMisc.memory_model );
/**/myassert( CanMisc.floating_point <= CAN_FLOAT_ALTERNATE );
    PrtMsg( MSG_TXT_FLOAT_PT, MSG_TXT_FP_0+CanMisc.floating_point );
    PrtMsg( MSG_TXT_OPTIMIZED, CanMisc.optimized ? MSG_TXT_YES : MSG_TXT_NO );
    PrtMsg( MSG_TXT_LINES_PRST,
                        CanMisc.lines_present ? MSG_TXT_YES : MSG_TXT_NO );
    PrtMsg( MSG_TXT_TYPES_PRST,
                        CanMisc.types_present ? MSG_TXT_YES : MSG_TXT_NO );
    PrtMsg( MSG_TXT_SYMBS_PRST,
                        CanMisc.symbs_present ? MSG_TXT_YES : MSG_TXT_NO );
/**/myassert( CanMisc.src_language <= CAN_LANG_BASIC );
    PrtMsg( MSG_TXT_SOURCE_LANG, MSG_TXT_LANG_0+CanMisc.src_language );
    PrtFmt( "\n" );
}

#pragma on (check_stack);
STATIC void center( const char *text ) {

    char        buf[81];
    unsigned    len;
    unsigned    spaces;

    len = strlen( text );
/**/myassert( len <= 80 );
    spaces = 40 - len / 2;
    memset( buf, ' ', spaces );
    buf[ spaces ] = 0;
    PrtFmt( "%s%s\n", buf, text );
    memset( buf + spaces, '-', len );
    buf[ spaces + len ] = 0;
    PrtFmt( "%s\n", buf );
}
#pragma off(check_stack);

STATIC void centerMsg( int msgnum ) {

    char        msgbuff[MAX_RESOURCE_SIZE];

    MsgGet( msgnum, msgbuff );
    center( msgbuff );
}

STATIC int can2txt( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL && objr->command == CMD_MODEND );
/**/myassert( state != NULL && state->pass == POBJ_WRITE_PASS );
    objr = objr;
    state = state;
    centerMsg( MSG_TXT_MISC_INFO );
    doPrintMisc();
    if( CanMisc.types_present ) {
        centerMsg( MSG_TXT_TYPE_INFO );
        CanTWalk( NULL, doPrintType );
    }
    if( CanMisc.symbs_present ) {
        centerMsg( MSG_TXT_SYMB_INFO );
        doPrintSymbs();
    }
    return( 0 );
}

void Can2TxtInit( void ) {
/**********************/

    PObjRegister( CMD_MODEND, POBJ_WRITE_PASS, can2txt );
}

void Can2TxtFini( void ) {
/**********************/

    PObjUnRegister( CMD_MODEND, POBJ_WRITE_PASS, can2txt );
}

