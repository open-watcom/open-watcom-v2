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
* Description:  OS query routine for QNX.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <i86.h>
#include <sys/proc_msg.h>
#include <sys/kernel.h>
#include <sys/utsname.h>
#include "crwd.h"

extern unsigned char    __87;
extern unsigned char    __r87;

void __DetOSInfo( void )
{
    struct _osinfo osdata;

    qnx_osinfo( 0, &osdata );
    _osmajor = osdata.version / 100;
    _osminor = osdata.version % 100;
#if !defined(__386__)
    if( (osdata.sflags & _PSF_PROTECTED) != 0 ) {
        _HShift = 3;
    } else {
        _HShift = 12;
    }
    if( osdata.sflags & _PSF_EMU16_INSTALLED ) {
#else
    if( osdata.sflags & _PSF_EMU32_INSTALLED ) {
#endif
        __r87 = 0;
        __87 = 3;
    } else if( osdata.fpu >= 387 ) {
        __r87 = 3;
        __87 = 3;
    } else if( osdata.fpu != 0 ) {
        __r87 = 1;
        __87 = 1;
    } else {
        __r87 = 0;
        __87 = 0;
    }
}
