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


extern HBITMAP  FAR PASCAL __CreateBitmap(int height, int width, BYTE nplanes,
                        BYTE bitcount, DWORD lpbits );
extern HBITMAP  FAR PASCAL __CreateBitmapIndirect( LPBITMAP bm );
extern LONG     FAR PASCAL __SetBitmapBits( HBITMAP bm, DWORD dw, LPSTR bits );
extern LONG     FAR PASCAL __GetBitmapBits( HBITMAP bm, DWORD dw, LPSTR bits );
extern int      FAR PASCAL __SetDIBits(HDC hdc,HANDLE hbitmap,WORD start,WORD num,
                                    LPSTR lpbits ,LPBITMAPINFO bmi,WORD usage);
extern int      FAR PASCAL __GetDIBits(HDC hdc,HANDLE hbitmap,WORD start,WORD num,
                                    LPSTR lpbits ,LPBITMAPINFO bmi,WORD usage);
extern int      FAR PASCAL __SetDIBitsToDevice(HDC hdc,WORD destx,WORD desty,
                                        WORD width,WORD height,
                                        WORD srcx,WORD srcy,WORD start,
                                        WORD num, LPSTR lpbits,
                                        LPBITMAPINFO bmi,WORD usage );
extern HBITMAP  FAR PASCAL __CreateDIBitmap( HDC hDC, LPBITMAPINFOHEADER lpInfoHeader,
                        DWORD dwUsage,DWORD lpInitBits,
                        LPBITMAPINFO lpInitInfo, WORD wUsage );
extern int      FAR PASCAL __StretchDIBits( HDC hdc, WORD destx, WORD desty,
                        WORD destwidth, WORD destheight, WORD srcx, WORD srcy,
                        WORD srcwidth, WORD srcheight, DWORD lpbits,
                        LPBITMAPINFO lpbitsinfo, WORD usage, DWORD rop );
