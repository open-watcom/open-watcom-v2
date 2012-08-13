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
* Description:
*
****************************************************************************/

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iomanip>
#include <iostream>
#endif

// Note that each manipulator function takes a single parameter called
// "__p". This name is defined in the SMANIP_make macro definition. Each
// function creates an object of type SMANIPint or SMANIPlong. It copies
// "__p" into the "__p" member of the returned object, and copies a
// function pointer into the "__f" member. Later, when the returned
// object is operated upon by << or >>, an indirect call is made via the
// stored function pointer, passing "__p" as a value. This called
// function is the one that actually changes the state of the stream.

// Handles "setw" manipulator.
static std::ios &__setw( std::ios &strm, int width ) {
    strm.width( width );
    return( strm );
}

#ifdef MACRO_IOMANIP
namespace std {
  SMANIP_make( int, setw ) {
    // SMANIPint setw( int __p )
    //      Set the width used for output conversion of values.
    return SMANIPint( __setw, __p );
  }
}
#endif


#ifdef __SIMPLIFY__
namespace std {
  smanip<int> setw( int parm ) {
    return smanip<int>( __setw, parm );
  }
}
#else
#pragma initialize 22; // just after the rest of the C++ library
namespace std {
  _WPRTLINK sapp<int> _HUGEDATA setw( __setw );
}
#endif
