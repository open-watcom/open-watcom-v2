/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  overlay support routines which can't be put in an overlay
*
****************************************************************************/


#include "linkstd.h"
#include "alloc.h"
#include "newmem.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "dbgwat.h"
#include "objpass2.h"
#include "objcalc.h"
#include "distrib.h"
#include "overlays.h"
#include "distrib.h"


#ifdef _EXE

static void OvlNumASect( section *sect )
/**************************************/
{
    sect->ovlref = OvlSectNum++;
}

void OvlNumberSections( void )
/****************************/
{
    if( FmtData.u.dos.distribute ) {
        DistribNumberSections();
    }
    /* OvlSectNum value 0 is reserved for Root */
    /* Overlayed sections start at 1 */
    OvlSectNum = 1;
    WalkAreas( Root->areas, OvlNumASect );
}

void OvlSetSegments( void )
/*************************/
{
    DistribSetSegments();
}

static void FillOutPtr( section *sec )
/************************************/
{
    if( sec->outfile == NULL ) {
        if( sec->parent != NULL ) {
            sec->outfile = sec->parent->outfile;  //same file as ancestor.
        } else {
            sec->outfile = Root->outfile;
        }
    }
}

void OvlFillOutFilePtrs( void )
/*****************************/
{
    WalkAreas( Root->areas, FillOutPtr );
}

void OvlTryDefVector( symbol *sym )
/*********************************/
{
    if( sym->info & SYM_DISTRIB ) {
        DefDistribSym( sym );
    } else {
        OvlDefVector( sym );
    }
}

void OvlTryUseVector( symbol *sym, extnode *newnode )
/***************************************************/
{
    if( newnode != NULL ) {
        newnode->ovlref = 0;
        OvlUseVector( sym, newnode );
    }
}

static void PSection( section *sec )
/**********************************/
{
    CurrSect = sec;
    PModList( sec->mods );
}

void OvlPass2( void )
/*******************/
{
    OvlEmitVectors();
    WalkAreas( Root->areas, PSection );
}

#endif
