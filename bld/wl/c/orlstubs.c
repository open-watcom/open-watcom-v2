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
* Description:  Stubs for removing Object Reading Library dependency.
*
****************************************************************************/


#include "orl.h"

orl_handle ORLInit( orl_funcs *funcs )
{
    funcs = funcs;
    return( NULL );
}

orl_return ORLFini( orl_handle hdl )
{
    hdl = hdl;
    return( ORL_OKAY );
}

orl_file_format ORLFileIdentify( orl_handle hdl, void *data )
{
    hdl = hdl;
    data = data;
    return( ORL_UNRECOGNIZED_FORMAT );
}

orl_file_handle ORLFileInit( orl_handle a, void *b, orl_file_format c )
{
    a = a;
    b = b;
    c = c;
    return( NULL );
}

orl_return ORLFileFini( orl_file_handle a )
{
    a = a;
    return( ORL_ERROR );
}

orl_return ORLFileScan( orl_file_handle a, char *b,
                               orl_sec_return_func c )
{
    a = a;
    b = b;
    c = c;
    return( ORL_ERROR );
}

orl_machine_type ORLFileGetMachineType( orl_file_handle a )
{
    a = a;
    return( ORL_MACHINE_TYPE_NONE );
}

orl_file_flags ORLFileGetFlags( orl_file_handle a )
{
    a = a;
    return( 0 );
}

orl_file_type ORLFileGetType( orl_file_handle a )
{
    a = a;
    return( ORL_FILE_TYPE_NONE );
}

char *ORLSecGetName( orl_sec_handle a )
{
    a = a;
    return( NULL );
}

orl_sec_size ORLSecGetSize( orl_sec_handle a )
{
    a = a;
    return( 0 );
}

orl_sec_type ORLSecGetType( orl_sec_handle a )
{
    a = a;
    return( ORL_SEC_TYPE_NONE );
}

orl_sec_flags ORLSecGetFlags( orl_sec_handle a )
{
    a = a;
    return( 0 );
}

orl_sec_alignment ORLSecGetAlignment( orl_sec_handle a )
{
    a = a;
    return( 0 );
}

orl_return ORLSecGetContents( orl_sec_handle a, char **b )
{
    a = a;
    b = b;
    return( ORL_ERROR );
}

orl_return ORLRelocSecScan( orl_sec_handle a, orl_reloc_return_func b )
{
    a = a;
    b = b;
    return( ORL_ERROR );
}

orl_return ORLSymbolSecScan( orl_sec_handle a, orl_symbol_return_func b )
{
    a = a;
    b = b;
    return( ORL_ERROR );
}

char *ORLSymbolGetName( orl_symbol_handle a )
{
    a = a;
    return( NULL );
}

orl_symbol_value ORLSymbolGetValue( orl_symbol_handle a )
{
    a = a;
    return( 0 );
}

orl_symbol_binding ORLSymbolGetBinding( orl_symbol_handle a )
{
    a = a;
    return( ORL_SYM_BINDING_NONE );
}

orl_symbol_type ORLSymbolGetType( orl_symbol_handle a )
{
    a = a;
    return( ORL_SYM_TYPE_NONE );
}

orl_sec_handle ORLSymbolGetSecHandle( orl_symbol_handle a )
{
    a = a;
    return( NULL );
}

orl_table_index ORLCvtSecHdlToIdx( orl_sec_handle a )
{
    a = a;
    return( 0 );
}

orl_sec_handle ORLCvtIdxToSecHdl( orl_file_handle a, orl_table_index b )
{
    a = a;
    b = b;
    return( 0 );
}

orl_return ORLNoteSecScan( orl_sec_handle a, orl_note_callbacks *b, void *c )
{
    a = a;
    b = b;
    c = c;
    return( ORL_ERROR );
}

orl_table_index ORLSecGetNumLines( orl_sec_handle a )
{
    a = a;
    return( 0 );
}

orl_linnum *ORLSecGetLines( orl_sec_handle a )
{
    a = a;
    return( NULL );
}

orl_symbol_handle ORLSymbolGetAssociated( orl_symbol_handle a )
{
    a = a;
    return( NULL );
}
