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


#ifndef LGADV_H_INCLUDED
#define LGADV_H_INCLUDED

typedef struct lg_adv {
    void        PGM *origin;        // pointer to array storage
    struct adv  PGM *adv_link;      // ADV link (chains all ADV's)
    int         elt_size;           // element size
    unsigned_32 num_elts;           // number of elements
    unsigned_16 num_ss;             // number of subscripts
    adv_entry   subscrs[MAX_DIM];   // bounds & elements in each dimension
} lg_adv;

#define ADV_ALLOC_LOC   0x2000      // ADV was allocated with location=
#define ADV_NOT_INIT    0x4000      // ADV has not been initialized
#define ADV_NO_SCALE    0x8000      // element size if not a power of 2
#define ADV_SUBSCRS     0x0007      // number of subscripts field
#define ADV_FIELD       0x0080      // ADV is part of a structure defn

#define ADV_BASE_SIZE   offsetof( lg_adv, subscrs )
#define ADV_DIM_SIZE    sizeof( adv_entry )

#endif
