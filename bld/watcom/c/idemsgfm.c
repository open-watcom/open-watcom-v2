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


#include <assert.h>
#include <string.h>

#include "idedll.h"

static char contextual_file[ 144 ];     // name of contextual file

#define TEXT_SEVERITY_WARNING   "Warning!"
#define TEXT_SEVERITY_ERROR     "Error!"
#define TEXT_SEVERITY_NOTE      "Note!"
#define TEXT_SEVERITY_NOTE_MSG  ""
#define TEXT_SEVERITY_BANNER    ""
#define TEXT_SEVERITY_DEBUG     "Debug!"

static char const * const text_severity[] =
{
#   define _IdeMsgSeverity(a) TEXT_SEVERITY_ ## a
    __IdeMsgSeverity
#   undef _IdeMsgSeverity
};

#define TEXT_MSG_NO_WARNING   "W"
#define TEXT_MSG_NO_ERROR     "E"
#define TEXT_MSG_NO_NOTE      "N"
#define TEXT_MSG_NO_NOTE_MSG  "N"
#define TEXT_MSG_NO_BANNER    "B"
#define TEXT_MSG_NO_DEBUG     "D"

static char const * const text_msg_no[] =
{
#   define _IdeMsgSeverity(a) TEXT_MSG_NO_ ## a
    __IdeMsgSeverity
#   undef _IdeMsgSeverity
};

#define _ARRAY_SIZE( a ) ( sizeof(a) / sizeof( (a)[0] ) )


typedef struct {                // CONSTRUCTION ELEMENT
    char* buffer;               // - buffer
    unsigned bsize;             // - buffer size
    unsigned used;              // - amount used
} CONSTRUCTION;


static void concatText          // CONCATENATE TEXT
    ( CONSTRUCTION* ct          // - construction info
    , char const *text )        // - text to be appended
{
    unsigned size = strlen( text ) + 1;
    unsigned left = ct->bsize - ct->used;
    if( size > left ) {
        size = left;
    }
    if( size >= 1 ) {
        char* tgt = &ct->buffer[ ct->used ];
        -- size;
        tgt[ size ] = '\0';
        if( size != 0 ) {
            ct->used += size;
            memcpy( tgt, text, size );
        }
    }
}


static void concatBlank         // CONCATENATE BLANK, IF NOT AT START
    ( CONSTRUCTION* ct )        // - construction info
{
    if( 0 != ct->used ) {
        concatText( ct, " " );
    }
}


static void concat3DigitNumber  // CONCATENTATE 3 DIGIT NUMBER
    ( CONSTRUCTION* ct          // - construction info
    , unsigned number )         // - the number
{
    char buf[32];

    utoa( number + ( number > 999 ? 10000 : 1000 )
        , buf
        , 10 );
    concatText( ct, buf + 1 );
}


static void concatNumber        // CONCATENTATE NUMBER
    ( CONSTRUCTION* ct          // - construction info
    , unsigned number )         // - the number
{
    char buf[32];

    utoa( number, buf, 10 );
    concatText( ct, buf );
}


static void concatSeverity      // CONCATENATE SEVERITY MESSAGE
    ( CONSTRUCTION* ct          // - construction info
    , IDEMsgInfo const * info ) // - message information
{
    concatBlank( ct );
    concatText( ct, text_severity[ info->severity ] );
}


static void concatMsgNumber     // CONCATENATE MESSAGE NUMBER
    ( CONSTRUCTION* ct          // - construction info
    , IDEMsgInfo const * info ) // - message information
{
    if( IDE_HAS_MSG_NUM( info ) ) {
        concatBlank( ct );
        if( IDE_HAS_MSG_GRP( info ) ) {
            if( '\0' == info->msg_group[0] ) {
                concatText( ct, text_msg_no[ info->severity ] );
                concat3DigitNumber( ct, info->msg_no );
            } else {
                concatText( ct, info->msg_group );
                concatText( ct, "-" );
                concat3DigitNumber( ct, info->msg_no & 255 );
            }
        } else {
            concatText( ct, text_msg_no[ info->severity ] );
            concat3DigitNumber( ct, info->msg_no );
        }
        concatText( ct, ":" );
    }
}


#if 0
static void displayContextMsg   // DISPLAY A CONTEXT MESSAGE
    ( IDECBHdl handle           // - handle for requestor
    , IDEMsgInfoFn displayer    // - display function
    , CONSTRUCTION* ct )        // - construction information
{
    IDEMsgInfo info;            // - info passed to display routine

    IdeMsgInit( &info, IDEMSGSEV_NOTE_MSG, ct->buffer );
    displayer( handle, &info );
    ct->used = 0;
}
#endif


// The "displayer" parameter is no longer required
//
void IdeMsgFormat               // FORMAT A MESSAGE
    ( IDECBHdl handle           // - handle for requestor
    , IDEMsgInfo const * info   // - message information
    , char * buffer             // - buffer
    , unsigned bsize            // - buffer size
    , IDEMsgInfoFn displayer )  // - display function
{
    CONSTRUCTION ct;            // - construction info

    handle = handle;
    displayer = displayer;  // ********** CAN BE REMOVED **********
    ct.buffer = buffer;
    ct.bsize = bsize;
    ct.used = 0;
    assert( info != NULL );
    assert( buffer != NULL );
    assert( bsize != 0 );
    assert( info->severity < _ARRAY_SIZE( text_severity ) );
    if( IDE_IS_READABLE( info ) ) {
#if 0

    // It is the responsibility of the processor to put out messages:
    //      File: current file
    //      Included from: file(line)
    //          ...
    //      Included from: file(line)
    // before a group of messages from the current file (and nesting).

        if( IDE_HAS_SRC_FILE( info ) ) {
            if( 0 != strncmp( info->src_file
                            , contextual_file
                            , sizeof( contextual_file ) - 1 )
              ) {
                concatText( &ct, "File: " );
                concatText( &ct, info->src_file );
                displayContextMsg( handle, displayer, &ct );
                strncpy( contextual_file
                       , info->src_file
                       , sizeof( contextual_file ) - 1 );
            }
        }
#endif
        switch( info->severity ) {
          case IDEMSGSEV_ERROR :
          case IDEMSGSEV_WARNING :
          case IDEMSGSEV_DEBUG :
            if( IDE_HAS_SRC_LINE( info ) ) {
                concatText( &ct, "(" );
                concatNumber( &ct, info->src_line );
                if( IDE_HAS_SRC_COL( info ) ) {
                    concatText( &ct, "," );
                    concatNumber( &ct, info->src_col );
                }
                concatText( &ct, "):" );
            }
            concatSeverity( &ct, info );
            concatMsgNumber( &ct, info );
            break;
          case IDEMSGSEV_NOTE :
            concatText( &ct, " " );
            break;
        }
    } else {
        switch( info->severity ) {
          case IDEMSGSEV_ERROR :
          case IDEMSGSEV_WARNING :
          case IDEMSGSEV_NOTE :
          case IDEMSGSEV_DEBUG :
            if( IDE_HAS_SRC_FILE( info ) ) {
                concatText( &ct, info->src_file );
                if( IDE_HAS_SRC_LINE( info ) ) {
                    concatText( &ct, "(" );
                    concatNumber( &ct, info->src_line );
                    concatText( &ct, "):" );
                }
            }
            concatSeverity( &ct, info );
            concatMsgNumber( &ct, info );
            if( IDE_HAS_SRC_COL( info ) ) {
                concatText( &ct, " col(" );
                concatNumber( &ct, info->src_col );
                concatText( &ct, ")" );
            }
            break;
        }
    }
    concatBlank( &ct );
    concatText( &ct, info->msg );
}


void IdeMsgStartDll             // START OF FORMATING FOR A DLL (EACH TIME)
    ( void )
{
    contextual_file[0] = '\0';
}
