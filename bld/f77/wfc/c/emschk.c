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


//
// EMSCHK       : check for existence of EMS
//

#include "ftnstd.h"

#include <dos.h>

extern  int     _EMS_available(void);
extern  int     _EMS_get_number_of_pages(unsigned int *,unsigned int *);
extern  int     _EMS_map_memory(int,int,int);
extern  int     _EMS_allocate_memory(unsigned int,int *);
extern  int     _EMS_get_page_frame_segment(unsigned int *);

extern  unsigned int    EMSsegment;
extern  int             EMShandle;


void    InitEMS() {
//=================

// Initialize EMS.

    unsigned            total;
    unsigned            unallocated;
    unsigned            index;
    unsigned int        __FAR *p;

    EMSsegment = 0;
    if( _EMS_available() ) {
        _EMS_get_number_of_pages( &unallocated, &total );
        _EMS_get_page_frame_segment( &EMSsegment );
        if( _EMS_allocate_memory( 4, &EMShandle ) == 0 ) {
            for( index = 0; index < 4; ++index ) {
                _EMS_map_memory( EMShandle, index, index );
            }
            p = MK_FP( EMSsegment, 0 );     // point to memory
            *p = 0xFFFE;                    // set size to 64K - 2
        }
    }
}
