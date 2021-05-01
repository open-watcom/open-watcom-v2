/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  InfoBench help file definition.
*
****************************************************************************/


#define IB_TOPIC_NAME           "::::"
#define IB_DEFAULT_TOPIC        "DEFTOPIC::::"
#define IB_DESCRIPTION          "DESCRIPTION::::"
#define IB_HEADER_BEG           ":h"
#define IB_HEADER_END           ":eh"
#define IB_TRAILER_BEG          ":t"
#define IB_TRAILER_END          ":et"

#define IB_TOPIC_NAME_SIZE      (sizeof( IB_TOPIC_NAME ) - 1)
#define IB_DEFAULT_TOPIC_SIZE   (sizeof( IB_DEFAULT_TOPIC ) - 1)
#define IB_DESCRIPTION_SIZE     (sizeof( IB_DESCRIPTION ) - 1)

#define IS_IB_TOPIC_NAME(s)     (memcmp((s), IB_TOPIC_NAME, IB_TOPIC_NAME_SIZE) == 0)
#define IS_IB_DEFAULT_TOPIC(s)  (memcmp((s), IB_DEFAULT_TOPIC, IB_DEFAULT_TOPIC_SIZE) == 0)
#define IS_IB_DESCRIPTION(s)    (memcmp((s), IB_DESCRIPTION, IB_DESCRIPTION_SIZE) == 0)
#define IS_IB_HEADER_BEG(s)     (memcmp((s), IB_HEADER_BEG, sizeof( IB_HEADER_BEG ) - 1) == 0)
#define IS_IB_HEADER_END(s)     (memcmp((s), IB_HEADER_END, sizeof( IB_HEADER_END ) - 1) == 0)
#define IS_IB_TRAILER_BEG(s)    (memcmp((s), IB_TRAILER_BEG, sizeof( IB_TRAILER_BEG ) - 1) == 0)
#define IS_IB_TRAILER_END(s)    (memcmp((s), IB_TRAILER_END, sizeof( IB_TRAILER_END ) - 1) == 0)

// Escape symbol
#define IB_ESCAPE               (char)'\x1B'
#define IB_ESCAPE_STR           "\x1B"

// Some special characters we use
#define IB_BULLET               (char)'\x07'
#define IB_SPACE_NOBREAK        (char)'\xFF'

// this symbol separates the hyper-link label and topic.
#define IB_HLINK                (char)'\xE0'
#define IB_HLINK_BREAK          (char)'\xE8'
#define IB_PLAIN_LINK_BEG       (char)'<'
#define IB_PLAIN_LINK_END       (char)'>'

#define IB_HLINK_STR            "\xE0"
#define IB_HLINK_BREAK_STR      "\xE8"
#define IB_PLAIN_LINK_BEG_STR   "<"
#define IB_PLAIN_LINK_END_STR   ">"

// undocumented symbols
#define IB_RIGHT_ARROW          (char)'\xAF'
#define IB_LEFT_ARROW           (char)'\xAE'

// InfoBench style codes
#define IB_BOLD_ON              'b'
#define IB_BOLD_OFF             'p'
#define IB_UNDERLINE_ON         'u'
#define IB_UNDERLINE_OFF        'w'

#define IB_BOLD_ON_STR          "\x1B" "b"
#define IB_BOLD_OFF_STR         "\x1B" "p"
#define IB_UNDERLINE_ON_STR     "\x1B" "u"
#define IB_UNDERLINE_OFF_STR    "\x1B" "w"
