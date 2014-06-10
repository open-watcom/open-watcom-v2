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


#ifndef DWINFO_H_INCLUDED
#define DWINFO_H_INCLUDED

#define InitDebugInfo           DW_InitDebugInfo
#define FiniDebugInfo           DW_FiniDebugInfo
#define InfoAllocate            DW_InfoAllocate
#define InfoReloc               DW_InfoReloc
#define Info8                   DW_Info8
#define Info16                  DW_Info16
#define Info32                  DW_Info32
#define InfoLEB128              DW_InfoLEB128
#define InfoULEB128             DW_InfoULEB128
#define InfoBytes               DW_InfoBytes
#define InfoString              DW_InfoString
#define InfoSkip                DW_InfoSkip
#define InfoPatch               DW_InfoPatch


void            InitDebugInfo( dw_client );
void            FiniDebugInfo( dw_client );

void            InfoReloc( dw_client, uint );
void            Info8( dw_client, uint_8 );
void            Info16( dw_client, uint_16 );
void            Info32( dw_client, uint_32 );
void            InfoLEB128( dw_client, dw_sconst );
void            InfoULEB128( dw_client, dw_uconst );
void            InfoBytes( dw_client, const void *, dw_size_t );
void            InfoString( dw_client, const char * );
debug_ref       InfoSkip( dw_client, dw_size_t );
void            InfoPatch( dw_client, debug_ref, const void *, dw_size_t );

#endif
