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


enum {
    T_DDEINIT,
    T_DDESERVER,
    T_CREATEDDESTRING,
    T_DELETEDDESTRING,
    T_DDECONNECT,
    T_DDEDISCONNECT,
    T_DDEREQUEST,
    T_DDEPOKE,
    T_DDERET,
    T_DDECREATEDATAHANDLE,
    T_DDEGETDATA,
    T_DDEQUERYSTRING,
    T_TAKEFOCUS,
    T_MINIMIZE,
    T_RESTORE,
    T_EXIT,
    T_SETMAINSIZE,
    T_UPDATEWINDOWS,
    T_WINHELP,
    T_PROMPT_FOR_SAVE,
    T_PROMPT_THIS_FILE_FOR_SAVE,
    T_QUERY_FILE,
    T_INPUT_BOOL,
    T_EDITFILE,
    T_LOCATE
};

bool RunDDECommand( int token, char *str, char *tmp1, vi_rc *result, vlist *vl );
