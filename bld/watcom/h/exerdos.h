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
* Description:  RDOS executable headers.
*
****************************************************************************/

#ifndef _EXERDOS_H

#include "pushpck1.h"

/* 16-bit RDOS .rdv device file header */
/* =================== */

typedef struct rdos_dev16_header {
    unsigned_16         signature;      /* signature to mark valid RDV file */
    unsigned_16         IP;             /* initial value for IP             */
    unsigned_16         code_size;      /* number of bytes in code segment  */
    unsigned_16         code_sel;       /* code selector allocated */
    unsigned_16         data_size;      /* number of bytes in data segment  */
    unsigned_16         data_sel;       /* data selector allocated */
} rdos_dev16_header;

#define RDOS_SIGNATURE_16    0x3652     /* R6 */

/* 32-bit RDOS .rdv device file header */
/* =================== */

typedef struct rdos_dev32_header {
    unsigned_16         signature;      /* signature to mark valid RDV file */
    unsigned_32         EIP;            /* initial value for EIP            */
    unsigned_32         code_size;      /* number of bytes in code segment  */
    unsigned_16         code_sel;       /* code selector allocated */
    unsigned_32         data_size;      /* number of bytes in code segment  */
    unsigned_16         data_sel;       /* data selector allocated */
} rdos_dev32_header;

#define RDOS_SIGNATURE_32    0x3252     /* R2 */

#define _EXERDOS_H

#include "poppck.h"

#endif
