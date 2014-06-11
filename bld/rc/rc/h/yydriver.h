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
* Description:  Public interface to yydriver module.
*
****************************************************************************/


#ifndef _YYDRIVER_H
#define _YYDRIVER_H

extern void ScanInitOS2( void );
extern void ScanInitStaticsOS2( void );
extern void ParseInitOS2( void );
extern void ParseFiniOS2( void );
extern bool ParseOS2( void );
extern void ParseInitStaticsOS2( void );

extern void ScanInitWIN( void );
extern void ScanInitStaticsWIN( void );
extern void ParseInitWIN( void );
extern void ParseFiniWIN( void );
extern bool ParseWIN( void );
extern void ParseInitStaticsWIN( void );

#endif
