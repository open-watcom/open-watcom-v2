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


/* parse tables used in CMDALL.C */

extern bool     ProcLine( void );
extern bool     ProcType( void );
extern bool     ProcLocal( void );
extern bool     ProcAll( void );
extern bool     ProcDBIStatic( void );
extern bool     ProcDefaultDBI( void );
extern bool     ProcWatcomDBI( void );
extern bool     ProcDwarfDBI( void );
extern bool     ProcCodeviewDBI( void );

extern bool     ProcSysBegin( void );
extern bool     ProcSysDelete( void );
extern bool     ProcSysEnd( void );
extern bool     ProcStartLink( void );
extern bool     ProcEndLink( void );

extern bool     ProcAlphabetical( void );
extern bool     ProcGlobal( void );

extern bool     ProcFiles( void );
extern bool     ProcModFiles( void );
extern bool     ProcLibrary( void );
extern bool     ProcOptLib( void );
extern bool     ProcName( void );
extern bool     ProcOptions( void );
extern bool     ProcDebug( void );
extern bool     ProcSystem( void );
extern bool     ProcLibPath( void );
extern bool     ProcLibFile( void );
extern bool     ProcPath( void );
extern bool     ProcFormat( void );
extern bool     ProcAlias( void );
extern bool     ProcReference( void );
extern bool     ProcDisable( void );
extern bool     ProcSort( void );
extern bool     ProcLanguage( void );
#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNX )
extern bool     ProcNewSegment( void );
#endif
extern bool     ProcSymTrace( void );
extern bool     ProcModTrace( void );
extern bool     ProcStart( void );

extern bool     ProcMap( void );
extern bool     ProcMapLines( void );
extern bool     ProcStack( void );
extern bool     ProcNoDefLibs( void );
extern bool     ProcQuiet( void );
extern bool     ProcDosSeg( void );
extern bool     ProcCase( void );
extern bool     ProcVerbose( void );
extern bool     ProcUndefsOK( void );
extern bool     ProcNoUndefsOK( void );
extern bool     ProcNameLen( void );
extern bool     ProcSymFile( void );
extern bool     ProcOSName( void );
extern bool     ProcEliminate( void );
extern bool     ProcMaxErrors( void );
extern bool     ProcNoCaseExact( void );
extern bool     ProcNoCache( void );
extern bool     ProcCache( void );
extern bool     ProcMangledNames( void );
extern bool     ProcOpResource( void );
extern bool     ProcStatics( void );
extern bool     ProcArtificial( void );
extern bool     ProcShowDead( void );
extern bool     ProcVFRemoval( void );
extern bool     ProcRedefsOK( void );
extern bool     ProcNoRedefs( void );
extern bool     ProcCVPack( void );
extern bool     ProcIncremental( void );
extern bool     ProcPackcode( void );
extern bool     ProcPackdata( void );
extern bool     ProcJapanese( void );
extern bool     ProcChinese( void );
extern bool     ProcKorean( void );
extern bool     ProcEntry( void );
extern bool     ProcNoExtension( void );
extern bool     ProcFarCalls( void );
extern bool     ProcNoFarCalls( void );

extern bool     ProcStub( void );
extern bool     ProcNoStub( void );
extern bool     ProcVersion( void );
extern bool     ProcImplib( void );
extern bool     ProcImpFile( void );
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
extern bool     ProcRuntime( void );
#endif
#ifdef _RAW
extern bool     ProcOutput( void );
extern bool     ProcOutputRaw( void );
extern bool     ProcOutputHex( void );
extern bool     ProcOutputOffset( void );
extern bool     ProcOutputStart( void );
extern bool     ProcOutputHshift( void );
#endif
extern bool     ProcHshift( void );
extern bool     ProcFillchar( void );
extern bool     ProcOrder( void );
extern bool     ProcOrdClass( void );
extern bool     ProcOrdSegAdr( void );
extern bool     ProcOrdOfsAdr( void );
extern bool     ProcOrdCopy( void );
extern bool     ProcOrdNoEmit( void );
extern bool     ProcOrdSeg( void );
extern bool     ProcOrdSegSegAdr( void );
extern bool     ProcOrdSegOfsAdr( void );
extern bool     ProcOrdSegCopy( void );
extern bool     ProcOrdSegNoEmit( void );
extern bool     ProcDescription( void );
extern bool     ProcObjAlign( void );
#ifdef _INT_DEBUG
extern bool     ProcXDbg( void );
extern bool     ProcIntDbg( void );
#endif

extern void     ResetCmdAll( void );
extern void     ChkBase( offset align );

extern bool     DoParseSysDirective( bool suicide );
extern bool     DoParseDirective( bool suicide );
extern bool     DoMatchDirective( const char *parse, size_t len );
