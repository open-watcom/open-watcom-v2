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


#ifndef OWLENTRY
#include "owl.h"
#endif

typedef struct asmreloc {
    struct asmreloc *next;
    char            *name;
    owl_offset      offset;
    owl_reloc_type  type;
} asmreloc;

enum sym_state {
    SYM_UNDEFINED,
    SYM_EXTERNAL,
    SYM_STACK,
};

extern uint_32          *AsmCodeBuffer; // Caller provides the buffer
extern uint_32          AsmCodeAddress; // Indicates the next address
extern asmreloc         *AsmRelocs;     // A list of relocs to be handled
                                        // by the caller

extern void AsmInit( void );    // Call before using the assembler
extern int  AsmLine( char * );  // return 1 if parsed  ok
extern void AsmFini( void );    // Call after giving all the input and
                                // before using the AsmCodeBuffer

/*
   The following function is supplied by the user of the mini-assembler.
   It returns either:
        SYM_UNDEFINED   - the name is not in the user's symbol table
        SYM_EXTERNAL    - the name is a static symbol in the user's
                                symbol table
        SYM_STACK       - the symbol is an auto symbol in the user's
                                symbol table
*/
extern enum sym_state   AsmQueryExternal( char *name );

// This function finds out how far away is the auto variable from $sp
extern uint_32          AsmQuerySPOffsetOf( char *name );
