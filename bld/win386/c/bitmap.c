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
* Description:  Covers for bitmap manipulation routines.
*
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include "stubs.h"

/*
 * __CreateBitmap - cover for CreateBitmap, makes sure lpbits are ok
 */
HBITMAP FAR PASCAL __CreateBitmap(int height, int width, BYTE nplanes,
                        BYTE bitcount, DWORD lpbits )
{
    DWORD       alias,size;
    HBITMAP     rc;

    size = (DWORD)nplanes * (DWORD)height * (DWORD)width * (DWORD)bitcount;
    size = (size + 7) >> 3;
    DPMIGetHugeAlias( (DWORD) lpbits, &alias, size );
    rc = CreateBitmap( height, width, nplanes, bitcount, (LPVOID) alias );
    DPMIFreeHugeAlias( alias, size );
    return( rc );

} /* CreateBitmap */

/*
 * __CreateBitmapIndirect - cover for CreateBitmapIndirect
 */
HBITMAP FAR PASCAL __CreateBitmapIndirect( LPBITMAP bm )
{
    HBITMAP     rc;
    DWORD       alias,size;
    LPSTR       old;

    size = (DWORD) bm->bmHeight * (DWORD) bm->bmWidthBytes;
    size *= (DWORD) bm->bmPlanes;
    DPMIGetHugeAlias( (DWORD) bm->bmBits, &alias, size );
    old = bm->bmBits;
    bm->bmBits = (LPSTR) alias;

    rc = CreateBitmapIndirect( bm );

    DPMIFreeHugeAlias( alias, size );
    bm->bmBits = old;

    return( rc );

} /* __CreateBitmapIndirect */

/*
 * __SetBitmapBits - make sure to get alias right for bits
 */
LONG FAR PASCAL __SetBitmapBits( HBITMAP bm, DWORD dw, LPSTR bits )
{
    DWORD       alias;
    LONG        rc;

    DPMIGetHugeAlias( (DWORD) bits, &alias, dw );
    rc = SetBitmapBits( bm, dw, (LPSTR) alias );
    DPMIFreeHugeAlias( alias, dw );
    return( rc );

} /* __SetBitmapBits */

/*
 * __GetBitmapBits - make sure to get alias right for bits
 */
LONG FAR PASCAL __GetBitmapBits( HBITMAP bm, DWORD dw, LPSTR bits )
{
    DWORD       alias;
    LONG        rc;

    DPMIGetHugeAlias( (DWORD) bits, &alias, dw );
    rc = GetBitmapBits( bm, dw, (LPSTR) alias );
    DPMIFreeHugeAlias( alias, dw );
    return( rc );

} /* __GetBitmapBits */

/*
 * getScanLineSize - get size of a scan line for a bitmap
 */
static DWORD getScanLineSize( LPBITMAPINFOHEADER bmh )
{
    WORD        size;

    size = bmh->biWidth;
    switch( bmh->biBitCount ) {
    case 1:
        size = (size + 7) >> 3;
        break;
    case 4:
        size = (size + 1) >> 1;
        break;
    case 24:
        size *= 3;
        break;
    }
    size = (size+3) & ~3;
    return( (DWORD) size );

} /* getScanLineSize */

/*
 * __SetDIBits - make sure the right alias gets in for lpbits
 */
int FAR PASCAL __SetDIBits(HDC hdc,HANDLE hbitmap,WORD start,WORD num,
                LPSTR lpbits ,LPBITMAPINFO bmi,WORD usage)
{
    DWORD       size;
    DWORD       alias;
    int         rc;

    size = (DWORD) num * getScanLineSize( &bmi->bmiHeader );
    DPMIGetHugeAlias( (DWORD) lpbits, &alias, size );
    rc = SetDIBits( hdc, hbitmap, start, num, (LPSTR) alias, bmi, usage );
    DPMIFreeHugeAlias( alias, size );
    return( rc );

} /* __SetDIBits */

/*
 * __GetDIBits - make sure the right alias gets in for lpbits
 */
int FAR PASCAL __GetDIBits(HDC hdc,HANDLE hbitmap,WORD start,WORD num,
                LPSTR lpbits ,LPBITMAPINFO bmi,WORD usage)
{
    DWORD       size;
    DWORD       alias;
    int         rc;

    size = (DWORD) num * getScanLineSize( &bmi->bmiHeader );
    DPMIGetHugeAlias( (DWORD) lpbits, &alias, size );
    rc = GetDIBits( hdc, hbitmap, start, num, (LPSTR) alias, bmi, usage );
    DPMIFreeHugeAlias( alias, size );
    return( rc );

} /* __GetDIBits */

/*
 * __SetDIBitsToDevice - make sure the right alias gets in for lpbits
 */
int FAR PASCAL __SetDIBitsToDevice(HDC hdc,WORD destx,WORD desty,
                                        WORD width,WORD height,
                                        WORD srcx,WORD srcy,WORD start,
                                        WORD num, LPSTR lpbits,
                                        LPBITMAPINFO bmi,WORD usage )
{
    DWORD       size;
    DWORD       alias;
    int         rc;

    size = (DWORD) num * getScanLineSize( &bmi->bmiHeader );
    DPMIGetHugeAlias( (DWORD) lpbits, &alias, size );
    rc = SetDIBitsToDevice( hdc, destx, desty, width, height, srcx, srcy,
                start, num, (LPSTR) alias, bmi, usage );
    DPMIFreeHugeAlias( alias, size );
    return( rc );

} /* __SetDIBitsToDevice */

/*
 * __CreateDIBitmap - make sure to get alias right for lpInitBits
 */
HBITMAP FAR PASCAL __CreateDIBitmap( HDC hDC, LPBITMAPINFOHEADER lpInfoHeader,
                        DWORD dwUsage,DWORD lpInitBits,
                        LPBITMAPINFO lpInitInfo, WORD wUsage )
{
    DWORD       size;
    DWORD       alias;
    HBITMAP     rc;

    if( lpInitBits != 0L ) {
        size = (DWORD) lpInitInfo->bmiHeader.biHeight *
                        getScanLineSize( &lpInitInfo->bmiHeader );
        DPMIGetHugeAlias( (DWORD) lpInitBits, &alias, size );
    } else {
        alias = 0;
    }
    rc = CreateDIBitmap( hDC, lpInfoHeader, dwUsage,
                        (LPSTR) alias,
                        lpInitInfo, wUsage );
    if( alias != 0 ) {
        DPMIFreeHugeAlias( alias, size );
    }
    return( rc );

} /* __CreateDIBitmap */

/*
 * __StretchDIBits
 */
int FAR PASCAL __StretchDIBits( HDC hdc, WORD destx, WORD desty,
                  WORD destwidth, WORD destheight, WORD srcx, WORD srcy,
                  WORD srcwidth, WORD srcheight, DWORD lpbits,
                  LPBITMAPINFO lpbitsinfo, WORD usage, DWORD rop )
{
    DWORD       size;
    DWORD       alias;
    int         rc;

//    size = (DWORD) srcheight * getScanLineSize( &lpbitsinfo->bmiHeader );
    size = (DWORD) lpbitsinfo->bmiHeader.biHeight * getScanLineSize( &lpbitsinfo->bmiHeader );
    DPMIGetHugeAlias( (DWORD) lpbits, &alias, size );

    rc = StretchDIBits( hdc, destx, desty, destwidth, destheight, srcx,
          srcy, srcwidth, srcheight, (LPVOID) alias, lpbitsinfo, usage, rop );
    DPMIFreeHugeAlias( alias, size );
    return( rc );

} /* __StretchDIBits */
