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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <malloc.h>

#include "asmglob.h"
#include "asmops1.h"
#include "asmops2.h"
#include "asmins1.h"//
#include "asmopnds.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmdefs.h"
#include "asmalloc.h"

#ifdef _WASM_
    #include "womp.h"
    #include "fixup.h"
    #include "directiv.h"

    #include "watcom.h"
    #include "objrec.h"
    #include "pcobj.h"
    #include "myassert.h"
    #include "fixup.h"
#endif

struct asmfixup         *InsFixups[3];
#ifndef _WASM_
    struct asmfixup             *FixupHead;
#endif

struct fixup            *FixupListHead; // head of list of fixups
struct fixup            *FixupListTail;
extern void             AsmByte( unsigned char );

#ifdef _WASM_
    extern int_8        PhaseError;
    extern seg_list     *CurrSeg;       // points to stack of opened segments
#endif

#ifdef _WASM_

void add_frame( void )
/********************/
/* determine the frame and frame datum for the fixup */
{
    struct asmfixup     *fixup;

    if( Parse_Pass != PASS_1 ) {
        fixup = InsFixups[Opnd_Count];
        if( fixup == NULL ) return;
        fixup->frame = Frame;
        #if 0 // fixme
        // fixme /**/myassert( Frame != EMPTY );
        if( Frame == EMPTY ) {
            AsmError( SYMBOL_NOT_DEFINED );
            return;
        }
        #endif
        fixup->frame_datum = Frame_Datum;
    }
}

#endif

struct asmfixup *AddFixup( struct asm_sym *sym, int fixup_type )
/**************************************************************/
/*
  put the correct target offset into the link list when forward reference of
  relocatable is resolved;
*/
{
    struct asmfixup     *fixup;

    switch( Code->info.token ) {
    case T_CMPS:
    case T_LODS:
    case T_MOVS:
    case T_OUTS:
    case T_INS2:
    case T_SCAS:
    case T_STOS:
    case T_XLAT:
        return( NULL );
    }

    fixup = AsmAlloc( sizeof( struct asmfixup ) );
    if( fixup != NULL ) {
        fixup->external = 0;
        fixup->name = sym->name;
#ifdef _WASM_
        fixup->offset = sym->offset;            // 20-Aug-92
        /* fixme */
        /**/myassert( fixup->offset != 0xA5A5A5A5 );
        fixup->def_seg = (CurrSeg != NULL) ? CurrSeg->seg : NULL;

        /* WHAT SHOULD REALLY BE GOING ON HERE ? */
        // fixup->frame = 0;
        // fixup->frame_datum = 0;
        fixup->frame = Frame;                   // this is just a guess
        fixup->frame_datum = Frame_Datum;
        fixup->next = sym->fixup;
        sym->fixup = fixup;
#else
        fixup->next = FixupHead;
        FixupHead = fixup;
        fixup->offset = 0;
#endif
        fixup->fix_type = fixup_type;
        InsFixups[Opnd_Count] = fixup;
    }
    return( fixup );
}

int BackPatch( struct asm_sym *sym )
/**********************************/
/*
- patching for forward reference label in Jmp instructions;
- call only when a new label appears;
*/
{
    long                disp;
    unsigned int        patch_addr;
    struct asmfixup     *patch;
    struct asmfixup     *next;
    long                max_disp;
    unsigned            size;
    #ifdef _WASM_
    dir_node            *seg;
    #endif

    #ifdef _WASM_
        patch = sym->fixup;
        sym->fixup = NULL;
    #else
        patch = FixupHead;
        FixupHead = NULL;
    #endif
    for( ; patch != NULL; patch = next ) {
        next = patch->next;
        #ifdef _WASM_
        seg = GetSeg( sym );
        if( seg == NULL || patch->def_seg != seg ) {
            /* can't backpatch if fixup location is in diff seg than symbol */
            patch->next = sym->fixup;
            sym->fixup = patch;
            continue;
        }
        #else
        if( patch->name == sym->name ) {
        #endif
        size = 0;
        switch( patch->fix_type ) {
        case FIX_RELOFF8:
            size = 1;
            /* fall through */
        case FIX_RELOFF16:
            if( size == 0 ) size = 2;
            /* fall through */
        case FIX_RELOFF32:
            if( size == 0 ) size = 4;
            patch_addr = patch->fix_loc;
            // calculate the displacement
            disp = patch->offset + Address - patch_addr - size;
            max_disp = (1UL << ((size * 8)-1)) - 1;
            if( disp > max_disp || disp < (-max_disp-1) ) {
                #ifdef _WASM_
                PhaseError = TRUE;
                switch( size ) {
                case 1:
                    if( Code->use32 ) {
                        sym->offset += 3;
                        AsmByte( 0 );
                        AsmByte( 0 );
                    } else {
                        sym->offset += 1;
                        AsmByte( 0 );
                    }
                    break;
                case 2:
                    sym->offset += 2;
                    AsmByte( 0 );
                    AsmByte( 0 );
                case 4:
                #if 0
                    AsmError( JUMP_OUT_OF_RANGE );
                    sym->fixup = NULL;
                    return( ERROR );
                #else
                    AsmWarn( 4, JUMP_OUT_OF_RANGE );
                    return( NOT_ERROR );
                #endif
                }
                #else
                    AsmError( JUMP_OUT_OF_RANGE );
                    FixupHead = NULL;
                    return( ERROR );
                #endif
            }
            // patching
            // fixme 93/02/15 - this can screw up badly if it is on pass 1
            #ifdef _WASM_
            if( Parse_Pass != PASS_1 && !PhaseError ) {
            #endif
                while( size > 0 ) {
                    CodeBuffer[patch_addr] = disp;
                    disp >>= 8;
                    patch_addr++;
                    size--;
                }
            #ifdef _WASM_
            }
            #endif
            AsmFree( patch );
            break;
        default:
            #ifdef _WASM_
                patch->next = sym->fixup;
                sym->fixup = patch;
            #else
                patch->next = FixupHead;
                FixupHead = patch;
            #endif
            break;
        }
        #ifndef _WASM_
        } else {
            patch->next = FixupHead;
            FixupHead = patch;
        }
        #endif
    }
    return( NOT_ERROR );
}

void mark_fixupp( unsigned long determinant, int index )
/******************************************************/
/*
  this routine marks the correct target offset and data record address for
  FIXUPP record;
*/
{
    struct asmfixup     *fixup;

    fixup = InsFixups[index];
    if( fixup != NULL ) {
        fixup->fix_loc = Address;
        #ifdef _WASM_
            // fixup->offset = Code->data[index];
            // Code->data[index] = 0; // fixme
            if( fixup->fix_type != FIX_SEG ) {
                Code->data[index] += fixup->offset;
            }
        /*
           20-Aug-92: put the offset in the location instead of attaching it
                      to the fixup
        */
        #else
            fixup->offset = Code->data[index];
            Code->data[index] = 0;
        #endif


        switch( determinant ) {
        case OP_I16:
        case OP_J32:
            switch( fixup->fix_type ) {
            case FIX_OFF32:
               fixup->fix_type = FIX_OFF16;
               break;
            case FIX_PTR32:
               fixup->fix_type = FIX_PTR16;
               break;
            }
            break;
        case OP_I32:
        case OP_J48:
            switch( fixup->fix_type ) {
            case FIX_OFF16:
               fixup->fix_type = FIX_OFF32;
               break;
            case FIX_PTR16:
               fixup->fix_type = FIX_PTR32;
               break;
            }
            break;
        }
    }
}

#ifdef _WASM_

struct fixup *CreateFixupRec( int index )
/* Create a fixup record for WOMP;
   Note that if Modend is TRUE, it means the fixup is the starting address
   for the module.
*/
{
    struct asmfixup     *fixup;         // fixup structure from WASM
    struct fixup        *fixnode;       // fixup structure from WOMP
    struct asm_sym      *sym;

    fixup = InsFixups[index];

    if( fixup != NULL ) {

        fixnode = FixNew();
        fixnode->self_relative = FALSE;
        fixnode->lr.target_offset = 0;
        if( !Modend ) {
            fixnode->lr.is_secondary = TRUE;
        } else {
            fixnode->lr.is_secondary = FALSE;
            fixnode->lr.target_offset = fixup->offset;
        }

        if( !Modend ) {
            switch( fixup->fix_type ) {
                case FIX_RELOFF8:
                    fixnode->self_relative = TRUE;
                    fixnode->loc_method = FIX_LO_BYTE;
                    break;
                case FIX_RELOFF16:
                    fixnode->self_relative = TRUE;
                case FIX_OFF16:
                    fixnode->loc_method = FIX_OFFSET;
                    break;
                case FIX_RELOFF32:
                    fixnode->self_relative = TRUE;
                case FIX_OFF32:
                    fixnode->loc_method = FIX_OFFSET386;
                    break;
                case FIX_SEG:
                    fixnode->loc_method = FIX_BASE;
                    break;
                case FIX_PTR16:
                    fixnode->loc_method = FIX_POINTER;
                    break;
                case FIX_PTR32:
                    fixnode->loc_method = FIX_POINTER386;
                    break;
            }
        }

        sym = AsmLookup( fixup->name );
        if( sym == NULL ) return NULL;

        fixnode->loader_resolved = FALSE;

        fixnode->loc_offset = Address - GetCurrSegStart();

        /*------------------------------------*/
        /* Determine the Target and the Frame */
        /*------------------------------------*/

        if( sym->state == SYM_UNDEFINED ) {
            AsmErr( SYMBOL_S_NOT_DEFINED, sym->name );
            return( NULL );
        } else if( sym->state == SYM_GRP ) {

            if( Modend ) {
                AsmError( INVALID_START_ADDRESS );
                return NULL;
            }

            fixnode->lr.frame = F_GRP;
            fixnode->lr.target = T_GRP;
            fixnode->lr.frame_datum =
            fixnode->lr.target_datum = GetDirIdx( fixup->name, TAB_GRP );

        } else if( sym->state == SYM_SEG ) {

            if( Modend ) {
                AsmError( INVALID_START_ADDRESS );
                return NULL;
            }

            fixnode->lr.frame = F_SEG;
            fixnode->lr.target = T_SEG;
            fixnode->lr.frame_datum =
            fixnode->lr.target_datum = GetDirIdx( fixup->name, TAB_SEG );

        } else {

            /* symbol is a label */

            if( sym->state == SYM_EXTERNAL ) {
                if( Modend ) {
                    if( sym->mem_type == T_BYTE ||
                        sym->mem_type == T_SBYTE ||
                        sym->mem_type == T_WORD ||
                        sym->mem_type == T_SWORD ||
                        sym->mem_type == T_DWORD ||
                        sym->mem_type == T_SDWORD ||
                        sym->mem_type == T_FWORD ||
                        sym->mem_type == T_PWORD ||
                        sym->mem_type == T_QWORD ||
                        sym->mem_type == T_TBYTE ||
                        sym->mem_type == T_ABS2 ) {

                        AsmError( MUST_BE_ASSOCIATED_WITH_CODE );
                        return NULL;
                    }
                    fixnode->lr.target = T_EXTWD;
                } else {
                    fixnode->lr.target = T_EXT;
                }
                fixnode->lr.target_datum = GetDirIdx( fixup->name, TAB_EXT );

                if( fixup->frame == F_GRP && fixup->frame_datum == 0 ) {
                    /* set the frame to the frame of the corresponding segment */
                    fixup->frame_datum = GetGrpIdx( sym );
                }
            } else {
                /**/myassert( sym->segidx != 0 );
                if( Modend ) {
                    fixnode->lr.target = T_SEGWD;
                    fixup->frame = F_TARG;
                } else {
                    fixnode->lr.target = T_SEG;
                }
                fixnode->lr.target_datum = sym->segidx;
            }

            /* HMMM .... what if fixup->frame is say -2 .... ie empty?
             * what should really be going on here?
             */
            // fixnode->lr.frame = (uint_8)fixup->frame;
            // fixnode->lr.frame_datum = fixup->frame_datum;

            if( fixup->frame != EMPTY ) {
                fixnode->lr.frame = (uint_8)fixup->frame;
            } else {
                fixnode->lr.frame = F_TARG;
            }
            fixnode->lr.frame_datum = fixup->frame_datum;


            if( Modend ) return fixnode;
        }

        /*--------------------*/
        /* Optimize the fixup */
        /*--------------------*/

        if( fixnode->lr.frame == ( fixnode->lr.target - 4 ) ) {
            fixnode->lr.frame = F_TARG;
        }

        fixnode->next = NULL;

        return fixnode;
    }
    return( NULL );
}

int store_fixup( int index )
/**************************/
/* Store the fixup information in a WOMP fixup record */
{
    struct fixup        *fixnode;

    if( Parse_Pass == PASS_1 ) return( NOT_ERROR );
    if( InsFixups[index] == NULL ) return( NOT_ERROR );
    fixnode = CreateFixupRec( index );
    if( fixnode == NULL ) return( ERROR );

    if( FixupListHead == NULL ) {
        FixupListTail = FixupListHead = fixnode;
    } else {
        FixupListTail->next = fixnode;
        FixupListTail = fixnode;
    }
    return( NOT_ERROR );
}

int MakeFpFixup( struct asm_sym *sym )
/*************************************/
{
    int old_count;
    int_8 old_frame;

    old_count = Opnd_Count;
    old_frame = Frame;
    Opnd_Count = 2;
    Frame = F_LOC;
    AddFixup( sym, FIX_OFF16 );
    Frame = old_frame;
    Opnd_Count = old_count;
    if( store_fixup( 2 ) == ERROR ) return( ERROR ); // extra entry in insfixups
    return( NOT_ERROR );
}

#endif
