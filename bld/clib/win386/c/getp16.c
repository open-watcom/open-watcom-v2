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


#include <windows.h>

extern void *UserProcs[];

CALLBACKPTR GetProc16( PROCPTR pp, long type )
{
    CALLBACKPTR ptr;

    switch( type ) {
    case GETPROC_CALLBACK:
//  case GETPROC_SETTIMER:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_WORD,
                                        GCB_DWORD, GCB_ENDLIST );
        break;

    case GETPROC_ABORTPROC:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_ENDLIST );
        break;

    case GETPROC_ENUMFONTS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_WORD,
                                        GCB_DWORD, GCB_ENDLIST );
        break;

    case GETPROC_ENUMMETAFILE:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_DWORD, GCB_DWORD,
                                        GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;

    case GETPROC_ENUMOBJECTS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;

    case GETPROC_ENUMPROPS_FIXED_DS:
//  case GETPROC_GRAYSTRING:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;

    case GETPROC_ENUMPROPS_MOVEABLE_DS:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_WORD, GCB_WORD,
                                        GCB_ENDLIST );
        break;

    case GETPROC_ENUMTASKWINDOWS:
//  case GETPROC_ENUMWINDOWS:
//  case GETPROC_ENUMCHILDWINDOWS:
//  case GETPROC_TOOLHELP_NOTIFY:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;

    case GETPROC_GLOBALNOTIFY:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_ENDLIST );
        break;

    case GETPROC_LINEDDA:
//  case GETPROC_SETWINDOWSHOOK:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_DWORD,
                                        GCB_ENDLIST );
        break;

    case GETPROC_SETRESOURCEHANDLER:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_WORD,
                                        GCB_ENDLIST );
        break;

    case GETPROC_DDEMLCALLBACK:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_DWORD,
                                        GCB_DWORD, GCB_DWORD, GCB_DWORD,
                                        GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_PENWIN_RCYIELDPROC:
        ptr = GetCallbackRoutine( pp, GCB_ENDLIST );
        break;
    case GETPROC_PENWIN_LPDF:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_DWORD, GCB_DWORD,
                                GCB_WORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_PENWIN_LPFNRAWHOOK:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_PENWIN_ENUMPROC:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_MMSYSTEM_MMIOPROC:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD,
                        GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_MMSYSTEM_YIELDPROC:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;
#if 0
    case GETPROC_MMSYSTEM_TIMECALLBACK:
        ptr = GetCallbackRoutine( pp, GCB_WORD, GCB_WORD, GCB_DWORD,
                        GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
#endif
#if 0
    case GETPROC_OLE_OV_QUERYPROTOCOL:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_RELEASE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SHOW:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_DOVERB:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_WORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_GETDATA:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETDATA:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETTARGETDEVICE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETBOUNDS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_ENUMFORMATS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETCOLORSCHEME:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_DELETE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETHOSTNAMES:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SAVETOSTREAM:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_CLONE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                                        GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_COPYFROMLINK:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                                        GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_EQUAL:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_COPYTOCLIPBOARD:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_DRAW:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD,
                                GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_ACTIVATE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_WORD, GCB_WORD,
                                  GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_EXECUTE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_CLOSE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_UPDATE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_RECONNECT:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_OBJECTCONVERT:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                        GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_GETLINKUPDATEOPTIONS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_SETLINKUPDATEOPTIONS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_RENAME:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYNAME:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYTYPE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYBOUNDS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYSIZE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYOPEN:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYOUTOFDATE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYRELEASESTATUS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYRELEASEERROR:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_QUERYRELEASEMETHOD:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_REQUESTDATA:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_OBJECTLONG:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_OV_CHANGEDATA:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_DWORD,
                                GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_SAVE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_CLOSE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_SETHOSTNAMES:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_SETDOCDIMENSIONS:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_GETOBJECT:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                                GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_RELEASE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_SETCOLORSCHEME:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SDV_EXECUTE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_OPEN:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                        GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_CREATE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                        GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_CREATEFROMTEMPLATE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                        GCB_DWORD, GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_EDIT:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_DWORD, GCB_DWORD,
                        GCB_DWORD, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_EXIT:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_RELEASE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_ENDLIST );
        break;
    case GETPROC_OLE_SV_EXECUTE:
        ptr = GetCallbackRoutine( pp, GCB_DWORD, GCB_WORD, GCB_ENDLIST );
        break;
#endif
    default:
        if( type >= GETPROC_USERDEFINED_1 && type <= GETPROC_USERDEFINED_32 ) {
            ptr = UserProcs[ type - GETPROC_USERDEFINED_1 ];
        }
        break;
    }
    return( ptr );

} /* GetProc16 */
