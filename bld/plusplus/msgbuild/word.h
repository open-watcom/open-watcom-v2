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


#ifndef __WORD_H__
#define __WORD_H__

// Word Class

#include "EncStats.h"
#include "FileCtl.h"
#include "Ring.h"
#include "Str.h"

class Word                      // Word
    : public Ring               // ****
{
    Space _count;               // use count
    Str   _word;                // characters in word
    unsigned short _code;       // encoding

    static int word_compare     // compare counts for sort
        ( void const *
        , void const * )
    ;
    static unsigned hashFun     // hash function
        ( void const * )        // - a Word entry
    ;
    static int hashCompare      // hash comparison
        ( void const *          // - a Word entry
        , void const * )        // - comparand
    ;

    static RingHdr wordRing;    // ring of words

public:

    Word                        // constructor
        ( char const *          // - characters
        , unsigned )            // - # characters
    ;
    static unsigned short encode // make an encoding
        ( void )
    ;
    unsigned encodedSize        // GET ENCODED SIZE
        ( void ) const
    ;
    static Word* newWord        // get new word
        ( char const *          // - characters
        , unsigned )            // - # characters
    ;
    static void sort            // sort the words
        ( void )
    ;
    static void writeEncoded    // WRITE ENCODED WORDS
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;
#if 0
    void writeMsgEncoding       // WRITE ENCODED FORM IN MESSAGE
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
        const
    ;
#else
    void writeMsgEncoding       // WRITE ENCODED FORM IN MESSAGE
        ( EncStats &            // - statistics
        , char * )              // - buffer
        const
    ;
#endif
    static void writeOffsets    // WRITE OFFSETS PORTION OF TEXT TABLE
        ( EncStats &            // - statistics
        , FileCtlOutput & )     // - output file
    ;

friend class WordIter;
};


class WordIter                  // Word Iterator
    : public RingIter           // *************
{
public:
    inline
    WordIter                    // CONSTRUCTOR
        ( void )
    ;
    inline
    Word* next                  // GET NEXT
        ( void );
};


//---------------------------------------------------------------------
// WordIter Implementation
//---------------------------------------------------------------------


WordIter::WordIter              // CONSTRUCTOR
    ( void )
    : RingIter( Word::wordRing )
{
}


Word* WordIter::next            // GET NEXT WORD
    ( void )
{
    return (Word*)RingIter::next();
}

#endif
