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
* Description:  Prototypes for command line parsing specific to DOS output.
*
****************************************************************************/


extern bool     ProcOverlay( void );
extern bool     ProcBegin( void );
extern bool     ProcFixedLib( void );
extern bool     ProcNoVector( void );
extern bool     ProcVector( void );
extern bool     ProcForceVector( void );
extern bool     ProcSmall( void );
extern bool     ProcDistribute( void );
extern bool     ProcDynamic( void );
extern bool     ProcStandard( void );
extern bool     ProcNoIndirect( void );
extern bool     ProcArea( void );
extern bool     ProcPadSections( void );
extern bool     ProcFullHeader( void );

extern bool     ProcDos( void );
extern bool     ProcCom( void );

extern bool     ProcSection( void );
extern bool     ProcEnd( void );
extern bool     ProcAutoSection( void );
extern bool     ProcInto( void );

extern void     SetDosFmt( void );
extern void     MakeNewSection( void );
extern void     CmdOvlFini( void );

extern byte     OvlLevel;
