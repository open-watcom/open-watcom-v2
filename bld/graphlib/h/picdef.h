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
* Description:  Image descriptor and calling convention for bitmap
*               copy functions.
*
****************************************************************************/


#pragma pack(push, 1);
struct picture {
    short           picwidth;               /* # of pixels across   */
    short           picheight;              /* # of pixels down     */
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES        buffer;                  /* Handle for image     */
    HBITMAP         bmp;
    HDC             pdc;
};
#else
    short           bpp;                    /* # of bits per pixel  */
    char            buffer;                 /* buffer for image     */
};

    /*  Use PASCAL pragma to define our convention for
        calling the copy and read functions.    */

    #if defined ( __386__ )
        #pragma aux PIC_FUNC "*" parm caller [es edi] [esi eax] [ecx] [edx] [ebx] \
                                 modify [eax];
    #else
        #pragma aux PIC_FUNC "*" parm caller [es di] [si ax] [cx] [dx] [bx] \
                                 modify [ax];
    #endif

    typedef void pic_fn( char far *, char far *, int, int, int );
    #pragma aux (PIC_FUNC) pic_fn;

#endif
#pragma pack (pop);
