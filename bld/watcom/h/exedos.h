/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS MZ executable header.
*
****************************************************************************/


#ifndef _EXEDOS_H
#define _EXEDOS_H

#include "exesigns.h"


/* DOS EXE file header */
/* =================== */

#define DOS_RELOC_OFFSET        0x0018L
#define NE_HEADER_OFFSET        0x003cL
#define NE_HEADER_FOLLOWS(x)    ((x) >= 0x0040) /* reloc table offset 0x40 */

#include "pushpck1.h"
typedef struct dos_exe_header {
    unsigned_16         signature;      /* signature to mark valid EXE file */
    unsigned_16         mod_size;       /* length of image mod 512          */
    unsigned_16         file_size;      /* number of 512 byte pages         */
    unsigned_16         num_relocs;     /* number of relocation items       */
    unsigned_16         hdr_size;       /* size of header (in paragraphs)   */
    unsigned_16         min_16;         /* minimum # of paragraphs          */
    unsigned_16         max_16;         /* maximum # of paragraphs          */
    unsigned_16         SS_offset;      /* offset of SS within load module  */
    unsigned_16         SP;             /* value for SP                     */
    unsigned_16         chk_sum;        /* check sum                        */
    unsigned_16         IP;             /* value for IP                     */
    unsigned_16         CS_offset;      /* offset of CS within load module  */
    unsigned_16         reloc_offset;   /* offset to 1st relocation item    */
    unsigned_16         overlay_num;    /* overlay number (0 if resident)   */
} dos_exe_header;
#include "poppck.h"

#endif
