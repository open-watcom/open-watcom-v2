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


// only needed for 16bit OS/2 DLL library
#if defined(__OS2__) && defined(_M_I86) && defined(__SW_BD)

/*
 * Disallow the use of the near heap by the user, since he won't be
 * able to reach it using a near pointer anyway. It will be in the
 * DLL's data space, not his
 */

#define brk             __hidden_brk
#define sbrk            __hidden_sbrk
#define _ncalloc        __hidden_ncalloc
#define _nexpand        __hidden_nexpand
#define _nmalloc        __hidden_nmalloc
#define _nrealloc       __hidden_nrealloc
#define _nfree          __hidden_nfree
#define _nheapchk       __hidden_nheapchk
#define _nheapgrow      __hidden_nheapgrow
#define _nheapmin       __hidden_nheapmin
#define _nheapset       __hidden_nheapset
#define _nheapshrink    __hidden_nheapshrink
#define _nheapwalk      __hidden_nheapwalk
#define _nmsize         __hidden_nmsize

#endif
