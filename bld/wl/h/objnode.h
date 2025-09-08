/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


typedef struct nodearray        *nodearray_handle;

extern void             InitNodes( void );
extern void             *FindNode( nodearray_handle , unsigned );
extern unsigned         GetNumNodes( nodearray_handle );
extern void             *AllocNode( nodearray_handle );
extern void             *AllocNodeIdx( nodearray_handle, unsigned );
extern mod_entry        *NewModEntry( void );
extern void             FreeModEntry( mod_entry * );
extern void             FreeNodes( nodearray_handle );
extern void             IterateNodelist( nodearray_handle, void (*)(void *,void *), void *);
extern void             BurnNodes( void );
extern void             ReleaseNames( void );
extern void             CollapseLazyExtdefs( void );
extern segnode          *FindSegdata( seg_leader * );
extern extnode          *FindExtHandle( orl_symbol_handle );
extern segdata          *AllocSegData( void );
extern void             FreeSegData( void * );
extern list_of_names    *MakeListName( char *, size_t );
extern unsigned long    BadObjFormat( void );

extern nodearray_handle ExtNodes;           // ptr to obj file import list
extern nodearray_handle SegNodes;           // ptr to obj file segment list
extern nodearray_handle GrpNodes;           // ptr to obj file group list
extern nodearray_handle NameNodes;          // ptr to obj file lname list
