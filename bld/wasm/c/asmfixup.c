/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "asmglob.h"
#include "asmalloc.h"
#include "asmfixup.h"
#if defined( _STANDALONE_ )
  #include "directiv.h"
  #include "myassert.h"
  #include "mangle.h"
#endif

#if defined( _STANDALONE_ )
asmfixup_list   FixupList;
#else
asmfixup        *FixupHead;
#endif
asmfixup        *InsFixups[OPND_MAX];

#if defined( _STANDALONE_ )

static const char * const FPPatchName[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) name,
    #include "fppatche.h"
    #undef pick_fp
};

static const char * const FPPatchAltName[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) alt_name,
    #include "fppatche.h"
    #undef pick_fp
};

void add_frame( void )
/*********************
 * determine the frame and frame datum for the fixup
 */
{
    asmfixup    *fixup;

    if( Parse_Pass != PASS_1 ) {
        fixup = InsFixups[Opnd_Count];
        if( fixup == NULL )
            return;
        fixup->frame = Frame;
    }
}

static void append_to_fixup_list_end( asmfixup *fixup )
/******************************************************
 * add new fixup to the end of list
 */
{
    fixup->next_loc = NULL;
    if( FixupList.head == NULL ) {
        FixupList.head = fixup;
    } else {
        FixupList.tail->next_loc = fixup;
    }
    FixupList.tail = fixup;
}
#endif

asmfixup *AddFixup( asm_sym_handle sym, fixup_types fixup_type, fixup_options fixup_option )
/*******************************************************************************************
 * put the correct target offset into the link list when forward reference of
 * relocatable is resolved;
 */
{
    asmfixup    *fixup;

    switch( Code->info.token ) {
    case T_CMPS:
    case T_LODS:
    case T_MOVS:
    case T_OUTS:
    case T_INS:
    case T_SCAS:
    case T_STOS:
    case T_XLAT:
        return( NULL );
    }

    fixup = AsmAlloc( sizeof( asmfixup ) );
    if( fixup != NULL ) {
        fixup->external = false;
        fixup->fixup_loc = AsmCodeAddress;
#if defined( _STANDALONE_ )
        fixup->sym = sym;
        fixup->u_offset = sym->offset;
        if( Modend ) {
            fixup->fixup_seg = NULL;
        } else {
            fixup->fixup_seg = GetCurrSeg();
        }
        fixup->frame = Frame;                   // this is just a guess
        fixup->next = sym->fixup;
        sym->fixup = fixup;
#else
        fixup->name = sym->name;
        fixup->next = FixupHead;
        FixupHead = fixup;
        fixup->u_offset = 0;
#endif
        fixup->fixup_type = fixup_type;
        fixup->fixup_option = fixup_option;
        InsFixups[Opnd_Count] = fixup;
    }
    return( fixup );
}

#if defined( _STANDALONE_ )

#define SkipFixup() \
    fixup->next = sym->fixup; \
    sym->fixup = fixup

#else

#define SkipFixup() \
    fixup->next = FixupHead; \
    FixupHead = fixup

static void PatchCodeBuffer( asmfixup *fixup, unsigned size )
/***********************************************************/
{
    long    disp;
    long    fixup_loc;

    fixup_loc = fixup->fixup_loc;
    disp = fixup->u_offset + AsmCodeAddress - fixup->fixup_loc - size;
    for( ; size > 0; size-- ) {
        AsmCodeBuffer[fixup_loc++] = (uint_8)disp;
        disp >>= 8;
    }
}

#endif

static bool DoPatch( asm_sym_handle sym, asmfixup *fixup )
/********************************************************/
{
    long                disp;
    long                max_disp;
    unsigned            size;
#if defined( _STANDALONE_ )
    dir_node_handle     seg;

    /*
     * all relative fixups should occure only at first pass and they signal
     * forward references they must be removed after patching or
     * skiped ( next processed as normal fixup )
     */
    seg = GetSeg( sym );
    if( seg == NULL || fixup->fixup_seg != seg ) {
        /*
         * can't backpatch if fixup location is in diff seg than symbol
         */
        SkipFixup();
        return( RC_OK );
    }
    if( Parse_Pass == PASS_1 ) {
        if( sym->mem_type == MT_FAR && fixup->fixup_option == OPTJ_CALL ) {
            /*
             * convert far call to near, only at first pass
             */
            PhaseError = true;
            sym->offset++;
            AsmByte( 0 );
            AsmFree( fixup );
            return( RC_OK );
        } else if( sym->mem_type == MT_NEAR ) {
            /*
             * near forward reference, only at first pass
             */
            switch( fixup->fixup_type ) {
            case FIX_RELOFF32:
            case FIX_RELOFF16:
                AsmFree( fixup );
                return( RC_OK );
            }
        }
    }
#else
    if( fixup->name != sym->name ) {
        SkipFixup();
        return( RC_OK );
    }
#endif
    size = 0;
    switch( fixup->fixup_type ) {
    case FIX_RELOFF32:
        size = 2;
        /* fall through */
    case FIX_RELOFF16:
        size++;
        /* fall through */
    case FIX_RELOFF8:
        size++;
        /*
         * calculate the displacement
         */
        disp = fixup->u_offset + AsmCodeAddress - fixup->fixup_loc - size;
        max_disp = (1UL << ((size * 8)-1)) - 1;
        if( disp > max_disp || disp < -(max_disp + 1) ) {
#if defined( _STANDALONE_ )
            PhaseError = true;
            switch( size ) {
            case 1:
                switch( fixup->fixup_option ) {
                case OPTJ_EXPLICIT:
                    AsmError( JUMP_OUT_OF_RANGE );
                    sym->fixup = NULL;
                    return( RC_ERROR );
                case OPTJ_EXTEND:
                    sym->offset++;
                    AsmByte( 0 );
                    /* fall through */
                case OPTJ_JXX:
                    sym->offset++;
                    AsmByte( 0 );
                    /* fall through */
                default:
                    if( Code->use32 ) {
                        sym->offset += 2;
                        AsmByte( 0 );
                        AsmByte( 0 );
                    }
                    sym->offset++;
                    AsmByte( 0 );
                    break;
                }
                break;
            case 2:
            case 4:
                AsmWarn( 4, JUMP_OUT_OF_RANGE );
                break;
            }
#else
            AsmError( JUMP_OUT_OF_RANGE );
            FixupHead = NULL;
            return( RC_ERROR );
        } else {
            PatchCodeBuffer( fixup, size );
#endif
        }
        AsmFree( fixup );
        break;
    default:
        SkipFixup();
        break;
    }
    return( RC_OK );
}

bool BackPatch( asm_sym_handle sym )
/*****************************
 * patching for forward reference labels in Jmp/Call instructions;
 * call only when a new label appears;
 */
{
    asmfixup    *fixup;
    asmfixup    *next;

#if defined( _STANDALONE_ )
    fixup = sym->fixup;
    sym->fixup = NULL;
#else
    fixup = FixupHead;
    FixupHead = NULL;
#endif
    for( ; fixup != NULL; fixup = next ) {
        next = fixup->next;
        if( DoPatch( sym, fixup ) ) {
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

void mark_fixupp( OPNDTYPE determinant, operand_idx index )
/**********************************************************
 * this routine marks the correct target offset and data record address for
 * FIXUPP record;
 */
{
    asmfixup    *fixup;

    fixup = InsFixups[index];
    if( fixup != NULL ) {
        fixup->fixup_loc = AsmCodeAddress;
#if defined( _STANDALONE_ )
        // fixup->offset = Code->data[index];
        // Code->data[index] = 0; // fixme
        if( fixup->fixup_type != FIX_SEG ) {
            Code->data[index] += fixup->u_offset;
        }
        /*
         * put the offset in the location instead of attaching it to the fixup
         */
#else
        fixup->u_offset = Code->data[index];
        Code->data[index] = 0;
#endif

        switch( determinant ) {
        case OP_I16:
        case OP_J32:
            switch( fixup->fixup_type ) {
            case FIX_OFF32:
                fixup->fixup_type = FIX_OFF16;
                break;
            case FIX_PTR32:
                fixup->fixup_type = FIX_PTR16;
                break;
            }
            break;
        case OP_I32:
        case OP_J48:
            switch( fixup->fixup_type ) {
            case FIX_OFF16:
                fixup->fixup_type = FIX_OFF32;
                break;
            case FIX_PTR16:
                fixup->fixup_type = FIX_PTR32;
                break;
            }
            break;
        }
    }
}

#if defined( _STANDALONE_ )

bool store_fixup( operand_idx index )
/************************************
 * Store the fixup information in a OMF output fixup record
 */
{
    asmfixup    *fixup;

    if( Parse_Pass == PASS_1 )
        return( RC_OK );
    if( InsFixups[index] == NULL )
        return( RC_OK );
    fixup = InsFixups[index];
    if( fixup == NULL )
        return( RC_ERROR );
    /*
     * add new fixup to the end of list
     */
    append_to_fixup_list_end( fixup );
    return( RC_OK );
}

static bool MakeFpFixup( const char *patch_name )
/***********************************************/
{
    dir_node_handle dir;
    asmfixup        *fixup;

    for( dir = Tables[TAB_FPPATCH].head; dir != NULL; dir = dir->next ) {
        if( strcmp( dir->sym.name, patch_name ) == 0 ) {
            break;
        }
    }
    if( dir == NULL ) {
        dir = AllocD( patch_name );
        if( dir != NULL ) {
            dir_init( dir, TAB_FPPATCH );
            GetSymInfo( &dir->sym );
            dir->sym.offset = 0;
            dir->sym.referenced = true;
            dir->sym.mem_type = MT_FAR;
            SetMangler( &dir->sym, "N", WASM_LANG_NONE );
        }
    }
    if( dir != NULL ) {
        if( Parse_Pass != PASS_1 ) {
            fixup = AsmAlloc( sizeof( asmfixup ) );
            if( fixup == NULL )
                return( RC_ERROR );
            fixup->external = false;
            fixup->fixup_loc = AsmCodeAddress;
            fixup->fixup_seg = NULL;
            fixup->sym = &dir->sym;
            fixup->u_offset = 0;
            fixup->frame = NULL;
            fixup->next = dir->sym.fixup;
            dir->sym.fixup = fixup;
            fixup->fixup_type = FIX_FPPATCH;
            fixup->fixup_option = OPTJ_NONE;
            /*
             * add new fixup to the end of list
             */
            append_to_fixup_list_end( fixup );
        }
        return( RC_OK );
    }
    return( RC_ERROR );
}

#endif

bool AddFPPatchAndFixups( fp_patches patch )
/******************************************/
{
#if defined( _STANDALONE_ )
    const char  *patch_name;

    patch_name = FPPatchName[patch];
    if( MakeFpFixup( patch_name ) )
        return( RC_ERROR );
    if( patch == FPP_WAIT ) {
        AsmCodeByte( OP_NOP );
    } else {
        AsmCodeByte( OP_WAIT );
        patch_name = FPPatchAltName[patch];
        if( patch_name != NULL ) {
            return( MakeFpFixup( patch_name ) );
        }
    }
#else
    asmfixup    *fixup;

    fixup = AsmAlloc( sizeof( asmfixup ) );
    if( fixup == NULL ) {
        return( RC_ERROR );
    }
    fixup->next = FixupHead;
    FixupHead = fixup;
    fixup->external = false;
    fixup->fixup_loc = AsmCodeAddress;
    fixup->name = NULL;
    fixup->u_fppatch = patch;
    fixup->fixup_type = FIX_FPPATCH;
    fixup->fixup_option = OPTJ_NONE;
    if( patch == FPP_WAIT ) {
        AsmCodeByte( OP_NOP );
    } else {
        AsmCodeByte( OP_WAIT );
    }
#endif
    return( RC_OK );
}

#if !defined( _STANDALONE_ )
void AsmFiniRelocs( void )
/************************/
{
    asmfixup    *fixup;

    while( (fixup = FixupHead) != NULL ) {
        FixupHead = fixup->next;
        AsmFree( fixup );
    }
}
#endif
