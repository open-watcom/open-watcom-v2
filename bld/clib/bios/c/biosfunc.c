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


#include "variety.h"
#undef  __INLINE_FUNCTIONS__
#include <bios.h>

extern unsigned short pascal _clib_bios_equiplist(void);
extern unsigned short pascal _clib_bios_keybrd(unsigned __cmd);
extern unsigned short pascal _clib_bios_memsize(void);
extern unsigned short pascal _clib_bios_printer(unsigned __cmd,unsigned __port,unsigned __data);
extern unsigned short pascal _clib_bios_serialcom(unsigned __cmd,unsigned __port,unsigned __data);

_WCRTLINK unsigned short _bios_equiplist(void) {
        return( _clib_bios_equiplist() );
}
_WCRTLINK unsigned short _bios_keybrd(unsigned __cmd) {
        return( _clib_bios_keybrd( __cmd ) );
}
_WCRTLINK unsigned short _bios_memsize(void) {
        return( _clib_bios_memsize() );
}
_WCRTLINK unsigned short _bios_printer(unsigned __cmd,unsigned __port,unsigned __data) {
        return( _clib_bios_printer( __cmd, __port, __data ) );
}
_WCRTLINK unsigned short _bios_serialcom(unsigned __cmd,unsigned __port,unsigned __data) {
        return( _clib_bios_serialcom( __cmd, __port, __data ) );
}
