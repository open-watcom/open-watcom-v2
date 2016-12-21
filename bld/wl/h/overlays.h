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


typedef void walksecs_fn( section * );
typedef void parmwalksecs_fn( section *, void * );

extern unsigned_16  AreaSize;

/* in ovlsupp.c */

extern void             WalkAreas( OVL_AREA *, walksecs_fn * );
extern void             ParmWalkAreas( OVL_AREA *, parmwalksecs_fn *, void * );
extern void             ProcOvlSectPubs( section * );
extern void             ProcOvlPubs( void );
extern void             FillOutPtr( section * );
extern void             CalcOvl( void );
extern void             FreeOvlStruct( void );
extern void             OvlDefVector( symbol * );
extern void             Vectorize( symbol * );
extern void             TryRefVector( symbol * );
extern void             OvlUseVector( symbol *, extnode * );
extern void             IndirectCall( symbol * );
extern void             GetVecAddr( int, targ_addr * );
extern bool             CheckOvlClass( char *, bool * );
extern section          *CheckOvlSect( char * );
extern void             EmitOvlVectors( void );
extern void             SetOvlStartAddr( void );
extern void             OvlPass1( void );
extern void             EmitOvlTable( void );
extern void             PadOvlFiles( void );
extern void             ResetOvlSupp( void );
extern void             SetOvlTableLoc( group_entry *, unsigned long );

/* in overlays.c */

extern void             WalkAllOvl( walksecs_fn * );
extern void             ParmWalkAllOvl( parmwalksecs_fn *, void * );
extern void             WalkAllSects( walksecs_fn * );
extern void             ParmWalkAllSects( parmwalksecs_fn *, void * );
extern void             NumberSections( void );
extern void             FillOutFilePtrs( void );
extern void             TryDefVector( symbol * );
extern void             TryUseVector( symbol *, extnode * );
extern void             OvlPass2( void );
extern virt_mem         GetLoadObjTarget( segdata * );
