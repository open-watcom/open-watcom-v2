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


extern void     StartTime( void );
extern void     StartMapBuffering( void );
extern void     StopMapBuffering( void );
extern void     MapInit( void );
extern void     WriteGroups( void );
extern void     WriteSegs( class_entry * );
extern void     WritePubHead( void );
extern void     WritePubModHead( void );
extern void     WriteOvlHead( void );
extern void     WriteModSegs( void );
extern void     ProcUndefined( symbol * );
extern void     RecordTracedSym( symbol * );
extern void     WriteUndefined( void );
extern void     FreeUndefs( void );
extern void     WriteLibsUsed( void );
extern void     MapSizes( void );
extern void     EndTime( void );
extern void     WriteMapNL( unsigned );
extern void     DoWriteMap( char *, va_list * );
extern void     WriteMap( char *, ... );
extern void     WriteFormat( int, char *, ... );
extern void     BufWrite( char *, int );

