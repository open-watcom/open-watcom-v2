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

//    enum         size     type         name

pick( IT_UB,        1,      unsigned_8,  ub    )
pick( IT_SB,        1,      signed_8,    sb    )
pick( IT_UW,        2,      unsigned_16, uw    )
pick( IT_SW,        2,      signed_16,   sw    )
pick( IT_UD,        4,      unsigned_32, ud    )
pick( IT_SD,        4,      signed_32,   sd    )
pick( IT_UQ,        8,      unsigned_64, uq    )
pick( IT_SQ,        8,      signed_64,   sq    )
pick( IT_SF,        4,      sreal,       sf    )
pick( IT_LF,        8,      lreal,       lf    )
pick( IT_XF,        10,     xreal,       xf    )
pick( IT_SO,        2,      addr32_off,  so    )    /* address offset 16-bit segment */
pick( IT_LO,        4,      addr48_off,  lo    )    /*                32-bit segment */
pick( IT_QO,        8,      unsigned_64, qo    )    /*                64-bit segment */
pick( IT_SA,        4,      addr32_ptr,  sa    )    /* address pointer 16-bit segment */
pick( IT_LA,        6,      addr48_ptr,  la    )    /*                 32-bit segment */
pick( IT_XA,        8,      address,     xa    )    /*                 64-bit segment */
pick( IT_SC,        8,      scomplex,    sc    )
pick( IT_LC,        16,     lcomplex,    lc    )
pick( IT_XC,        20,     xcomplex,    xc    )
pick( IT_NWSCB,     4,      scb_nw,      nwscb )    /* FORTRAN SCB 16-bit segment near pointer */
pick( IT_FWSCB,     6,      scb_fw,      fwscb )    /*                            far pointer  */
pick( IT_NDSCB,     8,      scb_nd,      ndscb )    /* FORTRAN SCB 32-bit segment near pointer */
pick( IT_FDSCB,     10,     scb_fd,      fdscb )    /*                            far pointer  */
