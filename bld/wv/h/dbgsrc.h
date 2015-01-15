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
* Description:  Source file access.
*
****************************************************************************/


extern void         InitSource( void );
extern void         FiniSource( void );
extern void         FreeRing( char_ring *p );
extern void         DeleteRing( char_ring **owner, const char *start, unsigned len, bool ucase );
extern void         InsertRing( char_ring **owner, const char *start, unsigned len, bool ucase );
extern char_ring    **RingEnd( char_ring **owner );
extern void         AddSourceSpec( const char *start, unsigned len );
extern char         *SourceName( char_ring *src );
extern char_ring    *NextSourceSpec( char_ring *curr );
extern void         SourceSet( void );
extern void         SourceConf( void );
extern void         *OpenSrcFile( cue_handle *ch );
