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


// Msg Class
//
// 95/01/31 -- J.W.Welch        -- defined

#include <string.h>

#include "BuildMsg.h"
#include "EncStats.h"
#include "Msg.h"
#include "Utility.h"

static RingHdr Msg::messageRing; // ring of messages


//---------------------------------------------------------------------
// Msg Implementation
//---------------------------------------------------------------------


Msg::Msg                        // CONSTRUCTOR
    ( unsigned short msg_no     // - message number
    , byte count                // - number of words
    , Word ** words )           // - word vector
    : Ring()
    , _number( msg_no )
    , _word_count( count )
    , _code_count( 0 )
    , _level( 0 )
    , _type( MSG_TYPE_ERROR )
    , _words( 0 )
{
    _words = (Word const * const *)new Word const *[ count ];
    memcpy( (void*)_words, words, count * sizeof( Word* ) );
    messageRing.append( this );
}


Msg::~Msg                       // DESTRUCTOR
    ( void )
{
    delete [] (void*)_words;
}


unsigned Msg::encodedSize       // GET ENCODED SIZE
    ( void ) const
{
    unsigned size;              // - size of encoding
    unsigned index;             // - index thru words

    size = 1;
    for( index = _word_count ; index > 0; ) {
        --index;
        size += _words[ index ]->encodedSize();
    }
    return size;
}


static void Msg::setModifier    // SET MESSAGE MODIFIER
    ( MsgType type              // - message type
    , byte level )              // - level
{
    Msg* msg = (Msg*)messageRing.last();
    msg->_type = type;
    msg->_level = level;
}


static void Msg::writeEncoded   // WRITE ENCODING FOR A MESSAGE
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    MsgIter iter;               // - iterator
    char buf[256];              // - buffer
    char *bptr;                 // - buffer ptr

    for( ; ; ) {
        Msg* msg = iter.next();
        if( 0 == msg ) break;
        bptr = concatStr( buf, "/" "* MESSAGE " );
        bptr = concatDec( bptr, msg->_number );
        bptr = concatStr( bptr, " *" "/" );
        out.write( buf );
        Word const * const * awrd = msg->_words;
        for( int count = msg->_word_count; count > 0; --count, ++awrd ) {
            (*awrd)->writeMsgEncoding( stats, buf );
            out.write( buf );
        }
        out.write( ", 0" );
    }
}


static void Msg::writeGroupOffsets // WRITE OFFSETS
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    MsgIter iter;               // - iterator
    char buf[256];              // - buffer

    for( ; ; ) {
        Msg* msg = iter.next();
        if( 0 == msg ) break;
        if( 0 == msg->_number ) {
            stats.writeSizeWritten( buf, 1 );
            out.write( buf );
        } else {
            ++stats.size_written;
        }
    }
}


static void Msg::writeLevels    // WRITE LEVELS INTO FILE
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    MsgIter iter;               // - iterator
    char buf[256];              // - buffer

    for( ; ; ) {
        Msg* msg = iter.next();
        if( 0 == msg ) break;
        buf[0] = stats.delim;
        concatHex( &buf[1], ( msg->_type << 4 ) + msg->_level );
        out.write( buf );
        stats.delim = ',';
        stats.size_written ++;
    }
}


static void Msg::writeOffsets   // WRITE OFFSETS
    ( EncStats& stats           // - statistics
    , FileCtlOutput& out )      // - output file
{
    MsgIter iter;               // - iterator
    char buf[256];              // - buffer

    for( ; ; ) {
        Msg* msg = iter.next();
        if( 0 == msg ) break;
        stats.writeSizeWritten( buf, msg->encodedSize() );
        out.write( buf );
    }
}
