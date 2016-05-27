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


#include <stdlib.h>
#include "cgstd.h"
#include "coderep.h"
#include "offset.h"
#include "zoiks.h"
#include "cgmem.h"
#include "s37bead.h"

extern hw_sym          *HWSymHandle(void );
extern hw_sym          *HWFRefSym( char *);
extern  char           *FEName(sym_handle);
extern  fe_attr         FEAttr(pointer);
extern  char           *AskRTName(int);

typedef struct label_struct {
    struct label_struct *link;
    sym_handle          sym;
    hw_sym             *hwlabel;
    hw_loc              hwbase;
    offset              address;
    unsigned            runtime         : 1;
    unsigned            reached         : 1;
    unsigned            condemned       : 1;
    unsigned            keep            : 1;
} label_struct;

#include "s37lbl.def"

static label_struct     *Handles = NULL;


extern  bool    ValidLbl( label_struct *lbl ) {
/*****************************************/

    label_struct        *chk;

    chk = Handles;
    for(;;) {
        if( chk == NULL ) break;
        if( chk == lbl ) return( true );
        chk = chk->link;
    }
    _Zoiks( ZOIKS_042 );
    return( false );
}


extern  label_handle    AskForLabel( sym_handle sym ) {
/*****************************************************/

    label_struct        *new;

    new = CGAlloc( sizeof( label_struct ) );
    new->link    = Handles;
    Handles = new;
    new->sym     = sym;
    new->hwlabel = NULL;
    new->hwbase.sym = NULL;
    new->hwbase.disp = 0;
    new->runtime = false;
    new->reached = false;
    new->condemned = false;
    new->keep = false;
    return( new );
}


extern  label_handle    AskForNewLabel() {
/****************************************/

    return( AskForLabel( NULL ) );
}


extern  label_handle    AskRTLabel( sym_handle *sym ) {
/****************************************/

    label_struct        *new;

    new = AskForLabel( sym );
    new->runtime = true;
    return( new );
}


extern  bool    TellReachedLabel( label_struct *lbl ) {
/******************************************************/

    ValidLbl( lbl );
    return( false );
}


extern  bool    AskIfReachedLabel( label_struct *lbl ) {
/******************************************************/

    ValidLbl( lbl );
    return( false );
}


extern  bool    AskIfRTLabel( label_struct *lbl ) {
/**********************************************/

    ValidLbl( lbl );
    return( lbl->runtime );
}


extern  void    TellAddress( label_struct *lbl, offset addr ) {
/**********************************************************/

    ValidLbl( lbl );
    lbl->address = addr;
}


extern  offset  AskAddress( label_struct *lbl ) {
/********************************************/
    hw_sym *hwlabel;
    offset ret;

    ValidLbl( lbl );
    hwlabel = lbl->hwlabel;
    if( hwlabel != NULL && hwlabel->class == HW_FIXED ){
        ret = hwlabel->def->address;
    }else{
        ret = -1;
    }
    return( ret );
}

extern void TellLblBase( label_struct *lbl, label_struct *base, offset disp ) {
/*******************************************************
 Set lbl's base with base+disp;
*/
    lbl->hwbase.sym = base->hwlabel;
    lbl->hwbase.disp = disp;
}

extern  hw_loc *AskForBaseHW(label_struct *lbl ) {
/******************************************************/

    ValidLbl( lbl );
    return( &lbl->hwbase );
}

extern  sym_handle      AskForLblSym( label_struct *lbl ) {
/******************************************************/

    ValidLbl( lbl );
    return( lbl->sym );
}

extern  hw_sym *AskForHWLabel( label_struct *lbl ) {
/******************************************************/

    ValidLbl( lbl );
    if( lbl->hwlabel == NULL ){
        AddHWLabel( lbl );
    }
    return( lbl->hwlabel );
}

extern  void    TellKeepLabel( label_struct *lbl ) {
/***********************************************/

    ValidLbl( lbl );
    lbl->keep = true;
}


extern  void    TellNoSymbol( label_struct *lbl ) {
/**********************************************/

    if( lbl != NULL ) {
        ValidLbl( lbl );
        lbl->sym = NULL;
    }
}


extern  void    TellScrapLabel( label_struct *lbl ) {
/************************************************/

    label_struct   **owner;

    owner = &Handles;
    for(;;) {
        if( *owner == NULL ) return;
        if( *owner == lbl ) break;
        owner = &(*owner)->link;
    }
    *owner = lbl->link;
    CGFree( lbl );
}


static  label_struct       *NextCondemned( label_struct *lbl ) {
/**************************************************************/

    for(;;) {
        if( lbl == NULL ) break;
        if( lbl->condemned ) {
            lbl->condemned = false;     /* this is not a reprieve */
            break;
        }
        lbl = lbl->link;
    }
    return( lbl );
}


extern  void    TellCondemnedLabel( label_struct *lbl ) {
/****************************************************/

    ValidLbl( lbl );
    lbl->condemned = true;
}


extern  void    TellBeginExecutions() {
/*************************************/

    label_struct   *dead;

    for(;;) {
        dead = NextCondemned( Handles );
        if( dead == NULL ) break;
    }
}


extern  void    TellFreeAllLabels() {
/***********************************/

    bool        unfreed;

    unfreed = false;
    while( Handles != NULL ) {
        TellScrapLabel( Handles );
    }
}

static  void    Copy8( char *nstr, char *name ) {
/***********************************************/

    char        *curr;

    for( curr = name; curr < &name[8]; curr++,nstr++ ) {
        if( *nstr == '\0' ) break;
        *curr = *nstr;
    }
    *curr = '\0';
}

static void AddHWLabel( label_struct  *lbl ){
/*** Make a hwsym for label add external name **/
    char      buff[8+1];
    fe_attr   attr;
    hw_sym    *hwlabel;

    sym_handle sym;
    sym = lbl->sym;
    if( lbl->runtime ){
        Copy8( AskRTName( (int)sym ), buff );
        hwlabel = HWFRefSym( buff );
    } else if( sym != NULL ) {
        attr = FEAttr( sym );
        if( attr & ( FE_IMPORT | FE_GLOBAL ) ) { /* 90-05-22 */
            Copy8( FEName( sym ), buff );
            hwlabel = HWFRefSym( buff );
            hwlabel->class = attr & FE_IMPORT ? HW_EXTERN : HW_ENTRY;
        } else { /* local sym */
            hwlabel = HWSymHandle();
        }
    } else { /* unknown sym like fref of label */
        hwlabel = HWSymHandle();
    }
    lbl->hwlabel = hwlabel;
}
