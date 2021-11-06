/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Spy message functions.
*
****************************************************************************/


#include "spy.h"
#include <dde.h>


static message *userMsg;
static message *appMsg;

/*
 * GetMessageDataFromID - use message id to look up message structure
 */
message *GetMessageDataFromID( UINT msgid, const char *class_name )
{
    WORD    i;
    WORD    j;

    for( i = 0; i < ClassMessagesSize; i++ ) {
        if( i == 0 || stricmp( class_name, ClassMessages[i].class_name ) == 0 ) {
            for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
                if( msgid == ClassMessages[i].message_array[j].id ) {
                    return( &ClassMessages[i].message_array[j] );
                }
            }
        }
    }
    return( NULL );

} /* GetMessageDataFromID */

/*
 * ProcessIncomingMessage - get a string associated with a message id
 */
void ProcessIncomingMessage( UINT msgid, const char *class_name, char *res )
{
    message     *msg;
    const char  *fmtstr;
    char        buf[256];
    char        hexstr[20];

    res[0] = '\0';
    msg = GetMessageDataFromID( msgid, class_name );
    if( msg != NULL ) {
        if( msg->watch ) {
            strcpy( res, msg->str );
        }
        if( msg->stopon ) {
            SetSpyState( OFF );
            RCsprintf( buf, STR_SPYING_STOPPED, msg->str );
            MessageBox( SpyMainWindow, buf, SpyName, MB_OK | MB_ICONINFORMATION );
        }
        msg->count++;
    } else if( msgid >= WM_USER && msgid < WM_APP ) {
        userMsg->count++;
        if( userMsg->watch ) {
            fmtstr = GetRCString( STR_WM_USER_PLUS );
            sprintf( res, fmtstr, msgid - WM_USER );
        }
        if( userMsg->stopon ) {
            SetSpyState( OFF );
            fmtstr = GetRCString( STR_WM_USER_PLUS );
            sprintf( res, fmtstr, msgid - WM_USER );
            RCsprintf( buf, STR_SPYING_STOPPED, res );
            MessageBox( SpyMainWindow, buf, SpyName, MB_OK | MB_ICONINFORMATION );
        }
    } else if( msgid >= WM_APP && msgid < 0xC000 ) {
        appMsg->count++;
        if( appMsg->watch ) {
            fmtstr = GetRCString( STR_WM_APP_PLUS );
            sprintf( res, fmtstr, msgid - WM_APP );
        }
        if( appMsg->stopon ) {
            SetSpyState( OFF );
            fmtstr = GetRCString( STR_WM_APP_PLUS );
            sprintf( res, fmtstr, msgid - WM_APP );
            RCsprintf( buf, STR_SPYING_STOPPED, res );
            MessageBox( SpyMainWindow, buf, SpyName, MB_OK | MB_ICONINFORMATION );
        }
    } else {
        if( Filters[MC_UNKNOWN].watch ) {
            GetHexStr( hexstr, msgid, 0 );
            fmtstr = GetRCString( STR_UNKNOWN_MSG );
            sprintf( res, fmtstr, hexstr );
        }
        if( Filters[MC_UNKNOWN].stopon ) {
            SetSpyState( OFF );
            GetHexStr( hexstr, msgid, 0 );
            fmtstr = GetRCString( STR_UNKNOWN_MSG );
            sprintf( res, fmtstr, hexstr );
            RCsprintf( buf, STR_SPYING_STOPPED, res );
            MessageBox( SpyMainWindow, buf, SpyName, MB_OK | MB_ICONINFORMATION );
        }
    }

} /* ProcessIncomingMessage */


/*
 * SetFilterMsgs
 */
void SetFilterMsgs( MsgClass type, bool val, bool is_watch )
{
    WORD    i;
    WORD    j;

    for( i = 0; i < ClassMessagesSize; i++ ) {
        for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
            if( ClassMessages[i].message_array[j].type == type ) {
                if( is_watch ) {
                    ClassMessages[i].message_array[j].watch = val;
                } else {
                    ClassMessages[i].message_array[j].stopon = val;
                }
            }
        }
    }

} /* SetFilterMsgs */

/*
 * SetFilterSaveBitsMsgs
 */
void SetFilterSaveBitsMsgs( MsgClass type, bool val, bool *bits )
{
    WORD    i;
    WORD    j;
    WORD    k;

    k = 0;
    for( i = 0; i < ClassMessagesSize; i++ ) {
        for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
            if( ClassMessages[i].message_array[j].type == type ) {
                bits[k] = val;
            }
            ++k;
        }
    }

} /* SetFilterSaveBitsMsgs */

/*
 * InitMessages - init. messages structres
 */
void InitMessages( void )
{
    userMsg = GetMessageDataFromID( WM_USER, NULL );
    appMsg = GetMessageDataFromID( WM_APP, NULL );

} /* InitMessages */

/*
 * SaveBitState - save current watch/stopon state
 */
bool *SaveBitState( bool is_watch )
{
    bool        *data;
    WORD        i;
    WORD        j;
    WORD        k;
    bool        ft;

    data = MemAlloc( TotalMessageArraySize );
    if( data == NULL ) {
        return( NULL );
    }
    k = 0;
    for( i = 0; i < ClassMessagesSize; i++ ) {
        for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
            if( is_watch ) {
                ft = ClassMessages[i].message_array[j].watch;
            } else {
                ft = ClassMessages[i].message_array[j].stopon;
            }
            data[k++] = ft;
        }
    }
    return( data );

} /* SaveBitState */

/*
 * CloneBitState - make a copy of a saved bitstate
 */
bool *CloneBitState( bool *old ) {
    bool        *data;

    data = MemAlloc( TotalMessageArraySize );
    if( data == NULL ) {
        return( NULL );
    }
    memcpy( data, old, TotalMessageArraySize );
    return( data );

} /* CloneBitState */

/*
 * RestoreBitState - put back watch/stopon state
 */
void RestoreBitState( bool *data, bool is_watch )
{
    WORD        i;
    WORD        j;
    WORD        k;
    bool        ft;

    if( data == NULL ) {
        return;
    }
    k = 0;
    for( i = 0; i < ClassMessagesSize; i++ ) {
        for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
            ft = data[k++];
            if( is_watch ) {
                ClassMessages[i].message_array[j].watch = ft;
            } else {
                ClassMessages[i].message_array[j].stopon = ft;
            }
        }
    }
    MemFree( data );

} /* RestoreBitState */

/*
 * FreeBitState - release memory used by a saved bit state
 *              - should not be called for states for which RestoreBitState
 *                has been called
 */
void FreeBitState( bool *data )
{
    MemFree( data );

} /* FreeBitState */

/*
 * CopyBitState
 */
void CopyBitState( bool *dst, bool *src )
{
    memcpy( dst, src, TotalMessageArraySize );

} /* CopyBitState */

/*
 * ClearMessageCount - clear count of each type of message
 */
void ClearMessageCount( void )
{
    WORD        i;
    WORD        j;

    for( i = 0; i < ClassMessagesSize; i++ ) {
        for( j = 0; j < ClassMessages[i].message_array_size; j++ ) {
            ClassMessages[i].message_array[j].count = 0L;
        }
    }

} /* ClearMessageCount */

