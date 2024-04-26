/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


extern void     MapInit( void );
extern void     MapFini( void );
extern void     StartTime( void );
extern void     EndTime( void );
extern void     ResetWriteMapIO( void );
extern void     WriteMapHead( section * );
extern void     WriteMapOvlHead( symbol *, symbol *, symbol *, symbol * );
extern void     WriteMapOvlPubHead( section *sec );
extern void     WriteMapModulesSegments( void );
extern void     RecordUndefinedSym( symbol * );
extern void     RecordTracedSym( symbol * );
extern void     PrintUndefinedSyms( void );
extern void     PrintTracedSyms( void );
extern void     FreeUndefinedSyms( void );
extern void     FreeTracedSyms( void );
extern void     WriteMapLibsUsed( void );
extern void     WriteMapSizes( void );
extern void     WriteMap( const char *, size_t );
extern void     WriteMapNL( void );
extern void     WriteMapPrintf( const char *, ... );
extern void     WriteMapPubStart( void );
extern void     WriteMapPubEnd( void );
extern void     WriteMapPubSortStart( pubdefinfo *info );
extern void     WriteMapPubSortEnd( pubdefinfo *info );
extern void     WriteMapPubEntry( pubdefinfo *info, symbol *sym );
