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

#if defined(_M_IX86)
extern  int     _DOS_Switch_Char( void );
#pragma aux     _DOS_Switch_Char = \
                        0x52            /* push dx */\
                        0xb4 0x37       /* mov ah,37h    */\
                        0xb0 0x00       /* mov al,00h    */\
                        0xcd 0x21       /* int 21h       */\
                        0x88 0xd0       /* mov al,dl     */\
                        0xb4 0x00       /* mov ah,0      */\
                        0x5a            /* pop dx        */;
#endif


CHAR_TYPE *__F_NAME(__Slash_C,__wSlash_C)( CHAR_TYPE *switch_c, CHAR_TYPE prot_mode286 )
{
#if defined( __WARP__ ) || defined( __NT__ )
    prot_mode286 = prot_mode286;
    switch_c[0] = '/';
#else
    if( prot_mode286 ) {
        switch_c[0] = '/';
    } else {
        switch_c[0] = _DOS_Switch_Char();
    }
#endif
    switch_c[1] = 'c';
    switch_c[2] = '\0';
    return( switch_c );
}
