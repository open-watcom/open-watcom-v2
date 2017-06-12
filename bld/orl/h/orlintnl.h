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
* Description: Internal ORL interfaces.
*
****************************************************************************/


#ifndef ORL_INTERNAL_INCLUDED
#define ORL_INTERNAL_INCLUDED

/* included in the ORL level through the lower level */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "orlglobl.h"

#if !defined( __386__ ) && !defined( __GNUC__ )
#define ORLUNALIGNED __unaligned
#else
#define ORLUNALIGNED
#endif

#define ORL_CLI_READ(x,a,b)             (x)->cli_read(a,b)
#define ORL_CLI_SEEK(x,a,b,c)           (x)->cli_seek(a,b,c)
#define ORL_CLI_ALLOC(x,a)              (x)->cli_alloc(a)
#define ORL_CLI_FREE(x,a)               (x)->cli_free(a)

#define ORL_FUNCS_READ(x,a,b)           (x)->funcs.cli_read(a,b)
#define ORL_FUNCS_SEEK(x,a,b,c)         (x)->funcs.cli_seek(a,b,c)
#define ORL_FUNCS_ALLOC(x,a)            (x)->funcs.cli_alloc(a)
#define ORL_FUNCS_FREE(x,a)             (x)->funcs.cli_free(a)

#define ORL_PTR_READ(x,a,b)             (x)->funcs->cli_read(a,b)
#define ORL_PTR_SEEK(x,a,b,c)           (x)->funcs->cli_seek(a,b,c)
#define ORL_PTR_ALLOC(x,a)              (x)->funcs->cli_alloc(a)
#define ORL_PTR_FREE(x,a)               (x)->funcs->cli_free(a)


#define LCL_SYM(s)          lcl_ ## s
#define LCL_CONST_SYM(s)    const_lcl_ ## s
#define ORL_LCL_STRUCT(s)   struct lcl_ ## s ## _struct

#define TYPEDEF_LCL_TYPE(t)  \
    typedef const ORL_LCL_STRUCT( t ) * LCL_CONST_SYM( t ); \
    typedef ORL_LCL_STRUCT( t ) * LCL_SYM( t )

#define TYPEDEF_LOCAL_TYPE(t) \
    typedef const ORL_STRUCT( t ) * const_ ## t; \
    typedef ORL_STRUCT( t ) * t


/* NB The following are sort of fake.  We are hiding the contents of
   the file-specific section and symbol handles from the ORL
   level, allowing it only to check their type. */

#define LCL_SEC_HND(p)          ((ORL_LCL_STRUCT( orl_sec_handle ) *)p)
#define LCL_SYM_HND(p)          ((ORL_LCL_STRUCT( orl_symbol_handle ) *)p)
#define LCL_GRP_HND(p)          ((ORL_LCL_STRUCT( orl_group_handle ) *)p)

TYPEDEF_LCL_TYPE( orl_sec_handle );
TYPEDEF_LCL_TYPE( orl_symbol_handle );
TYPEDEF_LCL_TYPE( orl_group_handle );

ORL_LCL_STRUCT( orl_sec_handle ) {
    orl_file_format         type;
};

ORL_LCL_STRUCT( orl_symbol_handle ) {
    orl_file_format         type;
};

ORL_LCL_STRUCT( orl_group_handle ) {
    orl_file_format         type;
};

#include "orlhshdf.h"
#include "orlcomon.h"

#endif
