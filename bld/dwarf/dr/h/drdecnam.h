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


#ifndef DRDECNAM_H
#define DRDECNAM_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * given a handle to a dwarf debug-info-entry, decorate the name of
 * the entry and return it as a char *.
 */
extern char * DRDecoratedName( dr_handle entry, dr_handle parent );

/*
 * given a handle to a dwarf die, decorate its name.  the string
 * is returned via a callback function, which is called with a string,
 * a flag telling whether it was user defined, and a handle to the die
 * for the name if it was user defined. the last call to the callback
 * has a NULL string. obj gets passed to the callback.
 */
extern void   DRDecoratedNameList( void * obj, dr_handle die, dr_handle parent,
                                   void (*)( void *, char *,
                                             int, dr_handle,
                                             dr_sym_type ) );

#define DRDECLABELLEN  (64)

/*
 * Give a label for a symbol given a handle to its die.  For instance,
 * structures in C give "Structure".  The buffer must be at least
 * DRDECLABELLEN bytes.
 */
extern void   DRDecorateLabel( dr_handle die, char * buf );

#ifdef __cplusplus
};
#endif
#endif
