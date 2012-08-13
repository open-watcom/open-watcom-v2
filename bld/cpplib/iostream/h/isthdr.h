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

#ifndef _ISTHDR_H_INCLUDED
#define _ISTHDR_H_INCLUDED

#define ERR_CHAR    '\0'
extern std::ios::iostate __getaline( std::istream &, char *, int, char, int, int & );
extern std::ios::iostate __getsign( std::streambuf *, char & );
extern std::ios::iostate __getbase( std::streambuf *, int &, int & );
extern std::ios::iostate __getnumber( std::streambuf *, unsigned long &, int, int & );
extern std::ios::iostate __getunsignedlong( std::streambuf *,
                                            unsigned long &,
                                            unsigned long,
                                            signed long,
                                            std::ios::fmtflags );
extern std::ios::iostate __getnumberint64( std::streambuf *, unsigned __int64 &, int, int &);
extern std::ios::iostate __getunsignedint64( std::streambuf *,
                                             unsigned __int64 &,
                                             unsigned __int64,
                                             signed __int64,
                                             std::ios::fmtflags );

#endif
