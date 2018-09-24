/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Implementation of delay() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include <time.h>


_WCRTLINK unsigned delay( unsigned mseconds )
{
    struct timespec     req, rem;

    req.tv_sec = mseconds / 1000;
    req.tv_nsec = ( mseconds % 1000 ) * 1000;
    nanosleep( &req, &rem );

    return( rem.tv_sec * 1000 + rem.tv_nsec / 1000 );
}
