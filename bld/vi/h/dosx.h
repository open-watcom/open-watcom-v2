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



#ifndef _DOSX_INCLUDED
#define _DOSX_INCLUDED

#define USE_FCNS

#if defined(__QNX__)
#include <sys/seginfo.h>
#include <sys/console.h>
#include <sys/con_msg.h>
#include <sys/osinfo.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/dev.h>
#include <sys/psinfo.h>
#include <sys/proxy.h>
#include <termios.h>
#include <i86.h>
#include "dosext.h"
#else
#include <dos.h>
#include "dosext.h"
#if defined(__OS2__)
#define INCL_BASE
#include "os2.h"
#endif
#endif

extern InterruptData idata;

#define KEY_SHIFT_RIGHT 0x01
#define KEY_SHIFT_LEFT  0x02
#define KEY_SHIFT       (KEY_SHIFT_RIGHT | KEY_SHIFT_LEFT)
#define KEY_CTRL        0x04
#define KEY_ALT         0x08
#define KEY_CAPS_LOCK   0x40

#endif
