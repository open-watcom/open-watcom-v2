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


#include "strtab.h"

extern void     DwarfInit( void );
extern void     DwarfInitModule( mod_entry * );
extern void     DwarfP1ModuleScanned( void );
extern void     DwarfP1ModuleFinished( mod_entry * );
extern void     DwarfAddModule( mod_entry *, section * );
extern void     DwarfGenModule( void );
extern void     DwarfDefClass( class_entry *, unsigned_32 );
extern void     DwarfAddGlobal( symbol * );
extern void     DwarfGenGlobal( symbol *, section * );
extern void     DwarfGenLines( segdata *, void *, unsigned, bool );
extern void     DwarfAddAddrInfo( seg_leader * );
extern void     DwarfStoreAddrInfo( mod_entry * );
extern void     DwarfGenAddrInfo( seg_leader * );
extern void     DwarfAddrSectStart( section * );
extern void     DwarfWriteDBI( void );
extern int      DwarfCountDebugSections( void );
extern offset   DwarfElfWriteDBI( offset curr_off, stringtable *strtab,
                                Elf32_Shdr *dbgsecbegin );
extern offset   DwarfWriteTrailer(offset);
