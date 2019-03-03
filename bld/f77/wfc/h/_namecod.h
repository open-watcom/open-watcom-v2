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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// constants used for parameter to ReqName and for error reporting
/*    enum                  class           msgid           */
pick( NAME_PARAMETER,       "PARAMETER",    0               )
pick( NAME_STMT_FUNC,       NULL,           MS_STMT_FUNC    )   // statement function
pick( NAME_SUBROUTINE,      "SUBROUTINE",   0               )
pick( NAME_REM_BLOCK,       "REMOTE BLOCK", 0               )
pick( NAME_FUNCTION,        "FUNCTION",     0               )
pick( NAME_BLOCK_DATA,      "BLOCK DATA",   0               )
pick( NAME_PROGRAM,         "PROGRAM",      0               )
pick( NAME_VARIABLE,        NULL,           MS_SIMP_VAR     )   // simple variable
pick( NAME_ARGUMENT,        NULL,           MS_SP_ARG       )   // subprogram argument
pick( NAME_SF_DUMMY,        NULL,           MS_SF_ARG       )   // statement function argument
pick( NAME_ARRAY,           NULL,           MS_ARRAY        )   // array
pick( NAME_COMMON_VAR,      NULL,           MS_COM_VAR      )   // variable in COMMON
pick( NAME_VAR_OR_ARR,      NULL,           MS_VAR_ARR      )   // variable or array
pick( NAME_EXT_PROC,        NULL,           MS_EXT_PROC     )   // external subprogram
pick( NAME_INTRINSIC,       NULL,           MS_INTR_FUNC    )   // intrinsic function
pick( NAME_COMMON,          "COMMON BLOCK", 0               )
pick( NAME_EQUIV_VAR,       NULL,           MS_EQUIV_VAR    )   // equivalenced variable
pick( NAME_STRUCTURE,       NULL,           MS_STRUCT_DEFN  )   // structure definition
pick( NAME_GROUP,           NULL,           MS_GROUP_NAME   )   // group name
pick( NAME_ALLOCATED_ARRAY, NULL,           MS_ALLOC_ARRAY  )   // allocated array
