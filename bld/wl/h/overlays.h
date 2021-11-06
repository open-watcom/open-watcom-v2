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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifdef _EXE

extern seg_leader       *OvlSeg;
extern unsigned_16      OvlAreaSize;
extern overlay_ref      OvlSectNum;
extern list_of_names    *OvlClasses;

/* in ovlsupp.c */

extern void             WalkAreas( OVL_AREA *, walksecs_fn * );
extern void             ParmWalkAreas( OVL_AREA *, parmwalksecs_fn *, void * );
extern void             OvlProcPubsSect( section * );
extern void             OvlProcPubs( void );
extern void             OvlCalc( void );
extern void             FreeOverlaySupp( void );
extern void             OvlDefVector( symbol * );
extern void             OvlVectorize( symbol * );
extern void             OvlTryRefVector( symbol * );
extern void             OvlUseVector( symbol *, extnode * );
extern void             OvlIndirectCall( symbol * );
extern void             OvlGetVecAddr( int, targ_addr * );
extern bool             OvlCheckOvlClass( const char *, bool * );
extern section          *OvlCheckOvlSection( const char * );
extern void             OvlEmitVectors( void );
extern void             OvlSetStartAddr( void );
extern void             OvlPass1( void );
extern void             OvlEmitTable( void );
extern void             OvlPadOvlFiles( void );
extern void             ResetOverlaySupp( void );
extern void             OvlSetTableLoc( group_entry *, unsigned long );

/* in overlays.c */

extern void             OvlNumberSections( void );
extern void             OvlSetSegments( void );
extern void             OvlFillOutFilePtrs( void );
extern void             OvlTryDefVector( symbol * );
extern void             OvlTryUseVector( symbol *, extnode * );
extern void             OvlPass2( void );

#endif
