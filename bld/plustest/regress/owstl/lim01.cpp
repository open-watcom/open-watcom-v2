/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: Functional tests for std::numeric_limits.
*
****************************************************************************/

#include <iostream>
#include <limits>

int main()
{
  std::numeric_limits< bool >               x01;
  std::numeric_limits< char >               x02;
  std::numeric_limits< signed char >        x03;
  std::numeric_limits< unsigned char >      x04;
  std::numeric_limits< wchar_t >            x05;
  std::numeric_limits< short >              x06;
  std::numeric_limits< int >                x07;
  std::numeric_limits< long >               x08;
  std::numeric_limits< long long >          x09;
  std::numeric_limits< unsigned short >     x10;
  std::numeric_limits< unsigned int >       x11;
  std::numeric_limits< unsigned long >      x12;
  std::numeric_limits< unsigned long long > x13;
  std::numeric_limits< float >              x14;
  std::numeric_limits< double >             x15;
  std::numeric_limits< long double >        x16;

  std::cout << x01.is_specialized << "\n";
  std::cout << x02.is_specialized << "\n";
  std::cout << x03.is_specialized << "\n";
  std::cout << x04.is_specialized << "\n";
  std::cout << x05.is_specialized << "\n";
  std::cout << x06.is_specialized << "\n";
  std::cout << x07.is_specialized << "\n";
  std::cout << x08.is_specialized << "\n";
  std::cout << x09.is_specialized << "\n";
  std::cout << x10.is_specialized << "\n";
  std::cout << x11.is_specialized << "\n";
  std::cout << x12.is_specialized << "\n";
  std::cout << x13.is_specialized << "\n";
  std::cout << x14.is_specialized << "\n";
  std::cout << x15.is_specialized << "\n";
  std::cout << x16.is_specialized << "\n";

  return 0;
}
