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


class C1                                                    {public: char a;};
class CenteringReallyLongNames4 : public C1                 {public: char a;};
class C2                                                    {public: char a;};
class CenteringReallyLongNames5 : public C2                 {public: char a;};
class C3                                                    {public: char a;};
class CenteringReallyLongNames6 : public C3                 {public: char a;};

class C7 : public CenteringReallyLongNames4,
           public CenteringReallyLongNames5,
           public CenteringReallyLongNames6                 {public: char a;};

class CenteringReallyLongNames8 : public C7                 {public: char a;};

class CenteringReallyLongNames10 : public CenteringReallyLongNames8
                                                            {public: char a;};
class C13 : public CenteringReallyLongNames10               {public: char a;};
class CenteringReallyLongNames11 : public CenteringReallyLongNames8
                                                            {public: char a;};
class C14 : public CenteringReallyLongNames11               {public: char a;};
class CenteringReallyLongNames12 : public CenteringReallyLongNames8
                                                            {public: char a;};
class C15 : public CenteringReallyLongNames12               {public: char a;};
