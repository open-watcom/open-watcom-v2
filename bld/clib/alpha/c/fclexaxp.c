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
* Description:  Clear Alpha FP exceptions.
*
****************************************************************************/


#include "rtdata.h"
#include "variety.h"
#include "clearfpe.h"


extern unsigned long _GetFPCR(void);
extern void          _SetFPCR(unsigned long);


/*
 * Alpha FPCR Exception Bits
 * -------------------------
 *
 * Bit 63 : Summary Bit (OR of Bits 57 - 52)
 * Bit 57 : Integer Overflow (IOV) Exception
 * Bit 56 : Inexact Result (INE) Exception
 * Bit 55 : Underflow (UNF) Exception
 * Bit 54 : Overflow (OVF) Exception
 * Bit 53 : Division by Zero (DZE) Exception
 * Bit 52 : Invalid Operation (INV) Exception
 *
 * Since we are using only the highword of the FPCR, these bits will map
 * to 31 and 25-20 respectively.
 */

void _ClearFPE(void)
{
    unsigned long fpcr;

    fpcr = _GetFPCR();
    fpcr &= 0x7c0fffffL;
    _SetFPCR(fpcr);
} /* _ClearFPE() */
