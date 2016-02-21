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


typedef struct event_record {
    struct event_record *next;
    address             ip;
    cmd_list            *cmd;
    mad_radix           radix;
    bool                after_asynch;
    char                *cue;
    char                *addr_string;
} event_record;


extern void     RecordInit( void );
extern void     RecordFini( void );
extern void     ShowReplay( void );
extern bool     OkToSaveReplay( void );
extern void     SaveReplayToFile( const char *name );
extern void     RestoreReplayFromFile( const char *name );
extern void     ProcRecord( void );
extern void     RecordStart( void );
extern void     ReplayTo( event_record *ev );
extern void     CheckEventRecorded( void );
extern void     RecordEvent( const char *p );
extern void     RecordAsynchEvent( void );
extern void     RecordGo( const char *p );
extern void     RecordCommand( const char *startpos, wd_cmd cmd );
extern void     SetRecord( bool on );
extern char     *GetEventAddress( event_record *ev );
