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
//  92/02/19    Steve McDowell  Initial implementation.
//  92/02/28    ...             Modified to delay allocation of buffers
//                              until overflow/underflow called.
//  92/09/08    Greg Bentz      Cleanup.
//  93/03/22    Greg Bentz      modify filebuf::open() and filebuf::attach()
//                              to assume ios::binary unless ios::text is
//                              found in the fstat() so that QNX is supported.
//  93/07/22    Greg Bentz      Make sure overflow() function sets up the
//                              put area
//  93/09/13    Greg Bentz      filebuf::~filebuf() must close if !__attached
//  93/10/15    Greg Bentz      let __plusplus_open() determine if default
//                              file mode is TEXT or BINARY
//  93/10/15    Raymond Tang    Modify filebuf::open() to return NULL if both
//                              ios::noreplace and ios::nocreate are specified
//  93/10/22    Raymond Tang    Split into separate files.
//  94/04/06    Greg Bentz      combine header files

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream.h>
#endif


streampos filebuf::seekoff( streamoff offset,
                            ios::seekdir dir,
                            ios::openmode ) {
/*******************************************/
// Seek to a new position in the file.

    int       origin;
    streampos newpos;

    __lock_it( __b_lock );
    if( fd() == EOF ) {
        return( EOF );
    }
    if( out_waiting() || in_avail() ) {
        if( sync() == EOF ) {           // flush buffers
            return( EOF );
        }
    }
    switch( dir ) {
    case ios::beg:
        origin = SEEK_SET;
        break;
    case ios::cur:
        origin = SEEK_CUR;
        break;
    case ios::end:
        origin = SEEK_END;
        break;
    default:
        return( EOF );
    }
    newpos = lseek( fd(), offset, origin );
    if( newpos == -1 ) {
        return( EOF );
    }
    return( newpos );
}
