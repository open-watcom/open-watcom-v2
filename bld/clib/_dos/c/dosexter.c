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


#include "variety.h"
#include <dos.h>

#define DOS_EXT_ERR     0x59


_WCRTLINK int dosexterr( struct DOSERROR *doserr )
    {
        auto union REGS regs;

#if defined(__386__) && !defined(__WINDOWS_386__)
        regs.h.ah = DOS_EXT_ERR;
        regs.x.ebx = 0;                 /* DOS version 3.00 */
        intdos( &regs, &regs );         /* get extended error */
        doserr->class = regs.h.bh;
        doserr->action = regs.h.bl;
        doserr->locus = regs.h.ch;
        return( doserr->exterror = regs.x.eax );
#else
        regs.h.ah = DOS_EXT_ERR;
        regs.x.bx = 0;                  /* DOS version 3.00 */
        intdos( &regs, &regs );         /* get extended error */
        doserr->class = regs.h.bh;
        doserr->action = regs.h.bl;
        doserr->locus = regs.h.ch;
        return( doserr->exterror = regs.x.ax );
#endif
   }

