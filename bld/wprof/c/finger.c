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
* Description:  WPROF's birth certificate.
*
****************************************************************************/


#include "banner.h"

char *AboutMessage[] = {
    "",
    banner1w1( "Execution Profiler" STR_BITNEES ),
    banner1w2( _WPROF_VERSION_ ),
    banner2,
    banner2a( "1987" ),
    "",
    banner3,
    banner3a,
#ifdef D32_NAME
    "",
    D32_NAME " " D32_VERSION,
    D32_COPYRIGHT,
#endif
};

int AboutSize = sizeof( AboutMessage ) / sizeof( AboutMessage[0] );

#ifdef __DOS__
char DOS4GOPTIONS[] =
        "[dos4g-global]\n"
        "Include=WPROFOPT.INI\n"
        "[dos4g-kernel]\n"
        "StartupBanner=FALSE\n"
;
#endif
