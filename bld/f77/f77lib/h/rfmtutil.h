/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Run-time formatted I/O utility routines
*
****************************************************************************/


extern void    R_ChkType( PTYPE lower, PTYPE upper );
extern void    R_NewRec( void );
extern void    R_ChkIType( void );
extern void    R_ChkFType( void );
extern void    R_ChkRecLen( void );
extern void    R_FOStr( void );
extern void    R_FIStr( void );
extern void    R_FOLog( void );
extern void    R_FILog( void );
extern void    R_FIFloat( void );
extern bool    GetReal( extended *value );
extern void    R_FOF( void );
extern void    R_FOE( int exp, char ch );
extern void    R_FIHex( void );
extern void    R_FOHex( void );
extern void    R_FIInt( void );
extern void    R_FOInt( void );
extern void    R_FOG( void );
extern void    ChkBuffLen( uint width );
