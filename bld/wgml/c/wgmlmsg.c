/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML message definition.
*               adapted from wlink file (bld\wl\c\wlnkmsg.c)
*
****************************************************************************/


#include <fcntl.h>
#include "wgml.h"

#include "wreslang.h"
#if defined( USE_WRESLIB )
    #include "wressetr.h"
    #include "wresset2.h"
#else
#define GlobalFlags dummy1
    #include <windows.h>
#undef GlobalFlags
#endif
#include "wgmlmsgs.rh"

#include "clibint.h"
#include "clibext.h"


#if defined( USE_WRESLIB )
HANDLE_INFO         Instance = { 0 };
#else
HINSTANCE           Instance;
#endif

static unsigned     MsgShift;

/***************************************************************************/
/*  initialize messages from resource file                                 */
/***************************************************************************/

bool init_msgs( void )
{
#ifdef USE_WRESLIB
    char        fname[_MAX_PATH];

    Instance.status = 0;
    if( _cmdname( fname ) != NULL && OpenResFile( &Instance, fname ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( get_msg( ERR_DUMMY, fname, sizeof( fname ) ) ) {
            return( true );
        }
    }
    CloseResFile( &Instance );
    printf( "Resources not found\n" );
    return( false );
#else
    Instance = GetModuleHandle( NULL );
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
    return( true );
#endif
}


/***************************************************************************/
/*  get a msg text string                                                  */
/***************************************************************************/

bool get_msg( msg_ids resid, char *buff, unsigned buff_len )
{
#ifdef USE_WRESLIB
    if( Instance.status == 0
      || WResLoadString( &Instance, resid + MsgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#else
    if( LoadString( Instance, resid + MsgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

/***************************************************************************/
/*  end of msg processing                                                  */
/***************************************************************************/

void fini_msgs( void )
{
#ifdef USE_WRESLIB
    CloseResFile( &Instance );
#else
#endif
}
