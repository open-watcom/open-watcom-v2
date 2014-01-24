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
* Description:  Internal routines and data types used by setup program.
*
****************************************************************************/


#include "vhandle.h"

#include "guitypes.h"
#if defined( __WINDOWS__ )
    #include "setupdlg.h"
    #include <windows.h>
#elif defined( __NT__ )
    #include "setupdlg.h"
    #include <windows.h>
#elif defined( __OS2__ )
    #include "setupdlg.h"
    #define  INCL_DOSMISC
    // For WPI
    #define INCL_DOSDEVICES
    #define INCL_DOSFILEMGR
    #define INCL_DOSMISC
    #define INCL_WINWINDOWMGR
    #define INCL_WINDIALOGS
    #define INCL_WINSYS
    #define INCL_WINBUTTONS
    #define INCL_WINWORKPLACE
    #define INCL_WINMENUS
    #define INCL_WINERRORS
    #define INCL_GPIBITMAPS
    #define INCL_GPIPRIMITIVES
    #define INCL_GPIBITMAPS
    #define INCL_WINTRACKRECT
    #define INCL_DOSPROCESS
    #define INCL_ERRORS
    #include <os2.h>
#else
    #include "setupui.h"
#endif

#ifdef __OS2__
    #define     _ID(x) x
#else
    #define     _ID(X) MAKEINTRESOURCE(X)
#endif

// Defines used by the SETUP program.

typedef enum {
    CFE_NOERROR,
    CFE_NOMEMORY,
    CFE_CANTOPENSRC,
    CFE_CANTOPENDST,
    CFE_ERROR,
    CFE_ABORT,
    CFE_BAD_CRC,
    CFE_DSTREADONLY
} COPYFILE_ERROR;

enum {
    MOD_IN_PLACE,
    MOD_LATER
};

#if defined( __NT__ )
enum {
    CURRENT_USER,
    LOCAL_MACHINE,
    NUM_REG_LOCATIONS
};
#endif

extern bool             ConfigModified;
extern gui_window       *MainWnd;

#define WIDTH  1000
#define HEIGHT 1100

extern bool             SetupPreInit( void );
extern bool             SetupInit( void );
extern void             SetupFini( void );
extern void             SetupError( char * );
extern gui_message_return MsgBox( gui_window *, char *, gui_message_type, ... );
extern void             ReplaceVars( char *, const char * );
extern bool             StatusInit( void );
extern void             StatusFini( void );
extern void             StatusShow( bool );
extern void             StatusLines( int, char * );
extern void             StatusAmount( long, long );
extern bool             StatusCancelled( void );
extern gui_message_return CheckInstallDLL( char*, vhandle );
extern gui_message_return CheckInstallNLM( char*, vhandle );
extern bool             IsNLMNewerThanExistingNLM(char *name);
extern bool             CreatePMInfo( bool );
extern bool             ModifyConfiguration( bool );
extern bool             ModifyAutoExec( bool );
extern bool             GenerateBatchFile( bool );
#if defined( __NT__ )
extern bool             GetRegString( HKEY, char *, char *, char *, DWORD );
extern bool             ModifyRegAssoc( bool );
extern bool             AddToUninstallList( bool );
#endif
#if defined( __NT__ ) && !defined( _M_X64 )
extern bool             IsWOW64( void );
#endif
extern long             GetODBCUsage( void );
extern void             SetODBCUsage( long );
extern void             WriteProfileStrings( bool );
extern void             SetDialogFont( void );
extern void             LabelDirs( void );
extern bool             CreateServices( void );
extern bool             UpdateODBC( void );
extern bool             RegUpdateODBC( void );
extern bool             ApplyLicense( void );
extern bool             DoDeleteFile( char * );
extern bool             PatchFiles( void );

enum {
     WND_PLAIN  = GUI_FIRST_UNUSED,
     WND_TABSTOP ,
     WND_SELECTED,
     WND_HOTSPOT,
     WND_CENSORED,
     WND_STATUS_BAR,
     WND_STATUS_TEXT,
     WND_STATUS_FRAME,
     WND_NUMBER_OF_COLORS
};

#undef pick
#define pick( x, y ) x,
enum {
    #include "status.h"
};
#undef pick

#define MAX_DRIVES 10
