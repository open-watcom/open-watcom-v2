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


#ifndef __CMDLNPRS_H__
#define __CMDLNPRS_H__

#ifdef __cplusplus
extern "C" {
#endif

// Standard Command-Line Parsing Routines

void CheckErrorLimit            // VALIDATE THE ERROR LIMIT
    ( unsigned *p )             // - value
;
void CheckWarnLevel             // VALIDATE WARNING LEVEL
    ( unsigned *p )             // - value
;
void OPT_INIT
	( OPT_STORAGE *data )
;
void OPT_FINI
	( OPT_STORAGE *data )
;
int OPT_PROCESS
	( OPT_STORAGE *data )
;
void OPT_CLEAN_NUMBER           // CLEAN UP NUMBERS
    ( OPT_NUMBER **h )          // - list
;
void OPT_CLEAN_STRING           // CLEAN UP STRINGS
    ( OPT_STRING **h )          // - list
;
int OPT_END( void )             // DETECT END OF CHAIN
;
int OPT_GET_ID                  // GET AN ID
    ( OPT_STRING **p )          // - target
;
int OPT_GET_ID_OPT              // GET A OPTIONAL ID
    ( OPT_STRING **p )          // - target
;
int OPT_GET_LOWER               // GET CHAR IN LOWERCASE
    ( void )
;
int OPT_GET_NUMBER              // PARSE: #
    ( unsigned *p )             // - target
;
int OPT_GET_NUMBER_DEFAULT(     // PARSE: OPTIONAL # WITH A DEFAULT VALUE
    unsigned *p,                // - target
    unsigned default_value )    // - default value
;
int OPT_GET_NUMBER_MULTIPLE     // PARSE: OPTION #
    ( OPT_NUMBER **h )          // - target
;
int OPT_GET_PATH                // PARSE: PATH
    ( OPT_STRING **p )          // - target
;
int OPT_GET_PATH_OPT            // PARSE: OPTIONAL PATH
    ( OPT_STRING **p )          // - target
;
int OPT_GET_FILE                // PARSE: FILE
    ( OPT_STRING **p )          // - target
;
int OPT_GET_FILE_OPT            // PARSE: OPTIONAL FILE
    ( OPT_STRING **p )          // - target
;
int OPT_GET_DIR                 // PARSE: DIR
    ( OPT_STRING **p )          // - target
;
int OPT_GET_DIR_OPT             // PARSE: OPTIONAL DIR
    ( OPT_STRING **p )          // - target
;
int OPT_GET_CHAR                // PARSE: CHAR
    ( int *c )                  // - target
;
int OPT_GET_CHAR_OPT            // PARSE: OPTIONAL CHAR
    ( int *c )                  // - target
;
int OPT_RECOG                   // RECOGNIZE CHAR
    ( int c )                   // - to be recog'ed
;
int OPT_RECOG_LOWER             // RECOGNIZE LOWERCASE CHAR
    ( int c )                   // - to be recog'ed
;
void OPT_UNGET                  // UNGET A CHARACTER
    ( void )
;
void StripQuotes                // STRIP QUOTES FROM A STRING
    ( char *fname )             // - the string
;

// The following are required to be supplied by the front end

void BadCmdLineId               // BAD ID DETECTED
    ( void )
;
void BadCmdLineNumber           // BAD NUMBER DETECTED
    ( void )
;
void BadCmdLinePath             // BAD PATH DETECTED
    ( void )
;
void BadCmdLineFile             // BAD FILE DETECTED
    ( void )
;

#ifdef __cplusplus
};
#endif

#endif // __CMDLNPRS_H__
