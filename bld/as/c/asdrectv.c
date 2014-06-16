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
* Description:  Assembler directive processing.
*
****************************************************************************/


#include "as.h"

#define MAX_ALIGNMENT   6       // 64 byte alignment

dir_set_flags       AsDirSetOptions;

static bool         autoAlignment;
static bool         getDirOpLine = FALSE; // decides if we'll grab the line
static directive_t  *lastDirective = NULL;


static bool dirHasOperand( directive_t *dir )
//*******************************************
{
    return( dir->num_operands > 0 );
}


static bool dirNumOperandsVerify( dir_opcount actual, dir_opcount wanted )
//************************************************************************
{
    if( actual < wanted ) {
        Error( DIROP_ERR_MISSING, actual );
        return( FALSE );
    }
    if( actual > wanted ) {
        Error( DIROP_ERR_TOO_MANY );
        return( FALSE );
    }
    return( TRUE );
}


#if 0
static bool dirFuncNYI( directive_t *dir, dir_table_enum parm )
//*************************************************************
{
    printf( "Directive '%s' is not yet implemented.\n", dir->dir_sym->name );
    return( TRUE );
}
#endif


static bool dirFuncAlign ( directive_t *dir, dir_table_enum parm )
//****************************************************************
{
    int_32      val;

    parm = parm;
    if( !dirNumOperandsVerify( dir->num_operands, 1 ) ) {
        return( TRUE );
    }
    assert( dir->operand_list->type == DIROP_INTEGER );
    val = NUMBER_INTEGER( dir->operand_list );
    if( val < 0 || val > MAX_ALIGNMENT ) {
        Error( OP_OUT_OF_RANGE, 0 );
        return( TRUE );
    }
    autoAlignment = FALSE;
    CurrAlignment = val;
    return( TRUE );
}


#ifdef _STANDALONE_
static bool dirFuncSwitchSection( directive_t *, dir_table_enum );
static bool dirFuncStorageAlloc( directive_t *, dir_table_enum );

static bool dirFuncBSS( directive_t *dir, dir_table_enum parm )
//*************************************************************
{
    if( !dirHasOperand( dir ) )
        return( dirFuncSwitchSection( dir, parm ) );
    return( dirFuncStorageAlloc( dir, parm ) ); // ".bss tag, bytes"
}


static bool dirFuncErr( directive_t *dir, dir_table_enum parm )
//*************************************************************
{
    char    *str;

    parm = parm;

    if( dirHasOperand( dir ) ) {
        assert( dir->num_operands == 1 );
        assert( dir->operand_list->type == DIROP_LINE );
        str = STRING_CONTENT( dir->operand_list );
        if( *str ) {
            Error( GET_STRING, str );
        }
    }
    return( FALSE );    // so that yyparse will terminate
}
#endif


static bool dirFuncIgnore( directive_t *dir, dir_table_enum parm )
//****************************************************************
// Silently ignore this directive...
{
    dir = dir;
    parm = parm;
    return( TRUE );
}


static bool dirFuncNop( directive_t *dir, dir_table_enum parm )
//*************************************************************
{
    uint_32     opcode = INS_NOP;

    dir = dir;
    if( parm == DT_NOP_NOP ) {
#ifdef _STANDALONE_
        ObjEmitData( CurrentSection, (char *)&opcode, sizeof( opcode ), TRUE );
#else
        ObjEmitData( (char *)&opcode, sizeof( opcode ), TRUE );
#endif
        return( TRUE );
    }
#ifdef AS_ALPHA
    assert( parm == DT_NOP_FNOP );
    opcode = INS_FNOP;
  #ifdef _STANDALONE_
    ObjEmitData( CurrentSection, (char *)&opcode, sizeof( opcode ), TRUE );
  #else
    ObjEmitData( (char *)&opcode, sizeof( opcode ), TRUE );
  #endif
    return( TRUE );
#else
    assert( FALSE );
    return( TRUE );
#endif
}


#ifdef _STANDALONE_
static bool dirFuncSetLinkage( directive_t *dir, dir_table_enum parm )
//********************************************************************
{
    sym_handle  sym;

    if( !dirNumOperandsVerify( dir->num_operands, 1 ) ) {
        return( TRUE );
    }
    sym = SYMBOL_HANDLE( dir->operand_list );
    assert( sym != NULL );
    assert( SymClass( sym ) == SYM_LABEL );
    switch( parm ) {
    case DT_LNK_GLOBAL:
        if( SymGetLinkage( sym ) == SL_STATIC ) {
            // Too bad the label is already emitted as static :(
            Warning( GLOBL_DECL_OUT_OF_ORDER );
        } else {
            SymSetLinkage( sym, SL_GLOBAL );
        }
        break;
    default:
        assert( FALSE );
    }
    return( TRUE );
}
#endif


static bool optionString( const char *str, const char *option )
//*************************************************************
{
    const char  *s;
    size_t      n;
    char        c;

    if( strncmp( str, option, n = strlen( option ) ) != 0 ) {
        return( FALSE );
    }
    s = str + n;
    while( (c = *s) != '\0' ) {
        // ignore trailing blanks only
        if( c != '\t' && c != ' ' ) return( FALSE );
        ++s;
    }
    return( TRUE );
}


static bool dirFuncSetOption( directive_t *dir, dir_table_enum parm )
//*******************************************************************
{
    char    *str;

    parm = parm;
    if( dirHasOperand( dir ) ) {
        assert( dir->num_operands == 1 );
        assert( dir->operand_list->type == DIROP_LINE );
        str = STRING_CONTENT( dir->operand_list );
        if( optionString( str, "at" ) ) {
            _DirSet( AT );
        } else if( optionString( str, "noat" ) ) {
            _DirUnSet( AT );
        } else if( optionString( str, "macro" ) ) {
            _DirSet( MACRO );
        } else if( optionString( str, "nomacro" ) ) {
            _DirUnSet( MACRO );
        } else if( optionString( str, "reorder" ) ) {
            _DirSet( REORDER );
        } else if( optionString( str, "noreorder" ) ) {
            _DirUnSet( REORDER );
        } else if( optionString( str, "volatile" ) ) {
            // ignore this for now
            // _DirSet( VOLATILE );
        } else if( optionString( str, "novolatile" ) ) {
            // ignore this for now
            // _DirUnSet( VOLATILE );
        } else if( optionString( str, "move" ) ) {
            // ignore this for now
            // _DirSet( MOVE );
        } else if( optionString( str, "nomove" ) ) {
            // ignore this for now
            // _DirUnSet( MOVE );
        } else {
            Error( IMPROPER_SET_DIRECTIVE, str );
        }
    }
    return( TRUE );
}


static bool dirFuncSpace( directive_t *dir, dir_table_enum parm )
//***************************************************************
{
    dir_operand                 *dirop;
    int_32                      count;

    parm = parm;
    if( !dirNumOperandsVerify( dir->num_operands, 1 ) ) {
        return( TRUE );
    }
    dirop = dir->operand_list;
    assert( dirop->type == DIROP_INTEGER );
    count = NUMBER_INTEGER( dirop );
    if( count < 0 ) {
        Error( OP_OUT_OF_RANGE, 0 );
        return( TRUE );
    }
#ifdef _STANDALONE_
    ObjNullPad( CurrentSection, (uint_8)count );
#else
    ObjNullPad( (uint_8)count );
#endif
    return( TRUE );
}


#ifdef _STANDALONE_
static bool dirFuncStorageAlloc( directive_t *dir, dir_table_enum parm )
//**********************************************************************
// e.g.) .comm name, expr
// Emit a label name, then emit expr bytes of data
{
    dir_operand                 *dirop;
    sym_handle                  sym;
    int_32                      expr;
    char                        *buffer;
    owl_section_handle          curr_section;
    reserved_section            as_section = 0;

    if( !dirNumOperandsVerify( dir->num_operands, 2 ) ) {
        return( TRUE );
    }
    dirop = dir->operand_list;
    if( dirop->type != DIROP_SYMBOL ) {
        Error( IMPROPER_DIROP, 0 );
        return( TRUE );
    }
    sym = SYMBOL_HANDLE( dirop );
    dirop = dirop->next;
    if( dirop->type != DIROP_INTEGER ) {
        Error( IMPROPER_DIROP, 1 );
        return( TRUE );
    }
    expr = NUMBER_INTEGER( dirop );
    if( expr <= 0 ) {
        Error( OP_OUT_OF_RANGE, 1 );
        return( TRUE );
    }
    if( ObjLabelDefined( sym ) ) {
        // then such label has already been emitted
        Error( SYM_ALREADY_DEFINED, SymName( sym ) );
        return( TRUE );
    }
    curr_section = CurrentSection;
    switch( parm ) {
    case DT_SEC_DATA:   // .comm => globl
        as_section = AS_SECTION_DATA;
        SymSetLinkage( sym, SL_GLOBAL );
        break;
    case DT_SEC_BSS:    // .lcomm, .bss
        as_section = AS_SECTION_BSS;
        break;
    default:
        assert( FALSE );
    }
    ObjSwitchSection( as_section );     // Have to switch to the right
    ObjEmitLabel( sym );                // section before emitting label.
    buffer = MemAlloc( expr );
    memset( buffer, 0, expr );
    ObjEmitData( CurrentSection, buffer, expr, TRUE ); // Align the data also.
    MemFree( buffer );
    CurrentSection = curr_section;      // Switch back to where you were.
    return( TRUE );
}
#endif

#define ESCAPE_CHAR     '\\'
#define ESCAPE_A        0x07
#define ESCAPE_B        0x08
#define ESCAPE_F        0x0C
#define ESCAPE_N        0x0A
#define ESCAPE_R        0x0D
#define ESCAPE_T        0x09
#define ESCAPE_V        0x0B

static char *getESCChar( char * const byte, char *ptr )
//*****************************************************
// Interpret the escape sequence, store the value in *byte,
// returns the pointer to the last character in the sequence.
{
    unsigned long int   num = 0;
    uint_8              ctr = 0;
    char                *buffer, *endptr;

    assert( *ptr == ESCAPE_CHAR );
    ptr++;
    while( *ptr >= '0' && *ptr <= '7' && ctr < 3) {
        ctr++;
        num = ( num << 3 ) + ( *(ptr++) - '0' );
    }
    if( ctr ) {     // we formed an octal number
        if( num > 0xFF ) Warning( CONST_OUT_OF_RANGE );
        *byte = (char)num;
        return( ptr - 1 );
    }
    if( *ptr == 'x' ) {     // Hex
        ptr++;
        buffer = MemAlloc( strlen( ptr ) + 2 + 1 );
        strcpy( buffer, "0x" );
        strcat( buffer, ptr );
        num = strtoul( buffer, &endptr, 16 );
        if( buffer == endptr ) {    // no hex found
            *byte = *(--ptr);       // *byte gets 'x'
            MemFree( buffer );
            return( ptr );
        }
        ptr += ( endptr - buffer - 3 );
        if( num > 0xFF ) Warning( CONST_OUT_OF_RANGE );
        *byte = (char)num;
        MemFree( buffer );
        return( ptr );
    }
    switch( *ptr ) {
    case 'a':
        *byte = ESCAPE_A; break;
    case 'b':
        *byte = ESCAPE_B; break;
    case 'f':
        *byte = ESCAPE_F; break;
    case 'n':
        *byte = ESCAPE_N; break;
    case 'r':
        *byte = ESCAPE_R; break;
    case 't':
        *byte = ESCAPE_T; break;
    case 'v':
        *byte = ESCAPE_V; break;
    default:
        *byte = *ptr; break;
    }
    return( ptr );
}


static bool dirFuncString( directive_t *dir, dir_table_enum parm )
//****************************************************************
{
    char        *str, *ptr, *byte;
    dir_operand *dirop;
    int         opnum;

    dirop = dir->operand_list;
    assert( dirop != NULL );
    opnum = 0;
    while( dirop ) {
        assert( dirop->type == DIROP_STRING );
        str = byte = MemAlloc( strlen( STRING_CONTENT( dirop ) ) + 1 );
        for( ptr = STRING_CONTENT( dirop ); *ptr != '\0'; ptr++ ) {
            if( *ptr == ESCAPE_CHAR ) {
                ptr = getESCChar( byte, ptr );
            } else {
                *byte = *ptr;
            }
            byte++;
        }
        if( parm == DT_STR_NULL ) {
            *byte++ = '\0';
        }
#ifdef _STANDALONE_
        ObjEmitData( CurrentSection, str, byte - str, ( opnum == 0 ) );
#else
        ObjEmitData( str, byte - str, ( opnum == 0 ) );
#endif
        MemFree( str );
        opnum++;
        dirop = dirop->next;
    }
    return( TRUE );
}


#ifdef _STANDALONE_
static bool dirFuncSwitchSection( directive_t *dir, dir_table_enum parm )
//***********************************************************************
{
    dir = dir;

    switch( parm ) {
    /* The ones that need autoAlignment are here. */
    case DT_SEC_TEXT:
    case DT_SEC_DATA:
        autoAlignment = TRUE;
        // fall through
    case DT_SEC_BSS:
    case DT_SEC_PDATA:
    case DT_SEC_DEBUG_P:
    case DT_SEC_DEBUG_S:
    case DT_SEC_DEBUG_T:
    case DT_SEC_RDATA:
    case DT_SEC_XDATA:
    case DT_SEC_YDATA:
#ifdef AS_PPC
    case DT_SEC_RELDATA:
    case DT_SEC_TOCD:
#endif
        // these enum values should correspond to reserved_section enums
        ObjSwitchSection( parm );
        break;
    default:
        Error( INTERNAL_UNKNOWN_SECTYPE );
        return( FALSE );
    }
    return( TRUE );
}
#endif


#ifdef AS_PPC
static bool dirFuncUnsupported( directive_t *dir, dir_table_enum parm )
//*********************************************************************
{
    Error( DIRECTIVE_NOT_SUPPORTED, SymName( dir->dir_sym ) );
    return( TRUE );
}
#endif


#ifdef _STANDALONE_
static bool dirFuncUserSection( directive_t *dir, dir_table_enum parm )
//*********************************************************************
{
    dir_operand                 *dirop;
    sym_handle                  sym;
    char                        *str, *s;
    owl_section_type            type = OWL_SEC_ATTR_NONE;
    owl_section_type            *ptype;
    owl_alignment               align = 0;

    if( dir->num_operands > 2 ) {
        Error( DIROP_ERR_TOO_MANY );
        return( TRUE );
    }
    dirop = dir->operand_list;
    if( dirop->type != DIROP_SYMBOL ) {
        Error( IMPROPER_DIROP, 0 );
        return( TRUE );
    }
    sym = SYMBOL_HANDLE( dirop );
    if( dir->num_operands == 2 ) {
        dirop = dirop->next;
        if( dirop->type != DIROP_STRING ) {
            Error( IMPROPER_DIROP, 1 );
            return( TRUE );
        }
        str = STRING_CONTENT( dirop );
    } else {
        str = NULL;
    }
    if( str ) {
        s = str;
        while( *s ) {
            switch( *s ) {
            case 'c':
                type |= OWL_SEC_ATTR_CODE;
                break;
            case 'd':
                type |= OWL_SEC_ATTR_DATA;
                break;
            case 'u':
                type |= OWL_SEC_ATTR_BSS;
                break;
            case 'i':
                type |= OWL_SEC_ATTR_INFO;
                break;
            case 'n':
                type |= OWL_SEC_ATTR_DISCARDABLE;
                break;
            case 'R':
                type |= OWL_SEC_ATTR_REMOVE;
                break;
            case 'r':
                type |= OWL_SEC_ATTR_PERM_READ;
                break;
            case 'w':
                type |= OWL_SEC_ATTR_PERM_WRITE;
                break;
            case 'x':
                type |= OWL_SEC_ATTR_PERM_EXEC;
                break;
            case 's':
                type |= OWL_SEC_ATTR_PERM_SHARE;
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6':
                align = 1 << (*s - '0');
                break;
            default:
                Error( INVALID_SECT_ATTR, *s );
                break;
            }
            s++;
        }
        ptype = &type;
    } else {
        // default is just read & write
        ptype = NULL;
    }
    if( parm == DT_USERSEC_NEW ) {
        SectionNew( SymName( sym ), ptype, align );
    } else {
        SectionSwitch( SymName( sym ), ptype, align );
    }
    return( TRUE );
}
#endif


static bool assignRelocType( owl_reloc_type *owlrtype, asm_reloc_type artype, dir_table_enum parm )
//*************************************************************************************************
{
    owl_reloc_type reloc_translate[] = {
        OWL_RELOC_ABSOLUTE,         // ASM_RELOC_UNSPECIFIED
        OWL_RELOC_WORD,
        OWL_RELOC_HALF_HI,
        OWL_RELOC_HALF_HA,
        OWL_RELOC_HALF_LO,
    };

    assert( parm == DT_VAL_INT16 || parm == DT_VAL_INT32 );
    if( parm == DT_VAL_INT16 ) {
        reloc_translate[ ASM_RELOC_UNSPECIFIED ] = OWL_RELOC_HALF_LO; //default
        switch( artype ) {
        case ASM_RELOC_HALF_HI:
        case ASM_RELOC_HALF_HA:
        case ASM_RELOC_HALF_LO:
        case ASM_RELOC_UNSPECIFIED:
            *owlrtype = reloc_translate[ artype ];
            break;
        default:
            return( FALSE );
        }
    } else {    // DT_VAL_INT32
        reloc_translate[ ASM_RELOC_UNSPECIFIED ] = OWL_RELOC_WORD; //default
        switch( artype ) {
        case ASM_RELOC_WORD:
        case ASM_RELOC_UNSPECIFIED:
            *owlrtype = reloc_translate[ artype ];
            break;
        default:
            return( FALSE );
        }
    }
    return( TRUE );
}


static bool dirFuncValues( directive_t *dir, dir_table_enum parm )
//****************************************************************
{
    dir_operand         *dirop;
    static int_8        byte;
    static int_16       half;
    static int_32       word;
    static signed_64    quad;
    static float        flt;
    static double       dbl;
    int_32              rep;
    uint_8              prev_alignment = 0;
    int                 opnum;
    void                *target = NULL;
    owl_reloc_type      rtype;
    struct { int size; void *ptr; uint_8 alignment; } data_table[] =
    {
        { 1, &byte,     0 },    // DT_VAL_INT8
        { 8, &dbl,      3 },    // DT_VAL_DOUBLE
        { 4, &flt,      2 },    // DT_VAL_FLOAT
        { 2, &half,     1 },    // DT_VAL_INT16
        { 4, &word,     2 },    // DT_VAL_INT32
        { 8, &quad,     3 },    // DT_VAL_INT64
    };
#define TABLE_IDX( x )      ( ( x ) - DT_VAL_FIRST )

#ifdef _STANDALONE_
    if( OWLTellSectionType( CurrentSection ) & OWL_SEC_ATTR_BSS ) {
        Error( INVALID_BSS_DIRECTIVE, SymName( dir->dir_sym ) );
        return( TRUE );
    }
#endif
    if( autoAlignment ) {
        prev_alignment = CurrAlignment;
        CurrAlignment = data_table[TABLE_IDX(parm)].alignment;
    }
    dirop = dir->operand_list;
    opnum = 0;
    while( dirop ) {
        rep = 1;
        switch( parm ) {
        case DT_VAL_INT8:
            assert( dirop->type == DIROP_INTEGER || dirop->type == DIROP_REP_INT );
            if( dirop->type == DIROP_INTEGER ) {
                byte = (int_8)NUMBER_INTEGER( dirop );
            } else { // repeat
                rep = REPEAT_COUNT( dirop );
                byte = (int_8)REPEAT_INTEGER( dirop );
            }
            break;
        case DT_VAL_INT64:
            assert( dirop->type == DIROP_INTEGER || dirop->type == DIROP_REP_INT );
            if( dirop->type == DIROP_INTEGER ) {
                quad.u._32[I64LO32] = NUMBER_INTEGER( dirop );
            } else { // repeat
                rep = REPEAT_COUNT( dirop );
                quad.u._32[I64LO32] = REPEAT_INTEGER( dirop );
            }
            break;
        case DT_VAL_DOUBLE:
            assert( dirop->type == DIROP_FLOATING || dirop->type == DIROP_REP_FLT );
            if( dirop->type == DIROP_FLOATING ) {
                dbl = NUMBER_FLOAT( dirop );
            } else {
                rep = REPEAT_COUNT( dirop );
                dbl = REPEAT_FLOAT( dirop );
            }
            break;
        case DT_VAL_FLOAT:
            assert( dirop->type == DIROP_FLOATING || dirop->type == DIROP_REP_FLT );
            if( dirop->type == DIROP_FLOATING ) {
                flt = (float)NUMBER_FLOAT( dirop );
            } else {
                rep = REPEAT_COUNT( dirop );
                flt = (float)REPEAT_FLOAT( dirop );
            }
            break;
        case DT_VAL_INT32:
            assert( dirop->type == DIROP_INTEGER || dirop->type == DIROP_REP_INT ||
                    dirop->type == DIROP_SYMBOL || dirop->type == DIROP_NUMLABEL_REF );
            if( dirop->type == DIROP_INTEGER ) {
                word = NUMBER_INTEGER( dirop );
            } else if( dirop->type == DIROP_REP_INT ) {
                rep = REPEAT_COUNT( dirop );
                word = REPEAT_INTEGER( dirop );
            } else {    // reloc
                word = SYMBOL_OFFSET( dirop );
                if( assignRelocType( &rtype, SYMBOL_RELOC_TYPE( dirop ), parm ) ) {
                    if( dirop->type == DIROP_SYMBOL ) {
                        target = SymName( SYMBOL_HANDLE( dirop ) );
                    } else {
                        assert( dirop->type == DIROP_NUMLABEL_REF );
                        target = &SYMBOL_LABEL_REF( dirop );
                    }
                } else {
                    Error( IMPROPER_DIROP, opnum );
                }
            }
            break;
        case DT_VAL_INT16:
            assert( dirop->type == DIROP_INTEGER || dirop->type == DIROP_REP_INT ||
                    dirop->type == DIROP_SYMBOL || dirop->type == DIROP_NUMLABEL_REF );
            if( dirop->type == DIROP_INTEGER ) {
                half = (int_16)NUMBER_INTEGER( dirop );
            } else if( dirop->type == DIROP_REP_INT ) {
                rep = REPEAT_COUNT( dirop );
                half = (int_16)REPEAT_INTEGER( dirop );
            } else {    // reloc
                half = (int_16)SYMBOL_OFFSET( dirop );
                if( assignRelocType( &rtype, SYMBOL_RELOC_TYPE( dirop ), parm ) ) {
                    if( dirop->type == DIROP_SYMBOL ) {
                        target = SymName( SYMBOL_HANDLE( dirop ) );
                    } else {
                        assert( dirop->type == DIROP_NUMLABEL_REF );
                        target = &SYMBOL_LABEL_REF( dirop );
                    }
                } else {
                    Error( IMPROPER_DIROP, opnum );
                }
            }
            break;
        default:
            Error( INTERNAL_UNKNOWN_DT_PARM, parm );
            return( FALSE );
        }
        if( target != NULL ) {
            assert( (parm == DT_VAL_INT32 || parm == DT_VAL_INT16) && rep == 1 );
#ifdef _STANDALONE_
            ObjEmitReloc( CurrentSection, target, rtype, ( opnum == 0 ), (dirop->type == DIROP_SYMBOL) ); // align with data
#else
            ObjEmitReloc( target, rtype, ( opnum == 0 ), (dirop->type == DIROP_SYMBOL ) ); // align with data
#endif
            target = NULL;
        }
#ifdef _STANDALONE_
        ObjEmitData( CurrentSection,
#else
        ObjEmitData(
#endif
                     data_table[TABLE_IDX(parm)].ptr,
                     data_table[TABLE_IDX(parm)].size,
                     ( opnum == 0 ) );  // only align for the first data operand
        for( rep--; rep > 0; rep-- ) {
#ifdef _STANDALONE_
            OWLEmitData( CurrentSection,
#else
            ObjDirectEmitData(
#endif
                         data_table[TABLE_IDX(parm)].ptr,
                         data_table[TABLE_IDX(parm)].size );
#if 0
            printf( "Size=%d\n", data_table[TABLE_IDX(parm)].size );
            switch( parm ) {
            case DT_VAL_INT8:
                printf( "Out->%d\n", *(int_8 *)(data_table[TABLE_IDX(parm)].ptr) );
                break;
            case DT_VAL_DOUBLE:
                printf( "Out->%lf\n", *(double *)(data_table[TABLE_IDX(parm)].ptr) );
                break;
            case DT_VAL_FLOAT:
                printf( "Out->%f\n", *(float *)(data_table[TABLE_IDX(parm)].ptr) );
                break;
            case DT_VAL_INT16:
                printf( "Out->%d\n", *(int_16 *)(data_table[TABLE_IDX(parm)].ptr) );
                break;
            case DT_VAL_INT32:
                printf( "Out->%d\n", *(int_32 *)(data_table[TABLE_IDX(parm)].ptr) );
                break;
            case DT_VAL_INT64:
                printf( "Out->%d\n", ((signed_64 *)(data_table[TABLE_IDX(parm)].ptr))->_32[0] );
                break;
            default:
                assert( FALSE );
            }
#endif
        }
        opnum++;
        dirop = dirop->next;
    }
    if( autoAlignment ) {
        CurrAlignment = prev_alignment;
    }
    return( TRUE );
}


#define INT     DOF_INT
#define FLT     DOF_FLT
#define NUM     ( DOF_INT | DOF_FLT )
#define SYM     ( DOF_SYM | DOF_NUMREF )
#define LINE    DOF_LINE
#define STR     DOF_STR
#define RINT    DOF_REP_INT
#define RFLT    DOF_REP_FLT
#define REP     ( DOF_REP_INT | DOF_REP_FLT )
#define NONE    DOF_NONE

static dir_table asm_directives[] = {
//  { name,         func,                   parm,           flags }
    { ".address",   dirFuncValues,          DT_VAL_INT32,   INT | SYM },
    { ".align",     dirFuncAlign,           DT_NOPARM,      INT },
    { ".ascii",     dirFuncString,          DT_STR_NONULL,  STR },
    { ".asciz",     dirFuncString,          DT_STR_NULL,    STR },
    { ".asciiz",    dirFuncString,          DT_STR_NULL,    STR },
#ifdef _STANDALONE_
    { ".bss",       dirFuncBSS,             DT_SEC_BSS,     INT | SYM | NONE },
#endif
    { ".byte",      dirFuncValues,          DT_VAL_INT8,    INT | RINT },
#ifdef _STANDALONE_
    { ".comm",      dirFuncStorageAlloc,    DT_SEC_DATA,    INT | SYM },
    { ".data",      dirFuncSwitchSection,   DT_SEC_DATA,    NONE },
    { ".debug$P",   dirFuncSwitchSection,   DT_SEC_DEBUG_P, NONE },
    { ".debug$S",   dirFuncSwitchSection,   DT_SEC_DEBUG_S, NONE },
    { ".debug$T",   dirFuncSwitchSection,   DT_SEC_DEBUG_T, NONE },
#endif
    { ".double",    dirFuncValues,          DT_VAL_DOUBLE,  FLT | RFLT },
#ifdef _STANDALONE_
    { ".err",       dirFuncErr,             DT_NOPARM,      LINE },
#endif
    { ".even",      dirFuncIgnore,          DT_NOPARM,      NONE },
#ifdef _STANDALONE_
    { ".extern",    dirFuncSetLinkage,      DT_LNK_GLOBAL,  SYM },
#endif
    { ".float",     dirFuncValues,          DT_VAL_FLOAT,   FLT | RFLT },
#ifdef _STANDALONE_
    { ".globl",     dirFuncSetLinkage,      DT_LNK_GLOBAL,  SYM },
#endif
    { ".half",      dirFuncValues,          DT_VAL_INT16,   INT | RINT | SYM },
#ifdef _STANDALONE_
    { ".ident",     dirFuncIgnore,          DT_NOPARM,      LINE },
    { ".lcomm",     dirFuncStorageAlloc,    DT_SEC_BSS,     INT | SYM },
#endif
    { ".long",      dirFuncValues,          DT_VAL_INT32,   INT | RINT | SYM },
#ifdef _STANDALONE_
    { ".new_section", dirFuncUserSection,   DT_USERSEC_NEW, SYM | STR },
#endif
    { "nop",        dirFuncNop,             DT_NOP_NOP,     NONE },
#ifdef _STANDALONE_
    { ".pdata",     dirFuncSwitchSection,   DT_SEC_PDATA,   NONE },
    { ".rdata",     dirFuncSwitchSection,   DT_SEC_RDATA,   NONE },
    { ".xdata",     dirFuncSwitchSection,   DT_SEC_XDATA,   NONE },
    { ".ydata",     dirFuncSwitchSection,   DT_SEC_YDATA,   NONE },
#ifdef AS_PPC
    { ".reldata",   dirFuncSwitchSection,   DT_SEC_RELDATA, NONE },
    { ".tocd",      dirFuncSwitchSection,   DT_SEC_TOCD,    NONE },
#endif
    { ".section",   dirFuncUserSection,     DT_NOPARM,      SYM | STR },
#endif
    { ".set",       dirFuncSetOption,       DT_NOPARM,      LINE },
    { ".short",     dirFuncValues,          DT_VAL_INT16,   INT | RINT | SYM },
    { ".space",     dirFuncSpace,           DT_NOPARM,      INT },
    { ".string",    dirFuncString,          DT_STR_NULL,    STR },
#ifdef _STANDALONE_
    { ".text",      dirFuncSwitchSection,   DT_SEC_TEXT,    NONE },
#endif
    { ".value",     dirFuncValues,          DT_VAL_INT16,   INT | RINT | SYM },
#ifdef _STANDALONE_
    { ".version",   dirFuncIgnore,          DT_NOPARM,      LINE },
#endif
#if defined( AS_ALPHA )
    { "unop",       dirFuncNop,             DT_NOP_NOP,     NONE },
    { "fnop",       dirFuncNop,             DT_NOP_FNOP,    NONE },
// The .quad directive is disabled because 64-bit integers are not parsed
// properly
//    { ".quad",      dirFuncValues,          DT_VAL_INT64,   INT | RINT },
    { ".s_floating",dirFuncValues,          DT_VAL_FLOAT,   FLT | RFLT },
    { ".t_floating",dirFuncValues,          DT_VAL_DOUBLE,  FLT | RFLT },
    { ".word",      dirFuncValues,          DT_VAL_INT16,   INT | RINT | SYM },
#elif defined( AS_PPC )
    { ".word",      dirFuncValues,          DT_VAL_INT32,   INT | RINT | SYM },
    { ".little_endian", dirFuncIgnore,      DT_NOPARM,      LINE },
    { ".big_endian",    dirFuncUnsupported, DT_NOPARM,      LINE },
#elif defined( AS_MIPS )
    { ".word",      dirFuncValues,          DT_VAL_INT32,   INT | RINT | SYM },
#endif
};


static bool dirValidate( directive_t *dir, dir_table *table_entry )
//*****************************************************************
// Check if the operands types are as expected
{
    static const dirop_flags flags[] = {    // corresponds to dirop_type
        DOF_INT,
        DOF_FLT,
        DOF_SYM,
        DOF_NUMREF,
        DOF_LINE,
        DOF_STR,
        DOF_REP_INT,
        DOF_REP_FLT,
        DOF_ERROR,
    };
    int                 opnum = 0;
    dir_operand         *dirop;

    dirop = dir->operand_list;
    if( dirop == NULL && ( table_entry->flags & DOF_NONE ) != DOF_NONE ) {
        Error( DIROP_ERR_MISSING, opnum );
        return( FALSE );
    }
    while( dirop ) {
        dirop_flags     flag;

        flag = flags[ dirop->type ];
        if( ( table_entry->flags & flag ) != flag ) {
            Error( IMPROPER_DIROP, opnum );
            return( FALSE );
        }
        opnum++;
        dirop = dirop->next;
    }
    return( TRUE );
}


static directive_t *dirAlloc( void )
//**********************************
{
    return( MemAlloc( sizeof( directive_t ) ) );
}


static void dirFree( directive_t *directive )
//*******************************************
{
    MemFree( directive );
}


extern void DirInit( void )
//*************************
{
    dir_table   *curr;
    sym_handle  sym;
    int         i, n;

    n = sizeof( asm_directives ) / sizeof( asm_directives[0] );
    for( i = 0; i < n; i++ ) {
        curr = &asm_directives[i];
        sym = SymAdd( curr->name, SYM_DIRECTIVE );
        SymSetLink( sym, curr );
    }
    autoAlignment = TRUE;
    AsDirSetOptions = NONE;
    _DirSet( AT );
    _DirSet( MACRO );
    _DirSet( REORDER );
}


extern void DirSetNextScanState( sym_handle sym )
//*************************************************
// Call this to set up what to scan for the next token.
// Necessary because some directives take the whole line as a token.
{
    dir_table   *table_entry;

    table_entry = SymGetLink( sym );
    if ( table_entry->flags & DOF_LINE ) {
        getDirOpLine = TRUE;
    }
}


extern bool DirGetNextScanState( void )
//*************************************
// Call this to check what to scan for the next token.
// Returns TRUE if we want the whole line as a token next.
// Necessary because some directives take the whole line as a token.
{
    if ( getDirOpLine ) {
        getDirOpLine = FALSE;
        return( TRUE );
    }
    return( getDirOpLine );
}


extern directive_t *DirCreate( sym_handle sym )
//*********************************************
{
    lastDirective = dirAlloc();
    lastDirective->dir_sym = sym;
    lastDirective->num_operands = 0;
    lastDirective->operand_list = NULL;
    lastDirective->operand_tail = NULL;
    return( lastDirective );
}


extern void DirAddOperand( directive_t *dir, dir_operand *dirop )
//***************************************************************
{
    if( dirop == NULL ) return;
    dir->num_operands++;
    if( dir->operand_tail ) {
        dir->operand_tail->next = dirop;
        dir->operand_tail = dirop;
    } else {
        dir->operand_list = dir->operand_tail = dirop;
    }
}


extern void DirDestroy( directive_t *directive )
//**********************************************
{
    dir_operand *dirop, *dirop_next;

    if( !directive ) return;
    dirop = directive->operand_list;
    while( dirop ) {
        dirop_next = dirop->next;
        DirOpDestroy( dirop );
        dirop = dirop_next;
    }
    dirFree( directive );
    lastDirective = NULL;
}


extern bool DirParse( directive_t *directive )
//********************************************
{
    dir_table   *table_entry;

    table_entry = SymGetLink( directive->dir_sym );
    _DBGMSG2( "Got directive '%s'\n", table_entry->name );
    if ( dirValidate( directive, table_entry ) ) {
        // Return FALSE only if you want to abort yyparse
        return( table_entry->func( directive, table_entry->parm ) );
    }
    return( TRUE );
}


extern void DirFini( void )
//*************************
{
    DirDestroy( lastDirective );
}
