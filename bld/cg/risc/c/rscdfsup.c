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


#include "cgstd.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include "coderep.h"
#include "typedef.h"
#include "procdef.h"
#include "model.h"
#include "ocentry.h"
#include "zoiks.h"
#include "cgaux.h"
#include "dw.h"
#include "dwarf.h"
#include "dfdbg.h"
#include "objout.h"
#include "rscobj.h"
#include "cgprotos.h"


extern  void            OutBckExport( const char *name, bool is_export );


extern  dw_client       Client;

struct dbg_seg_names {
    char        *seg_name;
};

static struct dbg_seg_names DwarfSegNames[DW_DEBUG_MAX] = {
    { ".debug_info" },
    { ".debug_pubnames" },
    { ".debug_aranges" },
    { ".debug_line" },
    { ".debug_loc" },
    { ".debug_abbrev" },
    { ".debug_macinfo" },
    { ".debug_str" },
    { ".WATCOM_reference" }
};


extern sect_info DwarfSegs[ DW_DEBUG_MAX ];



extern  void    DFDefSegs( void ){
/*************************************************/
    int         i;

    if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
        for( i = 0; i < DW_DEBUG_MAX; ++i ){
            DwarfSegs[i].seg = DbgSegDef( DwarfSegNames[i].seg_name );
            DwarfSegs[i].bck = NULL;
        }
    } else if( _IsModel( NUMBERS ) ) {
        DwarfSegs[DW_DEBUG_LINE].seg = DbgSegDef( DwarfSegNames[DW_DEBUG_LINE].seg_name );
        DwarfSegs[DW_DEBUG_LINE].bck = NULL;
    }
}

#define ABBREV_NAME  "___DFABBREV"
extern void DFAbbrevRef( void ){
/******************************/
    back_handle bck;

    bck = BENewBack( NULL );
//  BackImpPtr( ABBREV_NAME, bck, 0 );
    BEFreeBack( bck );
}

extern void DFAbbrevDef( void ){
/******************************/
//  OutBckExport( ABBREV_NAME, true );
}
