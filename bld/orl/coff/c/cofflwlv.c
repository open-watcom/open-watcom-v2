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
* Description:  COFF symbol table and relocations processing.
*
****************************************************************************/


#include "cofflwlv.h"
#include "cofforl.h"
#include "orlhash.h"
#include "walloca.h"

#include "clibext.h"


orl_return CoffCreateSymbolHandles( coff_file_handle file_hnd )
{
    unsigned            loop;
    unsigned            prev;
//    int                 len;
    uint_16             type; // type of CoffSymEnt
    coff_symbol_handle  current;
    coff_sym_section *  aux;
    coff_sym_weak *     weak;
    coff_sec_handle     sechdl;

    if( file_hnd->num_symbols == 0 ){
        file_hnd->symbol_handles = NULL;
        return( ORL_OKAY );
    }
    file_hnd->symbol_handles = (coff_symbol_handle)_ClientAlloc( file_hnd, sizeof( coff_symbol_handle_struct ) * file_hnd->num_symbols );
    if( file_hnd->symbol_handles == NULL )
        return( ORL_OUT_OF_MEMORY );
    prev = 0;
    for( loop = 0; loop < file_hnd->num_symbols; loop++ ) {
        current = file_hnd->symbol_handles + loop;
        current->file_format = ORL_COFF;
        current->coff_file_hnd = file_hnd;
        current->symbol = (coff_symbol *)( file_hnd->symbol_table->contents + sizeof( coff_symbol ) * loop );
        if( current->symbol->name.non_name.zeros == 0 ) {
            current->name = (char *)( file_hnd->string_table->contents + current->symbol->name.non_name.offset - sizeof( coff_sec_size ) );
            current->name_alloced = COFF_FALSE;
        } else {
//            len = strlen( current->symbol->name.name_string );
            if( strlen( current->symbol->name.name_string ) >= COFF_SYM_NAME_LEN ) {
                current->name = _ClientAlloc( file_hnd, COFF_SYM_NAME_LEN + 1 );
                strncpy( current->name, current->symbol->name.name_string, COFF_SYM_NAME_LEN );
                current->name[COFF_SYM_NAME_LEN] = '\0';
                current->name_alloced = COFF_TRUE;
            } else {
                current->name = current->symbol->name.name_string;
                current->name_alloced = COFF_FALSE;
            }
        }
        if( memcmp( current->name, ".bf", 4 ) == 0 ) {
            if( current->symbol->num_aux >= 1 ) {
                file_hnd->symbol_handles[prev].has_bf = COFF_TRUE;
            }
        }
        sechdl = NULL;
        current->type = 0;
        switch( current->symbol->sec_num ) {
        case IMAGE_SYM_DEBUG:
            current->type |= ORL_SYM_TYPE_DEBUG;
            current->binding = ORL_SYM_BINDING_NONE;
            break;
        case IMAGE_SYM_ABSOLUTE:
            current->type |= ORL_SYM_TYPE_ABSOLUTE;
            current->binding = ORL_SYM_BINDING_NONE; // ?
            break;
        case IMAGE_SYM_UNDEFINED:
            if( current->symbol->value == 0) {
                current->type |= ORL_SYM_TYPE_UNDEFINED;
            } else {
                current->type |= ORL_SYM_TYPE_COMMON;
            }
            break;
        default:
            current->type |= ORL_SYM_TYPE_DEFINED;
            sechdl = file_hnd->orig_sec_hnd[current->symbol->sec_num - 1];
            if( sechdl->flags & ORL_SEC_FLAG_COMDAT ) {
                current->type |= ORL_SYM_CDAT_MASK;
            }
            break;
        }
        switch( current->symbol->storage_class ) {
        case IMAGE_SYM_CLASS_EXTERNAL:
        case IMAGE_SYM_CLASS_LABEL:
        case IMAGE_SYM_CLASS_UNDEFINED_LABEL:
        case IMAGE_SYM_CLASS_WEAK_EXTERNAL:
            if( current->symbol->storage_class
                    == IMAGE_SYM_CLASS_LABEL ) {
                current->binding = ORL_SYM_BINDING_LOCAL;
            } else if( (current->symbol->storage_class == IMAGE_SYM_CLASS_EXTERNAL
                      || current->symbol->storage_class == IMAGE_SYM_CLASS_WEAK_EXTERNAL)
                    && current->symbol->sec_num == IMAGE_SYM_UNDEFINED
                    && current->symbol->value == 0
                    && current->symbol->num_aux == 1 ) {
                weak = (coff_sym_weak *)( current->symbol + 1 );
                switch( weak->characteristics ) {
                case IMAGE_WEAK_EXTERN_SEARCH_NOLIBRARY:
                    current->binding = ORL_SYM_BINDING_WEAK;
                    break;
                case IMAGE_WEAK_EXTERN_SEARCH_LIBRARY:
                    current->binding = ORL_SYM_BINDING_LAZY;
                    break;
                case IMAGE_WEAK_EXTERN_SEARCH_ALIAS:
                    current->binding = ORL_SYM_BINDING_ALIAS;
                    break;
                }
            } else {
                current->binding = ORL_SYM_BINDING_GLOBAL;
            }
            type = _CoffComplexType( current->symbol->type );
            if( type & IMAGE_SYM_DTYPE_FUNCTION ) {
                current->type |= ORL_SYM_TYPE_FUNCTION;
            } else {
                current->type |= ORL_SYM_TYPE_OBJECT;
            }
            break;
        case IMAGE_SYM_CLASS_STATIC:
            current->binding = ORL_SYM_BINDING_LOCAL;
            if( current->symbol->num_aux == 0 ) {
                if( sechdl != NULL && strcmp( sechdl->name, current->name ) == 0 ) {
                    current->type |= ORL_SYM_TYPE_SECTION;
                } else {
                    type = _CoffComplexType( current->symbol->type );
                    if( type & IMAGE_SYM_DTYPE_FUNCTION ) {
                        current->type |= ORL_SYM_TYPE_FUNCTION;
                    } else {
                        current->type |= ORL_SYM_TYPE_OBJECT;
                    }
                }
            } else if( current->symbol->num_aux == 1 && (current->type & ORL_SYM_CDAT_MASK) ) {
                current->type |= ORL_SYM_TYPE_SECTION;
                aux = (coff_sym_section *)(current->symbol + 1);
                current->type &= ~ORL_SYM_CDAT_MASK;
                current->type |= (aux->selection << ORL_SYM_CDAT_SHIFT) & ORL_SYM_CDAT_MASK;
            } else {
                type = _CoffComplexType( current->symbol->type );
                if( type & IMAGE_SYM_DTYPE_FUNCTION ) {
                    current->type |= ORL_SYM_TYPE_FUNCTION;
                }
            }
            break;
        case IMAGE_SYM_CLASS_FUNCTION:
            // The .bf, .lf and .ef symbols are not regular symbols
            // and their values in particular must not be interpreted
            // as offsets/addresses.
            if( !memcmp( current->name, ".bf", 4 )
                || !memcmp( current->name, ".lf", 4 )
                || !memcmp( current->name, ".ef", 4 ) )
                current->binding = ORL_SYM_BINDING_NONE;
            else
                current->binding = ORL_SYM_BINDING_LOCAL;
            current->type |= ORL_SYM_TYPE_FUNC_INFO;
            break;
        case IMAGE_SYM_CLASS_FILE:
            current->binding = ORL_SYM_BINDING_LOCAL;
            current->type |= ORL_SYM_TYPE_FILE;
            break;
        }
        prev = loop;
        loop += current->symbol->num_aux;
    }
    return( ORL_OKAY );
}

orl_return CoffBuildSecNameHashTable( coff_file_handle coff_file_hnd )
{
    unsigned                                    loop;
    orl_return                                  error;

    coff_file_hnd->sec_name_hash_table = ORLHashTableCreate( coff_file_hnd->coff_hnd->funcs, SEC_NAME_HASH_TABLE_SIZE, ORL_HASH_STRING, (orl_hash_comparison_func) stricmp );
    if( !(coff_file_hnd->sec_name_hash_table) ) {
        return( ORL_OUT_OF_MEMORY );
    }
    for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
        error = ORLHashTableInsert( coff_file_hnd->sec_name_hash_table, coff_file_hnd->coff_sec_hnd[loop]->name, coff_file_hnd->coff_sec_hnd[loop] );
        if( error != ORL_OKAY ) {
            return( error );
        }
    }
    return( ORL_OKAY );
}

orl_reloc_type CoffConvertRelocType( coff_file_handle coff_file_hnd, coff_reloc_type coff_type )
{
    if( coff_file_hnd->machine_type == ORL_MACHINE_TYPE_ALPHA ) {
        switch( coff_type ) {
        case IMAGE_REL_ALPHA_ABSOLUTE:
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case IMAGE_REL_ALPHA_REFLONG:
        case IMAGE_REL_ALPHA_REFQUAD:
            return( ORL_RELOC_TYPE_WORD_32 );
        case IMAGE_REL_ALPHA_BRADDR:
            return( ORL_RELOC_TYPE_REL_21_SH );
        case IMAGE_REL_ALPHA_REFHI:
        case IMAGE_REL_ALPHA_INLINE_REFLONG:
            return( ORL_RELOC_TYPE_HALF_HI );
        case IMAGE_REL_ALPHA_REFLO:
            return( ORL_RELOC_TYPE_HALF_LO );
        case IMAGE_REL_ALPHA_PAIR:
            return( ORL_RELOC_TYPE_PAIR );
        case IMAGE_REL_ALPHA_SECTION:
            return( ORL_RELOC_TYPE_SEGMENT );
        case IMAGE_REL_ALPHA_SECREL:
            return( ORL_RELOC_TYPE_SEC_REL );
        case IMAGE_REL_ALPHA_REFLONGNB:
            return( ORL_RELOC_TYPE_WORD_32_NB );
        default:
            return( ORL_RELOC_TYPE_NONE );
        }
    } else if( coff_file_hnd->machine_type == ORL_MACHINE_TYPE_I386 ) {
        switch( coff_type ) {
        case IMAGE_REL_I386_ABSOLUTE:
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case IMAGE_REL_I386_DIR16:
            return( ORL_RELOC_TYPE_WORD_16 );
        case IMAGE_REL_I386_REL16:
            return( ORL_RELOC_TYPE_REL_16 );
        case IMAGE_REL_I386_DIR32:
            return( ORL_RELOC_TYPE_WORD_32 );
        case IMAGE_REL_I386_DIR32NB:
            return( ORL_RELOC_TYPE_WORD_32_NB );
        case IMAGE_REL_I386_REL32:
            return( ORL_RELOC_TYPE_JUMP );
        case IMAGE_REL_I386_SECTION:
            return( ORL_RELOC_TYPE_SECTION );
        case IMAGE_REL_I386_SECREL:
            return( ORL_RELOC_TYPE_SEC_REL );
        default:
            return( ORL_RELOC_TYPE_NONE );
        }
    } else if( coff_file_hnd->machine_type == ORL_MACHINE_TYPE_AMD64 ) {
        switch( coff_type ) {
        case IMAGE_REL_AMD64_ABSOLUTE:
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case IMAGE_REL_AMD64_REL32:              // 32-Bit PC-relative offset
            return( ORL_RELOC_TYPE_REL_32 );
        case IMAGE_REL_AMD64_ADDR32:
            return( ORL_RELOC_TYPE_WORD_32 );
        case IMAGE_REL_AMD64_ADDR32NB:
            return( ORL_RELOC_TYPE_WORD_32_NB );
        case IMAGE_REL_AMD64_ADDR64:
            return( ORL_RELOC_TYPE_WORD_64 );
        case IMAGE_REL_AMD64_REL32_1:
            return( ORL_RELOC_TYPE_REL_32_ADJ1 );
        case IMAGE_REL_AMD64_REL32_2:
            return( ORL_RELOC_TYPE_REL_32_ADJ2 );
        case IMAGE_REL_AMD64_REL32_3:
            return( ORL_RELOC_TYPE_REL_32_ADJ3 );
        case IMAGE_REL_AMD64_REL32_4:
            return( ORL_RELOC_TYPE_REL_32_ADJ4 );
        case IMAGE_REL_AMD64_REL32_5:
            return( ORL_RELOC_TYPE_REL_32_ADJ5 );
        case IMAGE_REL_AMD64_SECREL:
            return( ORL_RELOC_TYPE_SEC_REL );
        default:
            return( ORL_RELOC_TYPE_NONE );
        }
    } else if( coff_file_hnd->machine_type == ORL_MACHINE_TYPE_PPC601 ) {
        switch( coff_type & IMAGE_REL_PPC_TYPEMASK ) {
        case IMAGE_REL_PPC_ABSOLUTE: // NOP
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case IMAGE_REL_PPC_ADDR64:   // 64-bit address
            return( ORL_RELOC_TYPE_WORD_64 );
        case IMAGE_REL_PPC_ADDR32:   // 32-bit address
            return( ORL_RELOC_TYPE_WORD_32 );
        case IMAGE_REL_PPC_ADDR24:   // 26-bit address, shifted left 2 (branch absolute)
            return( ORL_RELOC_TYPE_WORD_24 );
        case IMAGE_REL_PPC_ADDR16:   // 16-bit address
            return( ORL_RELOC_TYPE_WORD_16 );
        case IMAGE_REL_PPC_ADDR14:   // 16-bit address, shifted left 2 (load doubleword)
            return( ORL_RELOC_TYPE_WORD_14);
        case IMAGE_REL_PPC_REL24:   // 26-bit PC-relative offset, shifted left 2 (branch relative)
            return( ORL_RELOC_TYPE_REL_24 );
        case IMAGE_REL_PPC_REL14:    // 16-bit PC-relative offset, shifted left 2 (br cond relative)
            return( ORL_RELOC_TYPE_REL_14 );
        case IMAGE_REL_PPC_TOCREL16: // 16-bit offset from TOC base
            if( coff_type & IMAGE_REL_PPC_TOCDEFN ) {
                return( ORL_RELOC_TYPE_TOCVREL_16 );
            } else {
                return( ORL_RELOC_TYPE_TOCREL_16 );
            }
        case IMAGE_REL_PPC_TOCREL14: // 14-bit offset from TOC base, shifted left 2 (load doubleword)
            if( coff_type & IMAGE_REL_PPC_TOCDEFN ) {
                return( ORL_RELOC_TYPE_TOCVREL_14 );
            } else {
                return( ORL_RELOC_TYPE_TOCREL_14 );
            }
        case IMAGE_REL_PPC_SECREL:
            return( ORL_RELOC_TYPE_SEC_REL );
        case IMAGE_REL_PPC_SECTION:
            return( ORL_RELOC_TYPE_SEGMENT );
        case IMAGE_REL_PPC_ADDR32NB:  // 32-bit addr w/o image base
            return( ORL_RELOC_TYPE_WORD_32_NB );
        case IMAGE_REL_PPC_IFGLUE: // Substitute TOC restore instruction iff symbol is glue code
            return( ORL_RELOC_TYPE_IFGLUE );
        case IMAGE_REL_PPC_IMGLUE:
            return( ORL_RELOC_TYPE_IMGLUE );
        default:
            return( ORL_RELOC_TYPE_NONE );
        }
    } else if( coff_file_hnd->machine_type == ORL_MACHINE_TYPE_R3000
            || coff_file_hnd->machine_type == ORL_MACHINE_TYPE_R4000 ) {
        switch( coff_type ) {
        case IMAGE_REL_MIPS_ABSOLUTE:   // NOP
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case IMAGE_REL_MIPS_REFWORD:    // 32-bit address
            return( ORL_RELOC_TYPE_WORD_32 );
        case IMAGE_REL_MIPS_JMPADDR:    // 26-bit absolute address (j/jal)
            return( ORL_RELOC_TYPE_WORD_26 );
        case IMAGE_REL_MIPS_REFHI:
            return( ORL_RELOC_TYPE_HALF_HI );
        case IMAGE_REL_MIPS_REFLO:
            return( ORL_RELOC_TYPE_HALF_LO );
        case IMAGE_REL_MIPS_PAIR:
            return( ORL_RELOC_TYPE_PAIR );
        case IMAGE_REL_MIPS_GPREL:      // 16-bit offset from GP register
        case IMAGE_REL_MIPS_LITERAL:
            return( ORL_RELOC_TYPE_TOCREL_16 );
        case IMAGE_REL_MIPS_SECREL:
            return( ORL_RELOC_TYPE_SEC_REL );
        case IMAGE_REL_MIPS_SECTION:
            return( ORL_RELOC_TYPE_SEGMENT );
        case IMAGE_REL_MIPS_REFWORDNB:  // 32-bit addr w/o image base
            return( ORL_RELOC_TYPE_WORD_32_NB );
        default:
            return( ORL_RELOC_TYPE_NONE );
        }
    }
    return( 0 );
}

orl_return CoffCreateRelocs( coff_sec_handle orig_sec, coff_sec_handle reloc_sec )
{
    orl_return  return_val;
    unsigned    num_relocs;
    unsigned    loop;
    coff_reloc ORLUNALIGNED *rel;
    orl_reloc * o_rel;
    orl_reloc * prev_rel;

    if( reloc_sec->coff_file_hnd->symbol_handles == NULL ) {
        return_val = CoffCreateSymbolHandles( reloc_sec->coff_file_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    num_relocs = reloc_sec->size / sizeof( coff_reloc );
    reloc_sec->assoc.reloc.num_relocs = num_relocs;
    reloc_sec->assoc.reloc.relocs = (orl_reloc *)_ClientSecAlloc( reloc_sec, sizeof( orl_reloc ) * num_relocs );
    if( reloc_sec->assoc.reloc.relocs == NULL )
        return( ORL_OUT_OF_MEMORY );
    rel = (coff_reloc *)reloc_sec->contents;
    o_rel = (orl_reloc *)reloc_sec->assoc.reloc.relocs;
    for( loop = 0; loop < num_relocs; loop++ ) {
        o_rel->section = (orl_sec_handle)orig_sec;
        if( reloc_sec->coff_file_hnd->machine_type == ORL_MACHINE_TYPE_ALPHA
            && rel->type == IMAGE_REL_ALPHA_MATCH ) {
            o_rel->type = ORL_RELOC_TYPE_HALF_LO;
            prev_rel = o_rel - 1;
            o_rel->symbol = prev_rel->symbol;
            o_rel->offset = prev_rel->offset + rel->sym_tab_index;
        } else {
            o_rel->type = CoffConvertRelocType( reloc_sec->coff_file_hnd, rel->type );
            if( o_rel->type == ORL_RELOC_TYPE_PAIR ) {
                o_rel->symbol = NULL;
            } else {
                o_rel->symbol = (orl_symbol_handle)( reloc_sec->coff_file_hnd->symbol_handles + rel->sym_tab_index );
            }
            o_rel->offset = rel->offset - orig_sec->hdr->offset;
        }
        o_rel->addend = 0;
        o_rel->frame = NULL;
        rel++;
        o_rel++;
    }
    return( ORL_OKAY );
}

orl_linnum * CoffConvertLines( coff_sec_handle hdl, orl_table_index numlines )
/****************************************************************************/
{
    coff_line_num ORLUNALIGNED  *coffline;
    orl_linnum ORLUNALIGNED     *linestart;
    orl_linnum ORLUNALIGNED     *currline;
    coff_file_handle            fhdl;
    unsigned_32                 linebase;
    coff_symbol_handle          sym;
    coff_sym_bfef               *csym;
    unsigned_32                 offset;

    fhdl = hdl->coff_file_hnd;
    if( fhdl->symbol_handles == NULL ) {
        if( CoffCreateSymbolHandles( fhdl ) != ORL_OKAY ) {
            return( NULL );
        }
    }
    coffline = (coff_line_num *)( hdl->hdr->lineno_ptr - fhdl->initial_size + fhdl->rest_of_file_buffer );
    currline = (orl_linnum *)coffline;
    if( hdl->relocs_done )
        return( (orl_linnum *)currline );
    linestart = currline;
    linebase = 0;
    while( numlines > 0 ) {
        if( coffline->line_number == 0 ) {
            sym = fhdl->symbol_handles + coffline->ir.symbol_table_index;
            coffline->ir.RVA = sym->symbol->value;
            if( sym->has_bf ) {
                csym = (coff_sym_bfef *)( sym->symbol + sym->symbol->num_aux + 2 );
                linebase = csym->linenum;
            }
        }
        offset = coffline->ir.RVA;
        currline->linnum = coffline->line_number + linebase;
        currline->off = offset;
        coffline++;
        currline++;
        numlines--;
    }
    hdl->relocs_done = COFF_TRUE;
    return( (orl_linnum *)linestart );
}

static size_t strncspn( const char *s, const char *charset, size_t len )
{
    unsigned char   chartable[32];
    size_t          i;
    unsigned char   ch;

    memset( chartable, 0, sizeof( chartable ) );
    for( ; *charset != 0; charset++ ) {
        ch = *charset;
        chartable[ch / 8] |= 1 << ( ch % 8 );
    }
    for( i = 0; i < len; i++ ) {
        ch = s[i];
        if( chartable[ch / 8] & ( 1 << ( ch % 8 ) ) ) {
            break;
        }
    }
    return( i );
}

static const char *pstrncspn( const char *s, const char *charset, size_t *len )
{
    size_t  l;

    l = strncspn( s, charset, *len );
    *len -= l;
    return( s + l );
}

static void EatWhite( const char **contents, size_t *len )
/********************************************************/
{
    char ch;

    ch = **contents;
    while( (ch == ' ' || ch == '\t' || ch == '=' || ch == ',') && *len > 0 ) {
        (*len)--;
        *contents += 1;
        ch = **contents;
    }
}

static orl_return ParseExport( const char **contents, size_t *len,
                                orl_note_callbacks *cb, void *cookie )
/********************************************************************/
{
    char *      arg;
    size_t      l;

    l = strncspn( *contents, ", \t", *len );
    arg = alloca( l + 1 );
    memcpy( arg, *contents, l );
    arg[l] = 0;
    *len -= l;
    *contents += l;
    return( cb->export_fn( arg, cookie ) );
}


static orl_return ParseDefLibEntry( const char **contents, size_t *len,
                        callback_deflib_fn *deflibentry_fn, void *cookie )
/************************************************************************/
{
    char        *arg;
    size_t      l;
    orl_return  retval;

    for(;;) {
        l = strncspn( *contents, ", \t", *len );
        arg = alloca( l + 1 );
        memcpy( arg, *contents, l );
        arg[l] = 0;
        *len -= l;
        *contents += l;

        retval = deflibentry_fn( arg, cookie );
        if( retval != ORL_OKAY || **contents != ',' )
            break;
        (*contents)++;
    }
    return( retval );
}

orl_return CoffParseDrectve( const char *contents, size_t len, orl_note_callbacks *cb, void *cookie )
/***************************************************************************************************/
{
    const char  *cmd;

    EatWhite( &contents, &len );
    while( len > 0 ) {
        if( *contents != '-' )
            break;   // - should be start of token
        contents++; len--;
        cmd = contents;
        contents = pstrncspn( contents, ":", &len);
        if( contents == NULL )
            break;
        contents++; len--;
        if( memicmp( cmd, "export", 6 ) == 0 ) {
            if( ParseExport( &contents, &len, cb, cookie ) != ORL_OKAY ) {
                break;
            }
        } else if( memicmp( cmd, "defaultlib", 10 ) == 0 ) {
            if( ParseDefLibEntry( &contents, &len, cb->deflib_fn, cookie ) != ORL_OKAY ) {
                break;
            }
        } else if( memicmp( cmd, "entry", 5 ) == 0 ) {
            if( ParseDefLibEntry( &contents, &len, cb->entry_fn, cookie ) != ORL_OKAY ) {
                break;
            }
        }
        EatWhite( &contents, &len );
    }
    return( ORL_OKAY );
}
