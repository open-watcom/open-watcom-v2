/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Mainline for binary diff utility.
*
****************************************************************************/


/*
 * Utility routines
 * ================
 */

extern byte     *PatchBuffer;
extern byte     *OldFile;
extern byte     *NewFile;

extern char     *OldSymName;
extern char     *NewSymName;

extern bool     AppendPatchLevel;
extern bool     Verbose;

extern foff     DiffSize;
extern foff     EndOld;
extern foff     EndNew;
extern foff     NumHoles;

extern char     *CommentFile;

extern int      OldCorrection;
extern int      NewCorrection;

extern void     *ReadIn( const char *name, size_t buff_size, size_t read_size );
extern foff     FileSize( const char *name, int *correction );
extern void     ScanSyncString( const char *syncString );
extern void     FindRegions( void );
extern void     FindRegionsAlg( algorithm alg );
extern void     MakeHoleArray( void );
extern void     ProcessHoleArray( int write_holes );
extern long     HolesToDiffs( void );
extern void     WritePatchFile( const char *name, const char *new_name );
extern void     FreeHoleArray( void );
extern void     VerifyCorrect( const char *name );
extern void     SortHoleArray( void );
#ifdef USE_DBGINFO
extern void     SymbolicDiff( algorithm alg, char *old_exe, char *new_exe );
#endif

extern void     print_stats( long savings );
extern void     init_diff( void );
extern void     dump( void );
