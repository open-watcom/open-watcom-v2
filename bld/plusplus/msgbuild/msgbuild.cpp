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


// MSGBUILD.CPP -- version(2) message compression
//
// 95/01/30 -- J.W.Welch        -- defined

#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "BuildMsg.h"
#include "Exc.h"
#include "FileCtl.h"
#include "Msg.h"
#include "Ring.h"
#include "Utility.h"
#include "Word.h"


// STATIC STORAGE

static FileCtlInput  gml;       // input file
static FileCtlOutput codes;     // codes file
static FileCtlOutput msgs;      // encoded messages
static FileCtlOutput levels;    // levels file
static unsigned line_ctr;       // line counter
static EncStats stats;          // statistics


static char const * const help_text[] = // - help text
{   "Usage: msgencod source message codes levels" ,
    "" ,
    "Example: msgencod ftnmsgs ftnerrs ftnhead levels" ,
    "" ,
    "The program takes the \"source\" (in GML format) and produces a" ,
    "\"message.c\" file, a \"codes.h\" file, and a \"levels.h\" file." ,
    "" ,
    "The \"message.c\" file contains all the messages (encoded).",
    "The \"codes.h\" file contains tags for the messages." ,
    "The \"levels.h\" file contains message levels for the messages." ,
    NULL
};


static void display_help()      // DISPLAY HELP TEXT
{
    for( char const * const *text = help_text
       ; *text != NULL
       ; ++ text ) {
        puts( *text );
    }
}


static char const* scanOverWS   // SCAN OVER WHITE SPACE
    ( char const *scan )
{
    for( ; isspace( *scan ); ++ scan );
    return scan;
}


static char const* scanOverBS   // SCAN OVER BLACK SPACE
    ( char const *scan )
{
    for( ; *scan != '\0' && ! isspace( *scan ); ++ scan );
    return scan;
}

enum ProcState                  // ProcState: processing state
{   START_LINE                  // - beginning of program
,   FOUND_EOF                   // - found EOF
,   GROUP_LINE                  // - :MSGGRP found
,   SYM_GROUP                   // - :MSGSYM within a group
,   TEXT_GROUP                  // - :MSGTXT within a group
,   SYM_NO_GRP                  // - :MSGSYM first in file
,   TEXT_NO_GRP                 // - :MSGTXT without a group
,   MOD_GROUP                   // - modifier within group
,   MOD_NO_GRP                  // - modifier without group
,   FOUND_PHR                   // - :MSGPHR
,   MAX_STATE                   // - highest legal state possible
,   PRE_EOF                     // - premature end-of-dile
,   GRP_NOT_FRST                // - Group not first file
,   MISPL_TXT                   // - misplaced text token
,   MISPL_SYM                   // - misplaced symbol token
,   MISPL_MOD                   // - misplaced modifier
,   DUP_MOD                     // - more than one modifier
};

static ProcState stateTable[MAX_STATE][MAX_REC] =
//  GROUP      SYMBOL     TEXT        MODIFIER   EOF
{ GROUP_LINE  ,SYM_NO_GRP,MISPL_TXT  ,MISPL_MOD ,PRE_EOF   // START_LINE
, FOUND_EOF   ,FOUND_EOF ,FOUND_EOF  ,FOUND_EOF ,FOUND_EOF // FOUND_EOF
, GROUP_LINE  ,SYM_GROUP ,MISPL_TXT  ,MISPL_MOD ,PRE_EOF   // GROUP_LINE
, MISPL_SYM   ,MISPL_SYM ,TEXT_GROUP ,MISPL_MOD ,PRE_EOF   // SYM_GROUP
, GROUP_LINE  ,SYM_GROUP ,MISPL_TXT  ,MOD_GROUP ,FOUND_EOF // TEXT_GROUP
, GRP_NOT_FRST,MISPL_SYM ,TEXT_NO_GRP,MISPL_MOD ,PRE_EOF   // SYM_NO_GRP
, GRP_NOT_FRST,SYM_NO_GRP,MISPL_TXT  ,MOD_NO_GRP,FOUND_EOF // TEXT_NO_GRP
, GROUP_LINE  ,SYM_GROUP ,MISPL_TXT  ,DUP_MOD   ,FOUND_EOF // MOD_GROUP
, GRP_NOT_FRST,SYM_NO_GRP,MISPL_TXT  ,DUP_MOD   ,FOUND_EOF // MOD_NO_GRP
, FOUND_PHR   ,FOUND_PHR ,FOUND_PHR  ,FOUND_PHR ,FOUND_PHR // FOUND_PHR
};

struct _KwGml
{   char const *kw;
    RecType rec_type;
    MsgType msg_type;
    unsigned char kw_size;
    unsigned :0;
};

#define kw( text, rec, msg ) { text, rec, msg, sizeof( text ) - 1 }

static _KwGml const kwTable[] = // table of keywords
{   kw( "MSGSYM"    , REC_SYM, MSG_TYPE_ERROR )
,   kw( "MSGTXT"    , REC_TXT, MSG_TYPE_ERROR )
,   kw( "WARNING"   , REC_MOD, MSG_TYPE_WARNING )
,   kw( "ANSICOMP"  , REC_MOD, MSG_TYPE_ANSICOMP )
,   kw( "ANSIERR"   , REC_MOD, MSG_TYPE_ANSIERR )
,   kw( "ANSIWARN"  , REC_MOD, MSG_TYPE_ANSIWARN )
,   kw( "ANSI"      , REC_MOD, MSG_TYPE_ANSI )
,   kw( "EXTWARN"   , REC_MOD, MSG_TYPE_EXTWARN )
,   kw( "INFO"      , REC_MOD, MSG_TYPE_INFO )
,   kw( "MSGGRP"    , REC_GRP, MSG_TYPE_ERROR )
,   kw( "MSGPHR"    , REC_PHR, MSG_TYPE_ERROR )
,   kw( 0           , REC_NONE,MSG_TYPE_ERROR )
};

#undef kw


static void scanError           // THROW SCANNING ERROR
    ( char const *msg )         // - message
{
    char buffer[16];
    utoa( line_ctr, buffer, 10 );
    throw Exc( "SCAN", msg, "on line", buffer, NULL );
}


static void checkEOL            // CHECK FOR END-OF-LINE
    ( char const *text )        // - scanner
{
    text = scanOverWS( text );
    if( '\0' != *text ) {
        scanError( "extra stuff on line" );
    }
}


static byte scanWords           // SCAN WORDS ON A LINE
    ( char const *text          // - scanner
    , Word** words )            // - words vector
{
    byte word_count;            // - # words
    char const * word_beg;      // - start of word
    unsigned message_size;      // - size of message

    message_size = 0;
    for( word_count = 0; ; ++word_count ) {
        text = scanOverWS( text );
        if( '\0' == *text ) break;
        word_beg = text;
        text = scanOverBS( text );
        unsigned size = text - word_beg;
        message_size += 1 + size;
        words[ word_count ] = Word::newWord( word_beg, size );
    }
    if( message_size > stats.max_message ) {
        stats.max_message = message_size;
    }
    stats.space_read_in += message_size;
    return word_count;
}


static byte scanLevel           // SCAN MESSAGE LEVEL
    ( char const *text )        // - scanner
{
    byte retn;                  // - scanned level

    text = scanOverWS( text );
    if( *text == '\0' ) {
        scanError( "missing level" );
    }
    if( ! isdigit( *text ) ) {
        scanError( "invalid level" );
    }
    retn = (byte)( *text - '0' );
    ++text;
    if( *text != '\0' ) {
        if( ! isdigit( *text ) ) {
            scanError( "invalid level" );
        }
        retn = (byte)( retn * 10 + *text - '0' );
        ++ text;
    }
    checkEOL( text );
    return retn;
}


static void processText         // PROCESS :MSGTXT
    ( char const *text          // - scanner
    , unsigned short sym_num )  // - symbol number
{
    Word* words[50];            // - words in message
    byte word_count;            // - # words

    word_count = scanWords( text, words );
    new Msg( sym_num, word_count, words );
}


static void processSymbol       // PROCESS :MSGSYM
    ( char const *text          // - scanner
    , unsigned short grp_num    // - group number
    , unsigned short sym_num )  // - symbol number
{
    char buf[256];              // - buffer
    char* bptr;                 // - buffer ptr

    text = scanOverWS( text );
    char const* symbeg = text;
    text = scanOverBS( text );
    checkEOL( text );
    unsigned size = text - symbeg;
    if( 0 == size ) {
        scanError( "no symbol for :MSGSYM" );
    }
    char* sym = (char*)alloca( size + 1 );
    sym[ size ] = '\0';
    memcpy( sym, symbeg, size );
    bptr = concatStr( buf, "#define " );
    bptr = concatStr( bptr, sym );
    bptr[0] = ' ';
    bptr = concatHex( &bptr[1], ( grp_num << 10 ) + sym_num );
    codes.write( buf );
}


static void processInput        // PROCESS INPUT
    ( void )
{
    char buffer[256];           // - record buffer
    RecType rec_type;           // - record type
    MsgType msg_type;           // - message type
    ProcState proc_state;       // - processing state
    char const *text;           // - scanner in buffer
    unsigned short groupNum;    // - group number
    unsigned short symbolNum;   // - symbol number

    groupNum = 0;
    symbolNum = 0;
    proc_state = START_LINE;
    for( ; ; ) {
        if( gml.getline( buffer, sizeof( buffer ) ) ) {
            ++line_ctr;
            text = scanOverWS( buffer );
            if( text[0] != ':' ) {
                continue;
            } else {
                ++text;
                for( _KwGml const *kwp = kwTable; ; ++ kwp ) {
                    char const *kw = kwp->kw;
                    if( 0 == kw ) {
                        rec_type = REC_NONE;
                        break;
                    } else {
                        unsigned size = kwp->kw_size;
                        if( 0 == memicmp( text, kw, size ) ) {
                            rec_type = kwp->rec_type;
                            msg_type = kwp->msg_type;
                            text += size;
                            if( text[ 0 ] == '.' ) {
                                ++ text;
                            }
                            break;
                        }
                    }
                }
            }
            if( rec_type == REC_NONE ) continue;
        } else {
            rec_type = REC_EOF;
        }
        ProcState next_state = stateTable[ proc_state ][ rec_type ];
        switch( next_state ) {
          case GROUP_LINE :
            groupNum++;
            symbolNum = 0;
//          processGroup( text );
            proc_state = next_state;
            continue;
          case SYM_NO_GRP :
            processSymbol( text, 0, symbolNum  );
            proc_state = next_state;
            continue;
          case SYM_GROUP :
            processSymbol( text, groupNum, symbolNum  );
            proc_state = next_state;
            continue;
          case TEXT_GROUP :
          case TEXT_NO_GRP :
            processText( text, symbolNum );
            ++ symbolNum;
            proc_state = next_state;
            continue;
          case FOUND_PHR :
//          processPhrase( text );
            continue;
          case MOD_GROUP :
          case MOD_NO_GRP :
          { byte level;
            switch( msg_type ) {
              case MSG_TYPE_ERROR :
              case MSG_TYPE_WARNING :
              case MSG_TYPE_ANSI :
              case MSG_TYPE_EXTWARN :
              case MSG_TYPE_ANSIWARN :
                level = scanLevel( text );
                break;
              case MSG_TYPE_INFO :
              case MSG_TYPE_ANSIERR :
              case MSG_TYPE_ANSICOMP :
                level = 0;
                break;
            }
            Msg::setModifier( msg_type, level );
          } continue;
          case PRE_EOF :
            scanError( "Premature End-of-File" );
          case GRP_NOT_FRST :
            scanError( "Group is not first token in file" );
          case MISPL_TXT :
            scanError( "Misplaced text token" );
          case MISPL_SYM :
            scanError( "Misplaced symbol token" );
          case MISPL_MOD :
            scanError( "Misplaced message modifier" );
          case DUP_MOD :
            scanError( "More than one message modifier" );
          case FOUND_EOF :
            break;
          default :
            scanError( "Error when reading new state" );
        }
        break;
    }
}


static void produceMessages     // PRODUCE MESSAGES
    ( void )
{
    char buf[256];              // - buffer
    char* bptr;                 // - buffer ptr

    msgs.write( "#define ENC_BIT 0x80" );
    msgs.write( "#define LARGE_BIT 0x40" );
    bptr = concatStr( buf, "#define MAX_MSG " );
    bptr = concatHex( bptr, stats.max_message );
    msgs.write( buf );
    msgs.write( "" );
    Word::sort();
    unsigned short enced = Word::encode();
    msgs.write( "MSG_SCOPE unsigned short MSG_MEM group_table[]= " );
    stats.size_written = enced;
    stats.delim = '{';
    Msg::writeGroupOffsets( stats, msgs );
    msgs.write( "};\n" );
    msgs.write( "MSG_SCOPE unsigned short MSG_MEM text_table[] = " );
    stats.size_written = 0;
    stats.delim = '{';
    Word::writeOffsets( stats, msgs );
    msgs.write( "// messages" );
    Msg::writeOffsets( stats, msgs );
    msgs.write( "};\n" );
    bptr = concatStr( buf, "MSG_SCOPE unsigned short MSG_MEM word_count = " );
    bptr = concatDec( bptr, enced );
    bptr = concatStr( bptr, ";\n" );
    msgs.write( buf );
    msgs.write( "MSG_SCOPE unsigned char MSG_MEM encoded_text[] =" );
    stats.delim = '{';
    Word::writeEncoded( stats, msgs );
    Msg::writeEncoded( stats, msgs );
    msgs.write( "};\n" );
}


static char const * const levelData[] = // constant level data
{   "typedef enum"
,   "{   MSG_TYPE_ERROR"
,   ",   MSG_TYPE_WARNING"
,   ",   MSG_TYPE_INFO"
,   ",   MSG_TYPE_ANSI"
,   ",   MSG_TYPE_ANSIERR"
,   ",   MSG_TYPE_ANSIWARN"
,   ",   MSG_TYPE_ANSICOMP"
,   ",   MSG_TYPE_EXTWARN"
,   "} MSG_TYPE;"
,   " "
,   "static unsigned char msg_level[] ="
,   0
};

static void produceLevels       // PRODUCE LEVELS
    ( void )
{
    for( char const * const * dp = levelData; *dp != 0; ++dp ) {
        levels.write(  *dp );
    }
    stats.delim = '{';
    Msg::writeLevels( stats, levels );
    levels.write( "};\n" );
}


int main                        // MAIN-LINE
    ( int count                 // - # args
    , char const *args[] )      // - arguments
{
    int retn;                   // - return code
    char buf[256];              // - buffer
    char* bptr;                 // - buffer ptr

    try {
        if( count != 5 ) {
            display_help();
            retn = 1;
        } else {
            gml.open( args[1], "gml" );
            codes.open( args[3], "gh" );
            processInput();
            codes.close();
            gml.close();
            msgs.open( args[2], "gc" );
            produceMessages();
            msgs.close();
            levels.open( args[4], "gh" );
            produceLevels();
            levels.close();
            Space percent = ( stats.space_put_out * 100
                            + stats.space_read_in / 2 )
                            / stats.space_read_in;
            bptr = concatStr( buf, "original size = " );
            bptr = concatDec( bptr, stats.space_read_in );
            bptr = concatStr( bptr, " encoded size = " );
            bptr = concatDec( bptr, stats.space_put_out );
            bptr = concatStr( bptr, " compression = " );
            bptr = concatDec( bptr, percent );
            bptr[0] = '%';
            bptr[1] = '\0';
            puts( buf );
            retn = 0;
        }
    } catch( Exc const& except ) {
        puts( except );
        retn = -1;
    } catch( ... ) {
        puts( "MSGBUILD -- unexpected exception" );
        retn = -1;
    }
    return 0;
}
