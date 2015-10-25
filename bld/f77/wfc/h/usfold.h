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


extern  void            AddI(ftn_type *,ftn_type *);
extern  void            SubI(ftn_type *,ftn_type *);
extern  void            DivI(ftn_type *,ftn_type *);
extern  void            MulI(ftn_type *,ftn_type *);
extern  void            AddR(ftn_type *,ftn_type *);
extern  void            SubR(ftn_type *,ftn_type *);
extern  void            DivR(ftn_type *,ftn_type *);
extern  void            MulR(ftn_type *,ftn_type *);
extern  void            AddD(ftn_type *,ftn_type *);
extern  void            SubD(ftn_type *,ftn_type *);
extern  void            DivD(ftn_type *,ftn_type *);
extern  void            MulD(ftn_type *,ftn_type *);
extern  void            AddE(ftn_type *,ftn_type *);
extern  void            SubE(ftn_type *,ftn_type *);
extern  void            DivE(ftn_type *,ftn_type *);
extern  void            MulE(ftn_type *,ftn_type *);
extern  void            AddC(ftn_type *,ftn_type *);
extern  void            SubC(ftn_type *,ftn_type *);
extern  void            DivC(ftn_type *,ftn_type *);
extern  void            MulC(ftn_type *,ftn_type *);
extern  void            AddQ(ftn_type *,ftn_type *);
extern  void            SubQ(ftn_type *,ftn_type *);
extern  void            DivQ(ftn_type *,ftn_type *);
extern  void            MulQ(ftn_type *,ftn_type *);
extern  void            AddX(ftn_type *,ftn_type *);
extern  void            SubX(ftn_type *,ftn_type *);
extern  void            DivX(ftn_type *,ftn_type *);
extern  void            MulX(ftn_type *,ftn_type *);
extern  void            XINeg(ftn_type *,ftn_type *);
extern  void            XRNeg(ftn_type *,ftn_type *);
extern  void            XDNeg(ftn_type *,ftn_type *);
extern  void            XENeg(ftn_type *,ftn_type *);
extern  void            XCNeg(ftn_type *,ftn_type *);
extern  void            XQNeg(ftn_type *,ftn_type *);
extern  void            XXNeg(ftn_type *,ftn_type *);
extern  void            XIPlus(ftn_type *,ftn_type *);
extern  void            XRPlus(ftn_type *,ftn_type *);
extern  void            XDPlus(ftn_type *,ftn_type *);
extern  void            XEPlus(ftn_type *,ftn_type *);
extern  void            XCPlus(ftn_type *,ftn_type *);
extern  void            XQPlus(ftn_type *,ftn_type *);
extern  void            XXPlus(ftn_type *,ftn_type *);
extern  void            XLEqv(ftn_type *,ftn_type *);
extern  void            XLNeqv(ftn_type *,ftn_type *);
extern  void            XLOr(ftn_type *,ftn_type *);
extern  void            XLAnd(ftn_type *,ftn_type *);
extern  void            XLNot(ftn_type *,ftn_type *);
extern  void            XBitEqv(ftn_type *,ftn_type *);
extern  void            XBitNeqv(ftn_type *,ftn_type *);
extern  void            XBitOr(ftn_type *,ftn_type *);
extern  void            XBitAnd(ftn_type *,ftn_type *);
extern  void            XBitNot(ftn_type *,ftn_type *);
extern  void            XICmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XRCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XDCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XECmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XCCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XQCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XXCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);
extern  void            XChCmp(ftn_type *,ftn_type *,const logstar1 __FAR *);

extern void    GenExp( TYPE typ );
extern void    ExpI( TYPE typ, ftn_type *base, intstar4 exponent );
