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


#ifndef __FILTFLAG_H__
#define __FILTFLAG_H__

/*
 * this just defines the FilterFlags type. It's split off so that program
 * that use it don't need to be C++
 */

typedef unsigned_16 FilterFlags;

#define FILT_RESET_INHERITED( x ) ( x &= (FilterFlags) 0xfff8 )

#if 0   // C++ compiler emitting data items for these.
const FilterFlags FILT_BLANK        = 0;

const FilterFlags FILT_NO_INHERITED  =  0x0001;
const FilterFlags FILT_INHERITED     =  0x0002;
const FilterFlags FILT_ALL_INHERITED =  0x0004;

const FilterFlags FILT_PUBLIC       =  0x0008;
const FilterFlags FILT_PROTECTED    =  0x0010;
const FilterFlags FILT_PRIVATE      =  0x0020;

const FilterFlags FILT_STATIC       =  0x0040;
const FilterFlags FILT_NONSTATIC    =  0x0080;

const FilterFlags FILT_NONVIRTUAL   =  0x0100;
const FilterFlags FILT_VIRTUAL      =  0x0200;
const FilterFlags FILT_ALL          =  0x0FFF;

const FilterFlags FILT_ALL_VIRTUAL   = (FILT_NONVIRTUAL | FILT_VIRTUAL);
const FilterFlags FILT_ALL_STATIC    = (FILT_NONSTATIC | FILT_STATIC);
const FilterFlags FILT_LOCAL_ACCESS  = (FILT_PUBLIC | FILT_PROTECTED | FILT_PRIVATE );
const FilterFlags FILT_ALL_ACCESS    = (FILT_LOCAL_ACCESS | FILT_ALL_INHERITED );
const FilterFlags FILT_ALL_DEFAULT   = (FILT_ALL_VIRTUAL | FILT_LOCAL_ACCESS | FILT_ALL_STATIC);
#endif

#define FILT_BLANK         0

#define FILT_NO_INHERITED  0x0001
#define FILT_INHERITED     0x0002
#define FILT_ALL_INHERITED 0x0004

#define FILT_PUBLIC        0x0008
#define FILT_PROTECTED     0x0010
#define FILT_PRIVATE       0x0020

#define FILT_STATIC        0x0040
#define FILT_NONSTATIC     0x0080

#define FILT_NONVIRTUAL    0x0100
#define FILT_VIRTUAL       0x0200
#define FILT_ALL           0x0FFF

#define FILT_ALL_VIRTUAL   (FILT_NONVIRTUAL | FILT_VIRTUAL)
#define FILT_ALL_STATIC    (FILT_NONSTATIC | FILT_STATIC)
#define FILT_LOCAL_ACCESS  (FILT_PUBLIC | FILT_PROTECTED | FILT_PRIVATE )
#define FILT_ALL_ACCESS    (FILT_LOCAL_ACCESS | FILT_ALL_INHERITED )
#define FILT_ALL_DEFAULT   (FILT_ALL_VIRTUAL | FILT_LOCAL_ACCESS | FILT_ALL_STATIC)

#endif
