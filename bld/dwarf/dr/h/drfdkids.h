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


#ifndef __DRFDKIDS_H__
#define __DRFDKIDS_H__

typedef bool (*DRCLSSRCH)( dr_sym_type, drmem_hdl, char *, drmem_hdl, void * );

extern void         DRKidsSearch( drmem_hdl, dr_search, void *, DRCLSSRCH );
extern void         DRBaseSearch( drmem_hdl, void *, DRCLSSRCH );
extern void         DRFriendsSearch( drmem_hdl, void *, DRCLSSRCH );
extern void         DRDerivedSearch( drmem_hdl, void *, DRCLSSRCH );
extern dr_sym_type  DRGetSymType( drmem_hdl );

#endif // __DRFDKIDS_H__
