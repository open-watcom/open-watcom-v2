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


typedef struct pm_image_info {
    SHORT       xhotspot;
    SHORT       yhotspot;
    ULONG       clr_offset;
    ULONG       and_offset;
    BITMAPINFO2 *xorinfo;
    BITMAPINFO2 *andinfo;
} pm_image_info;

typedef struct a_pm_image {
    BYTE                *xor_mask;
    USHORT              xor_size;
    BYTE                *and_mask;
    USHORT              and_size;
    BYTE                *clr_mask;
    USHORT              clr_size;
} a_pm_image;

typedef struct a_pm_image_file {
    USHORT      type;
    USHORT      count;
    pm_image_info resources[1];
} a_pm_image_file;

#define PMFILE_HEADER_SIZE      2*sizeof(SHORT)+4*(sizeof(USHORT)+sizeof(ULONG))
#define NON_OS2_FILE            -1
#define PMBITMAP_FILETYPE       1
#define PMICON_FILETYPE         2
#define PMPOINTER_FILETYPE      3

extern a_pm_image_file *OpenPMImage( FILE *, int , int *);
extern void ClosePMImage( a_pm_image_file *);
extern a_pm_image *GetPMImageBits( a_pm_image_file *, FILE *, int );
extern void FiniPMImage( a_pm_image *);
extern HBITMAP PMImageToAndBitmap( a_pm_image *, a_pm_image_file *, int );
extern HBITMAP PMImageToClrBitmap( a_pm_image *, a_pm_image_file *, int );
extern HBITMAP PMImageToXorBitmap( a_pm_image *, a_pm_image_file *, int );
extern HBITMAP PMImageToWinXorBitmap( a_pm_image *, a_pm_image_file *, int , WPI_INST );
