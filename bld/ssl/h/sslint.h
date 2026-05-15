/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  SSL internal declarations.
*
****************************************************************************/


extern void             Error( const char *, ... );
extern void             Dump( const char *, ... );
extern void             OutStartSect( const char *, unsigned );
extern void             OutByte( unsigned char );
extern void             OutWord( unsigned );
extern void             OutEndSect( void );
extern unsigned         SrcLine( void );
extern void             Decls( void );
extern void             Rules( void );
extern void             DumpSymTbl( void );
extern void             FreeSymTbl( void );
extern void             GenCode( void );
extern void             DumpGenCode( void );
extern void             FreeGenCode( void );
extern void             WantColon( void );
extern void             Scan( void );
extern symbol           *NewSym( ssl_class );
extern symbol           *Lookup( ssl_class );
extern void             NewAlias( symbol * );
extern int              GetNum( void );
extern void             WantColon( void );
extern void             Scan( void );
extern instruction      *NewLabel( void );
extern void             GenLabel( instruction *lbl );
extern void             GenExportLabel( instruction *lbl );
extern void             GenInput( int );
extern void             GenOutput( int );
extern void             GenError( int );
extern void             GenInputAny( void );
extern void             GenJump( instruction *lbl );
extern void             GenReturn( void );
extern void             GenSetParm( int );
extern void             GenSetResult( int );
extern void             GenLblCall( instruction *lbl );
extern instruction      *GenInpChoice( void );
extern instruction      *GenChoice( void );
extern void             GenSemCall( int );
extern void             GenTblLabel( instruction *ins, instruction *lbl, int );
extern void             GenKill( void );
