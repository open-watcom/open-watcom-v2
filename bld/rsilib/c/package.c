/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi package manipulation functions.
*
****************************************************************************/


#include "rsi1632.h"

static int __fstreq( char FarPtr p1, char FarPtr p2 )
{
    while( *p1 == *p2 ) {
        if( *p1 == 0 )
            return( 1 );
        ++p1;
        ++p2;
    }
    return( 0 );
}

PACKAGE FarPtr rsi_find_package( char FarPtr name )
{
    PACKAGE FarPtr  p;

    for( p = _d16info.package_info_p; p != NULL_PTR; p = p->next_package ) {
        if( __fstreq( p->package_title, name ) ) {
            return( p );
        }
    }
    return( NULL_PTR );
}

ACTION *rsi_find_action( PACKAGE FarPtr pkg, char FarPtr name )
{
    ACTION_PACK FarPtr  act;
    int                 i;

    act = pkg->system_action;
    for( i = 0; i < pkg->system_action_count; ++i, ++act ) {
        if( __fstreq( act->action_name, name ) ) {
            return( act->action );
        }
    }
    act = pkg->user_action;
    for( i = 0; i < pkg->user_action_count; ++i, ++act ) {
        if( __fstreq( act->action_name, name ) ) {
            return( act->action );
        }
    }
    return( NULL_PTR );
}

