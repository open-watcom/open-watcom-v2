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
* Description: Benchmark program that does a lot of std::string copying.
*              This program passes strings (by value) in and out of a
*              function.
*
****************************************************************************/

#include <iostream>
#include <string>
#include "timer.h"

#define SIZE   40        // Base size. Use something 'typical'
#define COUNT  10000000  // Number of times main loop executes.

std::string f( std::string v )
{
  v.append( 1, 'y' );  // Do something with the string to change its value.
  return( v );
}

int main( )
{
  TimerOn( );
  for( int i = 0; i < COUNT; ++i ) {
    std::string s( SIZE, 'x' );
    s = f( s );
    if( s.size( ) != SIZE + 1 ) {
      std::cout << "Internal error!\n";
    }
  }
  TimerOff( );
  std::cout << "\n";
  std::cout << "loop: ";
  std::cout << ( TimerElapsed( )/COUNT ) * 1000 << " ms/pass\n";

  return( 0 );
}
