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

#include "rempktnr.h"


#define REMOTE_PKT_NO_REPLY_BIT     0x80


/*
 * Remote request encapsulation
 * ----------------------------
 *
 * The remote packet protocol must transfer the no-reply control state
 * together with the request payload. The current wire representation
 * stores this state in bit 7 of the first transferred request byte.
 *
 * This bit is remote-link control information. It is not part of the
 * trap request ID or of the original request payload.
 *
 * This representation is possible because the first byte of the
 * current trap request contains the request ID and all request IDs are
 * below 0x80. Bit 7 of the original first request byte is therefore
 * always clear.
 *
 * RemotePktEncodeNoReply() adds the remote control information before
 * transmission.
 *
 * RemotePktDecodeNoReply() extracts the control information and clears
 * its wire bit, restoring the original request data before the request
 * is passed to the trap layer.
 *
 * The remote protocol therefore depends on both of these properties:
 *
 *   1. every transferred request contains at least one byte;
 *   2. bit 7 of the original first request byte is clear.
 *
 * If either the remote packet format or the layout and meaning of the
 * first trap request byte changes, the encoding must be changed here.
 * No upper layer should depend on this wire representation.
 */


bool RemotePktEncodeNoReply( void *data, bool no_reply )
/*******************************************************
 * Encode the remote no-reply state into the request data.
 *
 * The request data are modified in place. The function returns false
 * if the original first byte cannot be represented by the current
 * remote wire format.
 *
 * The caller must pass a valid request containing at least one byte.
 */
{
    /*
     * A set high bit in the original data cannot be represented by
     * the current encoding, because decoding would clear that bit and
     * would not restore the original payload.
     */
    if( (((char *)data)[0] & REMOTE_PKT_NO_REPLY_BIT) != 0 ) {
        return( false );
    }

    if( no_reply ) {
        ((char *)data)[0] |= REMOTE_PKT_NO_REPLY_BIT;
    }

    return( true );
}


bool RemotePktDecodeNoReply( void *data )
/*
 * Decode the remote no-reply state and restore the original request
 * data.
 *
 * The function returns the decoded no-reply state. After it returns,
 * the request data are in their original form and may be passed to
 * the trap layer.
 *
 * The caller must pass a valid request containing at least one byte.
 */
{
    bool no_reply;

    no_reply = (((char *)data)[0] & REMOTE_PKT_NO_REPLY_BIT) != 0;

    /*
     * Remove the remote control information and restore the original
     * first payload byte.
     */
    ((char *)data)[0] &= ~REMOTE_PKT_NO_REPLY_BIT;

    return( no_reply );
}
