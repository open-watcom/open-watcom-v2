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


extern bool     ProcNovDBI( void );
extern bool     ProcExportsDBI( void );
extern bool     ProcModule( void );
extern bool     ProcScreenName( void );
extern bool     ProcCheck( void );
extern bool     ProcMultiLoad( void );
extern bool     ProcReentrant( void );
extern bool     ProcSynch( void );
extern bool     ProcCustom( void );
extern bool     ProcExit( void );
extern bool     ProcThreadName( void );
extern bool     ProcPseudoPreemption( void );
extern bool     ProcNLMFlags( void );
extern bool     ProcCopyright( void );
extern bool     ProcMessages( void );
extern bool     ProcHelp( void );
extern bool     ProcXDCData( void );
extern bool     ProcSharelib( void );
extern bool     ProcOSDomain( void );
extern bool     ProcNovell( void );

extern bool     ProcNLM( void );
extern bool     ProcLAN( void );
extern bool     ProcDSK( void );
extern bool     ProcNAM( void );

extern bool     ProcNovDBIExports( void );
extern bool     ProcNovDBIReferenced( void );
extern bool     ProcNovImport( void );
extern bool     ProcNovExport( void );
extern void     SetNovImportSymbol( symbol * );
extern void     SetNovFmt( void );
extern void     FreeNovFmt( void );
extern void     CmdNovFini( void );
