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


// example of member pointer use across virtual inheritance
// only WATCOM and Borland can execute this program correctly
#include <iostream.h>

struct V1 {
    int get() { return v; }
    void set( int x ) { v = x; }
private:
    int v;
};
struct V2 {
    int get() { return v; }
    void set( int x ) { v = x; }
private:
    int v;
};
struct XX : virtual V1, virtual V2 {
};
struct YY : virtual V2, virtual V1 {
};
struct ZZ : XX, YY {
};

unsigned errors;

ZZ data;

void test_XX( XX *p, int (XX::*mp)( void ), int should_be )
{
    int member_value;

    member_value = (p->*mp)();
    if( member_value != should_be ) {
        ++errors;
    }
    cout << "(" << member_value << ") should be " << should_be << endl;
}

void test_YY( YY *p, int (YY::*mp)( void ), int should_be )
{
    int member_value;

    member_value = (p->*mp)();
    if( member_value != should_be ) {
        ++errors;
    }
    cout << "(" << member_value << ") should be " << should_be << endl;
}

void test_ZZ( ZZ *p, int (ZZ::*mp)( void ), int should_be )
{
    int member_value;

    member_value = (p->*mp)();
    if( member_value != should_be ) {
        ++errors;
    }
    cout << "(" << member_value << ") should be " << should_be << endl;
}

void main()
{
    data.V1::set(-1);
    data.V2::set(-2);
    test_XX( &data, &V1::get, -1 );
    test_XX( &data, &V2::get, -2 );
    test_YY( &data, &V1::get, -1 );
    test_YY( &data, &V2::get, -2 );
    test_ZZ( &data, &V1::get, -1 );
    test_ZZ( &data, &V2::get, -2 );
    if( errors != 0 ) {
        cout << "FAILURE" << endl;
    }
}
