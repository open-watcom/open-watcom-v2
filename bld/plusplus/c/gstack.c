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


#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "plusplus.h"
#include "errdefns.h"
#include "memmgr.h"
#include "carve.h"
#include "stack.h"
#include "gstack.h"
#include "initdefs.h"

#define BLOCK_GLOBAL_STACK      16
static carve_t carveGLOBAL_STACK;

static void gstackInit( INITFINI* defn )
{
    defn = defn;
    carveGLOBAL_STACK = CarveCreate( sizeof(GLOBAL_STACK), BLOCK_GLOBAL_STACK );
}

static void gstackFini( INITFINI* defn )
{
    defn = defn;
#ifndef NDEBUG
    CarveVerifyAllGone( carveGLOBAL_STACK, "GLOBAL_STACK" );
#endif
    CarveDestroy( carveGLOBAL_STACK );
}

INITDEFN( global_stack, gstackInit, gstackFini )

gstack_type GStackType( GLOBAL_STACK *top )
/*****************************************/
{
    return( top->id );
}

GLOBAL_STACK *GStackPush( GLOBAL_STACK **head, gstack_type id )
/*************************************************************/
{
    GLOBAL_STACK *top;

    top = StackCarveAlloc( carveGLOBAL_STACK, head );
    top->id = id;
    return top;
}

void GStackPop( GLOBAL_STACK **head )
/***********************************/
{
    GLOBAL_STACK *top;

    top = StackPop( head );
    switch( top->id ) {
    case GS_DECL_SPEC:
        PTypePop( top->u.dspec );
        break;
    case GS_CLASS_DATA:
        ClassPop( &(top->u.classdata) );
        break;
    case GS_INIT_DATA:
        DataInitFinish( &(top->u.initdata) );
        break;
    case GS_TEMPLATE_DATA:
        TemplateDeclFini();
        break;
    }
    CarveFree( carveGLOBAL_STACK, top );
}
