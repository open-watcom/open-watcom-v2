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


// EXCCODE.CPP
//
// This sample can be compiled with different switch settings to see the
// effect of the -x switch.
//
// Compilation      Effect
//   Options
//
// -ox -xd          - exceptions disabled
// -ox -xdt         - destructors called directly
//                  - generates fastest code
//
// -ox -xds         - exceptions disabled
//                  - destructor epilogue implemented using run-time call
//                  - may generate smaller code for destructors with many
//                    destructable members and bases
//
// -ox -xs          - exceptions enabled
//                  - run-time routines process tables to effect destruction
//                    for both normal destruction and on throws
//
// -ox -xst         - exceptions enabled
//                  - destructors called directly
//                  - run-time routines process tables to effect destruction
//                    on throws
//                  - generates faster code than -xs at a space cost
//
// -ox -xss         - exceptions enabled
//                  - run-time routines process tables to effect destruction
//                    for both normal destruction and on throws
//                  - destructor epilogue implemented using run-time call
//                  - may generate smaller code for destructors with many
//                    destructable members and bases
//
// The implementation of class P illustrates how #pragma's can be used to
// override the -x switch settings.  This may be useful when optimizing.


static int ctored;  // number of constructions
static int dtored;  // number of destructions

struct S {          // A simple class S
    int s;
    S( int );
    ~S();
};

struct C {          // A class containing S members
    S m1;
    S m2;
    C( int, int );
    ~C();
};

struct P {          // A class containing S members
    S m1;
    S m2;
    P( int, int );
    ~P();
};


int main()
{
    {   C cv( 4, 5 );
        P pv( 6, 7 );
    }
    return ctored != dtored;
}


// Implementation of class S

S::S( int v ) : s(v)
{
    ++ ctored;
}

S::~S()
{
    ++ dtored;
}


// Implementation of class C

C::C( int v1, int v2 )
    : m1( v1 )
    , m2( v2 )
{
    ++ ctored;
}

C::~C()
{
    ++ dtored;
}


// Implementation of class P; #pragma's used to force specific code
// generation for routines regardless of command line switches

#pragma destruct direct         // use -xd

P::P( int v1, int v2 )
    : m1( v1 )
    , m2( v2 )
{
    ++ ctored;
}

#pragma destruct (table small)  // use -xss

P::~P()
{
    ++ dtored;
}

