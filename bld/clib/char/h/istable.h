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


#ifndef _ISTABLE_H_INCLUDED
#define _ISTABLE_H_INCLUDED

#if defined(__386__)
extern int IsWhat( int );
#pragma aux IsWhat = \
        "and eax,0xff" \
        "mov al,_IsTable+0x1[eax]" \
        parm [eax]
#elif defined(__I86__)
extern int IsWhat( int );
#pragma aux IsWhat = \
        "push bx" \
        "and ax,0xff" \
        "mov bx,ax" \
        "mov al,_IsTable+0x1[bx]" \
        "pop bx" \
        parm [ax]
#else
static int IsWhat( int c )
{
    return( _IsTable[TO_ASCII( c )+1] );
}
#endif

#endif
