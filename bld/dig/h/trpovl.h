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
* Description:  Overlay debugging supplementary trap requests.
*
****************************************************************************/


#ifndef TRPOVL_H

#include "trptypes.h"

#include "pushpck1.h"

#define OVERLAY_SUPP_NAME               "Overlays"

typedef struct {
    addr32_ptr  mach;
    unsigned_16 sect_id;
} ovl_address;


enum {
    REQ_OVL_STATE_SIZE,         /* 00 */
    REQ_OVL_GET_DATA,           /* 01 */
    REQ_OVL_READ_STATE,         /* 02 */
    REQ_OVL_WRITE_STATE,        /* 03 */
    REQ_OVL_TRANS_VECT_ADDR,    /* 04 */
    REQ_OVL_TRANS_RET_ADDR,     /* 05 */
    REQ_OVL_GET_REMAP_ENTRY     /* 06 */
};


typedef struct {
    supp_prefix         supp;
    access_req          req;
} ovl_state_size_req;

typedef struct {
    unsigned_16         size;
} ovl_state_size_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_16         sect_id;
} _WCUNALIGNED ovl_get_data_req;

typedef struct {
    unsigned_16         segment;
    unsigned_32         size;
} _WCUNALIGNED ovl_get_data_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
} ovl_read_state_req;

/* return message is state information data */

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by state information data */
} ovl_write_state_req;

/* no ovl_write_state_ret */

typedef struct {
    supp_prefix         supp;
    access_req          req;
    ovl_address         ovl_addr;
} _WCUNALIGNED ovl_trans_vect_addr_req;

typedef struct {
    ovl_address         ovl_addr;
} ovl_trans_vect_addr_ret;

typedef ovl_trans_vect_addr_req         ovl_trans_ret_addr_req;
typedef ovl_trans_vect_addr_ret         ovl_trans_ret_addr_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    ovl_address         ovl_addr;
} _WCUNALIGNED ovl_get_remap_entry_req;

typedef struct {
    unsigned_8          remapped;
    ovl_address         ovl_addr;
} _WCUNALIGNED ovl_get_remap_entry_ret;

#include "poppck.h"

#define TRPOVL_H

#endif
