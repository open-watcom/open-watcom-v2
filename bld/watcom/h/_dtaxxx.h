/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  internal DTA structure declaration
*               definition of macros for manipulation with structure members
*
****************************************************************************/

/*
 * DTA in directory/find structure is used only by DOS version of appropriate
 * functions. If DOS LFN version of _dos_find... functions are used then
 * conversion between LFN DTA <=> SFN DTA is made if access DOS
 * "find first/next" SFN functions.
 * For other systems it is used internaly to hold auxiliary data
 * The length of DTA is 21 bytes, that any internal structure must
 * respect this limit
 */

#if defined( __DOS__ ) && defined( __WATCOM_LFN__ )

#include "pushpck1.h"
typedef struct __doslfn_dta {
    unsigned short  cr_time;    /* creation time */
    unsigned short  cr_date;    /* creation date */
    unsigned short  ac_time;    /* last access time */
    unsigned short  ac_date;    /* last access date */
    unsigned long   sign;       /* LFN signature */
    unsigned short  handle;     /* LFN search handle */
} __doslfn_dta;
#include "poppck.h"

#define DTALFN_CRTIME_OF(x)     (((__doslfn_dta *)(x))->cr_time)
#define DTALFN_CRDATE_OF(x)     (((__doslfn_dta *)(x))->cr_date)
#define DTALFN_ACTIME_OF(x)     (((__doslfn_dta *)(x))->ac_time)
#define DTALFN_ACDATE_OF(x)     (((__doslfn_dta *)(x))->ac_date)
#define DTALFN_HANDLE_OF(x)     (((__doslfn_dta *)(x))->handle)
#define DTALFN_SIGN_OF(x)       (((__doslfn_dta *)(x))->sign)

#elif defined( __NT__ )

#include "pushpck1.h"
typedef struct __nt_dta {
    void        *hndl;
    time_t      tstamp;
    unsigned    attr;
} __nt_dta;
#include "poppck.h"

#define DTAXXX_HANDLE_OF(x)     (((__nt_dta *)(x))->hndl)
#define DTAXXX_TSTAMP_OF(x)     (((__nt_dta *)(x))->tstamp)
#define DTAXXX_ATTR_OF(x)       (((__nt_dta *)(x))->attr)

#define DTAXXX_INVALID_HANDLE   ((void *)(~0))

#elif defined( __OS2__ )

#include "pushpck1.h"
typedef struct __os2_dta {
    unsigned    hndl;
} __os2_dta;
#include "poppck.h"

#define DTAXXX_HANDLE_OF(x)     (((__os2_dta *)(x))->hndl)

#define DTAXXX_INVALID_HANDLE   ((unsigned)(~0))

#endif
