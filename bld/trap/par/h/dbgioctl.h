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



// Device type           -- in the "User Defined" range."
#define DBG_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_DBG_READ_PORT_U8 \
    CTL_CODE( DBG_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_DBG_READ_PORT_U16 \
    CTL_CODE( DBG_TYPE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_DBG_READ_PORT_U32 \
    CTL_CODE( DBG_TYPE, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_DBG_WRITE_PORT_U8 \
    CTL_CODE(DBG_TYPE,  0x810, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_DBG_WRITE_PORT_U16 \
    CTL_CODE(DBG_TYPE,  0x811, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_DBG_WRITE_PORT_U32 \
    CTL_CODE(DBG_TYPE,  0x812, METHOD_BUFFERED, FILE_WRITE_ACCESS)

typedef struct  {
    ULONG       port;
    union   {
        UCHAR   u8;
        USHORT  u16;
        ULONG   u32;
    } data;
}   DBGPORT_IO;
