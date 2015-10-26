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
* Description:  jump table of statement processor routines
*
****************************************************************************/


#include "ftnstd.h"

// Note: If you wish to add a new statement processor, there are four files
//       that need changing: PRDEFN.H, KWLIST.C, PROCTBL.C, CFTABLE.C

extern  void            BadStmt( void );
extern  void            CpBackSp( void );
extern  void            CpClose( void );
extern  void            CpEndfile( void );
extern  void            CpInquire( void );
extern  void            CpOpen( void );
extern  void            CpPrint( void );
extern  void            CpRead( void );
extern  void            CpRewind( void );
extern  void            CpWrite( void );
extern  void            CpAdmit( void );
extern  void            CpAtEnd( void );
extern  void            CpCase( void );
extern  void            CpSelect( void );
extern  void            CpElse( void );
extern  void            CpElseIf( void );
extern  void            CpEndAtEnd( void );
extern  void            CpEndBlock( void );
extern  void            CpEndSelect( void );
extern  void            CpDo( void );
extern  void            CpEnd( void );
extern  void            CpEndDo( void );
extern  void            CpEndGuess( void );
extern  void            CpEndif( void );
extern  void            CpEndLoop( void );
extern  void            CpEndWhile( void );
extern  void            CpGuess( void );
extern  void            CpLogIf( void );
extern  void            CpOtherwise( void );
extern  void            CpLoop( void );
extern  void            CpQuit( void );
extern  void            CpRemBlock( void );
extern  void            CpUntil( void );
extern  void            CpWhile( void );
extern  void            CpAsgnmt( void );
extern  void            CpStmtFunc( void );
extern  void            CpCall( void );
extern  void            CpContinue( void );
extern  void            CpGoto( void );
extern  void            CpArithIf( void );
extern  void            CpAssign( void );
extern  void            CpPause( void );
extern  void            CpStop( void );
extern  void            CpFormat( void );
extern  void            CpSubroutine( void );
extern  void            CpFunction( void );
extern  void            CpProgram( void );
extern  void            CpReturn( void );
extern  void            CpEntry( void );
extern  void            CpIntVar( void );
extern  void            CpRealVar( void );
extern  void            CpCmplxVar( void );
extern  void            CpDCmplxVar( void );
extern  void            CpXCmplxVar( void );
extern  void            CpDbleVar( void );
extern  void            CpXDbleVar( void );
extern  void            CpLogVar( void );
extern  void            CpCharVar( void );
extern  void            CpDimension( void );
extern  void            CpImplicit( void );
extern  void            CpParameter( void );
extern  void            CpCommon( void );
extern  void            CpEquivalence( void );
extern  void            CpExecute( void );
extern  void            CpData( void );
extern  void            CpSave( void );
extern  void            CpBlockData( void );
extern  void            CpExternal( void );
extern  void            CpIntrinsic( void );
extern  void            CpInclude( void );
extern  void            CpDoWhile( void );
extern  void            CpExit( void );
extern  void            CpCycle( void );
extern  void            CpStructure( void );
extern  void            CpEndStructure( void );
extern  void            CpUnion( void );
extern  void            CpEndUnion( void );
extern  void            CpMap( void );
extern  void            CpEndMap( void );
extern  void            CpRecord( void );
extern  void            CpNameList( void );
extern  void            CpAllocate( void );
extern  void            CpDeAllocate( void );
extern  void            CpVolatile( void );

void    (* const __FAR ProcTable[])( void ) = {
    #define pick(id,text,proc,flags) proc,
    #include "stmtdefn.h"
    #undef pick
};
