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


#ifndef COFF_LWLV_INCLUDED
#define COFF_LWLV_INCLUDED

#include "coffglbl.h"

extern orl_return       CoffCreateSymbolHandles( coff_file_handle );
extern orl_return       CoffBuildSecNameHashTable( coff_file_handle );
extern orl_return       CoffCreateRelocs( coff_sec_handle, coff_sec_handle );
extern orl_reloc_type   CoffConvertRelocType( coff_file_handle, coff_reloc_type );
extern orl_linnum       *CoffConvertLines( coff_sec_handle, orl_table_index );
extern orl_return       CoffParseDrectve( const char *, size_t len, orl_note_callbacks *, void * );

#endif
