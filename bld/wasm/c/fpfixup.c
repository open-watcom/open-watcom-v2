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

#include "asmglob.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmins1.h"
#include "asmsym.h"
#include "directiv.h"
#include "myassert.h"

extern struct asm_code          *Code;

extern int MakeFpFixup( struct asm_sym *sym );
extern char *AsmMangler( struct asm_sym *sym, char *buffer );

typedef enum {
    FPP_NONE,
    FPP_WAIT,
    FPP_NORMAL,
    FPP_ES,
    FPP_CS,
    FPP_SS,
    FPP_DS,
    FPP_FS,
    FPP_GS,
    FPP_NUMBER_OF_TYPES
} fp_patches;

static char *FPPatchName[] = {
    NULL,
    "FIWRQQ",
    "FIDRQQ",
    "FIERQQ",
    "FICRQQ",
    "FISRQQ",
    "FIARQQ",
    "FIFRQQ",
    "FIGRQQ"
};

static char *FPPatchAltName[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    "FJCRQQ",
    "FJSRQQ",
    "FJARQQ",
    "FJFRQQ",
    "FJGRQQ"
};

int AddFloatingPointEmulationFixup( const struct asm_ins ASMFAR *ins, bool secondary )
/************************************************************************************/
{
    fp_patches patch;
    struct asm_sym *sym;
    char **patch_name_array;

    patch_name_array = ( secondary ? FPPatchAltName : FPPatchName );

    if( ins->token == T_FWAIT ) {
        patch = FPP_WAIT;
    } else {
        switch( Code->seg_prefix ) {
        case EMPTY:
            patch = FPP_NORMAL;
            break;
        case PREFIX_ES:
            patch = FPP_ES;
            break;
        case PREFIX_CS:
            patch = FPP_CS;
            break;
        case PREFIX_SS:
            patch = FPP_SS;
            break;
        case PREFIX_DS:
            patch = FPP_DS;
            break;
        case PREFIX_FS:
            patch = FPP_FS;
            break;
        case PREFIX_GS:
            patch = FPP_GS;
            break;
        default:
            never_reach();
        }
    }

    /* put out an extern def for the the patch */
    if( patch_name_array[patch] == NULL ) return( NOT_ERROR );
    sym = AsmGetSymbol( patch_name_array[patch] );
    if( sym == NULL ) {
        sym = MakeExtern( patch_name_array[patch], T_FAR, FALSE );
        sym->mangler = AsmMangler;
    }
    if( MakeFpFixup( sym ) == ERROR ) return( ERROR );

    return( NOT_ERROR );
}
