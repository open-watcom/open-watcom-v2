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


#ifndef __EXCPGM_H__
#define __EXCPGM_H__

// ExcMsg -- message instance

class ExcPgm;

class ExcMsg
{                               // private functions:
                                // ------------------
virtual void display_msg_contents( // DISPLAY MESSAGE CONTENTS
    void ) const = 0;

friend class ExcPgm;
};


// ExcPgm -- general program exception
//
// 92/12/30 -- J.W.Welch            -- defined

class ExcPgm
{                               // private data:
private:                        // -------------

const char *pgmname;            // program name
int retval;                     // return value

private:                        // private functions:
                                // ------------------

void ExcPgm::diagnose_msg       // PRINT A MESSAGE
    ( ExcMsg& msg               // - message
    , const char* type )        // - type for message
;


public:                         // public functions:
                                // -----------------

ExcPgm::ExcPgm                  // CONSTRUCTOR
    ( const char *name          // - program name
        = 0                     //   ( default to 0 )
    , int retn                  // - return value
        = 0 )                   //   ( default to 0 )
;
int ExcPgm::diagnose_error      // PRINT ERROR DIAGNOSTIC
    ( ExcMsg& msg )             // - message
;

};

#endif
