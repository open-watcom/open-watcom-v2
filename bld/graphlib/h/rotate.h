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


extern short        _wror( short, short );
extern short        _wrol( short, short );
extern char         _rotr_b( char, char );
extern char         _rotl_b( char, char );

#if defined ( __386__ )
    #pragma aux         _wror = 0x66 0xd3 0xc8  /* ror  ax,cl   */ \
                                parm caller [ax] [cx] value [ax];
    #pragma aux         _wrol = 0x66 0xd3 0xc0  /* rol  ax,cl   */ \
                                parm caller [ax] [cx] value [ax];
    #pragma aux         _rotr_b = 0x66 0xd2 0xc8 /* ror al,cl   */ \
                                parm caller [al] [cl] value [al];
    #pragma aux         _rotl_b = 0x66 0xd2 0xc0 /* rol al,cl   */ \
                                parm caller [al] [cl] value [al];
#else
    #pragma aux         _wror = 0xd3 0xc8       /* ror  ax,cl   */ \
                                parm caller [ax] [cx] value [ax];
    #pragma aux         _wrol = 0xd3 0xc0       /* rol  ax,cl   */ \
                                parm caller [ax] [cx] value [ax];
    #pragma aux         _rotr_b = 0xd2 0xc8     /* ror  al,cl   */ \
                                parm caller [al] [cl] value [al];
    #pragma aux         _rotl_b = 0xd2 0xc0     /* rol  al,cl   */ \
                                parm caller [al] [cl] value [al];
#endif
