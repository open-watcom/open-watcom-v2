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
* Description:  SSL internal declarations.
*
****************************************************************************/


#ifndef __WATCOMC__
    #include "clibext.h"
#endif

extern void             Error( char *, ... );
extern void             Dump( char *, ... );
extern void             OutStartSect( char *, unsigned short );
extern void             OutByte( unsigned char );
extern void             OutWord( unsigned short );
extern void             OutEndSect( void );
extern unsigned short   SrcLine( void );
extern void             Decls(void);
extern void             Rules(void);
extern void             DumpSymTbl(void);
extern void             GenCode(void);
extern void             DumpGenCode(void);
extern void             WantColon(void);
extern void             Scan(void);
extern symbol           *NewSym( class );
extern symbol           *Lookup( class );
extern void             NewAlias( symbol * );
extern unsigned short   GetNum(void);
extern void             Error( char *, ... );
extern void             WantColon(void);
extern void             Scan(void);
extern void             GenCode(void);
extern void             GenLabel( instruction * );
extern void             GenExportLabel( instruction * );
extern void             GenInput( unsigned );
extern void             GenOutput( unsigned );
extern void             GenError( unsigned );
extern void             GenInputAny(void);
extern void             GenJump( instruction * );
extern void             GenReturn(void);
extern void             GenSetResult( unsigned );
extern void             GenLblCall( instruction * );
extern instruction      *GenInpChoice(void);
extern instruction      *GenChoice(void);
extern instruction      *GenNewLbl( void );
extern void             GenSetParm( unsigned );
extern void             GenSemCall( unsigned );
extern void             GenTblLabel( instruction *, instruction *, unsigned );
extern void             GenKill(void);

