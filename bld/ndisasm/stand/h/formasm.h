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


#ifndef WDIS_FORMASM_INCLUDED
#define WDIS_FORMASM_INCLUDED

extern return_val       DumpASMSection( section_ptr, unsigned_8 *, orl_sec_size,
                                        unsigned );
extern return_val       BssASMSection( section_ptr, orl_sec_size, unsigned );
extern dis_value        HandleAddend( ref_entry );
orl_sec_offset          RelocSize( ref_entry );
extern int              IsMasmOutput( void );
extern int              IsDataReloc( ref_entry );
extern orl_sec_offset   HandleRefInData( ref_entry, void *, bool );
extern return_val       DumpASMDataFromSection( unsigned_8 *, orl_sec_offset,
                                                orl_sec_offset, label_entry *,
                                                ref_entry *, section_ptr );
extern char             *SkipRef( ref_entry );

#endif
