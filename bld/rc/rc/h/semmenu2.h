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
* Description:  Semantic actions for processing menu resources (OS/2 grammar).
*
****************************************************************************/


#ifndef SEMMENU2_INCLUDED
#define SEMMENU2_INCLUDED

extern MenuFlags    SemOS2AddFirstMenuOption( YTOKEN token );
extern MenuFlags    SemOS2AddMenuOption( MenuFlags oldflags, YTOKEN token );
extern FullMenuOS2  *SemOS2NewMenu( FullMenuItemOS2 firstitem );
extern FullMenuOS2  *SemOS2AddMenuItem( FullMenuOS2 * currmenu, FullMenuItemOS2 curritem );
extern void         SemOS2WriteMenu( WResID * name, ResMemFlags flags, FullMenuOS2 * menu, YTOKEN, uint_32 codepage );

#endif
