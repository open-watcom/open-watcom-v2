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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 *  ORLSTUBS -- stubs for removing object reading library
 *
*/

#include "orl.h"

extern orl_handle ORLInit( orl_funcs *funcs )
{
    funcs = funcs;
    return NULL;
}

extern orl_return ORLFini( orl_handle hdl )
{
    hdl = hdl;
    return ORL_OKAY;
}

extern orl_file_format ORLFileIdentify( orl_handle hdl, void *data )
{
    hdl = hdl;
    data = data;
    return ORL_GUESS;
}

extern orl_file_handle ORLFileInit( orl_handle a, void *b, orl_file_format c )
{
    a = a;
    b = b;
    c = c;
    return NULL;
}

extern orl_return ORLFileFini( orl_file_handle a )
{
    a = a;
    return ORL_ERROR;
}

extern orl_return ORLFileScan( orl_file_handle a, char * b,
                                orl_sec_return_func c )
{
    a = a;
    b = b;
    c = c;
    return ORL_ERROR;
}

extern orl_machine_type ORLFileGetMachineType( orl_file_handle a )
{
    a = a;
    return ORL_MACHINE_TYPE_NONE;
}

extern orl_file_flags ORLFileGetFlags( orl_file_handle a )
{
    a = a;
    return 0;
}

extern orl_file_type ORLFileGetType( orl_file_handle a )
{
    a = a;
    return ORL_FILE_TYPE_NONE;
}

extern char * ORLSecGetName( orl_sec_handle a )
{
    a = a;
    return NULL;
}

extern orl_sec_size ORLSecGetSize( orl_sec_handle a )
{
    a = a;
    return 0;
}

extern orl_sec_type ORLSecGetType( orl_sec_handle a )
{
    a = a;
    return ORL_SEC_TYPE_NONE;
}

extern orl_sec_flags ORLSecGetFlags( orl_sec_handle a )
{
    a = a;
    return 0;
}

extern orl_sec_alignment ORLSecGetAlignment( orl_sec_handle a )
{
    a = a;
    return 0;
}

extern orl_return ORLSecGetContents( orl_sec_handle a, char **b )
{
    a = a;
    b = b;
    return ORL_ERROR;
}

extern orl_return ORLRelocSecScan( orl_sec_handle a, orl_reloc_return_func b )
{
    a = a;
    b = b;
    return ORL_ERROR;
}

extern orl_return ORLSymbolSecScan( orl_sec_handle a, orl_symbol_return_func b )
{
    a = a;
    b = b;
    return ORL_ERROR;
}

extern char * ORLSymbolGetName( orl_symbol_handle a )
{
    a = a;
    return NULL;
}

extern orl_symbol_value ORLSymbolGetValue( orl_symbol_handle a )
{
    a = a;
    return 0;
}

extern orl_symbol_binding ORLSymbolGetBinding( orl_symbol_handle a )
{
    a = a;
    return ORL_SYM_BINDING_NONE;
}

extern orl_symbol_type ORLSymbolGetType( orl_symbol_handle a )
{
    a = a;
    return ORL_SYM_TYPE_NONE;
}

extern orl_sec_handle ORLSymbolGetSecHandle( orl_symbol_handle a )
{
    a = a;
    return NULL;
}

orl_table_index ORLENTRY ORLCvtSecHdlToIdx( orl_file_handle a,
                                                    orl_sec_handle b )
{
    a = a;
    b = b;
    return 0;
}

orl_sec_handle  ORLENTRY ORLCvtIdxToSecHdl( orl_file_handle a,
                                                    orl_table_index b )
{
    a = a;
    b = b;
    return 0;
}

