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
* Description:  Internal FCB memory interface.
*
****************************************************************************/


/* fcbblock.c */
void    GiveBackBlock( long, unsigned char * );
bool    GetNewBlock( long *, unsigned char *, int );
int     MakeWriteBlock( fcb * );

/* fcbdisk.c */
vi_rc   SwapToMemoryFromDisk( fcb * );
vi_rc   SwapToDisk( fcb * );
void    GiveBackSwapBlock( long );
void    SwapFileClose( void );
void    SwapBlockInit( int );

/* fcbems.c */
int     EMSBlockTest( unsigned short );
void    EMSBlockRead( long, void *, unsigned );
void    EMSBlockWrite( long , void *, unsigned );
int     EMSGetBlock( long * );
int     SwapToEMSMemory( fcb * );
int     SwapToMemoryFromEMSMemory( fcb * );
void    EMSInit( void );
void    EMSFini( void );
void    GiveBackEMSBlock( long );
void    EMSBlockInit( int );

/* fcbswap.c */
void    SwapFcb( fcb * );
vi_rc   RestoreToNormalMemory( fcb *, int );

/* fcbxmem.c */
int     SwapToExtendedMemory( fcb * );
int     SwapToMemoryFromExtendedMemory( fcb * );
void    XMemInit( void );
void    XMemFini( void );
void    GiveBackXMemBlock( long );

/* fcbxms.c */
int     XMSBlockTest( unsigned short );
void    XMSBlockRead( long, void *, unsigned );
void    XMSBlockWrite( long , void *, unsigned );
int     XMSGetBlock( long * );
int     SwapToXMSMemory( fcb * );
int     SwapToMemoryFromXMSMemory( fcb * );
void    XMSInit( void );
void    XMSFini( void );
void    GiveBackXMSBlock( long );
void    XMSBlockInit( int );

#if defined( USE_EMS ) || defined( USE_XMS )

extern void XmemGiveBack( void (*rtn)( long ) );
extern void XmemBlockWrite( void (*rtn)(long, void*, unsigned), __segment buff, unsigned *size );
extern bool XmemBlockRead( void (*rtn)(long, void*, unsigned), __segment *buff );
extern void XSwapInit( int count, long *xHandle, unsigned short *xSize );
extern void Xopen( void );

#endif
