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
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#if defined(__DOS_386__)
    #include "extender.h"
    #include "dpmi.h"
#elif defined(__OS2__)
    #define INCL_DOSFILEMGR
    #include <wos2.h>
#elif defined(__WINDOWS_386__) || defined(__NT__)
    #include <windows.h>
#endif

#if defined(__NT__)
    #pragma library("advapi32")
    #pragma library("comdlg32")
    #pragma library("gdi32")
    #pragma library("kernel32")
    #pragma library("user32")
    #pragma library("winspool")
    #pragma library("shell32")
    #pragma library("version")
    #pragma library("netapi32")
    #pragma library("dlcapi")
    #pragma library("lz32")
    #pragma library("mpr")
    #pragma library("ole32")
    #pragma library("rpcns4")
    #pragma library("rpcrt4")
    #pragma library("vdmdbg")
    #pragma library("win32spl")
    #pragma library("winmm")
    #pragma library("winstrm")
    #pragma library("wsock32")
#elif defined(__OS2__)
    #if defined(__PPC__)
    #else
        #pragma library("os2386")
    #endif
    unsigned            __hmodule;
#endif

#ifdef __DOS_386__
    extern     __GETDSStart_;
    extern     __GETDSEnd_;
    extern int __DPMI_hosted(void);
#endif

void    __CommonInit()
/********************/
{
#if !defined(__NETWARE__) && !defined(__NT__)
    _amblksiz = 32 * 1024;      /* set minimum memory block allocation */
#endif

#ifdef __DOS_386__
    /*
     * If we are running under DOS/4G then we need to page lock interrupt
     * handlers (since we could be running under VMM).
     */
    if (!_IsPharLap() && (__DPMI_hosted() == 1))
    {
        DPMILockLinearRegion((long)&__GETDSStart_,
            ((long)&__GETDSEnd_ - (long)&__GETDSStart_));
    }
#endif
}
