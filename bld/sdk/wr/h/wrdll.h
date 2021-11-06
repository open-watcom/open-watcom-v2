/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef WRDLL_INCLUDED
#define WRDLL_INCLUDED

#ifdef INSIDE_WRDLL
    #define WRDLLENTRY    __declspec(dllexport)
#elif defined( __NT__ )
    #define WRDLLENTRY    __declspec(dllimport)
#else
    #define WRDLLENTRY
#endif

#if defined( __OS2__ )
    #define WRAPI   APIENTRY
    #define WORD    USHORT
    #define DWORD   ULONG
#else
    #define WRAPI   WINAPI
#endif

#include "wclbhelp.h"
#include "wrinfo.h"
#include "wrmain.h"
#include "wridfile.h"
#include "wrtmpfil.h"
#include "wrmem.h"
#include "wrdel.h"
#include "wrstr.h"
#include "wrclipbd.h"
#include "wrclean.h"
#include "wrdata.h"
#include "wrfindt.h"
#include "wrnamoor.h"
#include "wrwresid.h"
#include "wrhash.h"
#include "wrbitmap.h"
#include "wrdmsg.h"
#include "wricon.h"
#include "wrmemf.h"
#include "wrrnames.h"
#include "wrselft.h"
#include "wrselimg.h"
#include "wrutil.h"
#include "wrctl3d.h"

#endif
