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


//
// OPRTAB       : jump table for operator generator routines
//

#include "ftnstd.h"
#include "symtypes.h"
#include "optr.h"
#include "opr.h"

extern  void            LogOp(TYPE,TYPE,OPTR);
extern  void            AsgnOp(TYPE,TYPE,OPTR);
extern  void            RelOp(TYPE,TYPE,OPTR);
extern  void            BinOp(TYPE,TYPE,OPTR);
extern  void            ExpOp(TYPE,TYPE,OPTR);
extern  void            FieldOp(TYPE,TYPE,OPTR);

#ifdef pick
#undef pick
#endif
#define pick(id,const,gener) gener,

void    (* const __FAR GenOprTable[])(TYPE,TYPE,OPTR) = {
#include "optrdefn.h"
};

