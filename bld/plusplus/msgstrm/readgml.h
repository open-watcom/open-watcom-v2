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


#ifndef __READGML_H
#define __READGML_H

class ifstream;

struct GMLfilestat {
    int         last_good;
    int         last_bad;

                GMLfilestat( int a, int b )
                    { last_good = a; last_bad = b; };
};

enum GoodBad {
    GOOD = 0,
    BAD
};


void MakeTestFileName(          // MAKE NEW FILE NAME
    char* filename,             // - file name
    MsgDscr& file_des,          // - files descriptor
    int msg_ctr,                // - message no.
    int file_ctr )              // - file no.
;

GMLfilestat ReadGMLFile( MsgDscr& files_ok, MsgDscr& files_er );
extern int  DontActuallyDoAnything;

#endif
