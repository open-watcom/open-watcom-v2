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
* Description:  VxD device description block
*
****************************************************************************/

#ifndef _VXD_DDB_H
#define _VXD_DDB_H

typedef struct {
    unsigned_32 next;
    unsigned_16 SDK_version;
    unsigned_16 req_device_number;
    unsigned char device_major_version;
    unsigned char device_minor_version;
    unsigned_16 flags;
    unsigned char name[8];
    unsigned_32 init_order;
    unsigned_32 control_proc;
    unsigned_32 V86_API_proc;
    unsigned_32 PM_API_proc;
    unsigned_32 V86_API_CSIP;
    unsigned_32 PM_API_CSIP;
    unsigned_32 reference_data;
    unsigned_32 service_table_ptr;
    unsigned_32 service_table_size;
    //********************
    // Windows 9x, Me
    //********************
    unsigned_32 win32_service_table;
    unsigned_32 prev;
    unsigned_32 size;
    unsigned_32 reserved1;
    unsigned_32 reserved2;
    unsigned_32 reserved3;
    //********************
} vxd_ddb;

#endif
