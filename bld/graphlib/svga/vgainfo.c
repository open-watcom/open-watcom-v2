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


#include <graph.h>
#include <stdio.h>


extern short            _SuperVGAType();


char                    *SVGANames[] = {
    "is not a SuperVGA",                // _SV_NONE
    "supports the VESA standard",       // _SV_VESA
    "is made by Video-7",               // _SV_VIDEO7
    "is made by Paradise (WD)",         // _SV_PARADISE
    "is made by ATI",                   // _SV_ATI
    "is made by Tseng (3000)",          // _SV_TSENG3000
    "is made by Tseng (4000)",          // _SV_TSENG4000
    "is made by Oak",                   // _SV_OAK
    "is made by Trident",               // _SV_TRIDENT
    "is made by Chips and Technologies",// _SV_CHIPS
    "is made by Genoa",                 // _SV_GENOA
    "is made by S3",                    // _SV_S3
    "is made by Cirrus Logic",          // _SV_CIRRUS
    "is made by Diamond Viper"          // _SV_VIPER
};


void main()
//=========

{
    struct videoconfig  vc;
    short               type;

    _getvideoconfig( &vc );     // initialize graphics library variables
    type = _SuperVGAType();
    printf( "The graphics adapter in this machine %s\n", SVGANames[ type ] );
}
