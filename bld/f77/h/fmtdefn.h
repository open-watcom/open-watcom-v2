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

#include "fmtdef.h"
#include "fmttab.h"     /* For struct FmtElements */

gbl_defn char           PGM *Fmt_start;   // pointer to start of format string
gbl_defn char           PGM *Fmt_charptr; // pointer to current format character
gbl_defn char           PGM *Fmt_end;     // pointer to end of format string
gbl_defn int            Fmt_paren_level;  // parenthesis count
gbl_defn fmt_ptr        Fmt_revert;       // position to revert to if required
gbl_defn int            Fmt_rep_spec;     // repeat specification count

gbl_defn const FmtElements * FmtEmStruct;
                                          // pointer to run-time or compile-time
                                          // struct of format code generation
                                          // routines

gbl_defn byte           Fmt_delimited;    // has format code been delimite
