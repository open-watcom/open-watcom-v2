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


#define NIL_HANDLE      ((f_handle)-1)
#if _OS == _NT
#include <stdio.h>
    #define STDIN_HANDLE        (stdin->_handle)
    #define STDOUT_HANDLE       (stdout->_handle)
    #define STDERR_HANDLE       (stderr->_handle)
#else
    #define STDIN_HANDLE    ((f_handle)0)
    #define STDOUT_HANDLE   ((f_handle)1)
    #define STDERR_HANDLE   ((f_handle)2)
    #define STDAUX_HANDLE   ((f_handle)3)
    #define STDPRN_HANDLE   ((f_handle)4)
#endif

#define LSEEK_START    0       /* Seek relative to the start of file   */
#define LSEEK_CURRENT  1       /* Seek relative to current position    */
#define LSEEK_END      2       /* Seek relative to the end of the file */

#define MAX_OPEN_FILES 12       // the maximum number of open files.
#define MAX_FILE        2 + 63 + 8 + 1 + 3 + 1

#define EXTRA_NAME_DIR  2
#define EXTRA_OBJ_FILE  3
#define EXTRA_LIB_FILE  4
#define EXTRA_RES_FILE  5

extern char     NLSeq[];

/* i/o function prototypes */

extern void             LnkFilesInit( void );
extern void             PrintIOError( unsigned, char *, char * );
extern f_handle         QOpenR( char * );
extern f_handle         QOpenRW( char * );
extern f_handle         ExeCreate( char * );
extern f_handle         ExeOpen( char * );
extern int              ResOpen( const char *name, int access, ... );
extern unsigned         QRead( f_handle, void *, unsigned, char * );
extern unsigned         QWrite( f_handle, void *, unsigned, char * );
extern void             WriteStdOut( char * );
extern void             WriteNLStdOut( void );
extern void             WriteInfoStdOut( char *, unsigned, char * );
extern void             QWriteNL( f_handle, char * );
extern void             QClose( f_handle, char * );
extern long             QLSeek( f_handle, long, int, char * );
extern void             QSeek( f_handle, long, char * );
extern unsigned long    QPos( f_handle );
extern unsigned long    QFileSize( f_handle );
extern void             QDelete( char * );
extern bool             QReadStr( f_handle, char *, unsigned, char * );
extern bool             QIsDevice( f_handle );
extern f_handle         QObjOpen( char * );
extern f_handle         TempFileOpen( char * );
extern int              QMakeFileName( char **, char *, char * );
extern bool             QHavePath( char * );
extern bool             QSysHelp( char ** );
extern bool             QModTime( char *, time_t * );
extern time_t           QFModTime( int );
extern char             WaitForKey( void );
extern void             GetCmdLine( char * );
extern char *           GetEnvString( char * );
extern bool             GetAddtlCommand( unsigned, char * );
extern bool             IsStdOutConsole( void );
extern void             TrapBreak( int );
extern void             SetBreak( void );
extern void             RestoreBreak( void );
