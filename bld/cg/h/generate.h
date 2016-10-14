/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  High level code generation routines. Lots of action here.
*
****************************************************************************/


extern void             InitCG( void );
extern void             AbortCG( void );
extern void             FiniCG( void );
extern void             ProcMessage( msg_class msg );
extern void             Generate( bool routine_done );

extern void             OptSegs( void );
extern void             InitSegment( void );
extern void             FiniSegment( void );
extern bool             LdStAlloc( void );
extern void             LdStCompress( void );
extern bool             SetOnCondition( void );
extern void             BuildIndex( void );
extern void             MemtoBaseTemp( void );
extern void             FixMemBases( void );
extern bool             LoadAToMove( instruction *ins );
extern void             FixCallIns( instruction *ins );
//extern bool             CharsAndShortsToInts( void );
