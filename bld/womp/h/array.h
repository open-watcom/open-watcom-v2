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


#ifndef ARRAY_H
#define ARRAY_H 1
/*
    Implements an array type without a predetermined size.
*/
#include <stddef.h>

typedef struct array_hdr array_hdr;

extern void ArrInit( void );
extern void ArrFini( void );

extern array_hdr *ArrCreate( size_t elm_size, void *def );
/*
    If you must do special things to destroy each element in the array, then
    you should use ArrWalk before calling ArrDestroy.
*/
extern void ArrDestroy( array_hdr *arr );
/*
    The first time an element is accessed it must be done through ArrNewElm.
    Subsequent accesses may be done through ArrNewElm, or ArrAccess.
    ArrAccess has some simplifying assumptions that make it faster than
    ArrNewElm.
*/
extern void *ArrNewElm( array_hdr *arr, size_t elm_num );
extern void *ArrAccess( array_hdr *arr, size_t elm_num );
/*
    ArrWalk is guaranteed to give you the elements in the order 0, 1, ...,
    arr->num_elms-1.  If you want to know what the element number is for an
    element passed to you, then keep track of it in parm.
*/
extern int ArrWalk( array_hdr *arr, void *parm,
    int (*func)( void *elm, void *parm ) );

#endif

