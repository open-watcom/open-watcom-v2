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
#include <watcom.h>
#include "trmem.h"
#include "trmemcvr.h"
#include "asinline.h"

#define MAX_NUM_INS     256

static uint_32  buffer[MAX_NUM_INS];
static int      curLine;


enum sym_state AsmQueryExternal( char *name ) {
//*********************************************
// CC provides this

    char    str[80];
    int     ans;

    printf( "AsmQueryExternal: what do you know about '%s'?\n", name );
    printf( "1) Undefined\n2) Auto variable\n3) Static variable\n" );
    gets( str );
    ans = atoi( str );
    if( ans == 1 ) return( SYM_UNDEFINED );
    if( ans == 2 ) return( SYM_STACK );
    if( ans == 3 ) return( SYM_EXTERNAL );
    printf( "Bad input. Assumed SYM_UNDEFINED.\n" );
    return( SYM_UNDEFINED );
}

uint_32 AsmQuerySPOffsetOf( char *name ) {
//****************************************
// CC provides this

    char            str[80];
    uint_32         ans;

    printf( "AsmQuerySPOffsetOf( %s ) = ", name );
    gets( str );
    ans = atol( str );
    return( ans );
}

void AsmError( char *msg ) {
//**************************
// CC provides this

    printf( "AsmError (line %d): %s\n", curLine, msg );
}

void AsmWarning( char *msg ) {
//****************************
// CC provides this

    printf( "AsmWarning (line %d): %s\n", curLine, msg );
}

void *AsmAlloc( unsigned amount ) {
//*********************************
// CC provides this

    return( TRMemAlloc( amount ) );
}

void AsmFree( void *ptr ) {
//*************************
// CC provides this

    TRMemFree( ptr );
}

static char *typeName[] = {             // types from owl.h
    "OWL_RELOC_ABSOLUTE",               // ref to a 32-bit absolute address
    "OWL_RELOC_WORD",                   // a direct ref to a 32-bit address
    "OWL_RELOC_HALF_HI",                // ref to high half of 32-bit address
    "OWL_RELOC_HALF_HA",                // ditto adjusted for signed low 16 bits
    "OWL_RELOC_PAIR",                   // pair - used to indicate prev hi and next lo linked
    "OWL_RELOC_HALF_LO",                // ref to low half of 32-bit address
    "OWL_RELOC_BRANCH_REL",             // relative branch (Alpha: 21-bit; PPC: 14-bit)
    "OWL_RELOC_BRANCH_ABS",             // absolute branch (Alpha: not used; PPC: 14-bit)
    "OWL_RELOC_JUMP_REL",               // relative jump (Alpha: 14-bit hint; PPC: 24-bit)
    "OWL_RELOC_JUMP_ABS",               // absolute jump (Alpha: not used; PPC: 24-bit)
    "OWL_RELOC_SECTION_OFFSET",         // offset of item within it's section

    // meta relocs
    "OWL_RELOC_SECTION_INDEX",          // index of section within COFF file
    "OWL_RELOC_TOC_OFFSET",             // 16-bit offset within TOC (PPC)
    "OWL_RELOC_GLUE",                   // location of NOP for GLUE code
    "OWL_RELOC_FP_OFFSET",              // cheesy hack for inline assembler
};

void main( void ) {
//*****************

    int         ctr;
    char        in_str[80];
    asmreloc    *reloc, *next;

    TRMemOpen();

    AsmInit();
    curLine = 0;
    AsmCodeBuffer = (unsigned char*)buffer;
    AsmCodeAddress = 0;
    printf( "AsmCodeAddress = %u.\n", (unsigned)AsmCodeAddress );
    gets( in_str );
    while( strcmp( in_str, "q" ) != 0 ) {
        curLine++;
        AsmLine( in_str );
        printf( "AsmCodeAddress = %u.\n", (unsigned)AsmCodeAddress );
        // AsmCodeAddress is now updated, and code is filled from
        // AsmCodeBuffer[0] to AsmCodeBuffer[AsmCodeAddress/ins_size-1]
        // inclusive.
        gets( in_str );
    }

    printf( "Generated [before internal fixup]:\n" );
    for( ctr = 0; ctr < AsmCodeAddress / sizeof( buffer[0] ); ctr++ ) {
        printf( " [%#010x]\n", buffer[ctr] );
    }
    AsmFini();
    printf( "Generated [after internal fixup]:\n" );
    for( ctr = 0; ctr < AsmCodeAddress / sizeof( buffer[0] ); ctr++ ) {
        printf( " [%#010x]\n", buffer[ctr] );
    }

    for( reloc = AsmRelocs; reloc; reloc = next ) {
        next = reloc->next;
        printf( "Reloc:\tname = %s\n"
                "\toffset = %#010x\n"
                "\ttype = %s\n",
                reloc->name, reloc->offset, typeName[reloc->type] );
        AsmFree( reloc->name );
        AsmFree( reloc );
    }

#ifdef TRMEM
    TRMemPrtList();
#endif
    TRMemClose();
}
