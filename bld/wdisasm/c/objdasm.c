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


#include "disasm.h"

segment                 *Segment;
module                  *Mod;


/*
 * Static function prototypes
 */
static  void            DisAsm( void );
static  void            ObjCode( handle * );


void  DisAssemble()
/*****************/

{
    group       *grp;
    segment     *seg;

    TxtBuff = AllocMem( MAX_LINE_LEN * 3 );
    Pass = 1;
    for( seg = Mod->segments; seg != NULL; seg = seg->next_segment ) {
        if( !UseORL ) seg->data_seg = IsData( seg );
        if( seg->size != 0 || Options & FORM_ASSEMBLER ) {
            Obj2Asm( seg );
            seg->dumped = FALSE;
            seg->last_export = NULL;
            seg->last_fix    = NULL;
        }
    }
    Pass = 2;
    Mod->src_rover = Mod->src;
    InitSymList();
    EmitModule();
    if( !DO_UNIX ) {
        for( grp = Mod->groups; grp != NULL; grp = grp->next_group ) {
            EmitGroup( grp );
        }
    }
    for( seg = Mod->segments; seg != NULL; seg = seg->next_segment ) {
        if( seg->size != 0 || Options & FORM_ASSEMBLER ) {
            Obj2Asm( seg );
        }
    }
    EmitEndMod();
    FreeMem( TxtBuff );
}
