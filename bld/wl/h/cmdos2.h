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
* Description:  OS/2 and Windows command processing function prototypes.
*
****************************************************************************/


#ifdef _OS2

extern void     SetOS2Fmt( void );
extern void     FreeOS2Fmt( void );

extern bool     ProcOS2Runtime( void );

extern bool     ProcCommit( void );
extern bool     ProcAnonExport( void );
extern bool     ProcSingle( void );
extern bool     ProcMultiple( void );
extern bool     ProcNone( void );
extern bool     ProcOldLibrary( void );
extern bool     ProcModName( void );
extern bool     ProcNewFiles( void );
extern bool     ProcProtMode( void );
extern bool     ProcRWRelocCheck( void );
extern bool     ProcSelfRelative( void );
extern bool     ProcInternalRelocs( void );
extern bool     ProcToggleRelocsFlag( void );
extern bool     ProcNoStdCall( void );
extern bool     ProcResource( void );
extern bool     ProcMixed1632( void );

extern bool     ProcOS2Import( void );
extern bool     ProcOS2Export( void );
extern bool     ProcOS2Segment( void );
extern bool     ProcOS2Alignment( void );
extern bool     ProcOS2HeapSize( void );

/*
 *  These are PE only
 */
extern bool     ProcLinkVersion( void );
extern bool     ProcOsVersion( void );
extern bool     ProcChecksum( void );
extern bool     ProcLargeAddressAware( void );
extern bool     ProcNoLargeAddressAware( void );
extern bool     ProcPENoRelocs( void );

extern bool     ProcOS2( void );
extern bool     ProcWindows( void );

#endif
