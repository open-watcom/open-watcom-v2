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
*              This program copies "long" strings to emphasize the effect
*              of copying the characters in the string.
*
****************************************************************************/

#include <iostream>
#include <string>
#include "timer.h"

#define LONG_SIZE     1000    // Base size of a 'long' string.
#define LONGDUMP_SIZE 10000   // Number of strings in long string dump.
#define LONG_COUNT    10000   // Number of times long string loop executes.

std::string long_dump;

int main( )
{
  TimerOn( );
  for( int i = 0; i < LONG_COUNT; ++i ) {
    std::string long_s( LONG_SIZE, 'x' );
    for( int j = 0; j < LONGDUMP_SIZE; ++j ) {
      long_dump = long_s;
    }
  }
  TimerOff( );
  std::cout << "\n";
  std::cout << "long loop: ";
  std::cout << ( TimerElapsed( )/LONG_COUNT ) * 1000 << " ms/pass\n";

  return( 0 );
}
