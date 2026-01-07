/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef OMFFIXUP_H
#define OMFFIXUP_H

#include "omfrec.h"


extern void FixInit( void );
extern void FixFini( void );
/*
    FixInit must be called before any other routines in this modules.
    FixFini free's any memory used by this module, along with all the
    outstanding fixups.
*/

extern fixuprec *FixNew( void );
/*
    Allocate memory for a new fixup.
*/


extern fixuprec *FixDup( const fixuprec *fix );
/*
    Allocate a new fixup, and copy fix into it.
*/


extern void FixKill( fixuprec *fix );
/*
    Free the memory used by fix.
*/


/********** the following are used for parsing fixups *************/

extern fixinfo *FixBegin( void );
/*
    Begin parsing fixups.  The structure returned contains state
    information to deal with threads.
*/

extern fixuprec *FixGetFix( fixinfo *info, obj_rec_handle objr );
/*
    Read a fixup.
*/


extern void FixGetLRef( fixinfo *info, obj_rec_handle objr, logref *log );
/*
    Read a logical reference.
*/


extern void FixGetPRef( fixinfo *info, obj_rec_handle objr, physref *phys );
/*
    Read a physical reference.
*/


extern void FixGetRef( fixinfo *info, obj_rec_handle objr, logphys *lp, int is_logical );
/*
    Read a logical or a physical reference depending on is_logical.
*/


extern void FixEnd( fixinfo *info );
/*
    Called to free the memory allocated by FixBegin.
*/


/********** the following are used for generating fixups *************/

extern uint_16 FixGenFix( fixuprec *fix, uint_8 *buf, int type );
/*
    Create the binary representation of fix, for the OMF described by
    fixgen_type type, into buf.  Returns the number of bytes written.
    buf must be at least FIX_GEN_MAX bytes large.
*/


extern uint_16 FixGenLRef( logref *log, uint_8 *buf, int type );
extern uint_16 FixGenPRef( physref *phys, uint_8 *buf, int type );
extern uint_16 FixGenRef( logphys *lp, int is_logical, uint_8 *buf, int type );
/*
    Generate references.  See FixGenFix comment for details.
*/


#endif
