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


#ifndef _CGSEGID_H_
#define _CGSEGID_H_

#if TARGET_UINT_MAX < 0x010000
#define _CHECK_SIZE( n ) \
        ((n) > (TARGET_UINT_MAX+1))
#define _CHECK_ADJUST( a, n, o ) \
        if( ((n) < (o)) || _CHECK_SIZE((n)) ) { \
            a = 0; \
        }
#else
#define _CHECK_ADJUST( a, n, o ) \
        if( (n) < (o) ) { \
            a = 0; \
        }
#endif

typedef enum {
    SI_DEFAULT      =0x00,  // default behaviour
    SI_ALL_ZERO     =0x01,  // all data for this symbol is zero
    SI_NEEDS_CODE   =0x02,  // data needs code to initialize it
} SEGID_CONTROL;

fe_seg_id CgSegId( SYMBOL );
fe_seg_id CgSegIdData( SYMBOL, SEGID_CONTROL );
fe_seg_id CgSegIdFunction( SYMBOL );

#endif
