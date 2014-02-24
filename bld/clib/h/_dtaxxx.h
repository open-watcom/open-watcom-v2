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
* Description:  internal DTA structure declaration 
*               definition of macros for manipulation with structure members
*
****************************************************************************/

/*
 * DTA in directory/find structure is used only by DOS and _dos_find...
 * functions. If LFN version of _dos_find... functions are used then
 * conversion between LFN DTA <=> SFN DTA is made if access
 * DOS "find first/next" SFN functions.
 * For other systems it is used internaly to hold auxiliary data
 * The length of DTA is 21 bytes, that any internal structure must
 * respect this limit
 */

#if defined( __DOS__ ) && defined( __WATCOM_LFN__ )

#include "pushpck1.h"
typedef struct __doslfn_dta {
    unsigned short  cr_time;
    unsigned short  cr_date;
    unsigned short  ac_time;
    unsigned short  ac_date;
    unsigned long   sign;
    unsigned short  handle;
} __doslfn_dta;
#include "poppck.h"

#define LFN_CRTIME_OF(x)    (((__doslfn_dta *)(x)->reserved)->cr_time)
#define LFN_CRDATE_OF(x)    (((__doslfn_dta *)(x)->reserved)->cr_date)
#define LFN_ACTIME_OF(x)    (((__doslfn_dta *)(x)->reserved)->ac_time)
#define LFN_ACDATE_OF(x)    (((__doslfn_dta *)(x)->reserved)->ac_date)
#define LFN_HANDLE_OF(x)    (((__doslfn_dta *)(x)->reserved)->handle)
#define LFN_SIGN_OF(x)      (((__doslfn_dta *)(x)->reserved)->sign)

#elif defined( __NT__ )

typedef struct __nt_dta {
    HANDLE      hndl;
    DWORD       attr;
} __nt_dta;

#define DIR_HANDLE_OF(__dirp)   (((__nt_dta *)(__dirp)->d_dta)->hndl)
#define DIR_ATTR_OF(__dirp)     (((__nt_dta *)(__dirp)->d_dta)->attr)
#define FIND_HANDLE_OF(__find)  (((__nt_dta *)(__find)->reserved)->hndl)
#define FIND_ATTR_OF(__find)    (((__nt_dta *)(__find)->reserved)->attr)

#define BAD_HANDLE              ((HANDLE)(~0))

#elif defined( __OS2__ )

typedef struct __os2_dta {
    HDIR        hndl;
} __os2_dta;

#define DIR_HANDLE_OF(__dirp)   (((__os2_dta *)(__dirp)->d_dta)->hndl)
#define FIND_HANDLE_OF(__find)  (((__os2_dta *)(__find)->reserved)->hndl)

#define BAD_HANDLE              ((HDIR)(~0))

#endif
