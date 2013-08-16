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
* Description:  Calling convention for dot function.
*
****************************************************************************/


#if defined ( __386__ )
    #pragma aux PUT_DOT_FUNC "*" parm caller [es edi] [eax] [ecx];
#else
  #if defined( VERSION2 )
    #pragma aux PUT_DOT_FUNC "*" far parm caller [es di] [dx ax] [cx];
  #else
    #pragma aux PUT_DOT_FUNC "*" far parm caller [es di] [ax] [cx];
  #endif
#endif

#if defined ( __386__ )
    #pragma aux GET_DOT_FUNC "*" parm caller [es edi] [ecx];
#else
  #if defined( VERSION2 )
    #pragma aux GET_DOT_FUNC "*" far parm caller [es di] [cx];
  #else
    #pragma aux GET_DOT_FUNC "*" far parm caller [es di] [cx];
  #endif
#endif

typedef grcolor get_dot_fn( char far *, int );
typedef void  put_dot_fn( char far *, grcolor, int );

#pragma aux (GET_DOT_FUNC) get_dot_fn;
#pragma aux (PUT_DOT_FUNC) put_dot_fn;
