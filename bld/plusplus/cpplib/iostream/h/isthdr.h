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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  92/02/04    Steve McDowell  Initial implementation.
//  92/09/08    Greg Bentz      Cleanup.
//  93/03/15    Greg Bentz      fix uninitialized state variables.
//  93/07/29    Greg Bentz      - change istream::op>>(streambuf &) to
//                                istream::op>>( streambuf * )
//                              - fix istream::getline() to not set failbit
//                                if no input stored in user buffer
//  93/09/15    Greg Bentz      change getline() back to set ios::failbit
//                              if not input stored in user buffer
//  93/10/21    Greg Bentz      change get() and getline() to not set failbit
//                              if the delim character has been seen
//  93/10/28    Raymond Tang    Split into separate files.
//  93/11/16    Greg Bentz      remove base parm from __getsign()
//  96/07/23    Greg Bentz      __int64 support

#ifndef _ISTHDR_H_INCLUDED
#define _ISTHDR_H_INCLUDED

#define ERR_CHAR    '\0'
extern ios::iostate __getaline( istream &, char *, int, char, int, int & );
extern ios::iostate __getsign( streambuf *, char & );
extern ios::iostate __getbase( streambuf *, int &, int & );
extern ios::iostate __getnumber( streambuf *, unsigned long &, int, int & );
extern ios::iostate __getunsignedlong( streambuf *, unsigned long &,
        unsigned long, signed long, ios::fmtflags );
extern ios::iostate __getnumberint64( streambuf *, unsigned __int64 &, int, int &);
extern ios::iostate __getunsignedint64( streambuf *, unsigned __int64 &,
        unsigned __int64, signed __int64, ios::fmtflags );

#endif
