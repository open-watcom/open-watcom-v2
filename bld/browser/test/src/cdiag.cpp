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


class Diag24                                    { public: char a; };
class Diag23                                    { public: char a; };
class Diag22 : public Diag23, public Diag24     { public: char a; };
class Diag25 : public Diag22                    { public: char a; };
class DiagonalRoot : public Diag23              { public: char a; };
class Diag1 : public DiagonalRoot               { public: char a; };
class Diag2 : public DiagonalRoot               { public: char a; };
class Diag3 : public Diag2                      { public: char a; };
class Diag4 : public Diag2                      { public: char a; };
class Diag5 : public Diag4                      { public: char a; };
class Diag6 : public Diag4                      { public: char a; };
class Diag7 : public Diag6                      { public: char a; };
class Diag8 : public Diag6                      { public: char a; };
class Diag9 : public Diag8                      { public: char a; };
class Diag21 : public Diag24                    { public: char a; };
class Diag20 : public Diag21                    { public: char a; };
class Diag17 : public Diag21                    { public: char a; };
class Diag18 : public Diag17                    { public: char a; };
class Diag15 : public Diag17                    { public: char a; };
class Diag16 : public Diag15                    { public: char a; };
class Diag13 : public Diag15                    { public: char a; };
class Diag14 : public Diag13                    { public: char a; };
class Diag11 : public Diag13                    { public: char a; };
class Diag12 : public Diag11                    { public: char a; };
class Diag10 : public Diag8, public Diag11      { public: char a; };
