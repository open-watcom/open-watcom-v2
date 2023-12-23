/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "bool.h"
#include "watcom.h"
#include "trmemcvr.h"
#include "asinline.h"
#include "asalloc.h"


#define MAX_NUM_INS     256

static uint_32  buffer[MAX_NUM_INS];
static int      curLine;

static char     *typeName[] = {         // OWL_RELOCS macro from owl.h
    #define OWL_RELOC(e) #e,
    OWL_RELOCS
    #undef OWL_RELOC
};

void *AsmQuerySymbol( const char *name ) {
//****************************************
// CC provides this

    return( (void *)name );
}

enum sym_state AsmQueryState( void *name ) {
//******************************************
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

uint_32 AsmQuerySPOffsetOf( void *name ) {
//****************************************
// CC provides this

    char            str[80];
    uint_32         ans;

    printf( "AsmQuerySPOffsetOf( %s ) = ", name );
    gets( str );
    ans = atol( str );
    return( ans );
}

void AsmError( const char *msg )
//******************************
// CC provides this
{
    printf( "AsmError (line %d): %s\n", curLine, msg );
}

void AsmWarning( const char *msg )
//********************************
// CC provides this
{
    printf( "AsmWarning (line %d): %s\n", curLine, msg );
}

void *AsmAlloc( size_t amount ) {
//*******************************
// CC provides this

    return( TRMemAlloc( amount ) );
}

void AsmFree( void *ptr ) {
//*************************
// CC provides this

    TRMemFree( ptr );
}

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
