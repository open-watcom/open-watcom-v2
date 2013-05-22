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
#include "tree.h"
#include "addrname.h"
#include "cgmem.h"
#include "types.h"
#include "addrfold.h"
#include "makeins.h"

typedef struct {
#ifndef NDEBUG
    use_info    useinfo;
#endif
    union {
        tn              node;
        instruction     *ins;
    } u;
    bool        in_tree;        // are we hanging in a tree somewhere?
    bool        patched;        // has someone made a node out of us yet?
} patch;

extern  tn              TGPatch( patch_handle, type_def * );
extern  type_class_def  TypeClass( type_def *);
extern  name            *AllocTemp( type_class_def );
extern  name            *AllocS32Const( signed_32 );
extern  void            AddIns( instruction * );

extern  patch   *BGNewPatch() {
    patch               *p;

    p = CGAlloc( sizeof( patch ) );
    p->in_tree = FALSE;
    p->patched = FALSE;
#ifndef NDEBUG
    p->useinfo.hdltype = NO_HANDLE;
    p->useinfo.used = 0;
#endif
    return( p );
}

extern  an      TNPatch( tn node ) {
    patch               *p;
    an                  addr;
    type_class_def      tipe;

    p = (patch *)node->u.handle;
    p->in_tree = FALSE;
    tipe = TypeClass( node->tipe );
    addr = AddrName( AllocTemp( tipe ), node->tipe );
    p->u.ins = MakeMove( NULL, addr->u.name, tipe );
    p->u.ins->num_operands = 0;
    AddIns( p->u.ins );
    return( addr );
}

extern  cg_name BGPatchNode( patch *hdl, type_def *tipe ) {
    hdl->patched = TRUE;
    hdl->in_tree = TRUE;
    hdl->u.node = TGPatch( hdl, tipe );
    return( hdl->u.node );
}

extern  void    BGPatchInteger( patch *hdl, signed_32 value ) {
    tn                  node;
    name                *c;

    if( hdl->patched ) {
        c = AllocS32Const( value );
        if( hdl->in_tree ) {
            node = hdl->u.node;
            node->class = TN_CONS;
            node->u.name = c;
        } else {
            hdl->u.ins->operands[ 0 ] = c;
            hdl->u.ins->num_operands = 1;
        }
    }
}

extern  void    BGFiniPatch( patch *hdl ) {
    CGFree( hdl );
}
