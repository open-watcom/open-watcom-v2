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
* Description:  control flags table - attributes of each statement processor
*
****************************************************************************/


// Note: If you wish to add a new statement processor, there are four files
//       that need changing: PRDEFN.H, KWLIST.C, PROCTBL.C, CFTABLE.C

#include "ftnstd.h"
#include "ctrlflgs.h"
#include "global.h"

#define BD      CF_BAD_DO_ENDING
#define NS      CF_NOT_SIMPLE_STMT
#define NE      CF_NOT_EXECUTABLE
#define BB      CF_BAD_BRANCH_OBJECT
#define SL      CF_NEED_SET_LINE
#define SS      CF_SPECIFICATION
#define SP      CF_SUBPROGRAM
#define BL      CF_BLOCK_DATA
#define DB      CF_DBUG_BEFORE_LINE
#define IL      CF_IMPLICIT_LABEL
#define ST      CF_NOT_IN_STRUCTURE
#define UN      CF_NOT_IN_UNION
#define MA      CF_NOT_IN_MAP

// These control flags are used for various purposes.

// BAD_DO_ENDING     - these are statements that cannot be allowed on the
//                     terminal statement of a regular FORTRAN DO loop
// NOT_SIMPLE_STMT   - these are statements that cannot be allowed as the
//                     object of a logical IF or ATEND statement
// NOT_EXECUTABLE    - these are not executable statements and have
//                     certain restrictions outlined in the FORTRAN
//                     standard
// BAD_BRANCH_OBJECT - these are statements that would be meaningless if
//                     they were branched to
// NEED_SET_LINE     - before most statements are compiled, there needs to
//                     be a run time call emitted to set the line number
//                     currently executed
//                     WHILE and ELSEIF are special cases because they
//                     generate the run time call themselves so that the
//                     SetLine procedure will be called at the right time
// SPECIFICATION     - these statements are specification statements
//                     and must be compiled before a function/subroutine
//                     prologue is emitted.
// SUBPROGRAM        - SUBROUTINE, (typ)FUNCTION, BLKDATA, PROGRAM stmts
//                     also all type declaration statements have this bit
//                     on since typ*lenFUNCTION starts a subprogram
// BLOCK_DATA        - these statements are the only ones allowed in
//                     a BLOCK DATA subprogram
// DBUG_BEFORE_LINE  - need to call debugger ISN routine before statement
//                     processed
// IMPLICIT_LABEL    - used to flag those statements which have an
//                     implicit label (ENDIF, ENDGUESS, ENDATEND)
// NOT_IN_STRUCTURE  - used to flag those specification statements which cannot
//                     appear in a structure definition
// NOT_IN_UNION      - used to flag those specification statements which cannot
//                     appear in a union definition
// NOT_IN_MAP        - used to flag those specification statements which cannot
//                     appear in a map definition

#ifdef pick
#undef pick
#endif

#define pick(id,text,proc,flags) flags,

const unsigned_16 __FAR CFTable[] = {

#include "stmtdefn.h"

};


//
// CtrlFlgOn -- checks if a bit is on in statement control flags
//

#ifndef CtrlFlgOn

// for speed this is macro'd in ctrlflgs.h

bool    CtrlFlgOn( unsigned_16 bits ) {
//=====================================

    return( ( CtrlFlgs & bits ) != 0 );
}

#endif
