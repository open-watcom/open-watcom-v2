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


#ifndef __MSG_H__
#define __MSG_H__

// message class

#include "BuildMsg.h"
#include "Ring.h"
#include "Word.h"

class Msg                       // Message
    : public Ring               // *******
{
    Word const * const * _words;// vector of words
    unsigned short _number;     // message number
    byte _word_count;           // number of words
    byte _code_count;           // number of codes
    byte _level;                // message level
    MsgType _type;              // message type
    unsigned :0;

    static RingHdr messageRing; // ring of messages

public:

    Msg                         // constructor
        ( unsigned short        // - message number
        , byte                  // - # words
        , Word ** )             // - words
    ;
    ~Msg                        // destructor
        ( void )
    ;
    unsigned encodedSize        // GET ENCODED SIZE
        ( void ) const
    ;
    static void Msg::setModifier// SET LAST-MESSAGE MODIFIER
        ( MsgType               // - message type
        , byte )                // - level
    ;
    static void writeEncoded    // WRITE ENCODED FORMS
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;
    static void writeGroupOffsets // WRITE GROUP OFFSETS
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;
    static void writeLevels     // WRITE LEVEL INFORMATION
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;
    static void writeOffsets    // WRITE OFFSETS
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;

friend class MsgIter;
};


class MsgIter                   // Message Iterator
    : public RingIter           // ****************
{
public:
    inline
    MsgIter                     // CONSTRUCTOR
        ( void )
    ;
    inline
    Msg* next                   // GET NEXT MESSAGE
        ( void )
    ;
};


//---------------------------------------------------------------------
// MsgIter Implementation
//---------------------------------------------------------------------


MsgIter::MsgIter                // CONSTRUCTOR
    ( void )
    : RingIter( Msg::messageRing )
{
}


Msg* MsgIter::next              // GET NEXT WORD
    ( void )
{
    return (Msg*)RingIter::next();
}


#endif
