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
* Description:  Autdependency structure defintion shared between resource
*               compiler and wmake.
*
****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#define         DEP_LIST_TYPE   0x79
#define         DEP_LIST_NAME   "EBWF_XFMMTUPPE"

#include "pushpck1.h"

typedef struct {
    uint_32     time;           /* file's time taken from stat */
    uint_16     len;            /* sizeof the name array */
    char        name[1];        /* dynamic array */
} _WCUNALIGNED DepInfo;

#include "poppck.h"

DepInfo *WResGetAutoDep( char *fname );
void WResFreeAutoDep( DepInfo *ptr );

#ifdef __cplusplus
}
#endif
