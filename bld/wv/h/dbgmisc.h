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
* Description:  Miscellaneous odds and ends that don't have a home.
*
****************************************************************************/


extern void         Flip( unsigned wait );
extern void         ProcFlip( void );
extern void         ConfigFlip( void );
extern void         ProcRemark( void );
extern void         ProcDo( void );
extern void         ProcAssign( void );
extern void         GoToAddr( address addr );
extern void         StepIntoFunction( const char *func );
extern void         SkipToAddr( address addr );
extern void         GoToReturn( void );
extern void         ProcGo( void );
extern void         ProcSkip( void );
extern void         ProcIf( void );
extern void         ProcWhile( void );
extern void         ProcError( void );
extern void         ProcQuit( void );
extern void         MakeThdCurr( thread_state *thd );
extern void         MakeRunThdCurr( thread_state *thd );
extern dtid_t       RemoteSetThread( dtid_t tid );
extern bool         IsThdCurr( thread_state *thd );
extern dtid_t       GetNextTID( void );
extern void         NameThread( dtid_t tid, const char *name );
extern void         CheckForNewThreads( bool set_exec );
extern thread_state *FindThread( dtid_t tid );
extern void         ProcThread( void );
extern void         FreeThreads( void );
