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


// Define allocation flags:
// ------------------------

#define ALLOC_EXTENDED  0x0001  // extended pointer
#define ALLOC_NONE      0x0002  // no keyword specified
#define ALLOC_STAT      0x0004  // STAT keyword specified
#define ALLOC_LOC       0x0008  // LOCATION keyword specified
#define ALLOC_MEM       0x0010  // We have memory allocated
#define ALLOC_STRING    0x0020  // We are allocating a string

// The following flags specific to each individual ALLOCATE and are
// cleared after the ALLOCATE

#define ALLOC_MASK      (ALLOC_NONE | ALLOC_STAT | ALLOC_LOC | ALLOC_MEM )
