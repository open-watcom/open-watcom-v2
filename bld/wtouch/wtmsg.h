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
* Description:  Message output routines for wtouch.
*
****************************************************************************/


/* wtouch error messages */

#define MSG_RC_BASE         0
#define MSG_USAGE_BASE      500

#define MSG_LANG_SPACING    1000

#include "msg.gh"

// #define      MSG_NO_TIME_STAMP       16
// #define      MSG_SYS_ERR_0           48
// #define      MSG_USAGE_LN_0          64
// #define      MSG_USAGE_DATE_FORM     80

#define MAX_RESOURCE_SIZE   128


typedef struct table_type {
    int         msgid;
    char        *msgtype;
} TABLE_TYPE;

extern int MsgInit( void );
extern int MsgGet( int resourceid, char *buffer );
extern void MsgSubStr( int resourceid, char *buff, char *p );
extern void MsgFini( void );
