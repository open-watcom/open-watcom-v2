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


class LarryRoot                                             {public: int a; };
class Larry1 : public LarryRoot                             {public: int a; };
class Larry2                                                {public: int a; };
class Larry3 : public Larry1, public Larry2                 {public: int a; };
class Larry5                                                {public: int a; };
class Larry6                                                {public: int a; };
class Larry4 : public Larry3, public Larry5, public Larry6  {public: int a; };
class Larry8                                                {public: int a; };
class Larry7 : public Larry6, public Larry8                 {public: int a; };
class Larry9 : public Larry8                                {public: int a; };

class Larry12 : public LarryRoot                            {public: int a; };
class Larry11 : public Larry12                              {public: int a; };
class Larry10 : public Larry11                              {public: int a; };
