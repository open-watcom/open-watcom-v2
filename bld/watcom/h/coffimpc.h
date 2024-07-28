/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  COFF import library symbols definitions.
*
****************************************************************************/


#ifndef COFFIMPC_H_INCLUDED
#define COFFIMPC_H_INCLUDED

#include "namelen.h"


#define IMPLIB_STRS \
    IMPLIB_STR( str_coff_idata2,                 ".idata$2" ) \
    IMPLIB_STR( str_coff_idata3,                 ".idata$3" ) \
    IMPLIB_STR( str_coff_idata4,                 ".idata$4" ) \
    IMPLIB_STR( str_coff_idata5,                 ".idata$5" ) \
    IMPLIB_STR( str_coff_idata6,                 ".idata$6" ) \
    IMPLIB_STR( str_coff_text,                   ".text" ) \
    IMPLIB_STR( str_coff_pdata,                  ".pdata" ) \
    IMPLIB_STR( str_coff_reldata,                ".reldata" ) \
    IMPLIB_STR( str_coff_toc,                    ".toc" ) \
    IMPLIB_STR( str_coff_ppc_prefix,             ".." ) \
    IMPLIB_STR( str_coff_imp_prefix,             "__imp_" ) \
    IMPLIB_STR( str_coff_null_thunk_data,        "_NULL_THUNK_DATA" ) \
    IMPLIB_STR( str_coff_import_descriptor,      "__IMPORT_DESCRIPTOR_" ) \
    IMPLIB_STR( str_coff_null_import_descriptor, "__NULL_IMPORT_DESCRIPTOR" )

#define IMPLIB_STR(s,t) extern name_len s;
IMPLIB_STRS
#undef IMPLIB_STR

#endif
