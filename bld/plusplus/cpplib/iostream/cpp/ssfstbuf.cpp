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
//  92/02/10    Steve McDowell  Initial implementation.
//  92/02/28    ...             Modified to delay allocation of buffers
//                              until overflow/underflow called.
//  92/09/08    Greg Bentz      Cleanup.
//  93/10/21    Raymond Tang    Split into separate files.
//  93/11/12    Raymond Tang    Let the end of reserve area be the end of get
//                              area.
//  94/04/06    Greg Bentz      combine header files

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <string.h>
#include <strstrea.h>
#endif
#include "ioutil.h"

void strstreambuf::__strstreambuf( char *ptr, int size, char *pstart ) {
/**********************************************************************/
// Initialize a streambuf.
// If "ptr" is NULL, then leave the streambuf as a dynamic strstreambuf
// with no pre-allocated buffer.
// Otherwise, create a static (non-dynamic) streambuf using the specified area.
// If size > 0, it is the size of the streambuf area.
// If size = 0, "ptr" points to a C-string whose length determines the size
//              of the streambuf area.
// If size < 0, "ptr" points to an "unlimited" streambuf area. (!)
// If pstart = NULL, this is an input-only streambuf.
// Otherwise, it partitions the ptr/size area into a get area
// (from ptr to pstart-1), and a put area (from pstart to ptr+size-1).

    char *get;
    char *eget;
    char *put;
    char *eput;

    __lock_it( __b_lock );
    __dynamic         = 0;
    __unlimited       = 0;
    __frozen          = 0;
    __alloc_fn        = NULL;
    __free_fn         = NULL;
    __allocation_size = 32;
    __minbuf_size     = 0;
    if( ptr == NULL ) {                     // no buffer to set up?
        __dynamic = 1;
        return;
    }
    get = ptr;
    if( size > 0 ) {
        eget = get + size;
    } else if( size == 0 ) {
        eget = get + ::strlen( get );       // use up NULLCHAR, too!
    } else {
        __unlimited = 1;
        eget = get + DEFAULT_MAINBUF_SIZE;
    }
    setb( get, eget, FALSE );
    if( (pstart == NULL)
      ||(pstart < get)
      ||( (pstart >= eget) && (!__unlimited) ) ) {
        pstart = NULL;
        put  = NULL;
        eput = NULL;
    } else {
        put  = get;
        eput = eget;
    }
    setg( get, get, eget );
    setp( put, eput );
    if( pstart > put ) {
        pbump( pstart - put );
    }
}
