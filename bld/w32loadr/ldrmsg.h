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


pick(   LOADER_SUCCESS = 0,
        ""
),
pick(   LOADER_CANT_OPEN_EXE,
        E( "Can't open '%s'; rc=%d\r\n")
        J( "オープンできません '%s'; rc=%d\r\n")
),
pick(   LOADER_INVALID_EXE,
        E( "Invalid EXE\r\n" )
        J( "不正な EXE\r\n" )
),
pick(   LOADER_READ_ERROR,
        E( "Loader read error\r\n" )
        J( "ローダー読み込みエラー\r\n" )
),
pick(   LOADER_NOT_ENOUGH_MEMORY,
        E( "Memory allocation failed\r\n" )
        J( "メモリ割り付けが失敗しました\r\n" )
),
#undef  pick
#undef  E
#undef  J
