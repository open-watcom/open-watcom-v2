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


#ifndef TYPE_CACHE
#define TYPE_CACHE( id )        TYPC_##id,
#endif

TYPE_CACHE( TYPE_INFO_CONST_REF )
TYPE_CACHE( SEGMENT_SHORT )
TYPE_CACHE( CLEAN_INT )
TYPE_CACHE( VOID_PTR )
TYPE_CACHE( DEFAULT_INT )
TYPE_CACHE( VOID_FUN_OF_VOID )
TYPE_CACHE( VOID_FUN_OF_PTR_VOID )
TYPE_CACHE( PTR_VOID_FUN_OF_VOID )
TYPE_CACHE( PTR_STDOP_VOID )
TYPE_CACHE( PTR_CONST_STDOP_VOID )
TYPE_CACHE( PTR_STDOP_ARITH_VOID )
TYPE_CACHE( PTR_CONST_STDOP_ARITH_VOID )
TYPE_CACHE( CDTOR_ARG )
TYPE_CACHE( PTR_VOID_FUN_OF_CDTOR_ARG )
TYPE_CACHE( VOID_HANDLER_FUN_OF_VOID )
TYPE_CACHE( VOID_MEMBER_PTR )
TYPE_CACHE( CLASS_TEMPLATE )
TYPE_CACHE( PTR_VOLATILE_STDOP_VOID )
TYPE_CACHE( PTR_CONST_VOLATILE_STDOP_VOID )
// insert above this line for global cache
TYPE_CACHE( FIRST_LEVEL )
// insert above this line for TYPE.C local cache (TYPE.C depends on this file)
TYPE_CACHE( LAST )

#undef TYPE_CACHE
