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
* Description:  fixup related variables and routines
*
****************************************************************************/

#ifndef ASMFIXUP_H
#define ASMFIXUP_H

extern struct asmfixup  *InsFixups[OPND_MAX];
extern struct asmfixup  *FixupHead;
extern struct asmfixup  *AddFixup( struct asm_sym *sym, enum fixup_types fixup_type, enum fixup_options fixup_option );
extern void             add_frame( void );
extern bool             BackPatch( struct asm_sym *sym );
extern void             mark_fixupp( OPNDTYPE determinant, operand_idx index );
extern bool             store_fixup( operand_idx index );

extern struct asmfixup  *FixupListHead; // head of list of fixups
extern struct asmfixup  *FixupListTail;

#endif
