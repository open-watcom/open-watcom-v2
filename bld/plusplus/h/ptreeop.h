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


// PTREEOP -- operator definitions for PTREE
//
// 91/06/25 -- J.W.Welch        -- defined


{   PTOP( PT_NULL               , PTS_NULL                      )
,   PTOP( PT_STRING_CONSTANT    , PTS_NULL                      )
,   PTOP( PT_INT_CONSTANT       , PTS_NULL                      )
,   PTOP( PT_FLOATING_CONSTANT  , PTS_NULL                      )
,   PTOP( PT_TYPE               , PTS_NULL                      )
,   PTOP( PT_ID                 , PTS_NULL                      )
,   PTOP( PT_SYMBOL             , PTS_NULL                      )
,   PTOP( PT_UNARY              , PTS_UNARY | PTS_HAS_CGOP      )
,   PTOP( PT_BINARY             , PTS_BINARY | PTS_HAS_CGOP     )
,   PTOP( PT_DUP_EXPR           , PTS_UNARY                     )
,   PTOP( PT_IC                 , PTS_NULL                      )
,   PTOP( PT_MAX                , PTS_NULL                      )
,   PTOP( PT_FREE               , PTS_NULL                      )
}

