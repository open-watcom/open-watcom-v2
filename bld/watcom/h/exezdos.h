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
* Description:  ZDOS executable header.
*
****************************************************************************/


#ifndef _EXEZDOS_H
#define _EXEZDOS_H

#include "pushpck1.h"
typedef struct _zdos_exe_header {
    unsigned_32     signature;      /* signature to mark valid EXE file */
    unsigned_32     EIP;            /* initial EIP value                */
    unsigned_32     ESP;            /* initial ESP (marks end of BSS)   */
    unsigned_32     hdr_size;       /* size of header in bytes          */
    unsigned_32     chk_sum;        /* check sum                        */
    unsigned_32     image_size;     /* size of load image in bytes      */
    unsigned_32     image_offset;   /* offset of load image             */
    unsigned_32     extra_size;     /* unitialized data size in bytes   */
    unsigned_32     num_relocs;     /* number of relocation items       */
    unsigned_32     reloc_offset;   /* offset of first relocation item  */
    unsigned_32     reloc_base;     /* image base address               */
    unsigned_32     debug_offset;   /* offset of debug information      */
    unsigned_32     reserved[4];    /* reserved for future use          */
} zdos_exe_header;
#include "poppck.h"

#define ZDOS_SIGNATURE  0x20cd545a  /* 'ZT' followed by INT 20h         */

#endif
