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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include "_process.h"

#if defined(_M_IX86)
extern  char    _DOS_Switch_Char( void );
#ifdef _M_I86
#pragma aux     _DOS_Switch_Char = \
        "mov ax,3700h"  \
        "int 21h"       \
        value [dl] modify [ax]
#else
#pragma aux     _DOS_Switch_Char = \
        "xor eax,eax"   \
        "mov ah,37h"    \
        "int 21h"       \
        value [dl] modify [eax]
#endif
#endif


CHAR_TYPE *__F_NAME(__Slash_C,__wSlash_C)( CHAR_TYPE *switch_c, unsigned char use_slash )
{
    if( use_slash ) {
        switch_c[0] = STRING( '/' );
    } else {
#if defined( _M_I86 ) || defined( __DOS__ )
        switch_c[0] = _DOS_Switch_Char();
#else
        switch_c[0] = STRING( '/' );
#endif
    }
    switch_c[1] = STRING( 'c' );
    switch_c[2] = '\0';
    return( switch_c );
}
