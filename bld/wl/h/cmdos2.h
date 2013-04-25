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
* Description:  OS/2 and Windows command processing function prototypes.
*
****************************************************************************/


extern bool     ProcCommit( void );
extern bool     ProcAnonExport( void );
extern bool     ProcSingle( void );
extern bool     ProcMultiple( void );
extern bool     ProcNone( void );
extern bool     ProcOldLibrary( void );
extern bool     ProcModName( void );
extern bool     ProcNewFiles( void );
extern bool     ProcProtMode( void );
extern bool     ProcDescription( void );
extern bool     ProcObjAlign( void );
extern bool     ProcRWRelocCheck( void );
extern bool     ProcSelfRelative( void );
extern bool     ProcInternalRelocs( void );
extern bool     ProcToggleRelocsFlag( void );
extern bool     ProcNoStdCall( void );
extern bool     ProcResource( void );
extern bool     ProcMixed1632( void );

extern bool     ProcRunNative( void );
extern bool     ProcRunWindows( void );
extern bool     ProcRunConsole( void );
extern bool     ProcRunPosix( void );
extern bool     ProcRunOS2( void );
extern bool     ProcRunDosstyle( void );

extern bool     ProcOS2( void );
extern bool     ProcWindows( void );

extern bool     ProcOS2DLL( void );
extern bool     ProcLE( void );
extern bool     ProcLX( void );
extern bool     ProcPE( void );
extern bool     ProcVXD( void );
extern bool     ProcOldPE( void );

extern bool     ProcPM( void );
extern bool     ProcPMCompatible( void );
extern bool     ProcPMFullscreen( void );
extern bool     ProcPhysDevice( void );
extern bool     ProcVirtDevice( void );

extern bool     ProcMemory( void );
extern bool     ProcFont( void );

extern bool     ProcTNT( void );
extern bool     ProcRDOS( void );

extern bool     ProcDynamicDriver( void );
extern bool     ProcStaticDriver( void );

extern bool     ProcInitGlobal( void );
extern bool     ProcInitInstance( void );
extern bool     ProcInitThread( void );

extern bool     ProcTermGlobal( void );
extern bool     ProcTermInstance( void );
extern bool     ProcTermThread( void );

extern bool     ProcExpResident( void );
extern bool     ProcPrivate( void );

extern bool     ProcOS2Class( void );
extern bool     ProcSegType( void );

extern bool     ProcSegCode( void );
extern bool     ProcSegData( void );

extern bool     ProcPreload( void );
extern bool     ProcLoadoncall( void );
extern bool     ProcIopl( void );
extern bool     ProcNoIopl( void );
extern bool     ProcExecuteonly( void );
extern bool     ProcExecuteread( void );
extern bool     ProcShared( void );
extern bool     ProcNonShared( void );
extern bool     ProcReadOnly( void );
extern bool     ProcReadWrite( void );
extern bool     ProcConforming( void );
extern bool     ProcNonConforming( void );
extern bool     ProcMovable( void );
extern bool     ProcFixed( void );
extern bool     ProcDiscardable( void );
extern bool     ProcNonDiscardable( void );
extern bool     ProcInvalid( void );
extern bool     ProcContiguous( void );
extern bool     ProcResident( void );
extern bool     ProcOS2Dynamic( void );
extern bool     ProcNonPermanent( void );
extern bool     ProcPermanent( void );
extern bool     ProcPageable( void );
extern bool     ProcNonPageable( void );

extern bool     ProcCommitStack( void );
extern bool     ProcCommitHeap( void );

extern bool     ProcOS2Import( void );
extern bool     ProcOS2Export( void );
extern bool     ProcOS2Segment( void );
extern bool     ProcOS2Alignment( void );
extern bool     ProcOS2HeapSize( void );
extern bool     ProcPENoRelocs( void );

extern void     ChkBase( offset align );
extern void     SetOS2Fmt( void );
extern void     FreeOS2Fmt( void );

/*
//  These are PE only
*/
extern bool     ProcLinkVersion( void );
extern bool     ProcOsVersion( void );
extern bool     ProcChecksum( void );
extern bool     ProcLargeAddressAware( void );
extern bool     ProcNoLargeAddressAware( void );
