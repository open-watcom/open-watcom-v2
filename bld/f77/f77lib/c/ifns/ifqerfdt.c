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


//
// IFQERFDT     : error function data for extended PRECISION argument
//

#include "ftnstd.h"

const struct {
    extended p[5];
    extended q[4];
    extended p1[9];
    extended q1[8];
    extended p2[6];
    extended q2[5];
    extended xmin;
    extended xlarge;
    extended xbig;
    extended sqrpi;
    extended pt477;
    extended _4pt0;
} __FAR __qerfdat = {
                {       113.8641541510502e0,
                        377.4852376853020e0,
                        3209.377589138469e0,
                        .1857777061846032e0,
                        3.161123743870566e0 },
                {       244.0246379344442e0,
                        1282.616526077372e0,
                        2844.236833439171e0,
                        23.60129095234412e0 },
                {       8.883149794388376e0,
                        66.11919063714163e0,
                        298.6351381974001e0,
                        881.9522212417691e0,
                        1712.047612634071e0,
                        2051.078377826071e0,
                        1230.339354797997e0,
                        2.153115354744038e-8,
                        .5641884969886701e0 },
                {       117.6939508913125e0,
                        537.1811018620099e0,
                        1621.389574566690e0,
                        3290.799235733460e0,
                        4362.619090143247e0,
                        3439.367674143722e0,
                        1230.339354803749e0,
                        15.74492611070983e0 },
                {       -3.603448999498044e-01,
                        -1.257817261112292e-01,
                        -1.608378514874228e-02,
                        -6.587491615298378e-04,
                        -1.631538713730210e-02,
                        -3.053266349612323e-01 },
                {       1.872952849923460e0,
                        5.279051029514284e-01,
                        6.051834131244132e-02,
                        2.335204976268692e-03,
                        2.568520192289822e0 },
                {       1.0e-10 },
                {       6.375e0 },
                {       13.3e0 },
                {       .5641895835477563e0 },
                {       .477 },
                {       4.0e0 }
};
