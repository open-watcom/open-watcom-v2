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


#ifndef __MSGDSCR_H__
#define __MSGDSCR_H__

#include <stdlib.h>

#define MAX_MSGS 1024           // maximum # messages

// MsgDscr -- messages descriptor

class MsgDscr                           // MsgDscr -- message-file description
{                                       // - private data
    unsigned char file[ MAX_MSGS ];     // - - # files per message
    const char *file_prefix;            // - - file-name prefix
    int max_msg;                        // - - maximum message
    static char filename[ _MAX_FNAME ]; // - - buffer for file name (one only)
                                        // - private functions:
    MsgDscr( const MsgDscr& );          // - - copy CTOR (never defined)
public:                                 // - public functions:
    MsgDscr(                            // - - CTOR
        const char *prefix );           // - - - prefix for file
    int AddFile(                        // - - add a file for msg_no
        int msg_no );                   // - - - message number
    int GetNextFile(                    // - - GET NEXT SUB-FILE NO. FOR MSG
        int msg_no,                     // - - - message no.
        int last_no );                  // - - - last no.
    int GetMaxMsgNo(                    // - - GET MAX. MSG NO.
        void );
    const char *GetFileName(            // - - GET FILE NAME
        int msg_no,                     // - - - msg number
        unsigned char file_no );        // - - - file number
};
#endif
