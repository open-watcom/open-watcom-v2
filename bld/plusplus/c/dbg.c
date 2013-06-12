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
* Description:  Debugging routines (debug build only).
*
****************************************************************************/


#include "plusplus.h"
#include "preproc.h"
#include "cgfront.h"
#include "cgdata.h"
#include "datainit.h"
#include "vbuf.h"
#include "fmttype.h"
#include "fmtsym.h"
#include "dbg.h"
#include "ring.h"
#include "ptree.h"
#include "template.h"
#include "vstk.h"
#include "iosupp.h"
#include "codegen.h"
#include "initdefs.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif


#define F_ADDR      "%x"
#define F_NAME      "%s"
#define F_DECIMAL   "%d"
#define F_HEX       "%x"
#define F_BADDR     "[" F_ADDR "]"
#define F_PTR       "(" F_ADDR ")"
#define F_HEX_1     "(%x)"
#define F_HEX_2     "(%x)"
#define F_HEX_4     "(%x)"
#define F_S64       "(%i64d)"
#define F_STRING    "(" F_NAME ")"
#define F_QSTRING   "('" F_NAME "')"
#define F_EOL       "\n"
#define F_NL        "\n    "
#define F_TGT_OFFSET F_HEX_2
#define F_TGT_SIZE   F_HEX_4
#define F_FLOAT     "(%g)"
#define F_POINTS    "->"
#define F_INSTR     "%20s"
#define F_BUF_FMT   "[%3x-%4x]"
#define F_CPP_FLOAT "(%s)"

extern char *FEName( SYMBOL );

char *DbgSymNameFull(           // GET FULL SYMBOL NAME
    SYMBOL sym )                // - symbol
{
    VBUF vbuf;                  // - variable-sized buffer
    static char name[ 1024 ];   // - debugging buffer
    static char* name_ptr;

    if( sym == NULL ) {
        name_ptr = "**NULL**";
    } else {
        FormatSym( sym, &vbuf );
        stpcpy( name, VbufString( &vbuf ) );
        VbufFree( &vbuf );
        name_ptr = name;
    }
    return name_ptr;
}


char *DbgSymNameShort(          // GET Short SYMBOL NAME
    SYMBOL sym )                // - symbol
{
    return FEName( sym );
}


static void printToken(         // PRINT CURRENT TOKEN
    void )
{
    if( CurToken == T_ID ) {
        printf( "T_ID = '%s'\n", Buffer );
    } else if( CurToken == T_STRING ) {
        printf( "T_STRING = \"%s\"\n", Buffer );
    } else if( CurToken == T_LSTRING ) {
        printf( "T_LSTRING = L\"%s\"\n", Buffer );
    } else if( CurToken == T_CONSTANT ) {
        printf( "T_CONSTANT of type(%d) '%s'\n", ConstType, Buffer );
    } else {
        printf( "'%s'\n", Tokens[ CurToken ] );
    }
}


void DumpToken(                 // DUMP A TOKEN
    void )
{
    if( PragDbgToggle.dump_tokens ) {
        printf( "Token(%3d) Line(%4d) Column(%3d) ", CurToken, TokenLine, TokenColumn );
        printToken();
    }
}


void DumpMacToken(              // DUMP A MACRO TOKEN
    void )
{
    if( PragDbgToggle.dump_mtokens ) {
        printf( "MacroToken(%3d) Line(%4d) Column(%3d) ", CurToken, TokenLine, TokenColumn );
        printToken();
    }
}


void DumpMacPush(               // DUMP PUSH OF MACRO
    const void *p_mac,          // - macro being pushed
    const void **p_args )       // - arguments
{
    MEDEFN const *mac = p_mac;  // - macro being pushed
    const char**args = (const char **)p_args;  // - arguments
    unsigned count;
    if( PragDbgToggle.dump_mtokens ) {
        printf( "Macro Push: %s", mac->macro_name );
        if( ( mac->macro_defn != 0 ) && ( args != NULL ) ) {
            count = mac->parm_count;
            if( count == 1 ) {
                printf( " = %s", *args );
            } else if( count > 0 ) {
                printf( "( " );
                for( ; --count; ) {
                    printf( "%s%s", *args++, (count>1) ? ", " : " " );
                }
                printf( ")" );
            }
        }
        printf( "\n" );
    }
}



void DumpMDefn(                 // DUMP MACRO DEFINITION
    char *p )                  // - definition
{
    int             c;
    TOKEN           tok;

    if( p == NULL )
        return;
    for( ; (tok = *(TOKEN *)p) != T_NULL; ) {
        p += sizeof( TOKEN );
        switch( tok ) {
        case T_CONSTANT:
            switch( *p++ ) {
            case TYP_FLOAT :
            case TYP_DOUBLE :
            case TYP_LONG_DOUBLE :
                for( ; (c = *p++) != '\0'; )
                    putchar( c );
                break;
            default:
                printf( "%lld", Constant64 );
                p += sizeof( Constant64 );
                break;
            }
            break;
        case T_ID:
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            break;
        case T_STRING:
            putchar( '\"' );
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            putchar( '\"' );
            break;
        case T_WHITE_SPACE:
            putchar( ' ' );
            break;
        case T_BAD_CHAR:
            putchar( *p++ );
            break;
        case T_MACRO_PARM:
            printf( "parm#%c", '1' + *p++ );
            break;
        default:
            printf( "%s", Tokens[tok] );
            break;
        }
    }
    putchar( '\n' );
}


char *DbgOperator(              // GET CGOP NAME
    CGOP number )               // - index for name
{
    char *name;                 // - name

    static char *opnames[] ={   // - opcode names (binary,unary)
    #include "ppopsnam.h"
    };

    if( number < CO_MAX_OPCODES ) {
        name = opnames[ number ];
    } else if( ( number > CO_NAMES ) && ( number < CO_MAX_NAMES ) ) {
        name = opnames[ number - CO_NAMES + CO_MAX_OPCODES - 1 ];
    } else {
        name = "***INVALID***";
    }
    return( name );
}


char *DbgIcOpcode(              // GET IC OPCODE
    CGINTEROP opcode )          // - opcode
{
    static char *ic_names[] = {
        #define IC( code, type, mask ) # code
        #include "ic.h"
        #undef IC
    };

    if( opcode < IC_END ) {
        return( ic_names[opcode] );
    } else {
        return( "BAD OPCODE" );
    }
}

enum                            // types of opcodes
{   DBG_OPCODE_NUL              // - no operand
,   DBG_OPCODE_BIN              // - binary #
,   DBG_OPCODE_STR              // - string
,   DBG_OPCODE_CON              // - floating constant
,   DBG_OPCODE_SYM              // - symbol
,   DBG_OPCODE_SCP              // - scope
,   DBG_OPCODE_TYP              // - type
,   DBG_OPCODE_SRC              // - source file
};

static uint_8 optypes[] = {
    #define IC( code, type, mask ) DBG_OPCODE_##type
    #include "ic.h"
    #undef IC
};

void DumpCgFront(               // DUMP GENERATED CODE
    const char *prefix,         // - name added to print line
    DISK_ADDR disk_blk,         // - disk block
    DISK_OFFSET offset,         // - disk offset
    void *instruction )         // - intermediate code
{
    CGINTER *ins;               // - instruction
    char *opcode;               // - opcode
    unsigned uvalue;            // - value with opcode

    ins = instruction;
    opcode = DbgIcOpcode( ins->opcode );
    if( ins->opcode == IC_EOF ) {
        uvalue = 0;
    } else {
        uvalue = ins->value.uvalue;
    }
    switch( optypes[ ins->opcode ] ) {
      case DBG_OPCODE_SYM :
        printf(                 F_NAME
                " "             F_BUF_FMT
                " "             F_INSTR
                " "             F_HEX_4
                " "             F_NAME F_EOL
              , prefix
              , disk_blk, offset
              , opcode
              , uvalue
              , DbgSymNameFull( ins->value.pvalue ) );
        break;
      case DBG_OPCODE_TYP :
      { VBUF fmt_prefix, fmt_suffix;
        FormatType( ins->value.pvalue, &fmt_prefix, &fmt_suffix );
        printf(                 F_NAME
                " "             F_BUF_FMT
                " "             F_INSTR
                " "             F_HEX_4
                " %s<id>%s" F_EOL
              , prefix
              , disk_blk, offset
              , opcode
              , uvalue
              , VbufString( &fmt_prefix )
              , VbufString( &fmt_suffix ) );
        VbufFree( &fmt_prefix );
        VbufFree( &fmt_suffix );
      } break;
      case DBG_OPCODE_NUL :
        printf(                 F_NAME
                " "             F_BUF_FMT
                " "             F_INSTR F_EOL
              , prefix
              , disk_blk, offset
              , opcode );
        break;
      case DBG_OPCODE_SRC :
      {
        printf(                 F_EOL F_NAME
                " "             F_BUF_FMT
                " "             F_INSTR
                " "             F_NAME F_EOL
              , prefix
              , disk_blk, offset
              , opcode
              , SrcFileFullName( ins->value.pvalue ) );
      } break;
      case DBG_OPCODE_SCP :
        printf(                 F_NAME
                " "             F_BUF_FMT
                " "             F_INSTR
                " "             "scope: " F_HEX F_EOL
              , prefix
              , disk_blk, offset
              , opcode
              , uvalue );
        break;
      case DBG_OPCODE_STR :
      case DBG_OPCODE_CON :
      case DBG_OPCODE_BIN :
        switch( ins->opcode ) {
          case IC_OPR_BINARY :
          case IC_OPR_UNARY :
            printf(                 F_NAME
                    " "             F_BUF_FMT
                    " "             F_INSTR
                    " "             F_NAME F_EOL
                  , prefix
                  , disk_blk, offset
                  , opcode
                  , DbgOperator( ins->value.uvalue ) );
            break;
          case IC_DBG_LINE :
            printf(                 F_NAME
                    " "             F_BUF_FMT
                    " "             F_INSTR
                    " "             F_DECIMAL F_EOL F_EOL
                  , prefix
                  , disk_blk, offset
                  , opcode
                  , uvalue );
            break;
          default :
            printf(                 F_NAME
                    " "             F_BUF_FMT
                    " "             F_INSTR
                    " "             F_HEX F_EOL
                  , prefix
                  , disk_blk, offset
                  , opcode
                  , uvalue );
            break;
        }
        break;
      default :
        CFatal( "**** UNDEFINED OPCODE TYPE *****" );
        break;
    }
}

void DumpTemplateInfo( TEMPLATE_INFO *tinfo )
{
    TEMPLATE_SPECIALIZATION *tprimary;
    VBUF prefix, suffix;
    int i;
    char delim;

    tprimary = RingFirst( tinfo->specializations );
    printf( "    TEMPLATE_INFO" F_BADDR
            " defn"         F_PTR
            " num_args"     F_HEX_4
                            F_EOL
          , tinfo
          , tprimary->defn
          , tprimary->num_args
          );
    printf( "      %s", tinfo->sym->name->name );
    delim = '<';
    for( i = 0; i < tprimary->num_args; ++i ) {
        FormatType( tprimary->type_list[i], &prefix, &suffix );
        printf( "%c %s<id> %s", delim, VbufString( &prefix ), VbufString( &suffix ) );
        VbufFree( &prefix );
        VbufFree( &suffix );
        delim = ',';
    }
    printf( ">\n" );
}

static void dumpNameSpaceInfo( NAME_SPACE *ns )
{
    printf( "    NAME_SPACE" F_BADDR
            " scope"         F_PTR
            " all"           F_PTR
            " name"          F_PTR
                             F_EOL
          , ns
          , ns->scope
          , ns->all
          , ns->sym
          );
}

static void dumpLocation( TOKEN_LOCN *locn )
{
    if( locn->src_file != NULL ) {
        printf( "    " );
        DbgDumpTokenLocn( locn );
        printf( F_EOL );
    }
}


//
//  SYMBOL-TABLE FUNCTIONS
//
void DumpSymbol(                // DUMP SYMBOL ENTRY
    void *_sym )                // - symbol
{
    SYMBOL sym = _sym;
    VBUF vbuf;

    static char const *ids[] = {
        #define SC_DEF(a) #a
        SC_DEFS
        #undef SC_DEF
    };

    if( sym != NULL ) {
        FormatSym( sym, &vbuf );
        printf( "SYMBOL"        F_BADDR
                " next"         F_PTR
                " thread"       F_PTR
                " sym_type"     F_PTR
                " name"         F_PTR
                " cg_handle"    F_PTR
                F_NL
                " u"            F_PTR
                " id="          F_STRING
                " flag"         F_HEX_1
                " flag2"        F_HEX_1
                " segid"        F_HEX_2
                F_NL
                " symbol-name=" F_QSTRING
                F_EOL
              , sym
              , sym->next
              , sym->thread
              , sym->sym_type
              , sym->name
              , ( sym->flag2 & SF2_TOKEN_LOCN ) ? sym->locn->cg_handle : 0
              , sym->u.tinfo
              , ids[sym->id]
              , sym->flag
              , sym->flag2
              , sym->segid
              , VbufString( &vbuf )
              );
        if( sym->sym_type != NULL ) {
            DumpFullType( sym->sym_type );
        }
        VbufFree( &vbuf );
        if( sym->flag2 & SF2_TOKEN_LOCN ) {
            dumpLocation( &sym->locn->tl );
        }
        switch( sym->id ) {
        case SC_CLASS_TEMPLATE:
            DumpTemplateInfo( sym->u.tinfo );
            break;
        case SC_NAMESPACE:
            dumpNameSpaceInfo( sym->u.ns );
            break;
        }
    }
}

#define ENTRY_ERROR             "TYP_ERROR",
#define ENTRY_BOOL              "TYP_BOOL",
#define ENTRY_CHAR              "TYP_CHAR",
#define ENTRY_SCHAR             "TYP_SCHAR",
#define ENTRY_UCHAR             "TYP_UCHAR",
#define ENTRY_WCHAR             "TYP_WCHAR",
#define ENTRY_SSHORT            "TYP_SSHORT",
#define ENTRY_USHORT            "TYP_USHORT",
#define ENTRY_SINT              "TYP_SINT",
#define ENTRY_UINT              "TYP_UINT",
#define ENTRY_SLONG             "TYP_SLONG",
#define ENTRY_ULONG             "TYP_ULONG",
#define ENTRY_SLONG64           "TYP_SLONG64",
#define ENTRY_ULONG64           "TYP_ULONG64",
#define ENTRY_FLOAT             "TYP_FLOAT",
#define ENTRY_DOUBLE            "TYP_DOUBLE",
#define ENTRY_LONG_DOUBLE       "TYP_LONG_DOUBLE",
#define ENTRY_ENUM              "TYP_ENUM",
#define ENTRY_POINTER           "TYP_POINTER",
#define ENTRY_TYPEDEF           "TYP_TYPEDEF",
#define ENTRY_CLASS             "TYP_CLASS",
#define ENTRY_BITFIELD          "TYP_BITFIELD",
#define ENTRY_FUNCTION          "TYP_FUNCTION",
#define ENTRY_ARRAY             "TYP_ARRAY",
#define ENTRY_DOT_DOT_DOT       "TYP_DOT_DOT_DOT",
#define ENTRY_VOID              "TYP_VOID",
#define ENTRY_MODIFIER          "TYP_MODIFIER",
#define ENTRY_MEMBER_POINTER    "TYP_MEMBER_POINTER",
#define ENTRY_GENERIC           "TYP_GENERIC",

static char *id_names[] = {
    #include "type_arr.h"
};

static char unknown_type[] = "***UNKNOWN**=xx";

void DumpType(                  // DUMP TYPE ENTRY
    TYPE tp )                   // - type pointer
{
    char *id;                   // - id for symbol

    if( tp == NULL ) {
        printf( "NULL type\n" );
        return;
    }
    if( tp->id >= TYP_MAX ) {
        ultoa( tp->id, unknown_type + sizeof( unknown_type ) - 2, 16 );
        id = unknown_type;
    } else {
        id = id_names[ tp->id ];
    }
    printf( "TYPE"          F_BADDR
            " next"         F_PTR
            " id="          F_STRING
            " flag"         F_HEX_1
            " of"           F_PTR
            " u1"           F_PTR
            " u2"           F_PTR
            F_EOL
          , tp
          , tp->next
          , id
          , tp->flag
          , tp->of
          , tp->u.f.args
          , tp->u.f.pragma
          );
}

static void dumpFnType(         // DUMP EXTRA INFO FOR FUNCTION
    TYPE tp )                   // - type
{
    unsigned index;
    TYPE *  arg_list;

    if( ( tp != NULL ) && ( tp->u.f.args != NULL ) ) {
        for( arg_list = tp->u.f.args->type_list, index = 0
           ; index < tp->u.f.args->num_args
           ; ++ arg_list, ++ index ) {
            printf( "function arg[%d]:\n", index + 1 );
            DumpFullType( *arg_list );
        }
        printf( "--end of function arguments--" F_EOL );
    }
}


static void dumpBaseClass(      // DUMP BASE_CLASS
    void *_base )               // - points to information
{
    BASE_CLASS *base = _base;
    printf( "    BASECLASS" F_BADDR
            " next"         F_PTR
            " type"         F_PTR
            " delta"        F_TGT_OFFSET
            " vb_index"     F_TGT_OFFSET
            " flag"         F_HEX_1
                            F_EOL
            , base
            , base->next
            , base->type
            , base->delta
            , base->vb_index
            , base->flag
            );
}


void DumpClassInfo(             // DUMP CLASSINFO
    CLASSINFO *ci )             // - class information
{
    char *class_name;
    if( ci == NULL ) return;
    if( ci->name == NULL ) {
        class_name = "**UN-NAMED**";
    } else {
        class_name = ci->name;
    }
    printf( "    CLASSINFO" F_BADDR
            " bases"        F_PTR
            " friends"      F_PTR
            " name"         F_STRING
            " refno"        F_HEX_4
                            F_NL "  "
            " size"         F_TGT_OFFSET
            " vsize"        F_TGT_OFFSET
            " vb_offset"    F_HEX_4
            " vf_offset"    F_HEX_4
                            F_NL "  "
            " last_vfn"     F_HEX_2
            " last_vbase"   F_HEX_2
            " index"        F_HEX_2
            " max_align"    F_HEX_2
                            F_NL "  "
            " has_ctor"     F_HEX_2
            " has_dtor"     F_HEX_2
            " has_pure"     F_HEX_2
            " has_data"     F_HEX_2
            " has_vfn"      F_HEX_2
                            F_NL "  "
            " has_vcdtor"   F_HEX_2
            " has_vfptr"    F_HEX_2
            " has_vbptr"    F_HEX_2
                            F_NL "  "
            " needs_ctor"   F_HEX_2
            " needs_dtor"   F_HEX_2
            " needs_vdtor"  F_HEX_2
            " needs_assign" F_HEX_2
                            F_NL "  "
            " defined"      F_HEX_2
            " opened"       F_HEX_2
            " unnamed"      F_HEX_2
            " anonymous"    F_HEX_2
            " corrupted"    F_HEX_2
                            F_NL "  "
            " abstract"     F_HEX_2
            " abstract_OK"  F_HEX_2
                            F_NL "  "
            " const_copy"   F_HEX_2
            " const_assign" F_HEX_2
            " const_ref"    F_HEX_2
                            F_NL "  "
            " ctor_defined" F_HEX_2
            " copy_defined" F_HEX_2
            " dtor_defined" F_HEX_2
            " assign_defined" F_HEX_2
                            F_NL "  "
            " ctor_gen"     F_HEX_2
            " copy_gen"     F_HEX_2
            " dtor_gen"     F_HEX_2
            " assign_gen"   F_HEX_2
                            F_NL "  "
            " ctor_user_code" F_HEX_2
            " copy_user_code" F_HEX_2
            " dtor_user_code" F_HEX_2
            " assign_user_code" F_HEX_2
                            F_NL "  "
            " ctor_user_code_checked" F_HEX_2
            " copy_user_code_checked" F_HEX_2
                            F_NL "  "
            " dtor_user_code_checked" F_HEX_2
            " assign_user_code_checked" F_HEX_2
                            F_NL "  "
            " lattice" F_HEX_2
                            F_EOL
          , ci
          , ci->bases
          , ci->friends
          , class_name
          , ci->refno
          , ci->size
          , ci->vsize
          , ci->vb_offset
          , ci->vf_offset
          , ci->last_vfn
          , ci->last_vbase
          , ci->index
          , ci->max_align
          , ci->has_ctor
          , ci->has_dtor
          , ci->has_pure
          , ci->has_data
          , ci->has_vfn
          , ci->has_vcdtor
          , ci->has_vfptr
          , ci->has_vbptr
          , ci->needs_ctor
          , ci->needs_dtor
          , ci->needs_vdtor
          , ci->needs_assign
          , ci->defined
          , ci->opened
          , ci->unnamed
          , ci->anonymous
          , ci->corrupted
          , ci->abstract
          , ci->abstract_OK
          , ci->const_copy
          , ci->const_assign
          , ci->const_ref
          , ci->ctor_defined
          , ci->copy_defined
          , ci->dtor_defined
          , ci->assign_defined
          , ci->ctor_gen
          , ci->copy_gen
          , ci->dtor_gen
          , ci->assign_gen
          , ci->ctor_user_code
          , ci->copy_user_code
          , ci->dtor_user_code
          , ci->assign_user_code
          , ci->ctor_user_code_checked
          , ci->copy_user_code_checked
          , ci->dtor_user_code_checked
          , ci->assign_user_code_checked
          , ci->lattice
          );
}


static void dumpClassType(      // DUMP EXTRA INFO FOR CLASS
    TYPE tp )                   // - type
{
    CLASSINFO *ci;              // - class information

    ci = tp->u.c.info;
    DumpClassInfo( ci );
    RingWalk( ci->bases, &dumpBaseClass );
}

void PrintFullType(             // PRINT FULL TYPE INFORMATION
    TYPE tp )                   // - type pointer
{
    VBUF prefix, suffix;

    FormatType( tp, &prefix, &suffix );
    printf( "     Type[%x]: %s<id> %s" F_EOL, tp, VbufString( &prefix ), VbufString( &suffix ) );
    VbufFree( &prefix );
    VbufFree( &suffix );
}

void DumpExpandedType(          // DUMP EXPANDED TYPE
    TYPE tp )                   // - type
{
    if( tp == NULL ) {
        printf( "NULL type\n" );
        return;
    }
    while( tp != NULL ) {
        DumpType( tp );
        if( tp->id == TYP_FUNCTION ) {
            dumpFnType( tp );
        } else if( tp->id == TYP_CLASS ) {
            dumpClassType( tp );
        }
        tp = tp->of;
    }
}

void DumpFullType(              // DUMP FULL TYPE INFORMATION
    TYPE tp )                   // - type
{
    if( PragDbgToggle.dump_noformat ) {
        DumpExpandedType( tp );
    } else {
        PrintFullType( tp );
    }
}

void DumpSymbolName(            // DUMP SYMBOL_NAME ENTRY
    SYMBOL_NAME sn )            // - symbol name entry
{
    printf( "SYMBOL_NAME"   F_BADDR
            " next"         F_PTR
            " name_type"    F_PTR
            " name_syms"    F_PTR
                            F_NL
            " name"         F_PTR
            "="             F_STRING
                            F_EOL
          , sn
          , sn->next
          , sn->name_type
          , sn->name_syms
          , sn->name
          , sn->name
          );
}


static void print_delimit_line( // PRINT DELIMITER LINE
    void )
{
    puts( "--------------------------------------" );
}


void DumpCommentary(            // DUMP COMMENTARY LINE
    const char *comment )       // - comment
{
    print_delimit_line();
    puts( comment );
}


void DumpSymInfo(               // DUMP COMPLETE INFO FOR SYMBOL
    SYMBOL sym )                // - symbol
{
    print_delimit_line();
    DumpSymbol( sym );
    if( sym->name != NULL ) {
        DumpSymbolName( sym->name );
    }
}


static void dumpFriendRef(      // DUMP REFERENCE TO FRIEND SCOPE
    void *_fr )                 // - the reference
{
    FRIEND *fr = _fr;
    if( FriendIsType( fr ) ) {
        printf( "   FRIEND"     F_BADDR
                " next"         F_PTR
                " type"         F_PTR
                F_EOL
              , fr
              , fr->next
              , FriendGetType( fr )
              );
    } else {
        printf( "   FRIEND"     F_BADDR
                " next"         F_PTR
                " sym"          F_PTR
                F_EOL
              , fr
              , fr->next
              , FriendGetSymbol( fr )
              );
    }
}


static void dumpSymbolNameInfo( // DUMP SYMBOL_NAME ENTRY
    SYMBOL_NAME sn )            // - the entry
{
    DumpSymbolName( sn );
    DumpSymbol( sn->name_type );
    RingWalk( sn->name_syms, &DumpSymbol );
}


void DumpScope(                 // DUMP SCOPE INFO FOR SYMBOL
    SCOPE scope )               // - scope
{
    static char *scope_names[] = {
        #define SCOPE_DEF(a,b) # a
        SCOPE_DEFS
        #undef SCOPE_DEF
    };

    char *id;

    print_delimit_line();
    if( scope->id >= SCOPE_MAX ) {
        id = "***UNKNOWN***";
    } else {
        id = scope_names[ scope->id ];
    }
    printf( "SCOPE"         F_BADDR
            " enclosing"    F_PTR
            " names"        F_PTR
            " owner"        F_PTR
                            F_NL
            " id"           F_STRING
            " keep"         F_HEX_2
            " dtor_reqd"    F_HEX_2
            " dtor_naked"   F_HEX_2
            " try_catch"    F_HEX_2
            " in_unnamed"   F_HEX_2
            F_EOL
          , scope
          , scope->enclosing
          , scope->names
          , scope->owner.sym
          , id
          , scope->s.keep
          , scope->s.dtor_reqd
          , scope->s.dtor_naked
          , scope->s.try_catch
          , scope->s.in_unnamed
          );
    switch( scope->id ) {
    case SCOPE_CLASS:
        { TYPE ctype;     // class type
        ctype = ScopeClass( scope );
        if( ctype != NULL ) {
            DumpClassInfo( ctype->u.c.info );
        }
        } break;
    case SCOPE_FILE:
        dumpNameSpaceInfo( scope->owner.ns );
        break;
    }
    ScopeWalkNames( scope, &dumpSymbolNameInfo );
    RingWalk( ScopeFriends( scope ), &dumpFriendRef );
    RingWalk( ScopeInherits( scope ), &dumpBaseClass );
}


static void dump_sym_scope(     // DUMP SCOPE FOR A SYMBOL
    SYMBOL sym )                // - symbol
{
    TYPE tp;                    // - TYPE for scope symbol

    if( sym != NULL ) {
        for( tp = sym->sym_type; tp != NULL; tp = tp->of ) {
            if( tp->id == TYP_CLASS ) {
                DumpScopeInfo( tp->u.c.scope );
                break;
            }
        }
    }
}


static void dumpFriend(         // DUMP A FRIEND SCOPE
    void *_fr )                 // - symbol for the friend scope
{
    FRIEND *fr = _fr;
    dump_sym_scope( fr->u.sym );
}


void DumpScopeInfo(             // DUMP INFORMATION FOR SCOPE
    SCOPE scope )               // - starting scope
{
    DumpScope( scope );
    RingWalk( ScopeFriends( scope ), &dumpFriend );
    RingWalk( ScopeInherits( scope ), &dumpBaseClass );
}


typedef struct scope_defn SCOPE_DEFN;
struct scope_defn               // USED TO REMEMBER DEFINED SCOPES
{   SCOPE_DEFN *next;           // - next in ring
    SCOPE defn;                 // - a defined scope
};

static SCOPE_DEFN *scopes;      // ring of defined scopes


void DbgRememberScope(          // REMEMBER A NEWLY-DEFINED SCOPE
    SCOPE new_scope )           // - scope
{
    SCOPE_DEFN *new_defn;

    new_defn = RingAlloc( &scopes, sizeof( SCOPE_DEFN ) );
    new_defn->defn = new_scope;
}

static boolean findScope( void *e, const void *s )
{
    SCOPE_DEFN *holder = e;

    if( holder->defn == s ) {
        return( TRUE );
    }
    return( FALSE );
}

void DbgForgetScope(            // SCOPE is useless, so don't dump it
    SCOPE forget_me )
{
    SCOPE_DEFN *going_away;

    going_away = RingLookup( scopes, findScope, forget_me );
    RingDealloc( &scopes, going_away );
}


static void dump_scope_defn(    // DUMP SCOPE, GIVEN A SCOPE_DEFN
    void *_defn )               // - scope definition
{
    SCOPE_DEFN *defn = _defn;
    DumpScope( defn->defn );
}


void DumpScopes(                // DUMP ALL SCOPES
    void )
{
    RingWalk( scopes, dump_scope_defn );
}


static void dump_hash(          // DUMP HASH STAT FOR SCOPE, GIVEN A SCOPE_DEFN
    void *_defn )               // - scope definition
{
    SCOPE_DEFN *defn = _defn;
    printf( "SCOPE: %p\n", defn->defn );
    StatsHASHTAB( defn->defn->names );
}


void DumpHashStats(             // DUMP ALL SCOPES' HASH TAB STATS
    void )
{
    RingWalk( scopes, (void(*)(void*))dump_hash );
}

static void dumpNodeType( PTREE node )
{
    if( node->type == NULL ) {
        printf( " no type" F_EOL );
    } else {
        printf( F_EOL );
        DumpFullType( node->type );
    }
}


void DbgDumpTokenLocn           // DUMP A TOKEN_LOCN
    ( void const* parm )        // - the location
{
    TOKEN_LOCN const * locn;    // - the location

    locn = parm;
    if( NULL == locn
     || NULL == locn->src_file ) {
        printf( " <No SrcFile Location>" );
    } else {
        printf( " SrcFile(%s,%d-%d)"
              , SrcFileName( locn->src_file )
              , locn->line
              , locn->column
              );
    }
}


static char const * const flag_name[] =
{
    #define PtfFlag(a,b) # a
    PtfFlags
    #undef PtfFlag
};

static unsigned const flag_value[] =
{
    #define PtfFlag(a,b) b
    PtfFlags
    #undef PtfFlag
,   0
};

static char const * flag_fmt = "    flags:%s\n";


static void dumpPtreeFlags      // DUMP FLAGS IN PTREE NODE
    ( PTREE node )              // - node
{
    char line[72];
    char const * end = &line[ sizeof( line ) ];
    char * cur = line;
    unsigned ctr;
    stpcpy( cur, "" );
    for( ctr = 0; ; ++ ctr ) {
        unsigned flag = flag_value[ ctr ];
        if( 0 == flag ) break;
        if( flag & node->flags ) {
            char const * name = flag_name[ ctr ];
            unsigned size = strlen( name );
            if( end - cur <= 1 + size ) {
                printf( flag_fmt, line );
                cur = line;
            }
            *cur = ' ';
            cur = stpcpy( cur+1, name );
        }
    }
    printf( flag_fmt, line );
}


#define PUSH_NODE( ctl, node ) \
    if( node != NULL ) *(PTREE*)VstkPush(&ctl) = node
static void dumpPTreeNode(      // DUMP A PARSE TREE NODE
    PTREE node )                // - node in parse tree
{
    static char buffer[128];    // - debugging buffer
    char *node_name;            // - name of node
    VSTK_CTL ctl;               // - VSTK control information (nodes)
    VSTK_CTL dup;               // - VSTK control information (duplicates)
    int dup_out;                // - last duplicate printed

    VstkOpen( &ctl, sizeof( PTREE ), 32 );
    VstkOpen( &dup, sizeof( PTREE ), 8 );
    dup_out = -1;
    for( ; ; ) {
        switch( node->op ) {
          case PT_ERROR :
            printf( "PT_ERROR"      F_BADDR
                    " ***** ERROR TREE *****" F_EOL
                  , node
                  );
            break;
          case PT_INT_CONSTANT :
            node_name = "PT_INT_CONSTANT";
            printf( F_NAME          F_BADDR
                    " flags"        F_HEX_4
                    " type"         F_PTR
                  , node_name
                  , node
                  , node->flags
                  , node->type
                  );
            if( NULL == Integral64Type( node->type ) ) {
                printf( " integer"      F_HEX_4
                      , node->u.int_constant
                      );
            } else {
                printf( " integer-64" F_S64
                      , node->u.int64_constant
                      );
            }
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
          case PT_FLOATING_CONSTANT : {
            char buffer[256];

            BFCnvFS( node->u.floating_constant, buffer, 256 );
            printf( "PT_FLOATING_CONSTANT" F_BADDR
                    " flags"        F_HEX_4
                    " float"        F_CPP_FLOAT
                  , node
                  , node->flags
                  , buffer
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
          }
            break;
          case PT_STRING_CONSTANT :
            stvcpy( buffer, node->u.string->string, node->u.string->len );
            printf( "PT_STRING_CONSTANT" F_BADDR
                    " flags"        F_HEX_4
                    " string"       F_STRING
                  , node
                  , node->flags
                  , buffer
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
          case PT_ID :
            printf( "PT_ID"         F_BADDR
                    " flags"        F_HEX_4
                    " cgop"         F_STRING F_NL
                    " id"           F_PTR
                    "="             F_STRING
                    " id_cgop"      F_STRING
                    " u.id.scope"   F_PTR
                  , node
                  , node->flags
                  , DbgOperator( node->cgop )
                  , node->u.id.name
                  , node->u.id.name
                  , DbgOperator( node->id_cgop )
                  , node->u.id.scope
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
          case PT_TYPE :
            printf( "PT_TYPE"       F_BADDR
                    " cgop"         F_STRING
                    " flags"        F_HEX_4
                    " next"         F_PTR
                    " scope"        F_PTR
                  , node
                  , DbgOperator( node->cgop )
                  , node->flags
                  , node->u.type.next
                  , node->u.type.scope
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
          case PT_SYMBOL :
            if( node->cgop == CO_NAME_THIS ) {
                printf( "PT_SYMBOL"     F_BADDR
                        " flags"        F_HEX_4
                        " this "
                      , node
                      , node->flags
                      );
                dumpNodeType( node );
                dumpLocation( &node->locn );
                dumpPtreeFlags( node );
            } else if( node->cgop == CO_NAME_CDTOR_EXTRA ) {
                printf( "PT_SYMBOL"     F_BADDR
                        " flags"        F_HEX_4
                        " cdtor_extra_parm "
                      , node
                      , node->flags
                      );
                dumpNodeType( node );
                dumpLocation( &node->locn );
                dumpPtreeFlags( node );
            } else {
                printf( "PT_SYMBOL"     F_BADDR
                        " flags"        F_HEX_4
                        " cgop"         F_STRING F_NL
                        " symbol"       F_PTR
                        " result"       F_PTR
                      , node
                      , node->flags
                      , DbgOperator( node->cgop )
                      , node->u.symcg.symbol
                      , node->u.symcg.result
                      );
                dumpNodeType( node );
                dumpLocation( &node->locn );
                dumpPtreeFlags( node );
                DumpSymbol( node->u.symcg.symbol );
            }
            break;
          case PT_UNARY :
            printf( "PT_UNARY"      F_BADDR
                    F_POINTS        F_ADDR
                    " flags"        F_HEX_4
                    " cgop"         F_STRING
                  , node
                  , node->u.subtree[0]
                  , node->flags
                  , DbgOperator( node->cgop )
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            PUSH_NODE( ctl, node->u.subtree[0] );
            break;
          case PT_BINARY :
            printf( "PT_BINARY"     F_BADDR
                    F_POINTS        F_ADDR
                    ","             F_ADDR
                    " flags"        F_HEX_4
                    " cgop"         F_STRING
                  , node
                  , node->u.subtree[0]
                  , node->u.subtree[1]
                  , node->flags
                  , DbgOperator( node->cgop )
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            PUSH_NODE( ctl, node->u.subtree[1] );
            PUSH_NODE( ctl, node->u.subtree[0] );
            break;
          case PT_DUP_EXPR :
          { PTREE *duped;       // - duplicated expression
            printf( "PT_DUP_EXPR"   F_BADDR
                    F_POINTS        F_ADDR
                    " flags"        F_HEX_4
                    " node"         F_ADDR
                  , node
                  , node->u.dup.subtree[0]
                  , node->flags
                  , node->u.dup.node
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            if( node->u.subtree[0] != NULL ) {
                for( duped = VstkTop( &dup )
                   ;
                   ; duped = VstkNext( &dup, duped ) ) {
                    if( duped == NULL ) {
                        PUSH_NODE( dup, node->u.subtree[0] );
                    } else if( *duped == node->u.subtree[0] ) {
                        break;
                    }
                }
            }
          } break;
          case PT_IC :
            printf( "PT_IC"         F_BADDR
                    " "             F_NAME
                    " value"        F_HEX_4
                  , node
                  , DbgIcOpcode( node->u.ic.opcode )
                  , node->u.ic.value.pvalue
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
          default :
            printf( "***INVALID***" F_BADDR
                    " flags"        F_HEX_4
                    " op"           F_HEX_1
                  , node
                  , node->flags
                  , node->op
                  );
            dumpNodeType( node );
            dumpLocation( &node->locn );
            dumpPtreeFlags( node );
            break;
        }
        {
            PTREE *next;            // - addr[next node]
            next = VstkPop( &ctl );
            if( next != NULL ) {
                node = *next;
            } else {
                ++dup_out;
                if( dup_out > VstkDimension( &dup ) ) break;
                next = VstkIndex( &dup, dup_out );
                printf( "--------------- duplicate ------------\n" );
                node = *next;
            }
        }
    }
    VstkClose( &ctl );
    VstkClose( &dup );
}
#undef PUSH_NODE


void DumpPTree( PTREE node )    // DUMP A PTREE
{
    if( node != NULL ) {
        print_delimit_line();
        dumpPTreeNode( node );
    }
}


void DumpInitInfo( void *v_info )
// dump stack entry containing info about data initialization
{
    INITIALIZE_INFO *info = (INITIALIZE_INFO *)v_info;
    static char *entryName[3] = { "DE_ROOT_TYPE", "DE_BRACE", "DE_TYPE" };

    printf( "Stack Entry"       F_BADDR
            " stack"            F_HEX_4
            " prev"             F_HEX_4
            " entry="           F_STRING F_NL
            " base"             F_HEX_2
            " offset"           F_HEX_2
            " mem_size"         F_HEX_2
            " padded_size"      F_HEX_2
          , info
          , info->stack
          , info->previous
          , entryName[info->entry]
          , info->base
          , info->offset
          , info->mem_size
          , info->padded_size
          );
    if( info->entry != DE_BRACE ) {
        printf( " target=" );
        switch( info->target ) {
        case DT_CLASS:
            printf( "(CLASS)"
                    " stop"         F_HEX_4
                    " curr"         F_HEX_4 F_EOL
                  , info->u.c.stop
                  , info->u.c.curr
                  );
            break;
        case DT_ARRAY:
            printf( "(ARRAY)"
                    " index"        F_HEX_2 F_EOL
                  , info->u.a.index
                  );
            break;
        case DT_BITFIELD:
            printf( "(BITFIELD)"
                    " mask"         F_HEX_4 F_EOL
                  , info->u.b.mask
                  );
            break;
        case DT_SCALAR:
            printf( "(SCALAR)"
                    " bitf"         F_HEX_1 F_EOL
                  , info->u.s.bitf
                  );
            break;
        case DT_ERROR:
            printf( "(ERROR)"       F_EOL
                  );
            break;
        default:
            printf( "(UNKNOWN="     F_ADDR
                    ")"             F_EOL
                  , info->target
                  );
            break;
        }
        if( info->type != NULL ) {
            DumpFullType( info->type );
        }
    } else {
        printf( F_EOL );
    }
}


void DumpMemberPtrInfo(         // DUMP MEMBER_PTR_CAST STRUCTURE
    MEMBER_PTR_CAST *inf )      // - the information
{
    char flags[80];
    char *fptr;

    flags[0] = '\0';
    fptr = flags;
    if( inf->safe )           fptr = stpcpy( fptr, "safe," );
    if( inf->init_conv )      fptr = stpcpy( fptr, "init_conv," );
    if( inf->delta_reqd )     fptr = stpcpy( fptr, "delta_reqd," );
    if( inf->mapping_reqd )   fptr = stpcpy( fptr, "mapping_reqd," );
    if( inf->test_reqd )      fptr = stpcpy( fptr, "test_reqd," );
    if( inf->single_mapping ) fptr = stpcpy( fptr, "single_mapping," );
    --fptr;
    *fptr = '\0';
    printf( "MEMBER_PTR_CAST" F_BADDR
            " base"         F_PTR
            " derived"      F_PTR   F_NL
            " delta"        F_HEX_4
            " single_test"  F_HEX_4
            " vb_index"     F_HEX_4
            " mapping"      F_PTR   F_NL
            " flags="       F_NAME  F_NL
          , inf
          , inf->base
          , inf->derived
          , inf->delta
          , inf->single_test
          , inf->vb_index
          , inf->mapping
          , flags );
}


void DbgGenned(                 // INDICATE SYMBOL GENERATED
    SYMBOL sym )                // - the symbol
{
    if( NULL == sym ) {
        printf( "Generated: module initialization\n" );
    } else {
        printf( "Generated: %s\n"
              , DbgSymNameFull( sym ) );
        if( sym->flag2 & SF2_TOKEN_LOCN ) {
            dumpLocation( &sym->locn->tl );
        }
    }
}


void DumpTemplateSpecialization(// DUMP A TEMPLATE SPECIALIZATION
    TEMPLATE_SPECIALIZATION *tspec )// - template specialization
{
    TEMPLATE_INFO *tinfo = tspec->tinfo;
    VBUF vbuf;                  // - variable-sized buffer

    FormatTemplateSpecialization( tspec, &vbuf );
    printf( "    TEMPLATE_SPECIALIZATION" F_BADDR
            " tinfo"        F_BADDR
                            F_EOL
            , tspec, tinfo );
    printf( "      %s\n", VbufString( &vbuf ) );
    VbufFree( &vbuf );
}


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    scopes = NULL;
}

static void fini(               // FINALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    RingFree( &scopes );
}


INITDEFN( dbg, init, fini );
