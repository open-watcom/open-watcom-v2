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
* Description:  public enumeration for cg operators and types
*
****************************************************************************/


#ifndef _CG_DEFS_INCLUDED
#define _CG_DEFS_INCLUDED

/*  Start of Internal Code Generator Operators ... Must correspond to */
/*  code generator header file <opcodes.h> */

typedef enum {

#define PICK(e,i,d1,d2,ot,pnum,attr)  O_##e,
#include "cgops.h"
#undef PICK

/*  Beginning of FRONT END SPECIFIC Operators */
/* */
/*  - Bonus plan. Add your own ops here! */
/* */
/*  End       of FRONT END SPECIFIC Operators */

        MAX_OP

} cg_op;

typedef enum {

/*  The first part must correspond to <typclass> */

        TY_UINT_1,       /*   0 */
        TY_INT_1,
        TY_UINT_2,
        TY_INT_2,
        TY_UINT_4,
        TY_INT_4,
        TY_UINT_8,
        TY_INT_8,

        TY_LONG_POINTER,
        TY_HUGE_POINTER,
        TY_NEAR_POINTER,
        TY_LONG_CODE_PTR,
        TY_NEAR_CODE_PTR,

        TY_SINGLE,
        TY_DOUBLE,
        TY_LONG_DOUBLE,

        TY_UNKNOWN,
        TY_DEFAULT,      /*  11  Use defaults */

        TY_INTEGER,      /*  Default integer */
        TY_UNSIGNED,     /*  Default unsigned */
        TY_POINTER,      /*  Default data pointer */
        TY_CODE_PTR,     /*  Default code pointer */
        TY_BOOLEAN,      /*  Resultant type for O_FLOW, comparison ops */

        TY_PROC_PARM,    /*  For Pascal procedural parameters */
        TY_VA_LIST,      /*  For RISC-based O_VA_START support */

        TY_FIRST_FREE,   /*  First user definable type */

#if defined( BY_FORTRAN_FRONT_END )
  #include "fetypes.h"
#endif

        TY_LAST_FREE = 65530U - 1,

        TY_NEAR_INTEGER,
        TY_LONG_INTEGER,
        TY_HUGE_INTEGER
} cg_type;

#define TY_HUGE_CODE_PTR TY_LONG_CODE_PTR  /* for now */

#define MIN_OP          O_NOP
#define O_FIRST_COND    O_EQ
#define O_LAST_COND     O_GE
#define O_FIRST_FLOW    O_FLOW_AND
#define O_LAST_FLOW     O_FLOW_NOT

#endif
