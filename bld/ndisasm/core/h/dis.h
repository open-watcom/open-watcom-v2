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
* Description:  External disassembler routines.
*
****************************************************************************/


#include "distypex.h"

/*
        Disassembler routines
*/
extern dis_return       DisInit( dis_cpu, dis_handle *, bool );
extern unsigned         DisInsNameMax( dis_handle * );
extern unsigned         DisInsSizeInc( dis_handle * );
extern void             DisDecodeInit( dis_handle *, dis_dec_ins * );
extern dis_return       DisDecode( dis_handle *, void *, dis_dec_ins * );
extern dis_return       DisFormat( dis_handle *, void *, dis_dec_ins *, dis_format_flags, char *ins, char *opers );
extern void             DisFini( dis_handle * );

/*
        Client routines
*/
extern dis_return DisCliGetData( void *d, unsigned off, unsigned size, void *data );
extern unsigned DisCliGetAlign( void *d, unsigned off, unsigned align );
extern unsigned DisCliValueString( void *d, dis_dec_ins *, unsigned op, char *buff );
