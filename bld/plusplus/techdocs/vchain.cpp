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


#include <iostream.h>

struct V {
    int v;
};
struct V1 : virtual V {
    int v1;
};
struct V2 : virtual V1 {
    int v2;
};
struct V3 : virtual V2 {
    int v3;
};
struct V4 : virtual V3 {
    int v4;
};
struct V5 : virtual V4 {
    int v5;
};
struct V6 : virtual V5 {
    int v6;
};
struct V7 : virtual V6 {
    int v7;
};
struct V8 : virtual V7 {
    int v8;
};
struct V9 : virtual V8 {
    int v9;
};
struct V10 : virtual V9 {
    int v10;
};

void main()
{
    cout << "10 layers of virtual inheritance costs "
         << sizeof( V10 )
         << " bytes"
         << endl;
}
