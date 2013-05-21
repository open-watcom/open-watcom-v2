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


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "regset.h"
#include "addrname.h"


extern code_lbl *GetWeirdPPCDotDotLabel( code_lbl *lbl ) {
/********************************************************/

    code_lbl            *new;

    if( _TstStatus( lbl, HAS_PPC_ALIAS ) ) {
        return( lbl->ppc_alt_name );
    }
    if( AskIfRTLabel( lbl ) ) {
        new = AskRTLabel( lbl->lbl.sym );
    } else {
        new = AskForLabel( lbl->lbl.sym );
    }
    new->ppc_alt_name = lbl;
    lbl->ppc_alt_name = new;
    _SetStatus( lbl, HAS_PPC_ALIAS );
    _SetStatus( new, WEIRD_PPC_ALIAS );
    return( new );
}
