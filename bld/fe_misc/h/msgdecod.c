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
* Description:  Decode compressed messages.
*
****************************************************************************/


static char *decodeRaw(         // DECODE RAW WORD
    char const *table,          // - word table
    char *tgt,                  // - output location
    unsigned msg_start,         // - start of message
    unsigned msg_stop )         // - one past end of word
{
    char const *s;
    char const *t;
    char const *p;

    s = &table[ msg_start ];
    t = &table[ msg_stop ];
    for( p = s; p < t; ++p ) {
        *tgt = *p;
        ++tgt;
    }
    *tgt++ = ' ';
    return( tgt );
}

static char *decodePhrase(      // DECODE PHRASE
    char *tgt,                  // - output location
    unsigned msg_start,         // - start of message
    unsigned msg_stop )         // - one past end of message
{
    char const *word_table;
    unsigned msg_leader;
    unsigned word_index;
    unsigned word_start;
    unsigned word_stop;

    while( msg_start < msg_stop ) {
        msg_leader = msg_text[ msg_start++ ];
        if( msg_leader & ENC_BIT ) {
            if( msg_leader & LARGE_BIT ) {
                word_index = ( msg_leader & ~(ENC_BIT|LARGE_BIT) ) << 8;
                word_index |= msg_text[ msg_start++ ];
            } else {
                word_index = msg_leader & ~ENC_BIT;
            }
            word_table = word_text;
            word_start = word_base[ word_index ];
            word_stop = word_base[ word_index + 1 ];
        } else {
            word_table = (char const *)msg_text;
            word_start = msg_start;
            word_stop = msg_start + msg_leader;
            msg_start = word_stop;
        }
        tgt = decodeRaw( word_table, tgt, word_start, word_stop );
    }
    return( tgt );
}

static void decodeMsg(          // DECODE MESSAGE
    char *tgt,                  // - message location
    unsigned msg_num )          // - message number
{
    char *endp;
    unsigned msg_start;
    unsigned msg_stop;

    msg_start = msg_base[ msg_num ];
    msg_stop = msg_base[ msg_num + 1 ];
    endp = decodePhrase( tgt, msg_start, msg_stop );

    /* encodeRaw always adds a trailing space, remove it */
    if( ( endp != tgt ) && ( endp[-1] == ' ' ) ) {
        endp[-1] = '\0';
    } else {
        *endp = '\0';
    }
}


#ifdef MSGS_GROUPED
static unsigned indexMsg(       // GET ACTUAL INDEX FOR MESSAGE
    unsigned msg_num )          // - message number
{
    unsigned msg_group = msg_num >> 8;
    msg_num &= 255;
    msg_num += msg_group_base[ msg_group ];
    return msg_num;
}
#endif
