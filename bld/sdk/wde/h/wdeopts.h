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


#ifndef WDEOPTS_INCLUDED
#define WDEOPTS_INCLUDED

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_BAD_OPT_REQ  -1

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum
{
  WdeOptIsWResFmt = 1
, WdeOptUseDefDlg
, WdeOptReqGridX
, WdeOptReqGridY
, WdeOptIgnoreInc
, WdeOptReqIncPath
, WdeOptReqScreenRECT
, WdeOptReqCntlsTBarPos
, WdeOptIsScreenMax
, WdeOptIsCntlsTBarVisible
, WdeOptIsRibbonVisible
, WdeOptUse3DEffects
, WdeOptReqLastDir
} WdeOptReq;

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern char WdeProfileName[];
extern char WdeSectionName[];

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void        WdeOptsShutdown         ( void );
extern int         WdeGetOption            ( WdeOptReq );
extern int         WdeSetOption            ( WdeOptReq, int );
extern char       *WdeGetIncPathOption     ( void );
extern void        WdeSetIncPathOption     ( char * );
extern void        WdeGetScreenPosOption   ( RECT *pos );
extern void        WdeSetScreenPosOption   ( RECT *pos );
extern void        WdeGetCntlTBarPosOption ( RECT *pos );
extern void        WdeSetCntlTBarPosOption ( RECT *pos );
extern void        WdeInitOpts             ( void );
extern Bool        WdeDisplayOptions       ( void );

#endif
