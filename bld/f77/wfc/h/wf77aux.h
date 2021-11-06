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
* Description:  Auxiliary pragma information processing.
*
****************************************************************************/


#include <time.h>
#include "passby.h"
#include "cg.h"
#include "cgaux.h"
#include "auxflags.h"
#include "rtconst.h"


#if _CPU == 386
#define ARG_NEAR            ARG_SIZE_4
#define ARG_FAR             ARG_SIZE_8
#else
#define ARG_NEAR            ARG_SIZE_2
#define ARG_FAR             ARG_SIZE_4
#endif

typedef struct aux_info {
    call_class          cclass;
    byte_seq            *code;
    hw_reg_set          *parms;
    hw_reg_set          returns;
    hw_reg_set          streturn;
    hw_reg_set          save;
    char                *objname;
    unsigned            use;
    aux_flags           flags;
    pass_by             *arg_info;
    struct aux_info     *link;
    size_t              sym_len;
    char                sym_name[1];
} aux_info;

extern aux_info         ProgramInfo;

extern void             InitPragmaAux( void );
extern void             FiniPragmaAux( void );
extern void             PragmaAux( void );
extern void             PragmaLinkage( void );

#if _CPU == 386
extern void             CheckFar16Call( sym_id sp );
#endif
extern aux_info         *InfoLookup( sym_id sym );
extern call_handle      InitCall( RTCODE rtn_id );
extern void             InitRtRtns( void );
extern void             FreeRtRtns( void );
