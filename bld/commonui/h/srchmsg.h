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
* Description:  Search message list interface.
*
****************************************************************************/


#ifndef _SRCHMSG_H_INCLUDED
#define _SRCHMSG_H_INCLUDED

/*
 * Here is an example of how to set up a message list.
 * Notice the 0, NULL entry used to delimit the end of the list.
 *
 *   static msglist StrActionMsgs[] = {
 *          MH_CLEANUP,         "DELETED",
 *          MH_CREATE,          "CREATED",
 *          MH_DELETE,          "DELETED",
 *          MH_KEEP,            "KEPT",
 *          0,                  NULL
 *   };
 *
 * or for one to be used with InitSrchTable (here 0, -1 delimits the end)
 *
 *   static msglist StrActionMsgs[] = {
 *          MH_CLEANUP,         (char *)STR_DELETED,
 *          MH_CREATE,          (char *)STR_CREATED,
 *          MH_DELETE,          (char *)STR_DELETED,
 *          MH_KEEP,            (char *)STR_KEPT,
 *          0,                  (char *)-1
 *   };
 */

typedef struct {
     unsigned   msgno;
     char       *msg;
} msglist;

char    *SrchMsg( unsigned msgno, msglist *tbl, char *dflt );
BOOL    InitSrchTable( HANDLE inst, msglist *tbl );

#endif /* _SRCHMSG_H_INCLUDED */
