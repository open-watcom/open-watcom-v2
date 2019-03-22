/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal FCB memory interface.
*
****************************************************************************/


/* fcbblock.c */
void    GiveBackBlock( long, unsigned char * );
bool    GetNewBlock( long *, unsigned char *, int );
size_t  MakeWriteBlock( fcb * );

/* fcbdisk.c */
vi_rc   SwapToMemoryFromDisk( fcb * );
vi_rc   SwapToDisk( fcb * );
void    GiveBackSwapBlock( long );
void    SwapFileClose( void );
void    SwapBlockInit( int );

/* fcbems.c */
#if defined( USE_EMS )
vi_rc   EMSBlockTest( unsigned short );
void    EMSBlockRead( xhandle, void *, size_t );
void    EMSBlockWrite( xhandle, void *, size_t );
vi_rc   EMSGetBlock( xhandle * );
vi_rc   SwapToEMSMemory( fcb * );
vi_rc   SwapToMemoryFromEMSMemory( fcb * );
void    EMSInit( void );
void    EMSFini( void );
void    GiveBackEMSBlock( xhandle );
void    EMSBlockInit( int );
#endif

/* fcbswap.c */
void    SwapFcb( fcb * );
vi_rc   RestoreToNormalMemory( fcb *, size_t );

/* fcbxmem.c */
#if defined( USE_XTD )
vi_rc   SwapToExtendedMemory( fcb * );
vi_rc   SwapToMemoryFromExtendedMemory( fcb * );
void    XMemInit( void );
void    XMemFini( void );
void    GiveBackXMemBlock( long );

/* fcbxmini.c */
void __interrupt XMemIntHandler( volatile union INTPACK r );
#endif

/* fcbxms.c */
#if defined( USE_XMS )
vi_rc   XMSBlockTest( unsigned short );
void    XMSBlockRead( xhandle, void *, size_t );
void    XMSBlockWrite( xhandle, void *, size_t );
vi_rc   XMSGetBlock( xhandle * );
vi_rc   SwapToXMSMemory( fcb * );
vi_rc   SwapToMemoryFromXMSMemory( fcb * );
void    XMSInit( void );
void    XMSFini( void );
void    GiveBackXMSBlock( xhandle );
void    XMSBlockInit( int );
#endif
