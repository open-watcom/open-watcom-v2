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


#ifndef WREDLG_INCLUDED
#define WREDLG_INCLUDED

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern bool WRENewDialogResource( void );
extern bool WREEditDialogResource( WRECurrentResInfo *curr );
extern bool WRECommitDialogSession( HCONV server, HCONV client );
extern bool WREEndEditDialogResource( HCONV conv );
extern bool WREDumpPendingDialogSession( void );
extern bool WREEndAllDialogSessions( bool fatal_exit );
extern void WREEndLangDialogSession( WResLangNode *lnode );
extern void WREEndResDialogSessions( WREResInfo *rinfo );
extern bool WREGetDlgSessionFileName( HCONV server, void **data, uint_32 *size );
extern bool WREGetDlgSessionResName( HCONV server, void **data, uint_32 *size );
extern bool WREGetDlgSessionData( HCONV server, void **data, uint_32 *size );
extern bool WREGetDlgSessionIs32Bit( HCONV server, void **data, uint_32 *size );
extern bool WRESetDlgSessionResName( HCONV server, HDDEDATA hdata );
extern bool WRESetDlgSessionResData( HCONV server, HDDEDATA hdata );
extern void WREShowAllDialogSessions( bool show );

#endif
