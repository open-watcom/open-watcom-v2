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


#pragma pack(push, 1);
struct arcinfo {
    struct xycoord      centre;
    struct xycoord      start;
    struct xycoord      end;
    struct xycoord      vecta;
    struct xycoord      vectb;
    short               qinf[ 4 ];
    void                (*plot)();
};


// The following enumerated type describes how to draw an arc
// in each of the quadrants.

enum {
    ARC_EMPTY,          // don't draw any points in this quadrant
    ARC_FULL,           // draw all of the points in this quadrant
    ARC_VECT_A,         // only vector A intersects this quadrant
    ARC_VECT_B,         // only vector B intersects this quadrant
    ARC_BOTH_IN,        // both vectors intersect, plot between
    ARC_BOTH_OUT,       // both vectors intersect, plot outside
    ARC_LINE            // vectors coincide, plot single point
};
#pragma pack (pop);
