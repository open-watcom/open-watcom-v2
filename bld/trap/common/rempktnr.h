/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  Remote packet no-reply encoding.
*
****************************************************************************/

#ifndef REMPKTNR_H_INCLUDED
#define REMPKTNR_H_INCLUDED

#include "bool.h"

extern bool RemotePktEncodeNoReply( void *data, bool no_reply );
extern bool RemotePktDecodeNoReply( void *data );

#endif
