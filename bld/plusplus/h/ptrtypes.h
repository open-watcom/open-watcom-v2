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


// PTRTYPES.H -- classification of pointer types
//
// This table is embedded several times by CGTYPE.H, each time selecting a
// column.  This method is used to ensure that the elements are always
// properly ordered.
//
// 91/09/09 -- J.W.Welch        -- defined
// 92/01/02 -- J.W.Welch        -- add support for DATACODE

// need to fill out for based pointers
{   PTR_TYPE( _NR       ,&CompInfo.ptr_diff_near )
,   PTR_TYPE( _FR       ,&CompInfo.ptr_diff_far  )
,   PTR_TYPE( _HG       ,&CompInfo.ptr_diff_huge )
,   PTR_TYPE( _FAR16    ,&CompInfo.ptr_diff_far16)
}
#undef PTR_TYPE
