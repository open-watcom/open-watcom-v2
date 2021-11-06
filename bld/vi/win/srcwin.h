/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#define WINCMDS() \
    pick( "DDEINIT",                DDEINIT ) \
    pick( "DDESERVER",              DDESERVER ) \
    pick( "CREATEDDESTRing",        CREATEDDESTRING ) \
    pick( "DELETEDDESTRing",        DELETEDDESTRING ) \
    pick( "DDECONnect",             DDECONNECT ) \
    pick( "DDEDISconnect",          DDEDISCONNECT ) \
    pick( "DDEREQuest",             DDEREQUEST ) \
    pick( "DDEPOKE",                DDEPOKE ) \
    pick( "DDERET",                 DDERET ) \
    pick( "DDECREATEDATAHandle",    DDECREATEDATAHANDLE ) \
    pick( "DDEGETData",             DDEGETDATA ) \
    pick( "DDEQUERYSTRING",         DDEQUERYSTRING ) \
    pick( "TAKEFOCUS",              TAKEFOCUS ) \
    pick( "MINIMIZE",               MINIMIZE ) \
    pick( "RESTORE",                RESTORE ) \
    pick( "EXIT",                   EXIT ) \
    pick( "SETMAINSIZE",            SETMAINSIZE ) \
    pick( "UPDATEWINDOWS",          UPDATEWINDOWS ) \
    pick( "WINHELP",                WINHELP ) \
    pick( "PROMPTFORSAVE",          PROMPT_FOR_SAVE ) \
    pick( "PROMPTTHISFILEFORSAVE",  PROMPT_THIS_FILE_FOR_SAVE ) \
    pick( "QUERYFILE",              QUERY_FILE ) \
    pick( "INPUTBOOL",              INPUT_BOOL ) \
    pick( "EDITFILE",               EDITFILE ) \
    pick( "LOCATE",                 LOCATE )

enum {
    #define pick(t,e)   T_ ## e,
    WINCMDS()
    #undef pick
};

bool RunDDECommand( int token, const char *str, char *buffer, vi_rc *result, vars_list *vl );
