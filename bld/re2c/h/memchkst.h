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


MEMCHK_STATUS( MC_OK,                   "no error" )
MEMCHK_STATUS( MC_CHECKSUM_ERROR,       "memory tracker block corrupted" )
MEMCHK_STATUS( MC_LO_BOUND_ERROR,       "lower boundary of block corrupted" )
MEMCHK_STATUS( MC_HI_BOUND_ERROR,       "upper boundary of block corrupted" )
MEMCHK_STATUS( MC_FREE_UNALLOC_ERROR,   "free of unallocated memory (freed twice?)" )
MEMCHK_STATUS( MC_SIMPLE_ARRAY_ERROR,   "new() memory freed by delete[]()" )
MEMCHK_STATUS( MC_ARRAY_SIMPLE_ERROR,   "new[]() memory freed by delete()" )

#undef MEMCHK_STATUS
