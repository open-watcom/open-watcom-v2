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
* Description:  Definitions for regular expression processor built into aui.
*
****************************************************************************/


extern void             *WndCompileRX( const char *expr );
extern void             WndFreeRX( void *rx );
extern bool             WndRXFind( void *rx, const char **pos, const char **endpos );
extern void             WndSetMagicStr( const char *str );
extern bool             WndGetSrchIgnoreCase( void );
extern void             WndSetSrchIgnoreCase( bool );
extern bool             WndGetSrchRX( void );
extern void             WndSetSrchRX( bool );
extern char             *WndGetSrchMagicChars( void );
extern void             WndSetSrchMagicChars( const char *, unsigned );
