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


#ifndef WREIMG_INCLUDED
#define WREIMG_INCLUDED

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern WResID   *WRECreateImageTitle( uint_16 type );
extern bool     WRENewImageResource( WRESPT service, uint_16 type );
extern bool     WREEditImageResource( WRECurrentResInfo *curr );
extern bool     WRECommitImageSession( HCONV server, HCONV client );
extern bool     WREEndEditImageResource( HCONV conv );
extern bool     WREDumpPendingImageSession( void );
extern bool     WREEndAllImageSessions( bool fatal_exit );
extern void     WREEndLangImageSession( WResLangNode *lnode );
extern void     WREEndResImageSessions( WREResInfo *rinfo );
extern bool     WREGetImageSessionFileName( HCONV server, void **data, uint_32 *size );
extern bool     WREGetImageSessionResName( HCONV server, void **data, uint_32 *size );
extern bool     WREGetImageSessionData( HCONV server, void **data, uint_32 *size );
extern bool     WRESetImageSessionResName( HCONV server, HDDEDATA hdata );
extern bool     WRESetImageSessionResData( HCONV server, HDDEDATA hdata );

extern void     WREShowAllImageSessions( bool show );

#endif
