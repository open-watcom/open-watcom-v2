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


/*      INLINE FUNCTIONS */

aux_info        InlineInfo;

/*
  here come the code bursts ...
*/

#include "pragdefn.h"
#include "code386.gh"

#define PRAGMADEF(p,x)    #x, (byte_seq *)&p##x, p##x##_parms, p##x##_ret, p##x##_saves

static inline_funcs  OptTimeInlineTab[] = {
    PRAGMADEF( __RTIStrBlastEq, OT ), TY_INTEGER, NULL, NULL,
    PRAGMADEF( __RTIStrBlastNe, OT ), TY_INTEGER, NULL, NULL,
};

static inline_funcs  OptSpaceInlineTab[] = {
    PRAGMADEF( __RTIStrBlastEq, OS ), TY_INTEGER, NULL, NULL,
    PRAGMADEF( __RTIStrBlastNe, OS ), TY_INTEGER, NULL, NULL,
};

static hw_reg_set P_EDI_ESI_ECX_EAX[] = {
    HW_D( HW_EDI ),
    HW_D( HW_ESI ),
    HW_D( HW_ECX ),
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_EDI_EAX_ESI_ECX[] = {
    HW_D( HW_EDI ),
    HW_D( HW_EAX ),
    HW_D( HW_ESI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_EDI_ESI_ECX[] = {
    HW_D( HW_EDI ),
    HW_D( HW_ESI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static inline_funcs  *InlineTab = OptTimeInlineTab;

#define MAX_IN_INDEX    (sizeof( OptTimeInlineTab ) / sizeof( inline_funcs ))
