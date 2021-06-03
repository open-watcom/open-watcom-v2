/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef __INDEX_H_INCLUDED__
#define __INDEX_H_INCLUDED__

#include "watcom.h"


/* FILE format
 *
 * HelpHeader
 * DefaultStrings
 * PageItemNumIndex
 * HelpIndexPages
 * HelpDataPages
 * HelpText
 */

/*
 * DefaultStrings
 *      uint_16 ( str_cnt )             - number of strings
 *      uint_16 (* str_cnt)             - length of each string
 *      char* (*str_cnt)                - default strings
 */

/* PageItemNumIndex
 * uint_16      (* datapagecnt) -the index of the first item for the
 *                               corrosponding data page
 */
/* HelpIndexPage
 *
 * HelpPageHeader
 *      HelpIndexEntry (* num_entries)
 */

/* HelpDataPage
 *
 * HelpPageHeader
 *      PageIndexEntry (* num_entries)
 *      StringBlock
 */


#define HLP_PAGE_SIZE                   1024
#define INDEX_ENTRIES_PER_PAGE          \
        ( ( HLP_PAGE_SIZE - sizeof( HelpPageHeader ) ) / sizeof( HelpIndexEntry ) )

#define HELP_SIG_1              0x1359ddcc
#define HELP_SIG_2              0x95843561
#define HELP_MAJ_V1             1
#define HELP_MAJ_VER            2
#define HELP_MIN_VER            0

typedef struct {
    uint_32     sig1;
    uint_32     sig2;
    uint_16     ver_maj;
    uint_16     ver_min;
    uint_16     indexpagecnt;
    uint_16     datapagecnt;
    uint_32     topiccnt;
    uint_16     str_size;       /* does not exist in version 1 header */
//    uint_16     unused[6];
} HelpHeader;

#define HELP_HEADER_SIZE        (3 * sizeof( uint_32 ) + 11 * sizeof( uint_16 ))
#define HELP_HEADER_V1_SIZE     (3 * sizeof( uint_32 ) + 10 * sizeof( uint_16 ))

enum {
    PAGE_DATA,
    PAGE_INDEX
};

#include "pushpck1.h"

typedef struct {
    uint_16     type;
    uint_16     num_entries;
    uint_16     page_num;
} HelpPageHeader;

#define INDEX_LEN       20
typedef struct {
    uint_16     nextpage;
    char        start[INDEX_LEN];
} HelpIndexEntry;

typedef struct {
    uint_16     name_offset;
    uint_32     entry_offset;
} PageIndexEntry;

#include "poppck.h"

#endif
