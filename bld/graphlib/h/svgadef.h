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


// Super VGA adapters supported

enum {
    _SV_NONE,               // no SuperVGA
    _SV_VESA,               // VESA Standard
    _SV_VIDEO7,             // Video-7
    _SV_PARADISE,           // Western Digital (Paradise)
    _SV_ATI,                // ATI VGA Wonder
    _SV_TSENG3000,          // Tseng ET-3000
    _SV_TSENG4000,          // Tseng ET-4000
    _SV_OAK,                // Oak Technologies
    _SV_TRIDENT,            // Trident
    _SV_CHIPS,              // Chips and Technologies Inc.
    _SV_GENOA,              // Genoa
    _SV_S3,                 // S3-86C911
    _SV_CIRRUS,             // Cirrus Logic
    _SV_VIPER,              // Diamond Viper
    _SV_MAX
};


#if !defined ( __386__ )
extern short            GetVESAInfo( short, short, void far * );
#pragma aux             GetVESAInfo = \
                        "push bp" \
                        "int 10h"  \
                        "pop bp"  \
                        parm caller [ax] [cx] [es di] value [ax];
#elif defined( __QNX__ )
extern short            GetVESAInfo( short, short, void far * );
#pragma aux             GetVESAInfo = \
                        "push bp" \
                        "int 10h"  \
                        "pop bp"  \
                        parm caller [ax] [cx] [es edi] value [ax];
#endif
