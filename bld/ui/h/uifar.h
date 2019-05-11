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
* Description:  far memory internal functions prototypes.
*
****************************************************************************/


extern void         intern farfill( LP_PIXEL, PIXEL, uisize, bool );
extern void         intern farcopy( LP_PIXEL, LP_PIXEL, uisize, bool );
extern void         intern farstring( LP_PIXEL, ATTR, LPC_STRING, uisize, bool );
extern void         intern farattrib( LP_PIXEL, ATTR, uisize, bool );
extern void         intern farattrflip( LP_PIXEL, uisize, bool );
extern LP_PIXEL     intern faralloc( uisize size );
extern void         intern farfree( LP_PIXEL ptr );
