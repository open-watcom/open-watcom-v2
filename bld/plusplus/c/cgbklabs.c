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


#include "plusplus.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "vstk.h"


void CgLabel(                   // EMIT A LABEL
    label_handle lab )          // - the label
{
    CGControl( O_LABEL, NULL, lab );
}


void CgLabelsPop(               // POP A LABELS STACK
    VSTK_CTL *stack,            // - the stack
    unsigned count )            // - number of labels to pop
{
    label_handle *a_lab;        // - addr[ current label ]
    label_handle lab;           // - current label

    for( ; count > 0; --count ) {
        a_lab = VstkTop( stack );
        lab = *a_lab;
        VstkPop( stack );
        BEFiniLabel( lab );
    }
}


void CgLabelsFinish(            // FINISH LABELS IN A VIRTUAL STACK
    VSTK_CTL *stack,            // - the stack
    int base )                  // - base for stack
{
    label_handle *cur;          // - current label handle
    label_handle *bound;        // - bounding label handle

    bound = VstkBase( stack, base );
    VstkIterBeg( stack, cur ) {
        if( cur == bound )
            break;
        BEFiniLabel( *cur );
        VstkPop( stack );
    }
}


void CgLabelPlantReturn(        // PLANT RETURN LABEL, IF REQUIRED
    FN_CTL* fctl )              // - function being emitted
{
    label_handle lab;           // - return label

    lab = fctl->return_label;
    if( lab != UNDEFINED_LABEL ) {
        CgLabel( lab );
        BEFiniLabel( lab );
        fctl->return_label = UNDEFINED_LABEL;
    }
}


void CgGotoReturnLabel(         // GENERATE "GOTO RETURN-LABEL"
    FN_CTL* fctl )              // - function being emitted
{
    if( fctl->return_label == UNDEFINED_LABEL ) {
        fctl->return_label = BENewLabel();
    }
    CGControl( O_GOTO, NULL, fctl->return_label );
}
