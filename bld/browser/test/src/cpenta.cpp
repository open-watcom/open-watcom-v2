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


class PentagonRoot                                          {public: char a;};
class Pentagon1 : public PentagonRoot                       {public: char a;};
class Pentagon2 : public PentagonRoot                       {public: char a;};
class Pentagon3 : public PentagonRoot                       {public: char a;};

class Pentagon4 : public Pentagon1, public Pentagon2        {public: char a;};
class Pentagon6 : public Pentagon4                          {public: char a;};
class Pentagon5 : public Pentagon2, public Pentagon3        {public: char a;};
class Pentagon7 : public Pentagon5                          {public: char a;};

class Pentagon8 : public Pentagon1, public Pentagon6        {public: char a;};
class Pentagon9 : public Pentagon7, public Pentagon3        {public: char a;};
