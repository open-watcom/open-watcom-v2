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


#ifndef __BUILDMSG_H__
#define __BUILDMSG_H__

// Definitions for BuildMsg program

typedef unsigned char byte;
typedef unsigned long Space;

enum RecType                    // RecType: type of record
{   REC_GRP                     // - :MSGGRP token
,   REC_SYM                     // - :MSGSYM token
,   REC_TXT                     // - :MSGTXT token
,   REC_MOD                     // - modifiers
,   REC_EOF                     // - end of file
,   MAX_REC                     // - maximum record
,   REC_PHR                     // - :MSGPHR
,   REC_NONE                    // - no record read in
};

enum MsgType                    // MsgType: type of message
{   MSG_TYPE_ERROR              // - error
,   MSG_TYPE_WARNING            // - warning
,   MSG_TYPE_INFO               // - informational
,   MSG_TYPE_ANSI               // - warning when extensions, error otherwise
,   MSG_TYPE_ANSIERR            // - ignored when extensions, error otherwise
,   MSG_TYPE_ANSIWARN           // - ignored when extensions, warn otherwise
,   MSG_TYPE_ANSICOMP           // - error, can be changed to warning when extensions
,   MSG_TYPE_EXTWARN            // - warn when extensions, ignored otherwise
};

#endif
