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
    V()
    {
        cout << "V constructed at " << (void *) this << endl;
    }
    V & operator =( const V &src );
};

struct R : virtual V {
    int r;
};

struct S : virtual V {
    int s;
};

struct T : R, S {
    int t;
};

V & V::operator =( const V &src ) {
    cout << "assign V (" << (void*)this << "," << (void*)&src << ")" << endl;
    return *this;
}

void assign_R( R &s, R &d )
{
    cout << "R slicing" << endl;
    s = d;
}

void assign_S( S &s, S &d )
{
    cout << "S slicing" << endl;
    s = d;
}

void assign_T( T &s, T &d )
{
    cout << "T slicing" << endl;
    s = d;
}

T t;

void main()
{
    assign_R( t,t );
    assign_S( t,t );
    assign_T( t,t );
}
