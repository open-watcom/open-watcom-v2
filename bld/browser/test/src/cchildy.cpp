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


class ChildyRoot                                            {public: char a;};

class Childy11 : public ChildyRoot                          {public: char a;};
class Childy12 : public ChildyRoot                          {public: char a;};

class Childy1 : public Childy11                             {public: char a;};
class Childy2 : public Childy11                             {public: char a;};
class Childy3 : public Childy11                             {public: char a;};
class Childy4 : public Childy11                             {public: char a;};
class Childy5 : public Childy11                             {public: char a;};

class Childy6 : public Childy12                             {public: char a;};
class Childy7 : public Childy12                             {public: char a;};
class Childy8 : public Childy12                             {public: char a;};
class Childy9 : public Childy12                             {public: char a;};
class Childy10 : public Childy12                            {public: char a;};
