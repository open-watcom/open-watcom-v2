/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI host detection for Extended DOS.
*
****************************************************************************/


#include "variety.h"
#include "extender.h"
#include "dpmi.h"
#include "rtdata.h"
#include "rtinit.h"

extern short _CheckDPMIVersion( void );
#pragma aux _CheckDPMIVersion = \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
    __parm   [] \
    __value  [__ax] \
    __modify [__bx __cx __dx]

unsigned char   _DPMI = 0;

static void dpmi_check( void )
{
    if( DPMIModeDetect() == 0 || _IsRational() ) {
        if( _CheckDPMIVersion() ) {
            _DPMI = 1;
        }
    }
}

AXI( dpmi_check, INIT_PRIORITY_THREAD )
