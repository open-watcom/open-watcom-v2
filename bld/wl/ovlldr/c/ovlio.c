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
* Description:  Overlay loader IO routines.
*
****************************************************************************/



#include "ovlstd.h"

extern tiny_ret_t __near __OvlOpen__( char __far *fname )
/***************************************************/
{
    open_attr   openmode;

    openmode = TIO_READ;
    if( __OVLFLAGS__ & OVL_DOS3 ) {
        openmode |= TIO_INHERITANCE;
    }
    return( TinyFarOpen( fname, __OVLSHARE__ | openmode ) );
}

extern tiny_ret_t __near __OvlSeek__( tiny_handle_t hdl, unsigned long pos )
/************************************************************************/
{
    return( TinySeek( hdl, pos, TIO_SEEK_START ) );
}

extern tiny_ret_t __near __OvlRead__(tiny_handle_t hdl, void __far *buff, unsigned len)
/*********************************************************************************/
{
    return( TinyFarRead( hdl, buff, len ) );
}

extern void __near __OvlClose__( tiny_handle_t hdl )
/************************************************/
{
    TinyClose( hdl );
}
