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


class Parenty1                                              {public: char a;};
class Parenty2                                              {public: char a;};
class Parenty3                                              {public: char a;};
class Parenty4                                              {public: char a;};
class Parenty5                                              {public: char a;};
class Parenty6                                              {public: char a;};
class Parenty7                                              {public: char a;};
class Parenty8                                              {public: char a;};
class Parenty9                                              {public: char a;};
class Parenty10                                             {public: char a;};

class Parenty11 : public Parenty1, public Parenty2,
                  public Parenty3, public Parenty4,
                  public Parenty5                           {public: char a;};

class Parenty12 : public Parenty6, public Parenty7,
                  public Parenty8, public Parenty9,
                  public Parenty10                          {public: char a;};

class Parenty13 : public Parenty11, public Parenty12        {public: char a;};
