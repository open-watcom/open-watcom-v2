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


#ifndef _ERRDEFNS_H
#define _ERRDEFNS_H

// ERRDEFNS.H -- C++ Error Definitions
//
// 91/06/04 -- J.W.Welch        -- defined

#include <stdarg.h>
#include "errmsgs2.gh"
#include "srcfile.h"
#include "vbuf.h"
#include "idedll.h"

#define ERR_NULL                ERR_CALL_WATCOM

// PROTOTYPES: (Exposed)

typedef unsigned                error_state_t;

typedef enum {
    MS_PRINTED          = 0x01, // message was printed
    MS_WARNING          = 0x02, // could be an error; but is a warning
    MS_NULL             = 0x00
} msg_status_t;

#define WLEVEL_DEFAULT  1
#define WLEVEL_MAX      9
#define WLEVEL_DISABLE  10
#define WLEVEL_ENABLE   1

typedef struct suicide_call_back SUICIDE_CALLBACK;
struct suicide_call_back {
    SUICIDE_CALLBACK            *next;          // (ring)
    void                        (*call_back)(void);
};

void AddNoteMessage(            // ADD A NOTE TO A MESSAGE
    MSG_NUM msg_num,            // - message number
    ... )                       // - error message arguments
;
bool MsgWillPrint(              // TEST WHETHER A MESSAGE WILL BE SEEN
    MSG_NUM msgnum )            // - message number
;
msg_status_t CErr(              // ISSUE ERROR
    MSG_NUM msgnum,             // - message number
    ... )                       // - parameters for error
;
msg_status_t CWarnDontCount(    // ISSUE WARNING BUT DON'T COUNT IT
    MSG_NUM msgnum,             // - message number
    ... )                       // - parameters for warning
;
msg_status_t CErr1(             // ISSUE ERROR (NO PARAMETERS)
    MSG_NUM msgnum )            // - message number
;
msg_status_t CErr2(             // ISSUE ERROR (int PARAMETER)
    MSG_NUM msgnum,             // - message number
    int p1 )                    // - parameter
;
msg_status_t CErr2p(            // ISSUE ERROR (ptr PARAMETER)
    MSG_NUM msgnum,             // - message number
    void const *p1 )            // - parameter
;
void CErrSuppress(
    error_state_t *saved_save )
;
void CErrSuppressRestore(
    unsigned count )
;
unsigned CErrUnsuppress(
    void )
;
bool CErrSuppressedOccurred(
    error_state_t *saved_save )
;
void CErrCheckpoint(            // save current state of error reporting
    error_state_t *save )       // - location to save data
;
bool CErrOccurred(              // has an error occurred since state was saved?
    error_state_t *saved_state )// - saved state
;
void CFatal(                    // issue error message and CSuicide()
    const char *msg )           // - message
;
void CSuicide(                  // COMMIT SUICIDE
    void )
;
void ErrFileOpen(               // OPEN ERROR FILE
    void )
;
void ErrFileErase(              // ERASE ERROR FILE
    void )
;
unsigned ErrPCHVersion(         // PROVIDE A VERSION NUMBER FOR THE ERROR MESSAGES
    void )
;
void InfClassDecl(              // GENERATE CLASS-DECLARATION NOTE
    TYPE cltype )               // - a class type
;
void InfMacroDecl(              // GENERATE MACRO-DECLARATION NOTE
    void* mdef )                // - macro definition
;
void InfMsgPtr(                 // INFORMATION MESSAGE, PTR ARG.
    MSG_NUM msgnum,             // - message number
    void const *p1 )            // - extra information
;
void InfMsgInt(                 // INFORMATION MESSAGE, INT ARG.
    MSG_NUM msgnum,             // - message number
    int p1 )                    // - extra information
;
void InfSymbolAmbiguous(        // GENERATE SYMBOL-AMBIGUITY NOTE
    SYMBOL sym )                // - a symbol
;
void InfSymbolDeclaration(      // GENERATE SYMBOL-DECLARATION NOTE
    SYMBOL sym )                // - a symbol
;
void InfSymbolRejected(         // GENERATE SYMBOL-REJECTION NOTE
    SYMBOL sym )                // - a symbol
;
void MsgDisplay                 // DISPLAY A MESSAGE
    ( IDEMsgSeverity severity   // - message severity
    , MSG_NUM msgnum            // - message number
    , va_list args )            // - substitution arguments
;
void MsgDisplayArgs             // DISPLAY A MESSAGE WITH ARGS
    ( IDEMsgSeverity severity   // - message severity
    , MSG_NUM msgnum            // - message number
    , ... )                     // - arguments
;
void MsgDisplayBanner           // DISPLAY A BANNER LINE
    ( const char *line )        // - the line
;
void MsgDisplayLine             // DISPLAY A BARE LINE
    ( const char* line )        // - the line
;
void MsgDisplayLineArgs         // DISPLAY A BARE LINE, FROM ARGUMENTS
    ( char* seg                 // - the line segments
    , ... )
;
void RegisterSuicideCallback(   // REGISTER A FUNCTION TO BE CALLED BEFORE CSuicide
    SUICIDE_CALLBACK *ctl )     // - call-back control block
;
void SetErrLoc(                 // SET ERROR LOCATION
    TOKEN_LOCN *locn )          // - error location
;
void WarnChangeLevel(           // CHANGE WARNING LEVEL FOR A MESSAGE
    int level,                  // - new level
    MSG_NUM msgnum )            // - message number
;
void WarnChangeLevels(          // CHANGE WARNING LEVELS FOR ALL MESSAGES
    int level )                 // - new level
;
void DefAddPrototype(           // ADD PROTOTYPE FOR SYMBOL TO .DEF FILE
    SYMBOL fn )                 // - function
;
char const *IntlUsageText(      // GET INTERNATIONAL USAGE TEXT
    void )
;

#endif
