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


// developed by:
//      Erwin Unruh     erwin.unruh@mch.sni.de
//      Tom Pennello    tom@metaware.com
//
template <int i, int j, int k>
    struct Acker_is {
    };

template <int m, int n>
    struct Acker {
        enum {
            val = m == 0 ? n+1
                         : n == 0 ? Acker<m?m-1:0,m?1:0>::val
                                  : Acker<m?m-1:0,m?Acker<n?m:0,n?n-1:0>::val : 0>::val
        };
        Acker_is< m, n, (int) val > x;
        void f() {
            int y = x;
        }
    };

struct Acker<0,0> {
    enum {
        val = 1
    };
};

void foo() {
    Acker<3,3> x;
};
