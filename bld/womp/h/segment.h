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


#ifndef SEGMENT_H
#define SEGMENT_H   1
typedef struct seghdr seghdr;

#include <watcom.h>
#include "fixup.h"
#include "objrec.h"

struct seghdr {
    obj_rec         *segdef;    /* segdef record for this segment           */
    fixup           *fix_first; /* ordered list of fixups for this segment  */
    uint_32         alloc;      /* size of allocated data                   */
    uint_8          *data;      /* data for segment                         */
};

/*
    Note that fixups passed to SegAddFix, and returned from SegFindFix have
    the absolute offset within the segment of the location to be fixed up
    in loc_offset.  Therefore, you must ensure that any fix->loc_offset is
    set properly.  (i.e., When reading, you should add the offset of the
    most recent LEDATA; when writing you should subtract the offset of the
    most recent LEDATA.)
*/

extern seghdr   *SegNew( void );
extern void     SegKill( seghdr *seg );
extern void     SegAllocData( seghdr *seg, uint_16 seg_len );
extern void     SegDetachData( seghdr *seg );
extern void     SegAddFix( seghdr *seg, fixup *fix );
extern fixup    *SegFindFix( seghdr *seg, uint_32 offset );

#endif

