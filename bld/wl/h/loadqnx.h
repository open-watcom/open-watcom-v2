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


// the first 4 fields must be the same as the seg_flags structure defined in
// objstruc.h

typedef struct qnx_seg_flags {
    struct qnx_seg_flags *  next;
    unsigned_16             flags;      // as above.
    char *                  name;
    bool                    isclass;    // true if flags for a class.
} qnx_seg_flags;

#define QNX_PRIV_SHIFT  0x0002
#define QNX_RES_FILE    0x0080
#define QNX_FLAG_MASK   0x007F

#define QNX_SELECTOR( num ) (((num) << 3) | 4 | FmtData.u.qnx.priv_level)

#define QNX_SEL_NUM(selector) ((selector) >> 3)

#define QNX_DEFAULT_BASE 0x1000

#define QNX_GROUP_ALIGN 0x1000

extern void             SetQNXSegFlags( void );
extern void             FiniQNXLoadFile( void );
extern unsigned_16      ToQNXSel( unsigned_16 );
extern unsigned_16      ToQNXIndex( unsigned_16 );
