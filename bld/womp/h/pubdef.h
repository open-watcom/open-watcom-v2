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


#ifndef PUBDEF_H
#define PUBDEF_H    1
/*
    TypePubdefs must be called after the type and symbols have been parsed,
    and before any PUBDEF is written.  Ideally it can be called in
    THEADR during WRITE_PASS.

    During the read pass, the idx in the pubdef_data union is valid.  During
    the write pass the hdl (symb_handle) is valid IF TypePubdefs has been
    called.  A filter should then grab the symb_handle and convert it to an
    idx as appropriate.  ONLY ONE SUCH FILTER CAN EXIST!  The omf_generators
    expect an idx so if TypePubdefs() has been called then the idx will have
    to be set sometime before it gets to genmsomf or genphar.

    If a pubdef can't be typed, the hdl is set to CANS_NULL.
*/
extern void TypePubdefs( void );

#endif
