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


class ColourRoot                        {public: char root; };
class Colour11 : public ColourRoot      {public: char oneone; };
class Colour12 : protected ColourRoot   {public: char onetwo; };
class Colour13 : private ColourRoot     {public: char onethree; };
class Colour21 : public virtual ColourRoot {public: char twoone; };
class Colour22 : protected virtual ColourRoot {public: char twotwo; };
class Colour23 : private virtual ColourRoot {public: char twothree; };

class Colour2R { public: char root; };
class Colour2R2 : public Colour2R { public: char root; };
class Colour2R3 : public Colour2R { public: char root; };
class Colour2R4 : public Colour2R { public: char root; };

void fn1() { fn1(); }
void fn2() { fn2(); fn2(); fn2(); }
void fn3() {}
void fn4() {}
void fn5() {}
void fn6() {}

void fnRoot() {
    fn1();

    fn2(); fn2();

    fn3(); fn3(); fn3();

    fn4(); fn4(); fn4();

    fn5(); fn5(); fn5(); fn5(); fn5();

    fn6(); fn6(); fn6(); fn6(); fn6(); fn6();
}

void fnR1() {}
void fnR2() {}
void fnR3() {}
void fnRoot2() { fnR1(); fnR2(); fnR3(); }
