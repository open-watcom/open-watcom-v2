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
* Description:  Symbol lookup, including some special symbols.
*
****************************************************************************/


extern void         InitLook( void );
extern void         FiniLook( void );
extern void         LookCaseSet( bool respect );
extern void         LookSet( void );
extern void         LookConf( void );
extern int          Lookup( const char *tokenlist,  const char *what, size_t tokenlen );
extern mod_handle   LookupModName( mod_handle search, const char *start, unsigned len );
extern mod_handle   LookupImageName( const char *start, unsigned len );
extern sym_list     *LookupSymList( symbol_source ss, void *d, bool source_only, lookup_item *li );
extern void         FreeSymHandle( sym_list *sl );
extern void         PurgeSymHandles( void );
extern bool         FindNullSym( mod_handle mh, address *addr );
extern bool         SetWDPresent( mod_handle mh );
