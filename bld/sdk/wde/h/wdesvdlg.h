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


#ifndef WDESVDLG_INCLUDE
#define WDESVDLG_INCLUDE

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool WdeSaveRC( char *filename, WdeResInfo *res_info );
extern Bool WdeSaveObjectToRC( char *filename, WdeResInfo *rinfo,
                               WdeResDlgItem *ditem, Bool append );
extern Bool WdeSaveResInfoToRC  ( char *filename, WdeResInfo *rinfo, Bool append );
extern Bool WdeSetMemFlagsText( uint_16 flags, char **text );
extern Bool WdeSetWindowFlagText( unsigned long flags, char **text );
extern Bool WdeSetDialogFlagText( unsigned long flags, char **text );
extern Bool WdeSetControlFlagText( uint_8 class, unsigned long flags, char **text );
extern char *WdeConstructDLGInclude( WdeResInfo *rinfo );

#endif
