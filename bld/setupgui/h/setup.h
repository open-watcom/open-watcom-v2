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
* Description:  Internal routines and data types used by setup program.
*
****************************************************************************/


#include <stddef.h>
#if defined( __DOS__ )
    #include <dos.h>
#elif defined( __WINDOWS__ )
    #include <windows.h>
    #include <dos.h>
#elif defined( __NT__ )
    #include <wwindows.h>
#elif defined( __OS2__ )
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
    #define INCL_WINTRACKRECT
    #define INCL_DOSPROCESS
    #define INCL_ERRORS
    #include <os2.h>
#endif
#include "bool.h"
#include "vhandle.h"
#include "gui.h"
#include "strvbuf.h"


#ifdef __OS2__
    #define     _ID(x) x
#else
    #define     _ID(X) MAKEINTRESOURCE(X)
#endif

#define UCHAR_VALUE( c )    (unsigned char)(c)

// Defines used by the SETUP program.

#if defined( __UNIX__ )
    #define BATCHEXT        "sh"
#elif defined( __OS2__ )
    #define BATCHEXT        "cmd"
#else
    #define BATCHEXT        "bat"
#endif

#if defined( __OS2__ )
    #define BATCH_EXT_SAVED "OS2"
#elif defined( __NT__ )
    #define BATCH_EXT_SAVED "W95"
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    #define BATCH_EXT_SAVED "DOS"
#else
    // not used
#endif

#define IS_VALID_DIR(p)     ((p)->d_name[0] != '.' || (p)->d_name[1] != '\0' && ((p)->d_name[1] != '.' || (p)->d_name[2] != '\0'))

#define SKIP_SPACES(s)      while(isspace(*s)) s++
#define SKIP_CHAR_SPACES(s) while(isspace(*++s))

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

#if defined( __NT__ )
enum {
    CURRENT_USER,
    LOCAL_MACHINE,
    NUM_REG_LOCATIONS
};
#endif

#define WIDTH  1000
#define HEIGHT 1100

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

enum {
    #define pick( x, y ) x,
    #include "status.h"
    #undef pick
};

extern bool             ConfigModified;
extern gui_window       *MainWnd;
extern bool             CancelSetup;
extern bool             SkipDialogs;
extern bool             Invisible;
extern gui_coord        GUIScale;
extern char             *VariablesFile;
extern bool             VisibilityCondition;
extern gui_ord          BitMapBottom;
extern char             InstallerFile[];

extern void             ReplaceVars( VBUF *dst, const char *src );
#define ReplaceVars1(d) ReplaceVars(d, NULL)
extern bool             CheckInstallDLL( const VBUF *, vhandle );
extern bool             CheckInstallNLM( const VBUF *, vhandle );
extern bool             CreatePMInfo( bool );
extern bool             ModifyConfiguration( bool );
extern bool             ModifyAutoExec( bool );
extern bool             GenerateBatchFile( bool );
#if defined( __NT__ )
extern bool             GetRegString( HKEY, const char *, const char *, VBUF * );
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
extern bool             DoDeleteFile( const VBUF * );
