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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cgstd.h"
#include "coderep.h"
#include "offset.h"
#include "typedef.h"
#include "s37bead.h"

#include "s37data.def"

extern void EncLabel(label_handle, char  );
extern  seg_id          SetOP(seg_id);
extern void HWDataGen(int ,int ,byte *);
extern void HWExtern(hw_sym *);
extern bead_addr *HWSymAddr(hw_sym *,offset ,bool );
extern  sym_handle AskForLblSym(label_handle);
extern  hw_sym  *AskForHWLabel(label_handle);
extern char *FEName(sym_handle);
extern  fe_attr  FEAttr(pointer);
extern  bck_info *FEBack(pointer);
extern void HWAddrGen(char *,offset );

extern  void    DataBytes( unsigned_32 len, byte *src ) {
/*******************************************************/
    HWDataGen( len, 1, src );
}


extern  void    IterBytes( offset len, byte pat ) {
/***********************************************/

    HWDataGen( 1, len, &pat );
}


extern  void    FEPtr( sym_handle sym, type_def *tipe, offset plus ) {
/*******************************************************************/

    bck_info    *bck;
    hw_sym      *hwlabel;

    if( tipe->length != WORD_SIZE ) {
        /* can't handle yet */
    }
    bck = FEBack( sym );
    hwlabel = AskForHWLabel( bck->lbl );
    if( FEAttr( sym ) & FE_IMPORT  ){
        HWExtern( hwlabel );
    }
    HWSymAddr( hwlabel, plus, false );
}


extern  void    BackPtr( bck_info *bck, seg_id seg,
                         offset plus, type_def *tipe ) {
/*****************************************************/
/* drop address of back-end symbol+offset into seg seg */
    hw_sym      *hwlabel;

    seg = 0; /* maybe I can shine it up for an ashtray or something */
    if( tipe->length != WORD_SIZE ) {
        /* can't handle yet */
    }
    hwlabel = AskForHWLabel( bck->lbl );
    HWSymAddr( hwlabel, plus, false );
}

extern  void    DataLabel( label_handle lbl ) {
/*********************************************/
/* make a data label at current seg */
    EncLabel( lbl, 1 );
}
