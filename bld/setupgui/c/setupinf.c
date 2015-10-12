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
* Description:  Read and parse setup.inf file, create dialogs if necessary.
*               Also initialize other variables.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef __UNIX__
    #include <utime.h>
#else
    #include <sys/utime.h>
    #include <direct.h>
#endif
#if defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "gui.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "guiutil.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "genctrl.h"
#include "dlggen.h"
#include "utils.h"
#include "setupio.h"
#include "iopath.h"
#ifdef PATCH
    #include "bdiff.h"
#endif

#include "clibext.h"


#define IS_EMPTY(p)     ((p)[0] == '\0' || (p)[0] == '.' && (p)[1] == '\0')

extern char             *TrimQuote(char*);
extern bool             SkipDialogs;
extern int              VisibilityCondition;
extern char             *VariablesFile;

#define RoundUp( v, r ) (((v) + (r) - 1) & ~(unsigned long)((r)-1))

#define BUF_SIZE        8192

#include "dynarray.h"

static struct setup_info {
    long                stamp;
    char                *pm_group_file_name;
    char                *pm_group_name;
    char                *pm_group_icon;
    array_info          disks;
    array_info          dirs;
    array_info          files;
    array_info          pm_files;
    array_info          profile;
    array_info          autoexec;
    array_info          config;
    array_info          environment;
    array_info          target;
    array_info          label;
    array_info          upgrade;
#ifdef PATCH
    array_info          patch_files;
#endif
    array_info          spawn;
    array_info          delete;
    array_info          fileconds;
    array_info          dlls_to_count;
    array_info          force_DLL_install;
    array_info          all_pm_groups;
    array_info          associations;
} SetupInfo;

#ifdef PATCH
typedef enum {
    PATCH_NOTHING,
    PATCH_COPY_FILE,
    PATCH_DELETE_FILE,
    PATCH_FILE,
    PATCH_MAKE_DIR
} PATCHCOMMANDTYPE;

typedef enum {
    REG_EXE             = 0
} FILETYPE;

struct patch_info {
    PATCHCOMMANDTYPE    command;
    char                *destdir;
    char                *destfile;
    char                *srcfile;
    char                *exetype;
    char                *condition;
} *PatchInfo = NULL;

extern bool             ExeType( const char *name, char *type );

#endif

static struct disk_info {
    char                *desc;
} *DiskInfo = NULL;


static struct dir_info {
    char                *desc;
    int                 target;
    int                 parent;
    int                 num_files;
    int                 num_existing;
    unsigned            used            : 1;
} *DirInfo = NULL;


static struct target_info {
    char                *name;
    uint_64             space_needed;
    uint_64             max_tmp_file;
    int                 num_files;
    int                 supplimental;
    bool                needs_update;
    char                *temp_disk;
} *TargetInfo = NULL;

static struct label_info {
    char                *dir;
    char                *label;
} *LabelInfo = NULL;

static struct upgrade_info {
    char                *name;
} *UpgradeInfo = NULL;

static struct association_info {
    char    *ext;
    char    *keyname;
    char    *program;
    char    *description;
    int     icon_index;
    int     no_open;
    char    *condition;
} *AssociationInfo = NULL;

typedef struct a_file_info {
    char                *name;
    vhandle             dst_var;
    unsigned long       disk_size;
    unsigned long       disk_date;
    unsigned long       size;
    unsigned long       date;
    unsigned            in_old_dir  : 1;
    unsigned            in_new_dir  : 1;
    unsigned            read_only   : 1;
    unsigned            is_nlm      : 1;
    unsigned            is_dll      : 1;
    unsigned            executable  : 1;
} a_file_info;

typedef enum {
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_EXIST,
    OP_VAR,
    OP_FALSE,
    OP_TRUE
} tree_op;

typedef struct tree_node {
    union {
        struct tree_node    *left;
        vhandle             v;
    } u;
    struct tree_node        *right;
    tree_op                 op;
} tree_node;

typedef struct file_cond_info {
    tree_node   *cond;
    bool        one_uptodate;
    bool        dont_touch;
} file_cond_info;

file_cond_info *FileCondInfo = NULL;

static struct file_info {
    char                *filename;
    int                 dir_index, old_dir_index, disk_index;
    unsigned            num_files;
    a_file_info         *files;
    union {
        file_cond_info  *p;
        int             i;
    } condition;
    char                file_type;
    unsigned            add             : 1;
    unsigned            remove          : 1;
    unsigned            supplimental    : 1;
    unsigned            core_component  : 1;
} *FileInfo = NULL;

static struct pm_info {
    char                *filename;
    char                *parameters;
    char                *desc;
    char                *icoioname;
    int                 icon_pos;
    char                *condition;
} *PMInfo = NULL;

static struct profile_info {
    char                *hive_name;
    char                *app_name;
    char                *key_name;
    char                *value;
    char                *file_name;
    char                *condition;
} *ProfileInfo = NULL;

static struct spawn_info {
    char                *condition;
    char                *command;
    when_time           when;
} *SpawnInfo = NULL;

typedef enum {
    DELETE_DIALOG,
    DELETE_FILE,
    DELETE_DIR,
} delete_type;

static struct delete_info {
    char                *name;
    delete_type         type;
} *DeleteInfo = NULL;

static struct config_info {
    char                *var;
    char                *value;
    char                *condition;
} *ConfigInfo = NULL, *EnvironmentInfo = NULL, *AutoExecInfo = NULL;

static struct dlls_to_check {
    int                 index;
    char                *full_path;
} *DLLsToCheck = NULL;

static struct force_DLL_install {
    char        *name;
} *ForceDLLInstall = NULL;

static struct all_pm_groups {
    char        *group;
    char        *group_file_name;
} *AllPMGroups = NULL;

typedef enum {
    RS_UNDEFINED,
    RS_APPLICATION,
    RS_DISKS,
    RS_DIRS,
    RS_FILES,
    RS_PMINFO,
    RS_PROFILE,
    RS_AUTOEXEC,
    RS_CONFIG,
    RS_ENVIRONMENT,
    RS_TERMINATE,
    RS_DIALOG,
    RS_TARGET,
    RS_LABEL,
    RS_UPGRADE,
    RS_ERRORMESSAGE,
    RS_SETUPERRORMESSAGE,
    RS_STATUSLINEMESSAGE,
    RS_MISCMESSAGE,
    RS_LICENSEMESSAGE,
    RS_PATCH,
    RS_AUTOSET,
    RS_SPAWN,
    RS_RESTRICTIONS,
    RS_DELETEFILES,
    RS_FORCEDLLINSTALL,
    RS_ASSOCIATIONS
} read_state;

static read_state       State;
static size_t           NoLineCount;
static size_t           *LineCountPointer = &NoLineCount;
static bool             NeedGetDiskSizes = false;
static bool             NeedInitAutoSetValues = true;
static char             *ReadBuf;
static size_t           ReadBufSize;
static char             *RawReadBuf;
static char             *RawBufPos;
extern gui_coord        GUIScale;
static int              MaxWidthChars;
static int              CharWidth;

#define MAX_WINDOW_WIDTH 90

#define MAGICVARS( x, y ) \
    x( FullInstall, y ) \
    x( FullCDInstall, y ) \
    x( UnInstall, y ) \
    x( MinimalInstall, y ) \
    x( SelectiveInstall, y ) \
    x( PreviousInstall, y )

#define NONMAGICVARS( x, y ) \
    x( IsDos, y ) \
    x( IsOS2, y ) \
    x( IsOS2DosBox, y ) \
    x( IsWin, y ) \
    x( IsWin32, y ) \
    x( IsWin64, y ) \
    x( IsWin32s, y ) \
    x( IsWin95, y ) \
    x( IsWin98, y ) \
    x( IsWinNT, y ) \
    x( IsWinNT40, y ) \
    x( IsWin2000, y ) \
    x( IsLinux32, y ) \
    x( IsLinux64, y ) \
    x( IsAlpha, y ) \
    x( HelpFiles, y ) \

#define defvar( x, y ) vhandle x;
MAGICVARS( defvar, 0 )
NONMAGICVARS( defvar, 0 )

#define orvar( x, y ) x == y ||
#define IsMagicVar( v ) MAGICVARS( orvar, v ) false

typedef struct dialog_info {    // structure used when parsing a dialog
    array_info          controls;
    array_info          controls_ext;
    int                 num_push_buttons;
    int                 num_variables;
    int                 num_radio_buttons;
    int                 max_width;
    int                 wrap_width;
    a_dialog_header     *curr_dialog;
    int                 row_num;
    int                 col_num;
} DIALOG_INFO;

static vhandle GetTokenHandle( const char *p );
static void ZeroAutoSetValues( void );
static void InitAutoSetValues( void );

static void InitDlgArrays( DIALOG_INFO *dlg )
{
    InitArray( (void **)&dlg->curr_dialog->controls, sizeof( gui_control_info ), &dlg->controls );
    InitArray( (void **)&dlg->curr_dialog->controls_ext, sizeof( control_info_ext ), &dlg->controls_ext );
}

static bool BumpDlgArrays( DIALOG_INFO *dlg )
{
    return( BumpArray( &dlg->controls ) && BumpArray( &dlg->controls_ext ) );
}

/**********************************************************************/
/*                   EXPRESSION EVALUTORS                             */
/**********************************************************************/
static tree_node *TreeNode( tree_op op, void *left, void *right )
/***************************************************************/
{
    tree_node   *tree;

    tree = GUIMemAlloc( sizeof( tree_node ) );
    tree->op = op;
    tree->u.left = left;
    tree->right = right;
    return( tree );
}

static tree_node *BuildExprTree( const char *str )
/************************************************/
{
    char                *token;
    int                 stack_top;
    tree_node           *stack[16];
    char                *str2;
    char                *p;
    tree_node           *tree;

    if( str == NULL || IS_EMPTY( str ) ) {
        return( TreeNode( OP_TRUE, NULL, NULL ) );
    }
    stack_top = -1;
    str2 = GUIStrDup( str, NULL );  // copy string so we can use STRTOK
    token = strtok( str2, " " );
    while( token != NULL ) {
        if( token[0] == '|' ) { // or together top 2 values
            --stack_top;
            stack[stack_top] = TreeNode( OP_OR, stack[stack_top], stack[stack_top + 1] );
        } else if( token[0] == '&' ) { // and together top 2 values
            --stack_top;
            stack[stack_top] = TreeNode( OP_AND, stack[stack_top], stack[stack_top + 1] );
        } else if( token[0] == '!' ) { // not top value
            stack[stack_top] = TreeNode( OP_NOT, stack[stack_top], NULL );
        } else if( token[0] == '?' ) {  // check for file existence
            p = GUIStrDup( token + 1, NULL );
            ++stack_top;
            stack[stack_top] = TreeNode( OP_EXIST, p, NULL );
        } else {                // push current value
            ++stack_top;
            stack[stack_top] = TreeNode( OP_VAR, NULL, NULL );
            stack[stack_top]->u.v = GetTokenHandle( token );
        }
        token = strtok( NULL, " " );
    }
    // and together whatever is left on stack
    tree = stack[stack_top];
    while( --stack_top >= 0 ) {
        tree = TreeNode( OP_AND, tree, stack[stack_top] );
    }
    GUIMemFree( str2 );
    return( tree );
}

static bool SameExprTree( tree_node *a, tree_node *b )
/****************************************************/
{
    if( a == NULL )
        return( b == NULL );
    if( b == NULL )
        return( a == NULL );
    if( a->op != b->op )
        return( false );
    switch( a->op ) {
    case OP_AND:
    case OP_OR:
        return( SameExprTree( a->u.left, b->u.left ) &&
                SameExprTree( a->right, b->right ) );
    case OP_NOT:
        return( SameExprTree( a->u.left, b->u.left ) );
    case OP_EXIST:
        return( stricmp( (char *)a->u.left, (char *)b->u.left ) == 0 );
    case OP_VAR:
        return( (vhandle)(pointer_int)a->u.left == (vhandle)(pointer_int)b->u.left );
        break;
    case OP_TRUE:
    case OP_FALSE:
        return( true );
    default:
        return( false );
    }
}

static void BurnTree( tree_node *tree )
/*************************************/
{
    switch( tree->op ) {
    case OP_AND:
    case OP_OR:
        BurnTree( tree->right );
        /* fall thru */
    case OP_NOT:
        BurnTree( tree->u.left );
        break;
    case OP_EXIST:
        GUIMemFree( tree->u.left );
        break;
    case OP_VAR:
    case OP_TRUE:
    case OP_FALSE:
        break;
    }
    GUIMemFree( tree );
}

static int NewFileCond( char *str )
/*********************************/
{
    tree_node   *new;
    int         num;

    new = BuildExprTree( str );
    num = SetupInfo.fileconds.num;
    while( --num >= 0 ) {
        if( SameExprTree( new, FileCondInfo[num].cond ) ) {
            BurnTree( new );
            return( num );
        }
    }
    num = SetupInfo.fileconds.num;
    if( !BumpArray( &SetupInfo.fileconds ) ) return( 0 );
    FileCondInfo[num].cond = new;
    FileCondInfo[num].one_uptodate = false;
    FileCondInfo[num].dont_touch = false;
    return( num );
}

static vhandle GetTokenHandle( const char *p )
/********************************************/
{
    if( p == NULL ) {
        return( NO_VAR );
    } else if( p[0] ==  '#' ) {
        return( atoi( p + 1 ) );
    } else {
        return( AddVariable( p ) );
    }
}

int GetOptionVarValue( vhandle var_handle, bool is_minimal )
/**********************************************************/
{
//    if( GetVariableIntVal( "_Visibility_Condition_" ) ) {
    if( VisibilityCondition ) {
        return( VarGetIntVal( var_handle ) );
    } else if( IsMagicVar( var_handle ) ) {
        // these are special - we always want their "true" values
        return( VarGetIntVal( var_handle ) != 0 );
    } else if( VarGetIntVal( UnInstall ) ) {
        // uninstall makes everything false
        return( 0 );
    } else if( VarGetIntVal( FullInstall ) &&
               VarGetAutoSetCond( var_handle ) != NULL ) {
        // fullinstall pretends all options are turned on
        return( 1 );
    } else if( VarGetIntVal( FullCDInstall ) &&
               VarGetAutoSetCond( var_handle ) != NULL ) {
        // fullinstallcd pretends all options are turned on except 'HelpFiles'
        return( var_handle != HelpFiles );
    } else if( is_minimal ) {
        // is_minimal makes all file condition variables false
        return( false );
    } else {
        return( VarGetIntVal( var_handle ) != 0 );
    }
}


static int EvalExprTree( tree_node *tree, bool is_minimal )
/*********************************************************/
{
    int         value = 0;
    char        buff[_MAX_PATH];

    switch( tree->op ) {
    case OP_AND:
        value = EvalExprTree( tree->u.left, is_minimal ) &
                EvalExprTree( tree->right, is_minimal );
        break;
    case OP_OR:
        value = EvalExprTree( tree->u.left, is_minimal ) |
                EvalExprTree( tree->right, is_minimal );
        break;
    case OP_NOT:
        value = !EvalExprTree( tree->u.left, is_minimal );
        break;
    case OP_EXIST:
        ReplaceVars( buff, sizeof( buff ), (char *)tree->u.left );
        value = access( buff, F_OK ) == 0;
        break;
    case OP_VAR:
        value = GetOptionVarValue( (vhandle)(pointer_int)tree->u.left, is_minimal );
        break;
    case OP_TRUE:
        value = !is_minimal;
        break;
    case OP_FALSE:
        break;
    }
    return( value );
}

static int DoEvalCondition( const char *str, bool is_minimal )
/************************************************************/
{
    int         value;
    tree_node   *tree;

    tree = BuildExprTree( str );
    value = EvalExprTree( tree, is_minimal );
    BurnTree( tree );
    return( value );
}

int EvalCondition( const char *str )
/**********************************/
{
    if( str == NULL || *str == '\0' )
        return( true );
    return( DoEvalCondition( str, false ) );
}

static void PropagateValue( tree_node *tree, bool value )
/*******************************************************/
{
    vhandle     var_handle;

    switch( tree->op ) {
    case OP_AND:
        if( value ) {
            PropagateValue( tree->u.left, true );
            PropagateValue( tree->right, true );
        }
        break;
    case OP_OR:
        if( !value ) {
            PropagateValue( tree->u.left, false );
            PropagateValue( tree->right, false );
        }
        break;
    case OP_NOT:
        PropagateValue( tree->u.left, !value );
        break;
    case OP_VAR:
        if( value ) {
            var_handle = (vhandle)(pointer_int)tree->u.left;
            if( VarGetAutoSetCond( var_handle ) != NULL ) {
                if( !VarIsRestrictedFalse( var_handle ) ) {
                    SetVariableByHandle( PreviousInstall, "1" );
                    SetVariableByHandle( var_handle, "1" );
                }
            }
        }
        break;
    default:
        break;
    }
}

#ifdef PATCH
static void GetDestDir( int i, char *buff, size_t buff_len )
/**********************************************************/
{
    char                *temp;
    char                temp2[_MAX_PATH];
    char                drive[_MAX_DRIVE];
    int                 intvalue = 0;

    ReplaceVars( buff, buff_len, GetVariableStrVal( "DstDir" ) );
    ConcatDirSep( buff );
    intvalue = atoi( PatchInfo[i].destdir );
    if( intvalue != 0 ) {
        temp = strchr( DirInfo[intvalue - 1].desc, '=' ) + 1;
    } else {
        // if destination dir specifies the drive, just use it
        ReplaceVars( temp2, sizeof( temp2 ), PatchInfo[i].destdir );
        _splitpath( temp2, drive, NULL, NULL, NULL );
        if( drive[0] != 0 ) {  // drive specified
            strcpy( buff, temp2 );
            ConcatDirSep( buff );
            return;
        } else {
            temp = temp2;
        }
    }
    strcat( buff, temp );
    ConcatDirSep( buff );
}

int SecondaryPatchSearch( const char *filename, char *buff, int Index )
/*********************************************************************/
{
// search for patch output files (originals to be patched) in following order
// 1.)  check .INF specified directory in PatchInfo structure (if it exists)
// 2.)  check DstDir "variable"
// 2b)  if not found in %DstDir%\destdir just try %DstDir%
// 3.)  check system path

// this function performs the first two checks, findold() in OLDFILE.C does
// system path search, if first two searches fail and this function returns
// nonzero.

    char                path[_MAX_PATH];
    char                ext[_MAX_EXT];

    buff[0] = '\0';
    GetDestDir( Index, path, sizeof( path ) );
    strcat( path, filename );
    if( access( path, F_OK ) == 0 ) {
        strcpy( buff, path );
        return( true );
    } else {
        ReplaceVars( path, sizeof( path ), GetVariableStrVal( "DstDir" ) );
        ConcatDirSep( path );
        strcat( path, filename );
        if( access( path, F_OK ) == 0 ) {
            strcpy( buff, path );
            return( true );
        }
    }
    _splitpath( filename, NULL, NULL, NULL, ext );
    if( stricmp( ext, ".dll" ) == 0 ) {
        _searchenv( filename, "PATH", buff );
    }
    return( buff[0] != '\0' );
}

void PatchingFile( const char *patchname, const char *path )
/**********************************************************/
{
    char        buff[200];

    strcpy( buff, patchname );
    strcat( buff, " to file " );
    strcat( buff, path );
    StatusLines( STAT_PATCHFILE, buff );
    StatusShow( true );
}
#endif

static char *NextToken( char *buf, char delim )
/*********************************************/
// Locate the next 'token', delimited by the given character. Return a
// pointer to the next one, and trim trailing blanks off the current one.
{
    char                *p, *q;

    if( buf == NULL ) {
        return( NULL );
    }
    p = strchr( buf, delim );
    if( p != NULL ) {
        *p = '\0';
        q = p - 1;
        while( q >= buf && (*q == ' ' || *q == '\t') ) {
            *q = '\0';
            --q;
        }
        ++p;
        while( *p == ' ' || *p == '\t' ) ++p;
    }
    return( p );
}


static char *StripEndBlanks( char *p )
/************************************/
{
    char                *q;

    if( p == NULL ) {
        return p;
    }

    while( *p == ' ' || *p == '\t' )
        ++p;
    q = p + strlen( p ) - 1;
    while( q >= p && (*q == ' ' || *q == '\t' || *q == '\n') ) {
        *q = '\0';
        --q;
    }
    return( p );
}


// Dialog parsing functions

// Characters prohibited from beginning lines

#define NUM_INVALID_FIRST       56

static unsigned short InvalidFirst[NUM_INVALID_FIRST] =
{
    0x8141, 0x8142, 0x8143, 0x8144, 0x8146, 0x8147, 0x8148, 0x8149,
    0x814a, 0x814b, 0x8152, 0x8153, 0x8154, 0x8155, 0x8158, 0x815b,
    0x815c, 0x815d, 0x8166, 0x8168, 0x816a, 0x816c, 0x816e, 0x8170,
    0x8172, 0x8174, 0x8176, 0x8178, 0x817a, 0x818b, 0x818c, 0x818d,
    0x818e, 0x8193, 0x829f, 0x82a1, 0x82a3, 0x82a5, 0x82a7, 0x82c1,
    0x82e1, 0x82e3, 0x82e5, 0x82ec, 0x8340, 0x8342, 0x8344, 0x8346,
    0x8348, 0x8362, 0x8383, 0x8385, 0x8387, 0x838e, 0x8395, 0x8396
};

// Characters prohibited from terminating lines

#define NUM_INVALID_LAST        19

static unsigned short InvalidLast[NUM_INVALID_LAST] =
{
    0x8165, 0x8167, 0x8169, 0x816b, 0x816d, 0x816f, 0x8171, 0x8173,
    0x8175, 0x8177, 0x8179, 0x818f, 0x8190, 0x8191, 0x8192, 0x8197,
    0x8198, 0x81a7, 0x81f2
};

static bool valid_first_char( char *p )
/*************************************/

{
    int                 i;
    unsigned short      kanji_char;

    if( GUICharLen( *p ) == 2 ) {
        // Kanji
        kanji_char = (*p << 8) + *(p + 1);
        if( kanji_char < InvalidFirst[0] ||
            kanji_char > InvalidFirst[NUM_INVALID_FIRST - 1] ) {
            // list is sorted
            return( true );
        }
        for( i = 0; i < NUM_INVALID_FIRST; ++i ) {
            if( kanji_char == InvalidFirst[i] ) {
                return( false );        // invalid
            } else if( kanji_char < InvalidFirst[i] ) {
                return( true );
            }
        }
        return( true );
    } else {
        return( false );
    }
}

static bool valid_last_char( char *p )
/************************************/

{
    int                 i;
    unsigned short      kanji_char;

    if( GUICharLen( *p ) == 2 ) {
        // Kanji
        kanji_char = (*p << 8) + *(p + 1);
        if( kanji_char < InvalidLast[0] ||
            kanji_char > InvalidLast[NUM_INVALID_LAST - 1] ) {
            // list is sorted
            return( true );
        }
        for( i = 0; i < NUM_INVALID_LAST; ++i ) {
            if( kanji_char == InvalidLast[i] ) {
                return( false );        // invalid
            } else if( kanji_char < InvalidLast[i] ) {
                return( true );
            }
        }
        return( true );
    } else {
        return( false );
    }
}

static char *find_break( char *text, DIALOG_INFO *dlg, int *chwidth )
/*******************************************************************/
{
    char            *e;
    char            *n;
    char            *br;
    char            *s;
    int             len;
    gui_ord         width;
    int             winwidth;

    // Line endings are word breaks already
    s = text;
    while( *s && (*s != '\r') && (*s != '\n') ) s++;
    len = s - text;

    winwidth = dlg->wrap_width * CharWidth;
    // Use string length as cutoff to avoid overflow on width
    if( len < 2 * dlg->wrap_width ) {
        width = GUIGetExtentX( MainWnd, text, len );
        if( width < winwidth ) {
            *chwidth = (width / CharWidth) + 1;
            if( dlg->max_width < *chwidth )
                dlg->max_width = *chwidth;
            return( text + len );
        }
    }
    if( dlg->max_width < dlg->wrap_width )
        dlg->max_width = dlg->wrap_width;
    *chwidth = dlg->max_width;
    br = text;
    for( e = text;; ) {
        if( *e == '\0' )
            return( text );
        if( *e == '\\' && *( e + 1 ) == 'n' )
            return( e );
        n = e + GUICharLen( *e );
        width = GUIGetExtentX( MainWnd, text, n - text );
        if( width >= winwidth )
            break;
        // is this a good place to break?
        if( *e == ' ' || *e == '\t' ) { // English
            br = n;
        } else if( valid_last_char( e ) && valid_first_char( n ) ) {
            br = n;
        }
        e = n;
    }
    if( br == text )
        return( e );
    return( br );
}

static bool dialog_static( char *next, DIALOG_INFO *dlg )
/*******************************************************/
{
    char                *line;
    int                 len;
    char                *text;
    bool                rc = true;
    vhandle             var_handle;

    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    text = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        // dummy_var allows control to have an id - used by dynamic visibility feature
        var_handle = MakeDummyVar();
        if( text != NULL ) {
            text = AddInstallName( text, true );
            len = strlen( text );
            set_dlg_dynamstring( dlg->curr_dialog->controls, dlg->controls.num - 1,
                text, VarGetId( var_handle ), dlg->col_num, dlg->row_num, dlg->col_num + len );
            if( dlg->max_width < dlg->col_num + len ) {
                dlg->max_width = dlg->col_num + len;
            }
        } else {
            set_dlg_dynamstring( dlg->curr_dialog->controls, dlg->controls.num - 1,
                "", VarGetId( var_handle ), dlg->col_num, dlg->row_num, dlg->col_num + 0 );
        }
    } else {
        rc = false;
    }
    GUIMemFree( text );
    return( rc );
}

static char *textwindow_wrap( char *text, DIALOG_INFO *dlg, bool convert_newline, bool license_file )
/***************************************************************************************************/
// Makes newline chars into a string into \r\n combination.
// For license file remove single \r\n combination to revoke paragraphs.
// Frees passed in string and allocates new one.
{
    char        *big_buffer = NULL;
    char        *orig_index = NULL;
    char        *new_index = NULL;
    char        *break_candidate = NULL;
    int         chwidth;
    bool        new_line = true;

    if( text == NULL ) {
        return( NULL );
    }

    big_buffer = GUIMemAlloc( strlen( text ) * 2 + 1 );
    if( big_buffer == NULL ) {
        return( NULL );
    }
    if( license_file ) {
        // restore paragraphs by removing single cr/crlf/lf
        new_index = text;
        orig_index = text;
        for( ; *orig_index != '\0';  ) {
            if( *orig_index == '\r' ) {
                if( *(orig_index + 1) == '\n' ) {
                    if( *(orig_index + 2) == '\r' ) {
                        do {
                            *new_index++ = *orig_index++;
                            *new_index++ = *orig_index++;
                        } while( *orig_index == '\r' && *(orig_index + 1) == '\n' );
                    } else {
                        orig_index += 2;
                        *new_index++ = ' ';
                    }
                } else if( *(orig_index + 1) == '\r' ) {
                    do {
                        *new_index++ = *orig_index++;
                    } while( *orig_index == '\r' );
                } else {
                    orig_index++;
                    *new_index++ = ' ';
                }
                continue;
            }
            if( *orig_index == '\n' ) {
                if( *(orig_index + 1) == '\n' ) {
                    do {
                        *new_index++ = *orig_index++;
                    } while( *orig_index == '\n' );
                } else {
                    orig_index++;
                    *new_index++ = ' ';
                }
                continue;
            }
            *new_index++ = *orig_index++;
        }
        *new_index++ = *orig_index++;
    }
    orig_index = text;
    new_index = big_buffer;
    break_candidate = find_break( orig_index, dlg, &chwidth );
    for( ; *orig_index != '\0'; orig_index++ ) {
        if( new_line ) {
            while( *orig_index == '\t' || *orig_index == ' ' ) {
                orig_index++;
            }
        }

        if( convert_newline && *orig_index == '\\' && *(orig_index + 1) == 'n' ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            orig_index++;
            break_candidate = find_break( orig_index + 1, dlg, &chwidth );
        } else if( !convert_newline && *orig_index == '\r' ) {
        } else if( !convert_newline && *orig_index == '\n' ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            break_candidate = find_break( orig_index + 1, dlg, &chwidth );
        } else if( break_candidate != NULL && orig_index == break_candidate ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            *(new_index++) = *break_candidate;
            break_candidate = find_break( orig_index + 1, dlg, &chwidth );
            new_line = true;
            continue;
        } else if( *orig_index == '\t' ) {
            *(new_index++) = ' ';
        } else {
            *(new_index++) = *orig_index;
        }
        new_line = false;
    }
    *new_index = '\0';

    GUIMemFree( text );
    text = GUIStrDup( big_buffer, NULL );
    GUIMemFree( big_buffer );
    return( text );
}

static bool dialog_textwindow( char *next, DIALOG_INFO *dlg, bool license_file )
/******************************************************************************/
{
    char                *line;
    char                *text;
    char                *file_name;
    unsigned int        rows;
    bool                rc = true;
    void                *io;
    struct stat         buf;
    vhandle             var_handle;

    text = NULL;
    line = next; next = NextToken( line, ',' );
    rows = atoi( line );
    if( rows > 0 ) {
        rows -= 1;
    }
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    if( line == NULL ) {
        rc = false;
    } else {
        if( *line == '@' ) {
            file_name = GUIStrDup( line + 1, NULL );
            io = FileOpen( file_name, O_RDONLY + O_BINARY );
            if( io != NULL ) {
                FileStat( file_name, &buf );
                text = GUIMemAlloc( buf.st_size + 1 );  // 1 for terminating null
                if( text != NULL ) {
                    FileRead( io, text, buf.st_size );
                    text[buf.st_size] = '\0';
                    FileClose( io );
                }
            }
            GUIMemFree( file_name );
            //VERY VERY SLOW!!!!  Don't use large files!!!
            // bottleneck is the find_break function
            text = textwindow_wrap( text, dlg, false, license_file );
        } else {
            text = GUIStrDup( line, NULL );
            text = textwindow_wrap( text, dlg, true, false );
        }

        line = next; next = NextToken( line, ',' );
        line = next; next = NextToken( line, ',' );
        if( EvalCondition( line ) && text != NULL ) {
            line = next; next = NextToken( line, ',' );
            // condition for visibility (dynamic)
            dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_textwindow( dlg->curr_dialog->controls, dlg->controls.num - 1,
                text, VarGetId( var_handle ), C0, dlg->row_num, dlg->max_width + 2,
                rows, GUI_VSCROLL );
            dlg->curr_dialog->rows += rows;
            dlg->row_num += rows;
#if defined( __DOS__ )
            dlg->curr_dialog->rows += 2;
            dlg->row_num += 2;
#endif
        } else {
            rc = false;
        }
        GUIMemFree( text );
    }
    return( rc );
}

static bool dialog_dynamic( char *next, DIALOG_INFO *dlg )
/********************************************************/
{
    int                 len;
    char                *line;
    vhandle             var_handle;
    char                *vbl_name;
    char                *text;
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    var_handle = AddVariable( vbl_name );
    text = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        if( text != NULL ) {
            SetVariableByHandle( var_handle, text );
        }
        dlg->curr_dialog->pVariables[dlg->num_variables] = var_handle;
        dlg->curr_dialog->pConditions[dlg->num_variables] = NULL;
        dlg->num_variables++;
        len = strlen( text );
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        if( dlg->max_width < len )
            dlg->max_width = len;
        if( dlg->max_width < 60 )
            dlg->max_width = 60;
        set_dlg_dynamstring( dlg->curr_dialog->controls, dlg->controls.num - 1,
                             text, VarGetId( var_handle ), C0, dlg->row_num,
                             C0 + dlg->max_width );
    } else {
        rc = false;
    }
    GUIMemFree( vbl_name );
    GUIMemFree( text );
    return( rc );
}


static bool dialog_pushbutton( char *next, DIALOG_INFO *dlg )
/***********************************************************/
{
    char                *line;
    char                *line_start;
    gui_ctl_id          id;
    bool                def_ret;
    bool                rc = true;
    vhandle             var_handle;

    line_start = next; next = NextToken( line_start, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        dlg->num_push_buttons += 1;
        def_ret = false;
        if( *line_start == '.' ) {
            ++line_start;
            def_ret = true;
        }
        var_handle = GetVariableByName( line_start );
        id = set_dlg_push_button( var_handle, line_start, dlg->curr_dialog->controls,
                                  dlg->controls.num - 1, dlg->row_num,
                                  dlg->num_push_buttons, W / BW - 1, W, BW );
        if( def_ret ) {
            dlg->curr_dialog->ret_val = IdToDlgState( id );
        }
        if( dlg->max_width < dlg->num_push_buttons * ( ( 3 * BW ) / 2 ) )
            dlg->max_width = dlg->num_push_buttons * ( ( 3 * BW ) / 2 );
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
    } else {
        rc = false;
    }
    return( rc );
}

static bool dialog_edit_button( char *next, DIALOG_INFO *dlg )
/************************************************************/
{
//    int                 len;
    char                *line;
    char                *vbl_name;
    const char          *val;
    char                *section;
    char                *button_text;
    char                *dialog_name;
#if defined( __NT__ )
    char                *value;
#endif
    vhandle             var_handle;
    vhandle             var_handle_2;
    char                buff[MAXBUF];
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    var_handle = AddVariable( vbl_name );
    line = next; next = NextToken( line, ',' );
    val = NULL;
    if( line[0] != '\0' ) {
        if( line[0] == '%' ) {
            val = GetVariableStrVal( &line[1] );
        } else if( line[0] == '@' ) {
            // support @envvar@section:value - 2nd part is optional
            section = strchr( &line[1], '@' );
            if( section != NULL ) {
                // terminate envvar
                *section = '\0';
#if defined( __NT__ )
                ++section;
                value = strchr( section, ':' );
                if( value != NULL ) {
                    *value = '\0';
                    ++value;
                    if( GetRegString( HKEY_CURRENT_USER, section, value, buff, MAXBUF ) ) {
                        val = buff;
                    }
                }
#endif
            }
            if( val == NULL && line[1] != '\0' ) {
                val = getenv( &line[1] );
            }
        } else {
            val = line;
        }
    }
    if( val == NULL || val[0] == '\0' ) val = VarGetStrVal( var_handle );
    SetVariableByHandle( var_handle, val );
    if( VariablesFile != NULL ) {
        ReadVariablesFile( VarGetName( var_handle ) );
    }
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    ReplaceVars( buff, sizeof( buff ), line );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );

    line = TrimQuote( line );
    var_handle_2 = AddVariable( line );
//    len = max( BW, strlen( line + 2 ) );
    button_text = line;
    line = next; next = NextToken( line, ',' );
    dialog_name = line;
    line = next; next = NextToken( line, ',' );

    if( EvalCondition( line ) ) {
        dlg->curr_dialog->pVariables[dlg->num_variables] = var_handle;
        dlg->curr_dialog->pConditions[dlg->num_variables] = NULL;
        dlg->num_variables++;
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        var_handle_2 = MakeDummyVar();
        SetVariableByHandle( var_handle_2, dialog_name );
        set_dlg_push_button( var_handle_2, button_text, dlg->curr_dialog->controls,
                             dlg->controls.num - 1, dlg->row_num, 4, 4, W, BW );
        BumpDlgArrays( dlg );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_edit( dlg->curr_dialog->controls, dlg->controls.num - 1, buff,
                      VarGetId( var_handle ), C0, dlg->row_num, C0 + BW - 1 );
        if( buff[0] != '\0' ) {
            BumpDlgArrays( dlg );
            // condition for visibility (dynamic)
            dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls + 2].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_dynamstring( dlg->curr_dialog->controls, dlg->controls.num - 1, buff,
                                 VarGetId( var_handle ), C0, dlg->row_num,
                                 C0 + strlen( buff ) );
        }
        if( dlg->max_width < 2 * strlen( buff ) ) {
            dlg->max_width = 2 * strlen( buff );
        }
    } else {
        rc = false;
    }
    GUIMemFree( vbl_name );
    return( rc );
}

static bool dialog_other_button( char *next, DIALOG_INFO *dlg )
/*************************************************************/
{
    char                *line, *button_text, *next_copy, *text, *dialog_name;
    char                *condition;
    vhandle             var_handle;
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    button_text = TrimQuote( line );
    line = next; next = NextToken( line, ',' );
    dialog_name = line;
    next_copy = GUIStrDup( next, NULL );
    line = next; next = NextToken( line, ',' );
    text = line;
    line = next; next = NextToken( line, ',' );
    condition = line;
    line = next; next = NextToken( line, ',' );

    if( EvalCondition( condition ) ) {
        var_handle = MakeDummyVar();
        SetVariableByHandle( var_handle, dialog_name );
        set_dlg_push_button( var_handle, button_text, dlg->curr_dialog->controls,
                             dlg->controls.num - 1, dlg->row_num, 4, 4, W, BW );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        if( text != NULL ) {
            BumpDlgArrays( dlg );
            // condition for visibility (dynamic)
            dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            dlg->col_num = 1;
            dialog_static( next_copy, dlg );
        }
    } else {
        rc = false;
    }
    GUIMemFree( next_copy );
    return( rc );
}


static vhandle dialog_set_variable( DIALOG_INFO *dlg, const char *vbl_name,
                                    const char *init_cond )
/*******************************************************************/
{
    vhandle     var_handle;

    var_handle = AddVariable( vbl_name );
    dlg->curr_dialog->pVariables[dlg->num_variables] = var_handle;
    if( init_cond != NULL ) {
        if( *init_cond == '\0' ) {
            init_cond = NULL;
        } else {
            if( SkipDialogs ) {
                if( stricmp( init_cond, "true" ) == 0 ) {
                    SetVariableByHandle( var_handle, "1" );
                } else if( stricmp( init_cond, "false" ) == 0 ) {
                    SetVariableByHandle( var_handle, "0" );
                } else {
                    SetVariableByHandle( var_handle, init_cond );
                }
            }
        }
    }
    dlg->curr_dialog->pConditions[dlg->num_variables] = GUIStrDup( init_cond, NULL );
    dlg->num_variables++;
    return( var_handle );
}


static bool dialog_radiobutton( char *next, DIALOG_INFO *dlg )
/************************************************************/
{
    int                 len;
    char                *line;
    char                *vbl_name;
    vhandle             var_handle;
    char                *text;
    char                *init_cond;
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    init_cond = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    text = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        var_handle = dialog_set_variable( dlg, vbl_name, init_cond );
        dlg->num_radio_buttons += 1;
        len = strlen( text ) + 4; // room for button
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_radio( dlg->curr_dialog->controls, dlg->controls.num - 1,
                       dlg->num_radio_buttons, text, VarGetId( var_handle ), C0,
                       dlg->row_num, C0 + len );
        if( dlg->max_width < len ) {
            dlg->max_width = len;
        }
    } else {
        rc = false;
    }
    GUIMemFree( init_cond );
    GUIMemFree( vbl_name );
    GUIMemFree( text );
    return( rc );
}


static bool dialog_checkbox( char *next, DIALOG_INFO *dlg, bool detail_button )
/*****************************************************************************/
{
    int                 len;
    char                *line;
    char                *vbl_name;
    vhandle             var_handle;
    char                *text;
    char                *init_cond;
    bool                rc = true;
    char                *dialog_name;
    char                *button_text;
    vhandle             dlg_var_handle;

    if( detail_button ) {
        line = next; next = NextToken( line, ',' );
        dialog_name = line;
        line = next; next = NextToken( line, ',' );
        button_text = line;
    }
    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    init_cond = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    text = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        if( detail_button ) {
            dlg_var_handle = MakeDummyVar();
            SetVariableByHandle( dlg_var_handle, dialog_name );
        }
        var_handle = dialog_set_variable( dlg, vbl_name, init_cond );
        len = strlen( text ) + 4; // room for button
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_check( dlg->curr_dialog->controls, dlg->controls.num - 1, text,
                       VarGetId( var_handle ), dlg->col_num, dlg->row_num,
                       dlg->col_num + len );
        if( dlg->col_num == C0 ) {
            // 1st check-box on line
            if( dlg->max_width < len )
                dlg->max_width = len;
            dlg->col_num += len + 1;    // update col_num for next time
        } else {
            // 2nd check-box
            if( len < dlg->col_num - 1 )
                len = dlg->col_num - 1;
            if( dlg->max_width < 2 * len + 1 ) {    // add 1 for space
                dlg->max_width = 2 * len + 1;
            }
        }
        if( detail_button ) {
            BumpDlgArrays( dlg );
            dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            set_dlg_push_button( dlg_var_handle, button_text, dlg->curr_dialog->controls,
                                 dlg->controls.num - 1, dlg->row_num, 4, 4, W, BW );
        }
    } else {
        rc = false;
    }
    GUIMemFree( init_cond );
    GUIMemFree( vbl_name );
    GUIMemFree( text );
    return( rc );
}


void SimSetNeedGetDiskSizes( void )
/*********************************/
{
    NeedGetDiskSizes = true;
}


static bool dialog_editcontrol( char *next, DIALOG_INFO *dlg )
/************************************************************/
{
    char                *line;
    char                *vbl_name;
    const char          *val;
    char                *section;
#if defined( __NT__ )
    char                *value;
#endif
    vhandle             var_handle;
    char                buff[MAXBUF];
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    var_handle = AddVariable( vbl_name );
    line = next; next = NextToken( line, ',' );
    val = NULL;
    if( line[0] != '\0' ) {
        if( line[0] == '%' ) {
            val = GetVariableStrVal( &line[1] );
        } else if( line[0] == '@' ) {
            // support @envvar@section:value - 2nd part is optional
            section = strchr( &line[1], '@' );
            if( section != NULL ) {
                // terminate envvar
                *section = '\0';
#if defined( __NT__ )
                ++section;
                value = strchr( section, ':' );
                if( value != NULL ) {
                    *value = '\0';
                    ++value;
                    if( GetRegString( HKEY_CURRENT_USER, section, value, buff, MAXBUF ) ) {
                        val = buff;
                    }
                }
#endif
            }
            if( val == NULL && line[1] != '\0' ) {
                val = getenv( &line[1] );
            }
        } else {
            val = line;
        }
    }
    if( val == NULL || val[0] == '\0' ) {
        val = VarGetStrVal( var_handle );
    }
    SetVariableByHandle( var_handle, val );
    if( VariablesFile != NULL ) {
        ReadVariablesFile( VarGetName( var_handle ) );
    }
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    ReplaceVars( buff, sizeof( buff ), line );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        dlg->curr_dialog->pVariables[dlg->num_variables] = var_handle;
        dlg->curr_dialog->pConditions[dlg->num_variables] = NULL;
        dlg->num_variables++;
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_edit( dlg->curr_dialog->controls, dlg->controls.num - 1,
                      buff, VarGetId( var_handle ), C0, dlg->row_num, C0 + W - 1 );
        if( buff[0] != '\0' ) {
            BumpDlgArrays( dlg );
            // condition for visibility (dynamic)
            dlg->curr_dialog->controls_ext[dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_dynamstring( dlg->curr_dialog->controls, dlg->controls.num - 1, buff,
                                 VarGetId( var_handle ), C0, dlg->row_num,
                                 C0 + strlen( buff ) );
        }
        if( dlg->max_width < 2 * strlen( buff ) ) {
            dlg->max_width = 2 * strlen( buff );
        }
    } else {
        rc = false;
    }
    GUIMemFree( vbl_name );
    return( rc );
}

static char *CompileCondition( const char *str );

static void GrabConfigInfo( char *line, array_info *info )
/********************************************************/
{
   size_t               num;
   char                 *next;
   struct config_info   *array;

    num = info->num;
    if( !BumpArray( info ) )
        return;
    array = *(info->array);
    next = NextToken( line, '=' );
    array[num].var = GUIStrDup( line, NULL );
    line = next; next = NextToken( line, ',' );
    array[num].value = GUIStrDup( line, NULL );
    array[num].condition = CompileCondition( next );
}

static bool ProcLine( char *line, pass_type pass )
/************************************************/
{
    char                *next;
    int                 num;
    int                 tmp;
    char                *p;

    // Remove leading and trailing white-space.
    line = StripEndBlanks( line );

    // Check for comment
    if( *line == '#' ) {
        return( true );
    }

    // Check if the state has changed.
    if( *line == '[' ) {
        LineCountPointer = &NoLineCount;
        if( stricmp( line, "[End]" ) == 0 ) {
            State = RS_TERMINATE;
        } else if( stricmp( line, "[Application]" ) == 0 ) {
            State = RS_APPLICATION;
        } else if( stricmp( line, "[Disks]" ) == 0 ) {
            State = RS_DISKS;
            LineCountPointer = &SetupInfo.disks.alloc;
        } else if( stricmp( line, "[Dirs]" ) == 0 ) {
            State = RS_DIRS;
            LineCountPointer = &SetupInfo.dirs.alloc;
        } else if( stricmp( line, "[Files]" ) == 0 ) {
            State = RS_FILES;
            LineCountPointer = &SetupInfo.files.alloc;
        } else if( stricmp( line, "[PM Info]" ) == 0 ) {
            State = RS_PMINFO;
            LineCountPointer = &SetupInfo.pm_files.alloc;
        } else if( stricmp( line, "[Profile]" ) == 0 ) {
            State = RS_PROFILE;
            LineCountPointer = &SetupInfo.profile.alloc;
        } else if( stricmp( line, "[Autoexec]" ) == 0 ) {
            State = RS_AUTOEXEC;
            LineCountPointer = &SetupInfo.autoexec.alloc;
        } else if( stricmp( line, "[Config]" ) == 0 ) {
            State = RS_CONFIG;
            LineCountPointer = &SetupInfo.config.alloc;
        } else if( stricmp( line, "[Environment]" ) == 0 ) {
            State = RS_ENVIRONMENT;
            LineCountPointer = &SetupInfo.environment.alloc;
        } else if( stricmp( line, "[Dialog]" ) == 0 ) {
            State = RS_DIALOG;
        } else if( stricmp( line, "[Targets]" ) == 0 ) {
            State = RS_TARGET;
            LineCountPointer = &SetupInfo.target.alloc;
        } else if( stricmp( line, "[Labels]" ) == 0 ) {
            State = RS_LABEL;
            LineCountPointer = &SetupInfo.label.alloc;
        } else if( stricmp( line, "[Upgrade]" ) == 0 ) {
            State = RS_UPGRADE;
            LineCountPointer = &SetupInfo.upgrade.alloc;
        } else if( stricmp( line, "[ErrorMessage]" ) == 0 ) {
            State = RS_ERRORMESSAGE;
        } else if( stricmp( line, "[SetupErrorMessage]" ) == 0 ) {
            State = RS_SETUPERRORMESSAGE;
        } else if( stricmp( line, "[StatuslineMessage]" ) == 0 ) {
            State = RS_STATUSLINEMESSAGE;
        } else if( stricmp( line, "[MiscMessage]" ) == 0 ) {
            State = RS_MISCMESSAGE;
        } else if( stricmp( line, "[LicenseMessage]" ) == 0 ) {
            State = RS_LICENSEMESSAGE;
#ifdef PATCH
        } else if( stricmp( line, "[Patch]" ) == 0 ) {
            State = RS_PATCH;
            LineCountPointer = &SetupInfo.patch_files.alloc;
#endif
        } else if( stricmp( line, "[Restrictions]" ) == 0 ) {
            State = RS_RESTRICTIONS;
        } else if( stricmp( line, "[DeleteFiles]" ) == 0 ) {
            State = RS_DELETEFILES;
            LineCountPointer = &SetupInfo.delete.alloc;
        } else if( stricmp( line, "[AutoSet]" ) == 0 ) {
            State = RS_AUTOSET;
        } else if( stricmp( line, "[Spawn]" ) == 0 ) {
            State = RS_SPAWN;
        } else if( stricmp( line, "[ForceDLLInstall]" ) == 0 ) {
            State = RS_FORCEDLLINSTALL;
            LineCountPointer = &SetupInfo.force_DLL_install.alloc;
        } else if( stricmp( line, "[Associations]" ) == 0 ) {
            State = RS_ASSOCIATIONS;
        } else {
            State = RS_UNDEFINED;   // Unrecognized section in SETUP.INF file.
        }
        return( true );
    }

    // line is part of the current state.
    if( *line == ';' || *line == '\0' )
        return( true );
    if( pass == PRESCAN_FILE ) {
        ++*LineCountPointer;
        return( true );
    }

    switch( State ) {
    case RS_DIALOG:
        {
            static DIALOG_INFO  dlg;
            bool                added = false;

            next = NextToken( line, '=' );
            if( stricmp( line, "name" ) == 0 ) {
                // new dialog
                memset( &dlg, 0, sizeof( DIALOG_INFO ) );
                dlg.curr_dialog = AddNewDialog( next );
                InitDlgArrays( &dlg );
                dlg.wrap_width = MaxWidthChars;
            } else if( stricmp( line, "condition" ) == 0 ) {
                dlg.curr_dialog->condition = CompileCondition( next );
            } else if( stricmp( line, "title" ) == 0 ) {
                dlg.curr_dialog->title = GUIStrDup( next, NULL );
            } else if( stricmp( line, "any_check" ) == 0 ) {
                dlg.curr_dialog->any_check = AddVariable( next );
            } else if( stricmp( line, "width" ) == 0 ) {
                int         wrap_width;
                wrap_width = atoi( next );
                if( wrap_width > 0 && wrap_width <= MaxWidthChars ) {
                    dlg.wrap_width = wrap_width;
                }
            } else if( stricmp( line, "vis_condition" ) == 0 ) {
                line = next; next = NextToken( line, ',' );
                if( !EvalCondition( next ) ) {
                    line = NULL;
                }
                dlg.curr_dialog->controls_ext[dlg.curr_dialog->num_controls - 1].pVisibilityConds = GUIStrDup( line, NULL );
            } else {
                // add another control to current dialog
                if( !BumpDlgArrays( &dlg ) ) {
                    SetupError( "IDS_NOMEMORY" );
                    exit( 1 );
                }
                if( stricmp( line, "static_text" ) == 0 ) {
                    dlg.col_num = C0;
                    added = dialog_static( next, &dlg );
                } else if( stricmp( line, "dynamic_text" ) == 0 ) {
                    added = dialog_dynamic( next, &dlg );
                } else if( stricmp( line, "other_button" ) == 0 ) {
                    added = dialog_other_button( next, &dlg );
                } else if( stricmp( line, "edit_button" ) == 0 ) {
                    added = dialog_edit_button( next, &dlg );
                } else if( stricmp( line, "push_button" ) == 0 ) {
                    added = dialog_pushbutton( next, &dlg );
                    if( added ) {
                        dlg.row_num -= 1;
                    }
                } else if( stricmp( line, "radio_button" ) == 0 ) {
                    added = dialog_radiobutton( next, &dlg );
                } else if( stricmp(line, "check_box") == 0 ) {
                    dlg.col_num = C0;
                    added = dialog_checkbox( next, &dlg, false );
                } else if( stricmp(line, "detail_check") == 0 ) {
                    dlg.col_num = C0;
                    added = dialog_checkbox( next, &dlg, true );
                } else if( stricmp(line, "check_box_continue") == 0 ) {
                    dlg.row_num -= 1;
                    added = dialog_checkbox( next, &dlg, false );
                } else if( stricmp(line, "edit_control") == 0 ) {
                    added = dialog_editcontrol( next, &dlg );
                } else if( stricmp(line, "text_window") == 0 ) {
                    added = dialog_textwindow( next, &dlg, false );
                } else if( stricmp(line, "text_window_license") == 0 ) {
                    added = dialog_textwindow( next, &dlg, true );
                }
                if( added ) {
                    dlg.row_num += 1;
                    // in case this was the last control, set some values
                    dlg.curr_dialog->pVariables[dlg.num_variables] = NO_VAR;
                    dlg.curr_dialog->pConditions[dlg.num_variables] = NULL;
                    dlg.curr_dialog->num_controls = dlg.controls.num;
                    dlg.curr_dialog->num_push_buttons = dlg.num_push_buttons;
                    dlg.curr_dialog->rows = dlg.row_num  + HEIGHT_BORDER;
                    if( dlg.num_push_buttons != 0 ) {
                        dlg.curr_dialog->rows += 1;
                    }
                    dlg.curr_dialog->cols = dlg.max_width + WIDTH_BORDER;
                } else {
                    dlg.controls.num--;
                    dlg.controls_ext.num--;
                }
            }
        }
        break;
    case RS_APPLICATION:
        next = NextToken( line, '=' );
        if( stricmp( line, "DefPMGroup" ) == 0 ) {
            line = next; next = NextToken( line, ',' );
            SetupInfo.pm_group_file_name = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            SetupInfo.pm_group_name = GUIStrDup( line, NULL );
            num = SetupInfo.all_pm_groups.num;
            if( !BumpArray( &SetupInfo.all_pm_groups ) )
                return( false );
            AllPMGroups[num].group = GUIStrDup( line, NULL );
            AllPMGroups[num].group_file_name = GUIStrDup( SetupInfo.pm_group_file_name, NULL );
            SetupInfo.pm_group_icon = GUIStrDup( next, NULL );
        } else {
            if( line[0] == '$' ) {
                // global variables start with '$'
                if( GetVariableByName( line ) == NO_VAR ) {
                    // if variable already is set, do not change it
                    SetVariableByName( line, next );
                }
            } else {
                SetVariableByName( line, next );
            }
        }
        break;

    case RS_DISKS:
        num = SetupInfo.disks.num;
        if( !BumpArray( &SetupInfo.disks ) )
            return( false );
        DiskInfo[num].desc = GUIStrDup( line, NULL );
        break;

    case RS_DIRS:
        num = SetupInfo.dirs.num;
        if( !BumpArray( &SetupInfo.dirs ) )
            return( false );
        next = NextToken( line, ',' );
        DirInfo[num].desc = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        DirInfo[num].target = atoi( line ) - 1;
        DirInfo[num].parent = atoi( next );
        if( DirInfo[num].parent != -1 ) {
            DirInfo[num].parent--;
        }
        break;

    case RS_FILES:
        num = SetupInfo.files.num;
        if( !BumpArray( &SetupInfo.files ) )
            return( false );
        next = NextToken( line, ',' );
        FileInfo[num].filename = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        /*
            Multiple files in archive. First number is number of files,
            followed by a list of file sizes in 512 byte blocks.
        */
        #define get36( x ) strtol( x, NULL, 36 )
        tmp = get36( line );
        if( tmp == 0 ) {
            FileInfo[num].files = NULL;
        } else {
            FileInfo[num].files = GUIMemAlloc( tmp * sizeof( a_file_info ) );
            if( FileInfo[num].files == NULL ) {
                return( false );
            }
        }
        FileInfo[num].supplimental = false;
        FileInfo[num].core_component = false;
        FileInfo[num].num_files = tmp;
        while( --tmp >= 0 ) {
            a_file_info *file = &FileInfo[num].files[tmp];

            line = next; next = NextToken( line, ',' );
            p = NextToken( line, '!' );
            file->name = GUIStrDup( line, NULL );
            {
                char    fext[_MAX_EXT];
                _splitpath( file->name, NULL, NULL, NULL, fext );
                file->is_nlm = stricmp( fext, ".nlm" ) == 0;
                file->is_dll = stricmp( fext, ".dll" ) == 0;
            }
            line = p; p = NextToken( line, '!' );
            file->size = get36( line ) * 512UL;
            if( p != NULL && *p != '\0' && *p != '!' ) {
                file->date = get36( p );
            } else {
                file->date = SetupInfo.stamp;
            }
            line = p; p = NextToken( line, '!' );
            if( p != NULL && *p != '\0' && *p != '!' ) {
                file->dst_var = AddVariable( p );
            } else {
                file->dst_var = NO_VAR;
            }
            line = p; p = NextToken( line, '!' );
            file->executable = false;
            if( p != NULL ) {
                if( *p == 'e' ) {
                    file->executable = true;
                }
            }
            line = p; p = NextToken( line, '!' );
            if( p != NULL ) {
                if( *p == 's' ) {
                    FileInfo[num].supplimental = true;
                } else if( *p == 'k' ) {
                    FileInfo[num].core_component = true;
                }
            }
        }
        line = next; next = NextToken( line, ',' );
        FileInfo[num].dir_index = get36( line ) - 1;
        line = next; next = NextToken( line, ',' );
        FileInfo[num].old_dir_index = get36( line );
        if( FileInfo[num].old_dir_index != -1 ) {
            FileInfo[num].old_dir_index--;
        }
        line = next; next = NextToken( line, ',' );
        FileInfo[num].disk_index = get36( line ) - 1;
        line = next; next = NextToken( line, ',' );
        FileInfo[num].file_type = tolower( *line );
        line = next; next = NextToken( line, ',' );
        FileInfo[num].condition.i = NewFileCond( line );
        break;

#ifdef PATCH
    case RS_PATCH:
        num = SetupInfo.patch_files.num;
        if( !BumpArray( &SetupInfo.patch_files ) )
            return( false );
        memset( &PatchInfo[num], 0, sizeof( *PatchInfo ) );
        next = NextToken( line, ',' );
        if( stricmp( line, "copy" ) == 0 ) {
            PatchInfo[num].command = PATCH_COPY_FILE;
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].srcfile = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destdir = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destfile = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].condition = GUIStrDup( line, NULL );
        } else if( stricmp( line, "patch" ) == 0 ) {
            PatchInfo[num].command = PATCH_FILE;
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].srcfile = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destdir = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destfile = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].exetype = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].condition = GUIStrDup( line, NULL );
        } else if( stricmp( line, "delete" ) == 0 ) {
            PatchInfo[num].command = PATCH_DELETE_FILE;
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destfile = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destdir = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].condition = GUIStrDup( line, NULL );
        } else if( stricmp( line, "mkdir" ) == 0 ) {
            PatchInfo[num].command = PATCH_MAKE_DIR;
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].destdir = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            PatchInfo[num].condition = GUIStrDup( line, NULL );
        }
        break;
#endif

    case RS_AUTOSET:
        next = NextToken( line, '=' );
        if( VariablesFile == NULL ) {
            VarSetAutoSetCond( AddVariable( line ), next );
        } else {
            AddVariable( line );
        }
        break;

    case RS_RESTRICTIONS:
        next = NextToken( line, '=' );
        VarSetAutoSetRestriction( AddVariable( line ), next );
        break;

    case RS_SPAWN:
        num = SetupInfo.spawn.num;
        if( !BumpArray( &SetupInfo.spawn ) )
            return( false );
        next = NextToken( line, '=' );
        if( stricmp( line, "after" ) == 0 ) {
            SpawnInfo[num].when = WHEN_AFTER;
        } else if( stricmp( line, "end" ) == 0 ) {
            SpawnInfo[num].when = WHEN_END;
        } else {
            SpawnInfo[num].when = WHEN_BEFORE;
        }
        line = next; next = NextToken( line, ',' );
        SpawnInfo[num].command = GUIStrDup( line, NULL );
        SpawnInfo[num].condition = CompileCondition( next );
        break;

    case RS_DELETEFILES:
        num = SetupInfo.delete.num;
        if( !BumpArray( &SetupInfo.delete ) )
            return( false );
        next = NextToken( line, '=' );
        if( stricmp( line, "dialog" ) == 0 ) {
            DeleteInfo[num].type = DELETE_DIALOG;
        } else if( stricmp( line, "file" ) == 0 ) {
            DeleteInfo[num].type = DELETE_FILE;
        } else if( stricmp( line, "directory" ) == 0 ) {
            DeleteInfo[num].type = DELETE_DIR;
        }
        line = next; next = NextToken( line, ',' );
        DeleteInfo[num].name = GUIStrDup( line, NULL );
        break;

    case RS_PMINFO:
        num = SetupInfo.pm_files.num;
        if( !BumpArray( &SetupInfo.pm_files ) )
            return( false );
        next = NextToken( line, ',' );
        PMInfo[num].filename = GUIStrDup( line, NULL );
        if( strcmp( line, "GROUP" ) == 0 ) {
            tmp = true;
        } else {
            tmp = false;
        }
        line = next; next = NextToken( line, ',' );
        PMInfo[num].parameters = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        PMInfo[num].desc = GUIStrDup( line, NULL );
        if( tmp ) {
            AllPMGroups[SetupInfo.all_pm_groups.num].group = GUIStrDup( line, NULL );
            AllPMGroups[SetupInfo.all_pm_groups.num].group_file_name = GUIStrDup( PMInfo[num].parameters, NULL );
            if( !BumpArray( &SetupInfo.all_pm_groups ) ) {
                return( false );
            }
        }
        if( next == NULL ) {
            PMInfo[num].icoioname = NULL;
            PMInfo[num].icon_pos = 0;
        } else {
            line = next; next = NextToken( line, ',' );
            PMInfo[num].icoioname = GUIStrDup( line, NULL );
            if( next == NULL ) {
                PMInfo[num].icon_pos = 0;
            } else {
                line = next; next = NextToken( line, ',' );
                PMInfo[num].icon_pos = atoi( line );
            }
        }
        PMInfo[num].condition = CompileCondition( next );
        break;

    case RS_PROFILE:
        num = SetupInfo.profile.num;
        if( !BumpArray( &SetupInfo.profile ) )
            return( false );
        next = NextToken( line, ',' );
        ProfileInfo[num].app_name = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        ProfileInfo[num].key_name = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        ProfileInfo[num].value = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        ProfileInfo[num].file_name = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        ProfileInfo[num].hive_name = GUIStrDup( line, NULL );
        ProfileInfo[num].condition = CompileCondition( next );
        break;

    case RS_AUTOEXEC:
        GrabConfigInfo( line, &SetupInfo.autoexec );
        break;

    case RS_CONFIG:
        GrabConfigInfo( line, &SetupInfo.config );
        break;

    case RS_ENVIRONMENT:
        GrabConfigInfo( line, &SetupInfo.environment );
        break;

    case RS_TARGET:
        num = SetupInfo.target.num;
        if( !BumpArray( &SetupInfo.target ) )
            return( false );
        next = NextToken( line, ',' );
        TargetInfo[num].name = GUIStrDup( line, NULL );
        TargetInfo[num].supplimental = false;
        if( next != NULL && stricmp( next, "supplimental" ) == 0 ) {
            TargetInfo[num].supplimental = true;
        }
        TargetInfo[num].temp_disk = GUIMemAlloc( _MAX_PATH );
        if( TargetInfo[num].temp_disk == NULL ) {
            return( false );
        }
        *TargetInfo[num].temp_disk = 0;
        break;

    case RS_LABEL:
        num = SetupInfo.label.num;
        if( !BumpArray( &SetupInfo.label ) )
            return( false );
        next = NextToken( line, '=' );
        LabelInfo[num].dir = GUIStrDup( line, NULL );
        LabelInfo[num].label = GUIStrDup( next, NULL );
        break;

    case RS_UPGRADE:
        num = SetupInfo.upgrade.num;
        if( !BumpArray( &SetupInfo.upgrade ) )
            return( false );
        UpgradeInfo[num].name = GUIStrDup( line, NULL );
        break;

    case RS_FORCEDLLINSTALL:
        num = SetupInfo.force_DLL_install.num;
        if( !BumpArray( &SetupInfo.force_DLL_install ) )
            return( false );
        ForceDLLInstall[num].name = GUIStrDup( line, NULL );
        break;

    case RS_ASSOCIATIONS:
        num = SetupInfo.associations.num;
        if( !BumpArray( &SetupInfo.associations ) )
            return (false );
        next = NextToken( line, '=' );
        AssociationInfo[num].ext = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].keyname = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].program = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].description = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].icon_index = strtol( line, NULL, 10 );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].no_open = strtol( line, NULL, 10 );
        AssociationInfo[num].condition = CompileCondition( next );
        break;

        /* for now Setup Error Messages, Status line Messages and Misc Messages
            are treated as global symbolic variables just like
            regular Error Messages */
    case RS_SETUPERRORMESSAGE:
    case RS_STATUSLINEMESSAGE:
    case RS_MISCMESSAGE:
    case RS_ERRORMESSAGE:
#ifdef LICENSE
    case RS_LICENSEMESSAGE:
#endif
        {
            char    *remnewline;

            next = NextToken( line, '=' );
            next = StripEndBlanks( next );
            next++;
            *(strchr( next, '\0' ) - 1) = '\0';
            for( ;; ) {
                remnewline = strstr( next, "\\n" );
                if( remnewline == NULL )
                    break;
                *remnewline = '\n';
                memmove( remnewline + 1, remnewline + 2, strlen( remnewline + 2 ) + 1 );
            }
            SetVariableByName( line, next );
            break;
        }
    default:
        break;
    }

    return( true );
}


static bool GetFileInfo( int dir_index, int i, bool in_old_dir, bool *pzeroed )
/*****************************************************************************/
{
    char        buff[_MAX_PATH];
    char        *dir_end;
    struct stat buf;
    int         j,k;
    bool        found;
    bool        supp;
    a_file_info *file;

    if( dir_index == -1 )
        return( false );
    SimDirNoSlash( dir_index, buff, sizeof( buff ) );
    if( access( buff, F_OK ) != 0 )
        return( false );

    ConcatDirSep( buff );
    dir_end = buff + strlen( buff );
    found = false;
    supp = TargetInfo[DirInfo[FileInfo[i].dir_index].target].supplimental;
    if( supp ) {
        // don't turn off supplimental bit if file is already marked
        FileInfo[i].supplimental = supp;
    }
    for( j = 0; j < FileInfo[i].num_files; ++j ) {
        file = &FileInfo[i].files[j];
        file->disk_size = 0;
        if( file->name == NULL )
            continue;
        strcpy( dir_end, file->name );
        if( access( buff, F_OK ) == 0 ) {
            stat( buff, &buf );
            found = true;
            file->disk_size = buf.st_size;
            file->disk_date = buf.st_mtime;
            if( in_old_dir ) {
                file->in_old_dir = true;
            } else {
                file->in_new_dir = true;
            }
            file->read_only = !(buf.st_mode & S_IWRITE);
            if( supp )
                continue;
            if( !*pzeroed ) {
                ZeroAutoSetValues();
                for( k = 0; k < SetupInfo.fileconds.num; ++k ) {
                    FileCondInfo[k].one_uptodate = false;
                }
                *pzeroed = true;
            }
            PropagateValue( FileInfo[i].condition.p->cond, true );
            if( file->in_new_dir &&
                RoundUp( file->disk_size, 512 ) == file->size &&
                file->date == file->disk_date ) {
                FileInfo[i].condition.p->one_uptodate = true;
            }
        }
    }
    return( found );
}

static bool GetDiskSizes( void )
/******************************/
{
    int         i, j;
    long        status_amount;
    long        status_curr;
    bool        zeroed;
    bool        rc = true;
    bool        asked, dont_touch;
    bool        uninstall;

    status_amount = 0;
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        status_amount += FileInfo[i].num_files;
    }
    StatusShow( true );
    StatusLines( STAT_CHECKING, "" );
    SetVariableByHandle( PreviousInstall, "0" );
    zeroed = false;
    status_curr = 0;
    InitAutoSetValues();
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        StatusAmount( status_curr, status_amount );
        if( StatusCancelled() ) {
            rc = false;
            break;
        }
        status_curr += FileInfo[i].num_files;
        if( FileInfo[i].num_files == 0 )
            continue;
        for( j = 0; j < FileInfo[i].num_files; ++j ) {
            FileInfo[i].files[j].disk_size = 0;
            FileInfo[i].files[j].disk_date = 0;
            FileInfo[i].files[j].in_old_dir = false;
            FileInfo[i].files[j].in_new_dir = false;
            FileInfo[i].files[j].read_only = false;
        }
        GetFileInfo( FileInfo[i].dir_index, i, false, &zeroed );
        GetFileInfo( FileInfo[i].old_dir_index, i, true, &zeroed );
    }
    StatusLines( STAT_BLANK, "" );
    StatusAmount( 0, 1 );
    StatusShow( false );
    if( !rc )
        return( rc );
    dont_touch = false;
    uninstall = VarGetIntVal( UnInstall );
    if( uninstall ) {
        // if uninstalling - remove all files, don't prompt
        asked = true;
    } else {
        asked = false;
    }
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        if( FileInfo[i].condition.p->one_uptodate &&
            FileInfo[i].num_files != 0 &&
            !FileInfo[i].supplimental &&
            !SimFileUpToDate( i ) ) {
            if( !asked ) {
                dont_touch = MsgBox( NULL, "IDS_INCONSISTENT", GUI_YES_NO ) == GUI_RET_NO;
                asked = true;
            }
            FileInfo[i].condition.p->dont_touch = dont_touch;
        }
    }
    return( rc );
}


static char *readLine( void *handle, char *buffer, size_t length )
/****************************************************************/
{
    static int      raw_buf_size;
    char            *line_start;
    size_t          len;
    bool            done;

    done = false;
    do {
        // Read data into raw buffer if it's empty
        if( RawBufPos == NULL ) {
            raw_buf_size = FileRead( handle, RawReadBuf, BUF_SIZE );
            if( raw_buf_size <= 0 ) {
                return( NULL );
            }
            RawBufPos = RawReadBuf;
        }

        line_start = RawBufPos;
        // Look for a newline; check for end of source buffer and size
        // of target buffer
        while( (*RawBufPos != '\n') &&
               (RawBufPos < RawReadBuf + raw_buf_size) &&
               (RawBufPos - line_start < length) ) {
            ++RawBufPos;
        }

        if( *RawBufPos == '\n' ) {
            // Found a newline; increment past it
            ++RawBufPos;
            done = true;
        } else if( RawBufPos == RawReadBuf + raw_buf_size ) {
            // We're at the end of the buffer; copy what we have to output buffer
            len = RawBufPos - line_start;
            memcpy( buffer, line_start, len );
            length -= len;
            buffer += len;

            // Force read of more data into buffer
            RawBufPos = NULL;
        } else {
            // No more space in output buffer
            done = true;
        }
    } while( !done );

    len = RawBufPos - line_start;

    memcpy( buffer, line_start, len );
    buffer[len] = '\0';

    return( buffer );
}


static int PrepareSetupInfo( FILE *io, pass_type pass )
/*****************************************************/
{
    int                 result;
    void                *cursor;
    bool                done;
    size_t              len;
    char                *p;

    LineCountPointer = &NoLineCount;
    cursor = GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
    result = SIM_INIT_NOERROR;
    if( pass == PRESCAN_FILE ) {
        State = RS_UNDEFINED;
    }

    // Read file in blocks, break up into lines
    done = false;
    for( ;; ) {
        len = 0;
        for( ;; ) {
            if( readLine( io, ReadBuf + len, ReadBufSize - len ) == NULL ) {
                done = true;
                break;
            }
            // Eliminate leading blanks on continued lines
            if( len > 0 ) {
                p = ReadBuf + len;
                while( *p == ' ' || *p == '\t' )
                    ++p;
                memmove( ReadBuf + len, p, strlen( p )+1 );
            }
            len = strlen( ReadBuf );
            if( len == 0 )
                break;

            // Manually convert CR/LF if needed
            if( (len > 1) && (ReadBuf[len - 1] == '\n') ) {
                if( ReadBuf[len - 2] == '\r' ) {
                    ReadBuf[len - 2] = '\n';
                    ReadBuf[len - 1] = '\0';
                    --len;
                }
            }

            if( ReadBuf[len - 1] == '\n' ) {
                if( len == 1 )
                    break;
                if( ReadBuf[len - 2] != '\\' )
                    break;
                len -= 2;
            }
            if( ReadBufSize - len < BUF_SIZE / 2 ) {
                ReadBufSize += BUF_SIZE;
                ReadBuf = GUIMemRealloc( ReadBuf, ReadBufSize );
            }
        }
        if( done )
            break;
        if( !ProcLine( ReadBuf, pass ) ) {
            result = SIM_INIT_NOMEM;
            break;
        }
        if( State == RS_TERMINATE )
            break;
    }
    GUIResetMouseCursor( cursor );
    return( result );
}

extern bool CheckForceDLLInstall( char *name )
/********************************************/
{
    int         i;
    if( name == NULL ) {
        return( true );
    }
    for( i = 0; i < SetupInfo.force_DLL_install.num; i++ ) {
        if( stristr( ForceDLLInstall[i].name, name ) != NULL ) {
            return( true );
        }
    }
    return( false );
}

extern long SimInit( char *inf_name )
/***********************************/
{
    long                result;
    void                *io;
    struct stat         stat_buf;
    int                 i;
    gui_text_metrics    metrics;

    memset( &SetupInfo, 0, sizeof( struct setup_info ) );
    FileStat( inf_name, &stat_buf );
    SetupInfo.stamp = stat_buf.st_mtime;
#define setvar( x, y ) x = AddVariable( #x );
    MAGICVARS( setvar, 0 )
    NONMAGICVARS( setvar, 0 )
    SetDefaultGlobalVarList();
    ReadBufSize = BUF_SIZE;
    ReadBuf = GUIMemAlloc( BUF_SIZE );
    if( ReadBuf == NULL ) {
        return( SIM_INIT_NOMEM );
    }
    RawReadBuf = GUIMemAlloc( BUF_SIZE );
    if( RawReadBuf == NULL ) {
        return( SIM_INIT_NOMEM );
    }
    RawBufPos = NULL;       // reset buffer position

    io = FileOpen( inf_name, O_RDONLY + O_BINARY );
    if( io == NULL ) {
        GUIMemFree( ReadBuf );
        GUIMemFree( RawReadBuf );
        return( SIM_INIT_NOFILE );
    }
    SetVariableByName( "SetupInfFile", inf_name );
    result = PrepareSetupInfo( io, PRESCAN_FILE );
#if 0
    // Currently doesn't work for archives
    FileSeek( io, 0, SEEK_SET );
#else
    FileClose( io );
    io = FileOpen( inf_name, O_RDONLY + O_BINARY );
    if( io == NULL ) {
        GUIMemFree( ReadBuf );
        GUIMemFree( RawReadBuf );
        return( SIM_INIT_NOFILE );
    }
#endif
    InitArray( (void **)&DiskInfo, sizeof( struct disk_info ), &SetupInfo.disks );
    InitArray( (void **)&DirInfo, sizeof( struct dir_info ), &SetupInfo.dirs );
    InitArray( (void **)&FileInfo, sizeof( struct file_info ), &SetupInfo.files );
    InitArray( (void **)&PMInfo, sizeof( struct pm_info ), &SetupInfo.pm_files );
    InitArray( (void **)&ProfileInfo, sizeof( struct profile_info ), &SetupInfo.profile );
    InitArray( (void **)&AutoExecInfo, sizeof( struct config_info ), &SetupInfo.autoexec );
    InitArray( (void **)&ConfigInfo, sizeof( struct config_info ), &SetupInfo.config );
    InitArray( (void **)&EnvironmentInfo, sizeof( struct config_info ), &SetupInfo.environment );
    InitArray( (void **)&TargetInfo, sizeof( struct target_info ), &SetupInfo.target );
    InitArray( (void **)&LabelInfo, sizeof( struct label_info ), &SetupInfo.label );
    InitArray( (void **)&UpgradeInfo, sizeof( struct upgrade_info ), &SetupInfo.upgrade );
#ifdef PATCH
    InitArray( (void **)&PatchInfo, sizeof( struct patch_info ), &SetupInfo.patch_files );
#endif
    InitArray( (void **)&SpawnInfo, sizeof( struct spawn_info ), &SetupInfo.spawn );
    InitArray( (void **)&DeleteInfo, sizeof( struct spawn_info ), &SetupInfo.delete );
    InitArray( (void **)&FileCondInfo, sizeof( struct file_cond_info ), &SetupInfo.fileconds );
    InitArray( (void **)&DLLsToCheck, sizeof( struct dlls_to_check ), &SetupInfo.dlls_to_count );
    InitArray( (void **)&ForceDLLInstall, sizeof( struct force_DLL_install ), &SetupInfo.force_DLL_install );
    InitArray( (void **)&AllPMGroups, sizeof( struct all_pm_groups ), &SetupInfo.all_pm_groups );
    InitArray( (void **)&AssociationInfo, sizeof( struct association_info ), &SetupInfo.associations );
#ifndef _UI
    SetDialogFont();
#endif
    GUIGetTextMetrics( MainWnd, &metrics );
    GUIGetDlgTextMetrics( &metrics );
    CharWidth = metrics.avg.x;
    MaxWidthChars = GUIScale.x / CharWidth - 4 * WIDTH_BORDER;
    if( MaxWidthChars > MAX_WINDOW_WIDTH )  {
        MaxWidthChars = MAX_WINDOW_WIDTH;
    }
    result = PrepareSetupInfo( io, FINAL_SCAN );
    FileClose( io );
    GUIMemFree( ReadBuf );
    GUIMemFree( RawReadBuf );
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        FileInfo[i].condition.p = &FileCondInfo[FileInfo[i].condition.i];
    }
    return( result );
}

/*
 * =======================================================================
 * API to DeleteInfo[]
 * =======================================================================
 */

int SimNumDeletes( void )
/***********************/
{
    return( SetupInfo.delete.num );
}

char *SimDeleteName( int i )
/**************************/
{
    return( DeleteInfo[i].name );
}

bool SimDeleteIsDialog( int i )
/*****************************/
{
    return( DeleteInfo[i].type == DELETE_DIALOG );
}

bool SimDeleteIsDir( int i )
/**************************/
{
    return( DeleteInfo[i].type == DELETE_DIR );
}

/*
 * =======================================================================
 * API to TargetInfo[]
 * =======================================================================
 */

int SimNumTargets( void )
/***********************/
{
    return( SetupInfo.target.num );
}

void SimTargetDir( int i, char *buff, size_t buff_len )
/*****************************************************/
{
    ReplaceVars( buff, buff_len, GetVariableStrVal( TargetInfo[i].name ) );
}

void SimTargetDirName( int i, char *buff, size_t buff_len )
/*********************************************************/
{
    // same as SimTargetDir, only don't expand macros
    ReplaceVars( buff, buff_len, TargetInfo[i].name );
}

extern bool SimTargetNeedsUpdate( int i )
/***************************************/
{
    return( TargetInfo[i].needs_update );
}

extern uint_64 SimTargetSpaceNeeded( int i )
/******************************************/
{
    return( TargetInfo[i].space_needed );
}

extern uint_64 SimMaxTmpFile( int i )
/***********************************/
{
    return( TargetInfo[i].max_tmp_file );
}

extern int SimGetTargNumFiles( int i )
/************************************/
{
    return( TargetInfo[i].num_files );
}

extern void SimSetTargTempDisk( int parm, char disk )
/****************************************************/
{
    *TargetInfo[parm].temp_disk = disk;
}

extern char *SimGetTargTempDisk( int parm )
/*****************************************/
{
    return( TargetInfo[parm].temp_disk );
}

/*
 * =======================================================================
 * API to DiskInfo[]
 * =======================================================================
 */

int SimGetNumDisks( void )
/************************/
{
    return( SetupInfo.disks.num );
}

/*
 * =======================================================================
 * API to DirInfo[]
 * =======================================================================
 */

extern int SimDirTargNum( int i )
/*******************************/
{
    return( DirInfo[i].target );
}

extern int SimDirParent( int i )
/******************************/
{
    return( DirInfo[i].parent );
}

int SimNumDirs( void )
/********************/
{
    return( SetupInfo.dirs.num );
}

void SimDirNoSlash( int i, char *buff, size_t buff_len )
/******************************************************/
{
    char                dir[_MAX_DIR];

    SimTargetDir( DirInfo[i].target, buff, buff_len );
    strcpy( dir, DirInfo[i].desc );
    if( !IS_EMPTY( dir ) ) {
        ConcatDirSep( buff );
        strcat( buff, dir );
    }
}

extern bool SimDirUsed( int i )
/*****************************/
{
    return( DirInfo[i].used );
}

extern void SimGetDir( int i, char *buff, size_t buff_len )
/*********************************************************/
{
    SimDirNoSlash( i, buff, buff_len );
    ConcatDirSep( buff );
}

/*
 * =======================================================================
 * API to FileInfo[]
 * =======================================================================
 */

int SimNumFiles( void )
/*********************/
{
    return( SetupInfo.files.num );
}

extern void SimGetFileDesc( int parm, char *buff )
/************************************************/
{
    if( FileInfo[parm].num_files == 0 ) {
        strcpy( buff, FileInfo[parm].filename );
    } else {
        strcpy( buff, FileInfo[parm].files[0].name );
    }
}

extern void SimGetFileName( int parm, char *buff )
/************************************************/
{
    strcpy( buff, FileInfo[parm].filename );
}

extern long SimFileSize( int parm )
/*********************************/
{
    long        size;
    int         len;

    size = 0;
    len = FileInfo[parm].num_files;
    while( --len >= 0 ) {
        size += FileInfo[parm].files[len].size;
    }
    return( size );
}


extern long SimSubFileSize( int parm, int subfile )
/*************************************************/
{
    return( FileInfo[parm].files[subfile].size );
}


extern int SimFileDisk( int parm, char *buff )
/********************************************/
{
    strcpy( buff, DiskInfo[FileInfo[parm].disk_index].desc );
    return( FileInfo[parm].disk_index );
}

extern int SimFileDiskNum( int parm )
/***********************************/
{
    return( FileInfo[parm].disk_index );
}

void SimFileDir( int parm, char *buff, size_t buff_len )
/******************************************************/
{
    SimGetDir( FileInfo[parm].dir_index, buff, buff_len );
}

extern int SimFileDirNum( int parm )
/**********************************/
{
    return( FileInfo[parm].dir_index );
}

bool SimFileOldDir( int parm, char *buff, size_t buff_len )
/*********************************************************/
{
    if( FileInfo[parm].old_dir_index == -1 )
        return( false );
    SimGetDir( FileInfo[parm].old_dir_index, buff, buff_len );
    return( true );
}

extern bool SimFileSplit( int parm )
/**********************************/
{
    return( FileInfo[parm].file_type == '1' ||
            FileInfo[parm].file_type == 'm' ||
            FileInfo[parm].file_type == '$' );
}

static bool SimFileFirstSplit( int parm )
/***************************************/
{
    return( FileInfo[parm].file_type == '1' );
}

extern bool SimFileLastSplit( int parm )
/**************************************/
{
    return( FileInfo[parm].file_type == '$' );
}

extern int SimNumSubFiles( int parm )
/***********************************/
{
    return( FileInfo[parm].num_files );
}

extern bool SimSubFileInOldDir( int parm, int subfile )
/*****************************************************/
{
    return( FileInfo[parm].files[subfile].in_old_dir != 0 );
}

extern bool SimSubFileInNewDir( int parm, int subfile )
/*****************************************************/
{
    return( FileInfo[parm].files[subfile].in_new_dir != 0 );
}

extern bool SimSubFileReadOnly( int parm, int subfile )
/*****************************************************/
{
    return( FileInfo[parm].files[subfile].read_only != 0 );
}

extern bool SimSubFileExecutable( int parm, int subfile )
/*******************************************************/
{
    return( FileInfo[parm].files[subfile].executable );
}

extern bool SimSubFileIsNLM( int parm, int subfile )
/**************************************************/
{
    return( FileInfo[parm].files[subfile].is_nlm );
}

extern bool SimSubFileIsDLL( int parm, int subfile )
/**************************************************/
{
    return( FileInfo[parm].files[subfile].is_dll );
}

extern bool SimSubFileNewer( int parm, int subfile )
/**************************************************/
{
    return( FileInfo[parm].files[subfile].disk_date >
            FileInfo[parm].files[subfile].date );
}

extern time_t SimSubFileDate( int parm, int subfile )
/***************************************************/
{
    return( FileInfo[parm].files[subfile].date );
}

extern int SimSubFileExists( int parm, int subfile )
/**************************************************/
{
    return( SimSubFileInOldDir( parm, subfile ) || SimSubFileInNewDir( parm, subfile ) );
}

extern void SimSubFileName( int parm, int subfile, char *buff )
/*************************************************************/
{
    strcpy( buff, FileInfo[parm].files[subfile].name );
}

extern vhandle SimSubFileVar( int parm, int subfile )
/***************************************************/
{
    return( FileInfo[parm].files[subfile].dst_var );
}

extern bool SimFileUpToDate( int parm )
/*************************************/
{
    struct file_info    *info;
    int                 i;

    info = &FileInfo[parm];
    if( info->num_files == 0 )
        return( false );
    for( i = 0; i < info->num_files; ++i ) {
        if( !info->files[i].in_new_dir )
            return( false );
        if( info->files[i].disk_date > info->files[i].date )
            return( true );
        if( info->files[i].date > info->files[i].disk_date )
            return( false );
        if( RoundUp( info->files[i].disk_size, 512 ) != info->files[i].size )
            return( false );
    }
    return( true );
}


bool SimFileAdd( int parm )
/*************************/
{
    return( FileInfo[parm].add );
}

bool SimFileRemove( int parm )
/****************************/
{
    return( FileInfo[parm].remove );
}

/*
 * =======================================================================
 * API to PMInfo[]
 * =======================================================================
 */

void SimGetPMGroupFileName( char *buff, size_t buff_len )
/*******************************************************/
{
    if( SetupInfo.pm_group_file_name != NULL ) {
        strncpy( buff, SetupInfo.pm_group_file_name, buff_len - 1 );
        buff[buff_len - 1] = '\0';
    } else {
        buff[0] = '\0';
    }
}

void SimGetPMGroupIcon( char *buff, size_t buff_len )
/***************************************************/
{
    if( SetupInfo.pm_group_icon != NULL ) {
        strncpy( buff, SetupInfo.pm_group_icon, buff_len - 1 );
        buff[buff_len - 1] = '\0';
    } else {
        buff[0] = '\0';
    }
}

void SimGetPMGroup( char *buff, size_t buff_len )
/***********************************************/
{
    if( SetupInfo.pm_group_name != NULL ) {
        strncpy( buff, SetupInfo.pm_group_name, buff_len - 1 );
        buff[buff_len - 1] = '\0';
    } else {
        buff[0] = '\0';
    }
}

extern int SimGetNumPMProgs( void )
/*********************************/
{
    return( SetupInfo.pm_files.num );
}

static int SimFindDirForFile( char *buff )
/****************************************/
{
    int         i, j;

    for( i = 0; i < SetupInfo.files.num; i++ ) {
        for( j = 0; j < FileInfo[i].num_files; ++j ) {
            if( stricmp( buff, FileInfo[i].files[j].name ) == 0 ) {
                return( FileInfo[i].dir_index );
            }
        }
    }
    return( (int)SIM_INIT_ERROR );
}


extern int SimGetPMProgName( int parm, char *buff )
/*************************************************/
{
    strcpy( buff, PMInfo[parm].filename );
    // Return directory index.
    if( buff[0] == '+' ) {    // OS/2 shadow
        return( SimFindDirForFile( &buff[1] ) );
    } else {
        return( SimFindDirForFile( buff ) );
    }
}

void SimGetPMParms( int parm, char *buff, size_t buff_len )
/*********************************************************/
{
    strncpy( buff, PMInfo[parm].parameters, buff_len - 1 );
    buff[buff_len - 1] = '\0';
}

void SimGetPMDesc( int parm, char *buff, size_t buff_len )
/********************************************************/
{
    strncpy( buff, PMInfo[parm].desc, buff_len - 1 );
    buff[buff_len - 1] = '\0';
}

extern long SimGetPMIconInfo( int parm, char *buff, size_t buff_len )
/*******************************************************************/
{
    if( PMInfo[parm].icoioname == NULL ) {
        ReplaceVars( buff, buff_len, PMInfo[parm].filename );
    } else {
        strncpy( buff, PMInfo[parm].icoioname, buff_len - 1 );
        buff[buff_len - 1] = '\0';
    }
    return( (unsigned long)(unsigned short)SimFindDirForFile( buff ) | ((unsigned long)(unsigned short)PMInfo[parm].icon_pos ) << 16 );
}

extern bool SimCheckPMCondition( int parm )
/*****************************************/
{
    return( EvalCondition( PMInfo[parm].condition ) );
}

int SimGetNumPMGroups( void )
/***************************/
{
    return( SetupInfo.all_pm_groups.num );
}

void SimGetPMGroupName( int parm, char *buff, size_t buff_len )
/*************************************************************/
{
    strncpy( buff, AllPMGroups[parm].group, buff_len - 1 );
    buff[buff_len - 1] = '\0';
}

void SimGetPMGroupFName( int parm, char *buff, size_t buff_len )
/**************************************************************/
{
    strncpy( buff, AllPMGroups[parm].group_file_name, buff_len - 1 );
    buff[buff_len - 1] = '\0';
}

/*
 * =======================================================================
 * API to ProfileInfo[]
 * =======================================================================
 */

int SimNumProfile( void )
/***********************/
{
    return( SetupInfo.profile.num );
}

extern void SimProfInfo( int parm, char *app_name, char *key_name, char *value,
                         char *file_name, char *hive_name )
/*****************************************************************************/
{
    strcpy( app_name, ProfileInfo[parm].app_name );
    strcpy( key_name, ProfileInfo[parm].key_name );
    strcpy( value, ProfileInfo[parm].value );
    if( ProfileInfo[parm].file_name == NULL ) {
        file_name[0] = '\0';
    } else {
        strcpy( file_name, ProfileInfo[parm].file_name );
    }
    if( ProfileInfo[parm].hive_name == NULL ) {
        hive_name[0] = '\0';
    } else {
        strcpy( hive_name, ProfileInfo[parm].hive_name );
    }
}

extern bool SimCheckProfCondition( int parm )
/*******************************************/
{
    return( EvalCondition( ProfileInfo[parm].condition ) );
}

/*
 * =======================================================================
 * API to AutoExecInfo[] EnvironmentInfo[] ConfigInfo[]
 * =======================================================================
 */

static append_mode SimGetConfigStringsFrom( struct config_info *array, int i, 
                           const char **new_var, char *buff, size_t buff_len )
/****************************************************************************/
{
    append_mode append;
    char        *p;

    ReplaceVars( buff, buff_len, array[i].value );
    p = array[i].var;
    if( *p != '+' ) {
        append = AM_OVERWRITE;
    } else if( *p == '+' ) {
        ++p;
        if( *p == '+' ) {
            ++p;
            append = AM_BEFORE;
        } else {
            append = AM_AFTER;
        }
    }
    *new_var = p;
    return( append );
}

int SimNumAutoExec( void )
/************************/
{
    return( SetupInfo.autoexec.num );
}

extern append_mode SimGetAutoExecStrings( int i, const char **new_var, char *buff, size_t buff_len )
/**************************************************************************************************/
{
    return( SimGetConfigStringsFrom( AutoExecInfo, i, new_var, buff, buff_len ) );
}

extern bool SimCheckAutoExecCondition( int parm )
/***********************************************/
{
    return( EvalCondition( AutoExecInfo[parm].condition ) );
}

int SimNumConfig( void )
/**********************/
{
    return( SetupInfo.config.num );
}


extern append_mode SimGetConfigStrings( int i, const char **new_var, char *buff, size_t buff_len )
/************************************************************************************************/
{
    return( SimGetConfigStringsFrom( ConfigInfo, i, new_var, buff, buff_len ) );
}

extern bool SimCheckConfigCondition( int parm )
/*********************************************/
{
    return( EvalCondition( ConfigInfo[parm].condition ) );
}

int SimNumEnvironment( void )
/***************************/
{
    return( SetupInfo.environment.num );
}


extern append_mode SimGetEnvironmentStrings( int i, const char **new_var, char *buff, size_t buff_len )
/*****************************************************************************************************/
{
    return( SimGetConfigStringsFrom( EnvironmentInfo, i, new_var, buff, buff_len ) );
}

extern bool SimCheckEnvironmentCondition( int parm )
/**************************************************/
{
    return( EvalCondition( EnvironmentInfo[parm].condition ) );
}

/*
 * =======================================================================
 * API to AssociationInfo[]
 * =======================================================================
 */

int SimNumAssociations( void )
/****************************/
{
    return( SetupInfo.associations.num );
}

extern void SimGetAssociationExt( int parm, char *buff )
/******************************************************/
{
    strcpy( buff, AssociationInfo[parm].ext );
}

extern void SimGetAssociationKeyName( int parm, char *buff )
/**********************************************************/
{
    strcpy( buff, AssociationInfo[parm].keyname );
}

extern void SimGetAssociationProgram( int parm, char *buff )
/**********************************************************/
{
    strcpy( buff, AssociationInfo[parm].program );
}

extern void SimGetAssociationDescription( int parm, char *buff )
/**************************************************************/
{
    strcpy( buff, AssociationInfo[parm].description );
}

extern int SimGetAssociationIconIndex( int parm )
/***********************************************/
{
    return( AssociationInfo[parm].icon_index );
}

extern int SimGetAssociationNoOpen( int parm )
/********************************************/
{
    return( AssociationInfo[parm].no_open );
}

extern bool SimCheckAssociationCondition( int parm )
/**************************************************/
{
    return( EvalCondition( AssociationInfo[parm].condition ) );
}

/*
 * =======================================================================
 * API to LabelInfo[]
 * =======================================================================
 */

int SimNumLabels( void )
/**********************/
{
    return( SetupInfo.label.num );
}

extern void SimGetLabelDir( int parm, char *buff )
/************************************************/
{
    strcpy( buff, LabelInfo[parm].dir );
}

extern void SimGetLabelLabel( int parm, char *buff )
/************************************************/
{
    strcpy( buff, LabelInfo[parm].label );
}


/*
 * =======================================================================
 * API to UpgradeInfo[]
 * =======================================================================
 */

int SimNumUpgrades( void )
/************************/
{
    return( SetupInfo.upgrade.num );
}

extern char *SimGetUpgradeName( int parm )
/****************************************/
{
    return( UpgradeInfo[parm].name );
}

/*
 * =======================================================================
 *
 * =======================================================================
 */

extern char *SimGetTargetDriveLetter( int parm )
/**********************************************/
{
    char *buff;
    char temp[_MAX_PATH];

    buff = GUIMemAlloc( _MAX_PATH );
    if( buff == NULL ) {
        return( NULL );
    }

    SimTargetDir( parm, buff, _MAX_PATH );
    if( buff[0] == '\0' ) {
        getcwd( buff, _MAX_DIR );
    } else if( buff[0] != '\\' || buff[1] != '\\' ) {
        if( buff[0] == '\\' && buff[1] != '\\' ) {
            strcpy( temp, buff );
            getcwd( buff, _MAX_DIR );
            strcat( buff, temp );
        } else if( buff[1] != ':' ) {
            getcwd( buff, _MAX_DIR );
        }
    }
    return( buff );
}


static void MarkUsed( int dir_index )
/***********************************/
{
    int         parent;

    DirInfo[dir_index].used = true;
    parent = DirInfo[dir_index].parent;
    while( parent != -1 ) {
        DirInfo[parent].used = true;
        parent = DirInfo[parent].parent;
    }
}

#if defined ( __NT__ )
extern void CheckDLLCount( char *install_name )
/*********************************************/
{
    // Takes care of DLL usage counts in the Win95/WinNT registry;
    // removes DLLs if their usage count goes to zero and the user
    // agrees to delete them.
    int                 i;

    install_name=install_name;
    for( i = 0; i < SetupInfo.dlls_to_count.num; i++ ) {
        if( FileInfo[DLLsToCheck[i].index].core_component ) {
            continue;
        }
        if( VarGetIntVal( UnInstall ) ||
            FileInfo[DLLsToCheck[i].index].remove ||
            (!FileInfo[DLLsToCheck[i].index].add &&
            GetVariableIntVal( "ReInstall" ) != 0) ) {
            if( DecrementDLLUsageCount( DLLsToCheck[i].full_path ) == 0 ) {
                if( MsgBox( MainWnd, "IDS_REMOVE_DLL", GUI_YES_NO,
                            DLLsToCheck[i].full_path ) == GUI_RET_YES ) {
                    FileInfo[DLLsToCheck[i].index].add = false;
                    FileInfo[DLLsToCheck[i].index].remove = true;
                }
            }
        } else if( FileInfo[DLLsToCheck[i].index].add ) {
            IncrementDLLUsageCount( DLLsToCheck[i].full_path );
        }
    }
}

#endif

void SimCalcAddRemove( void )
/***************************/
{
    int                 i, j, k;
    int                 targ_index = 0;
    int                 dir_index;
    unsigned            cs; /* cluster size */
    bool                previous;
    bool                add;
    bool                uninstall;
    bool                remove;
    long                diskette;
    uint_64             tmp_size = 0;
    vhandle             reinstall;
#if defined( __NT__ )
    char                ext[_MAX_EXT];
#endif

    // for each file that will be installed, total the size
    diskette = strtol( GetVariableStrVal( "DisketteSize" ), NULL, 10 );
    if( NeedInitAutoSetValues ) {
        InitAutoSetValues();
    }

    previous = VarGetIntVal( PreviousInstall );
    uninstall = VarGetIntVal( UnInstall );
    // look for existence of ReInstall variable - use this to decide
    // if we should remove unchecked components (wanted for SQL installs)
    reinstall = GetVariableByName( "ReInstall" );
    if( reinstall != NO_VAR ) {
        // it is defined, treat same as PreviousInstall
        previous = VarGetIntVal( reinstall );
    }

    for( i = 0; i < SetupInfo.files.num; ++i ) {
        dir_index = FileInfo[i].dir_index;
        targ_index = DirInfo[dir_index].target;
        add = EvalExprTree( FileInfo[i].condition.p->cond,
                            VarGetIntVal( MinimalInstall ) != 0 );
        if( FileInfo[i].supplimental ) {
            remove = false;
            if( uninstall ) {
                add = false;
            }
        } else {
            if( uninstall && !FileInfo[i].core_component ) {
                add = false;
                remove = true;
            } else if( FileInfo[i].condition.p->dont_touch ) {
                add = false;
                remove = false;
            } else if( FileInfo[i].core_component ) {
                add = !SimSubFileExists( i, 0 );
                remove = false;
            } else {
                remove = !add && previous;
            }
        }
        if( add ) {
            MarkUsed( dir_index );
            if( SimFileFirstSplit( i ) ) {
                tmp_size = diskette;
            } else if( SimFileLastSplit( i ) ) {
                tmp_size += diskette;
                if( tmp_size > TargetInfo[targ_index].max_tmp_file ) {
                    TargetInfo[targ_index].max_tmp_file = tmp_size;
                }
            } else if( SimFileSplit( i ) ) {
                tmp_size += diskette;
            }
            DirInfo[dir_index].num_files += FileInfo[i].num_files;
        }
        TargetInfo[targ_index].num_files += FileInfo[i].num_files;
        cs = GetClusterSize( *TargetInfo[targ_index].temp_disk );
        FileInfo[i].remove = remove;
        FileInfo[i].add = add;
        for( k = 0; k < FileInfo[i].num_files; ++k ) {
            a_file_info *file = &FileInfo[i].files[k];
            if( file->size == 0 ) continue;
            if( file->disk_size != 0 ) {
                DirInfo[dir_index].num_existing++;
                if( !TargetInfo[targ_index].supplimental ) {
                    SetVariableByHandle( PreviousInstall, "1" );
                }
            }

#if defined( __NT__ )
            // if ( supplimental is_dll & ) then we want to
            // keep a usage count of this dll.  Store its full path for later.
            if( FileInfo[i].supplimental ) {
                _splitpath( file->name, NULL, NULL, NULL, ext );
                if( stricmp( ext, ".DLL" ) == 0 ) {
                    char                file_desc[MAXBUF], dir[_MAX_PATH], disk_desc[MAXBUF];
                    char                file_name[_MAX_FNAME + _MAX_EXT];
                    int                 disk_num;
                    char                dst_path[_MAX_PATH];
                    bool                flag;
                    int                 m;

                    SimFileDir( i, dir, sizeof( dir ) );
                    SimGetFileDesc( i, file_desc );
                    SimGetFileName( i, file_name );
                    disk_num = SimFileDisk( i, disk_desc );
                    _makepath( dst_path, NULL, dir, file_desc, NULL );

                    flag = false;
                    for( m = 0; m < SetupInfo.dlls_to_count.num; m++ ) {
                        if( stricmp( DLLsToCheck[m].full_path, dst_path ) == 0 ) {
                            flag = true;
                            break;
                        }
                    }
                    if( !flag ) {
                        bool    ok;

                        if( !BumpArray( &SetupInfo.dlls_to_count ) )
                            return;
                        DLLsToCheck[SetupInfo.dlls_to_count.num - 1].full_path = GUIStrDup( dst_path, &ok );
                        if( !ok )
                            return;
                        DLLsToCheck[SetupInfo.dlls_to_count.num - 1].index = i;
                    }
                }
            }
#endif

            if( add ) {
                TargetInfo[targ_index].space_needed += RoundUp( file->size, cs );
#if 0   // I don't think this logic is right...
                if( !file->is_nlm ) {
                    TargetInfo[targ_index].space_needed -= RoundUp( file->disk_size, cs );
                }
#else
                TargetInfo[targ_index].space_needed -= RoundUp( file->disk_size, cs );
#endif
                TargetInfo[targ_index].needs_update = true;
            } else if( remove ) {
                TargetInfo[targ_index].space_needed -= RoundUp( FileInfo[i].files[k].disk_size, cs );
                TargetInfo[targ_index].needs_update = true;
            }
        }
    }
    /* Estimate space used for directories. Be generous. */
    if( !uninstall ) {
        for( i = 0; i < SetupInfo.target.num; ++i ) {
            cs = GetClusterSize( *TargetInfo[targ_index].temp_disk );
            for( j = 0; j < SetupInfo.dirs.num; ++j ) {
                if( DirInfo[j].target != i )
                    continue;
                if( !DirInfo[j].used )
                    continue;
                if( DirInfo[j].num_files <= DirInfo[j].num_existing )
                    continue;
                TargetInfo[i].space_needed += RoundUp( (((DirInfo[j].num_files -
                    DirInfo[j].num_existing) / 10) + 1) *1024UL, cs);
            }
        }
    }
}


bool SimCalcTargetSpaceNeeded( void )
/***********************************/
{
    int                 i;
    void                *cursor;
    char                *temp;

    /* assume power of 2 */

    if( NeedGetDiskSizes ) {
        if( !GetDiskSizes() )
            return( false );
        NeedGetDiskSizes = false;
    }
    cursor = GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
    for( i = 0; i < SetupInfo.target.num; ++i ) {
        temp = SimGetTargetDriveLetter( i );
        if( temp == NULL )
            return( false );
        strcpy( TargetInfo[i].temp_disk, temp );
        GUIMemFree( temp );
        TargetInfo[i].space_needed = 0;
        TargetInfo[i].max_tmp_file = 0;
        TargetInfo[i].num_files = 0;
        TargetInfo[i].needs_update = false;
    }
    for( i = 0; i < SetupInfo.dirs.num; ++i ) {
        DirInfo[i].used = false;
        DirInfo[i].num_existing = 0;
        DirInfo[i].num_files = 0;
    }
    SimCalcAddRemove();
    GUIResetMouseCursor( cursor );
    return( true );
}


#ifdef PATCH
static void AddFileName( int i, char *buffer, int rename )
/********************************************************/
{
    ConcatDirSep( buffer );
    if( !rename ) {
        if( PatchInfo[i].destfile ) {
            strcat( buffer, PatchInfo[i].destfile );
        } else {
            if( PatchInfo[i].srcfile ) {
                strcat( buffer, PatchInfo[i].srcfile );
            }
        }
    }
}


static void GetSourcePath( int i, char *buff, size_t buff_len )
/*************************************************************/
{
    ReplaceVars( buff, buff_len, GetVariableStrVal( "Srcdir" ) );
    strcat( buff, PatchInfo[i].srcfile );
}


static bool CopyErrorDialog( int ret, int i, char *file )
/*******************************************************/
{
    gui_message_return      guiret;

    i = i;
    if( ret != CFE_NOERROR ) {
        if( ret != CFE_ABORT ) {
            guiret = MsgBox( NULL, "IDS_COPYFILEERROR", GUI_YES_NO, file );
            if( guiret == GUI_RET_NO ) {
                return( false );
            }
        } else {
            return( false );
        }
    }
    return( true );
}


static bool PatchErrorDialog( PATCH_RET_CODE ret, int i )
/*******************************************************/
{
    gui_message_return      guiret;

    if( ret != PATCH_RET_OKAY && ret != PATCH_CANT_FIND_PATCH ) {
        if( ret != PATCH_RET_CANCEL ) {
            // error, attempt to continue patch process
            guiret = MsgBox ( NULL, "IDS_PATCHFILEERROR", GUI_YES_NO, PatchInfo[i].srcfile );
            if( guiret == GUI_RET_NO ) {
                return( false );
            }
        } else {
            return( false );
        }
    }
    return( true );
}


static bool FindStr( FILE *fp, char *fullpath, char *pattern )
/************************************************************/
{
    char            *buff;
    size_t          len;
    size_t          readsize;
    char            *p;
    size_t          i;
    bool            found;
    size_t          patternlen;

    patternlen = strlen( pattern );
    found = false;

    buff = NULL;
    for( readsize = 8 * 1024; readsize > 0; readsize = readsize / 2 ) {
        buff = malloc( readsize + patternlen );
        if( buff != NULL ) {
            break;
        }
    }
    if( readsize == 0 ) {
        free( buff );
        return( false );
    }
    memset( buff, 0, patternlen );
    while( !found && !feof( fp ) ) {
        len = fread( &buff[patternlen], 1, readsize, fp );
        for( p = buff, i = 0; i < len; ++i, ++p ) {
            if( *p == pattern[0] && memcmp( p, pattern, patternlen ) == 0 ) {
                found = true;
                break;
            }
        }
        if( len > patternlen ) {
            memcpy( buff,  &buff[len], patternlen );
        }
    }
    if( found ) {
        fseek( fp,  -((long)len + patternlen - i), SEEK_CUR );
        free( buff );
        return( true );
    }
    free( buff );
    return( false );
}


bool ReadBlock( char *fullpath, char *pattern, void *block, long blocklen )
/*************************************************************************/
{
    FILE            *fp;
    int             len;
    struct stat     statbuf;

    if( stat( fullpath, &statbuf ) != 0 ) {
        // Cannot open file
        return( false );
    }
    fp = fopen( fullpath, "rb" );
    if( fp == NULL ) {
        return( false );
    }
    if( FindStr( fp, fullpath, pattern ) ) {
        len = fread( block, 1, blocklen, fp );
        if( len != blocklen ) {
            fclose( fp );
            return( false );
        }
        if( fclose( fp ) != 0 ) {
            return( false );
        }
        return( true );
    }
    fclose( fp );
    return( false );
}


bool WriteBlock( char *fullpath, char *pattern, void *block, long blocklen )
/**************************************************************************/
{
    bool            foundstr;
    FILE            *fp;
    int             len;
    struct stat     statbuf;
    struct utimbuf  utimbuf;

    if( stat( fullpath, &statbuf ) != 0 ) {
        // Cannot open file
        return( false );
    }
    utimbuf.actime = statbuf.st_atime;
    utimbuf.modtime = statbuf.st_mtime;
    fp = fopen( fullpath, "rb+" );
    if( fp == NULL ) {
        return( false );
    }

    foundstr = false;

    //there may be more than one block
    while( FindStr( fp, fullpath, pattern ) ) {
        len = fwrite( block, 1, blocklen, fp );
        fflush( fp );
        if( len != blocklen ) {
            fclose( fp );
            return( false );
        }
        foundstr = true;
    }
    fclose( fp );
    utime( fullpath, &utimbuf );

    return( foundstr );
}


typedef struct {
    FILE    *log_file;
    bool    do_log;
} log_state;


static FILE *LogFileOpen( void )
/******************************/
{
    gui_message_return guiret;
    FILE               *logfp;
    const char         *patchlog;

    patchlog = GetVariableStrVal( "PatchLog" );
    if( patchlog == NULL || patchlog[0] == '\0' ) {
        return( NULL );
    }

    if( access( patchlog, F_OK | W_OK | R_OK ) == 0 ) {
        guiret = MsgBox( NULL, "IDS_LOGFILE_EXISTS", GUI_YES_NO, patchlog );
        if( guiret == GUI_RET_NO ) {
            return( NULL );
        }
    } else if( access( patchlog, F_OK ) == 0 ) {
        MsgBox( NULL, "IDS_CANT_OPEN_LOGFILE", GUI_OK, patchlog );
        return( NULL );
    }
    remove( patchlog );
    logfp = fopen( patchlog, "wt+" );
    if( logfp == NULL ) {
        MsgBox( NULL, "IDS_CANT_OPEN_LOGFILE", GUI_OK, patchlog );
    }

    return( logfp );
}


static void LogFileClose( log_state *ls )
/***************************************/
{
    if( ls->do_log && (fclose( ls->log_file ) != 0) ) {
        MsgBox( NULL, "IDS_CANT_WRITE_LOGFILE", GUI_OK,
                GetVariableStrVal( "PatchLog" ) );
    }
    return;
}

#define GetVariableMsgVal GetVariableStrVal

static void LogWriteMsg( log_state *ls, const char *msg_id )
/**********************************************************/
{
    if( ls->do_log ) {
        fprintf( ls->log_file, "%s\n", GetVariableMsgVal( msg_id ) );
    }
}


static void LogWriteMsgStr( log_state *ls, const char *msg_id, const char *str )
/******************************************************************************/
{
    if( ls->do_log ) {
        fprintf( ls->log_file, GetVariableMsgVal( msg_id ), str );
    }
}


static int DoPatchFile( const char *src, char *dst, unsigned_32 flag )
/********************************************************************/
{
    // TODO: Perform some useful function here
     
    return( DoPatch( src, 0, 0, 0, dst ) );
}


extern bool PatchFiles( void )
/****************************/
{
    // this function performs the operations normally done in a batch file
    // (like applyd.bat).  Operations are:  patch file, copy (create) file,
    // and delete file.  Commands are set in [Patch] section of .INF file/
    // "Patch" in [Application] section must be set to 1 for this function to
    // be called.  Setting "Patch" to 1 overrides any regular setup (ie.
    // only this function will be called )

    unsigned            i;
    char                destfullpath[_MAX_PATH];
    char                srcfullpath[_MAX_PATH];
    gui_message_return  guiret;
    int                 count;  // count successful patches
    const char          *appname;
    int                 Index;  // used in secondary search during patch
    bool                go_ahead;
    char                exetype[3];
    log_state           logstate;
    log_state           *log;


    // note:  Up until this point, PatchInfo[x].destdir contains an
    //        integer string representing the directory defined in
    //        the [dirs] section on the .INF file.  This function will replace
    //        the .destdir member with the actual (full) directory path.
    //        The conversion is done here (instead of in initialization) so
    //        that that user can change the DstDir before the patch process
    //        begins.

    count = 0;
    log = &logstate;

    if( GetVariableIntVal( "DoPatchLog" ) ) {
        log->log_file = LogFileOpen();
        if( log->log_file == NULL ) {
            MsgBox( NULL, "IDS_PATCHABORT", GUI_OK );
            return( false );
        }
        log->do_log = true;
        appname = GetVariableStrVal( "AppName" );
        fprintf( log->log_file, "%s\n\n", appname );
    } else {
        log->log_file = NULL;
        log->do_log = false;
    }

    for( i = 0; i < SetupInfo.patch_files.num; i++, Index = -1 ) {
        destfullpath[0] = srcfullpath[0] = '\0';
        if( !EvalCondition( PatchInfo[i].condition ) ) {
            StatusAmount( i + 1, SetupInfo.patch_files.num );
            continue;
        }
        switch( PatchInfo[i].command ) {

        case PATCH_FILE:
            Index = i;              // used in secondary search
            GetSourcePath( i, srcfullpath, sizeof( srcfullpath ) );

            if( access( srcfullpath, R_OK ) == 0 ) {
                GetDestDir( i, destfullpath, sizeof( destfullpath ) );
                go_ahead = SecondaryPatchSearch( PatchInfo[i].destfile, destfullpath, Index );
                if( go_ahead ) {
                    if( PatchInfo[i].exetype[0] != '.' &&
                        ExeType( destfullpath, exetype ) &&
                        strcmp( exetype, PatchInfo[i].exetype ) != 0 ) {
                        go_ahead = false;
                    }
                }

                if( go_ahead ) {
                    PATCH_RET_CODE ret;

                    StatusLines( STAT_PATCHFILE, destfullpath );
                    StatusShow( true );
                    LogWriteMsgStr( log, "IDS_UNPACKING", destfullpath );
                    ret = DoPatchFile( srcfullpath, destfullpath, 0 );
                    if( ret == PATCH_RET_OKAY ) {
                        ++count;
                        LogWriteMsg( log, "IDS_SUCCESS" );
                        break;
                    } else {
                        LogWriteMsg( log, "IDS_FAILED_UNPACKING" );
                        if( !PatchErrorDialog( ret, i ) ) {
                            LogWriteMsg( log, "IDS_PATCHABORT" );
                            LogFileClose( log );
                            return( false );
                        }
                    }
                }
            }
            break;

        case PATCH_COPY_FILE:
            GetSourcePath( i, srcfullpath, sizeof( srcfullpath ) );
            GetDestDir( i, destfullpath, sizeof( destfullpath ) );

            // get rid of trailing slash: OS/2 needs this for access(...) to work
            if( destfullpath[strlen( destfullpath ) - 1] == '\\' ) {
                destfullpath[strlen( destfullpath ) - 1] = '\0';
            }

            if( access( destfullpath, F_OK ) == 0 ) {
                AddFileName( i, destfullpath, 0 );
                StatusLines( STAT_CREATEFILE, destfullpath );
                StatusShow( true );
                if( access( srcfullpath, R_OK ) == 0 ) {
                    LogWriteMsgStr( log, "IDS_UNPACKING", destfullpath );
                    if( DoCopyFile( srcfullpath, destfullpath, false ) == CFE_NOERROR ) {
                        ++count;
                        LogWriteMsg( log, "IDS_SUCCESS" );
                        break;
                    }
                    LogWriteMsg( log, "IDS_FAILED_UNPACKING" );
                    if( !CopyErrorDialog( CFE_ERROR, i, srcfullpath ) ) {
                        LogWriteMsg( log, "IDS_PATCHABORT" );
                        LogFileClose( log );
                        return( false );
                    }
                }
            }
            break;

        case PATCH_DELETE_FILE:
            GetDestDir( i, destfullpath, sizeof( destfullpath ) );
            AddFileName( i, destfullpath, 0 );
            StatusLines( STAT_DELETEFILE, destfullpath );
            StatusShow( true );
            if( access( destfullpath, F_OK | W_OK ) == 0 ) {
                LogWriteMsgStr( log, "IDS_DELETING", destfullpath );
                if( DoDeleteFile( destfullpath ) ) {
                    ++count;
                    LogWriteMsg( log, "IDS_SUCCESS" );
                } else {
                    LogWriteMsg( log, "IDS_FAILED_DELETING" );
                    guiret = MsgBox( NULL, "IDS_DELETEFILEERROR", GUI_YES_NO,
                                     destfullpath );
                    if( guiret == GUI_RET_NO ) {
                        LogWriteMsg( log, "IDS_PATCHABORT" );
                        LogFileClose( log );
                        return( false );
                    }
                }
            }
            break;

        case PATCH_MAKE_DIR:
            ReplaceVars( destfullpath, sizeof( destfullpath ), PatchInfo[i].destdir );

            StatusLines( STAT_CREATEDIRECTORY, destfullpath );
            StatusShow( true );
            if( access( destfullpath, F_OK ) != 0 ) {
                LogWriteMsgStr( log, "IDS_CREATINGDIR", destfullpath );
                if( mkdir( destfullpath ) == 0 ) {
                    LogWriteMsg( log, "IDS_SUCCESS" );
                } else {
                    guiret = MsgBox( NULL, "IDS_CREATEDIRERROR", GUI_YES_NO, destfullpath );
                    if( guiret == GUI_RET_NO ) {
                        LogWriteMsg( log, "IDS_FAILED_CREATINGDIR" );
                        LogFileClose( log );
                        return( false );
                    }
                }
            }
            break;

        default:
            /* Something went wrong, but what can we do about it now? */
            break;
        }

        StatusAmount( i + 1, SetupInfo.patch_files.num );
        if( StatusCancelled() ) {
            LogWriteMsg( log, "IDS_PATCHABORT" );
            LogFileClose( log );
            return( false );
        }
    }
    StatusCancelled(); /* make sure display gets updated */

    if( count == 0 ) {
        LogWriteMsg( log, "IDS_NO_FILES_PATCHED" );
    }
    LogFileClose( log );

    if( count == 0 ) {
        // no files patched successfully
        MsgBox( NULL, "IDS_NO_FILES_PATCHED", GUI_OK );
        return( false );
    }
    return( true );
}


void MsgPut( int resourceid, va_list arglist )
{
    const char  *msgbuf;
    char        *messageid;
    char        *argbuf[3];
    int         i;

    for( i = 0; i < 3; i++ ) {
        argbuf[i] = va_arg( arglist, char * );
    }
    switch( resourceid ) {
#if !defined( __UNIX__ )
    case ERR_TWO_NAMES:
        messageid = "IDS_TWONAMES";
        break;
    case ERR_WRONG_SIZE:
        messageid = "IDS_BADLENGTH";
        break;
    case ERR_CANT_RENAME:
        messageid = "IDS_CANTRENAME";
        break;
    case ERR_WRONG_CHECKSUM:
        messageid = "IDS_WRONGCHECKSUM";
        break;
    case ERR_PATCH_ABORTED:
        messageid = "IDS_PATCHABORT";
        break;
    case ERR_NOT_PATCHFILE:
        messageid = "IDS_NOTPATCHFILE";
        break;
    case ERR_BAD_PATCHFILE:
        messageid = "IDS_BADPATCHFILE";
        break;
    case ERR_CANT_FIND:
        messageid = "IDS_CANTOPENSRC";
        break;
    case ERR_CANT_READ:
        messageid = "IDS_CANTREADFILE";
        break;
    case ERR_CANT_WRITE:
        messageid = "IDS_CANTWRITE";
        break;
    case ERR_CANT_OPEN:
        messageid = "IDS_CANTFINDOUTPUT";
        break;
    case ERR_IO_ERROR:
        messageid = "IDS_IOERROR";
        break;
    case ERR_MEMORY_OUT:
        messageid = "IDS_NOMEMORY";
        break;
    case ERR_CANT_GET_ATTRIBUTES:
        messageid = "IDS_NOATTRIBUTES";
        break;
#endif
    default:
        messageid = "IDS_ERROR";
    }
    msgbuf = GetVariableStrVal( messageid );
    MsgBox( NULL, messageid, GUI_OK, argbuf[0], argbuf[1], argbuf[2] );
}


void PatchError( int format, ... )
{
    va_list     args;

    // don't give error message if the patch file cant be found
    // just continue
#if !defined( __UNIX__ )
    if( format == ERR_CANT_FIND )
        return;
#endif
    if( GetVariableIntVal( "Debug" ) != 0 ) {
        va_start( args, format );
        MsgPut( format, args );
        va_end( args );
    }
}


void FilePatchError( int format, ... )
{
    va_list     args;

#if !defined( __UNIX__ )
    if( format == ERR_CANT_FIND )
        return;
    if( format == ERR_CANT_OPEN )
        return;
#endif
    va_start( args, format );
    MsgPut( format, args );
    va_end( args );
}
#endif


/* ********** Free up all structures associated with this file ******* */


static void FreeSetupInfoVal( void )
/**********************************/
{
    GUIMemFree( SetupInfo.pm_group_file_name );
    GUIMemFree( SetupInfo.pm_group_name );
    GUIMemFree( SetupInfo.pm_group_icon );
}


static void FreeTargetVal( void )
/*******************************/
{
    int i;

    for( i = 0; i < SetupInfo.target.num; i++ ) {
        GUIMemFree( TargetInfo[i].name );
        GUIMemFree( TargetInfo[i].temp_disk );
    }
    GUIMemFree( TargetInfo );
}


static void FreeDiskInfo( void )
/******************************/
{
    int i;
    for( i = 0; i < SetupInfo.disks.num; i++ ) {
        GUIMemFree( DiskInfo[i].desc );
    }
    GUIMemFree( DiskInfo );
}


static void FreeDirInfo( void )
/*****************************/
{
    int i;
    for( i = 0; i < SetupInfo.dirs.num; i++ ) {
        GUIMemFree( DirInfo[i].desc );
    }
    GUIMemFree( DirInfo );
}


static void FreeFileInfo( void )
/******************************/
{
    int i;
    int j;

    if( FileInfo != NULL ) {
        for( i = 0; i < SetupInfo.files.num; i++ ) {
            GUIMemFree( FileInfo[i].filename );
            for( j = 0; j < FileInfo[i].num_files; ++j ) {
                GUIMemFree( FileInfo[i].files[j].name );
            }
            GUIMemFree( FileInfo[i].files );
        }
        GUIMemFree( FileInfo );
    }
}

static void FreeDLLsToCheck( void )
/*********************************/
{
    int                 i;

    if( DLLsToCheck != NULL ) {
        for( i = 0; i < SetupInfo.dlls_to_count.num; i++ ) {
            GUIMemFree( DLLsToCheck[i].full_path );
        }
        GUIMemFree( DLLsToCheck );
        DLLsToCheck = NULL;
    }
}

static void FreeFileCondInfo( void )
/**********************************/
{
    int i;

    if( FileCondInfo != NULL ) {
        for( i = 0; i < SetupInfo.fileconds.num; i++ ) {
            BurnTree( FileCondInfo[i].cond );
        }
        GUIMemFree( FileCondInfo );
        FileCondInfo = NULL;
    }
}

static void FreeForceDLLInstall( void )
/*************************************/
{
    int i;

    if( ForceDLLInstall != NULL ) {
        for( i = 0; i < SetupInfo.force_DLL_install.num; i++ ) {
            GUIMemFree( ForceDLLInstall[i].name );
        }
        GUIMemFree( ForceDLLInstall );
        ForceDLLInstall = NULL;
    }
}

#ifdef PATCH
static void FreePatchInfo( void )
/*******************************/
{
    int i;

    for( i = 0; i < SetupInfo.patch_files.num; i++ ) {
        GUIMemFree( PatchInfo[i].destdir );
        GUIMemFree( PatchInfo[i].destfile );
        GUIMemFree( PatchInfo[i].srcfile );
        GUIMemFree( PatchInfo[i].condition );
        GUIMemFree( PatchInfo[i].exetype );
    }
    GUIMemFree( PatchInfo );
}
#endif


static void FreeSpawnInfo( void )
/*******************************/
{
    int i;

    for( i = 0; i < SetupInfo.spawn.num; i++ ) {
        GUIMemFree( SpawnInfo[i].command );
        GUIMemFree( SpawnInfo[i].condition );
    }
    GUIMemFree( SpawnInfo );
}


static void FreeDeleteInfo( void )
/*******************************/
{
    int i;

    for( i = 0; i < SetupInfo.delete.num; i++ ) {
        GUIMemFree( DeleteInfo[i].name );
    }
    GUIMemFree( DeleteInfo );
}


static void FreePMInfo( void )
/****************************/
{
    int i;

    for( i = 0; i < SetupInfo.pm_files.num; i++ ) {
        GUIMemFree( PMInfo[i].desc );
        GUIMemFree( PMInfo[i].filename );
        GUIMemFree( PMInfo[i].parameters );
        GUIMemFree( PMInfo[i].icoioname );
        GUIMemFree( PMInfo[i].condition );
    }
    GUIMemFree( PMInfo );
}


static void FreeProfileInfo( void )
/*********************************/
{
    int i;

    for( i = 0; i < SetupInfo.profile.num; i++ ) {
        GUIMemFree( ProfileInfo[i].hive_name );
        GUIMemFree( ProfileInfo[i].app_name );
        GUIMemFree( ProfileInfo[i].key_name );
        GUIMemFree( ProfileInfo[i].value );
        GUIMemFree( ProfileInfo[i].file_name );
        GUIMemFree( ProfileInfo[i].condition );
    }
    GUIMemFree( ProfileInfo );
}


static void FreeOneConfigInfo( array_info *info, struct config_info *array )
/**************************************************************************/
{
    size_t      i;

    for( i = 0; i < info->num; i++ ) {
        GUIMemFree( array[i].var );
        GUIMemFree( array[i].value );
        GUIMemFree( array[i].condition );
    }
    GUIMemFree( array );
}


static void FreeUpgradeInfo( void )
/*********************************/
{
    int i;

    if( UpgradeInfo != NULL ) {
        for( i = 0; i < SetupInfo.upgrade.num; i++ ) {
            GUIMemFree( UpgradeInfo[i].name );
        }
        GUIMemFree( UpgradeInfo );
        UpgradeInfo = NULL;
    }
}

static void FreeLabelInfo( void )
/*******************************/
{
    int i;

    if( LabelInfo != NULL ) {
        for( i = 0; i < SetupInfo.label.num; i++ ) {
            GUIMemFree( LabelInfo[i].dir );
            GUIMemFree( LabelInfo[i].label );
        }
        GUIMemFree( LabelInfo );
        LabelInfo = NULL;
    }
}

static void FreeAllPMGroups( void )
/*********************************/
{
    int i;

    if( AllPMGroups != NULL ) {
        for( i = 0; i < SetupInfo.all_pm_groups.num; i++ ) {
            GUIMemFree( AllPMGroups[i].group );
            GUIMemFree( AllPMGroups[i].group_file_name );
        }
        GUIMemFree( AllPMGroups );
        AllPMGroups = NULL;
    }
}

static void FreeAssociationInfo( void )
/*************************************/
{
    int i;

    if( AssociationInfo != NULL ) {
        for( i = 0; i < SetupInfo.associations.num; i++ ) {
            GUIMemFree( AssociationInfo[i].ext );
            GUIMemFree( AssociationInfo[i].keyname );
            GUIMemFree( AssociationInfo[i].program );
            GUIMemFree( AssociationInfo[i].description );
            GUIMemFree( AssociationInfo[i].condition );
        }
        GUIMemFree( AssociationInfo );
        AssociationInfo = NULL;
    }
}

extern void FreeAllStructs( void )
/********************************/
{
    FreeTargetVal();
    FreeDiskInfo();
    FreeDirInfo();
    FreeFileInfo();
    FreeFileCondInfo();
#ifdef PATCH
    FreePatchInfo();
#endif
    FreePMInfo();
    FreeProfileInfo();
    FreeOneConfigInfo( &SetupInfo.environment, EnvironmentInfo );
    FreeOneConfigInfo( &SetupInfo.autoexec, AutoExecInfo );
    FreeOneConfigInfo( &SetupInfo.config, ConfigInfo );
    FreeSetupInfoVal();
    FreeSpawnInfo();
    FreeDeleteInfo();
    FreeDLLsToCheck();
    FreeForceDLLInstall();
    FreeUpgradeInfo();
    FreeLabelInfo();
    FreeAllPMGroups();
    FreeAssociationInfo();
}


void SimGetSpawnCommand( char *buff, size_t buff_len, int i )
/***********************************************************/
{
    buff[0] = '\0';
    if( SpawnInfo[i].command == NULL || SpawnInfo[i].command[0] == '\0' )
        return;
    ReplaceVars( buff, buff_len, SpawnInfo[i].command );
}

extern bool SimEvalSpawnCondition( int i )
/****************************************/
{
    return( EvalCondition( SpawnInfo[i].condition ) );
}

extern when_time SimWhen( int i )
/*******************************/
{
    return( SpawnInfo[i].when );
}

int SimNumSpawns( void )
/**********************/
{
    return( SetupInfo.spawn.num );
}

static void ZeroAutoSetValues( void )
/***********************************/
{
    vhandle     var_handle;

    var_handle = NextGlobalVar( NO_VAR );
    while( var_handle != NO_VAR ) {
        if( VarGetAutoSetCond( var_handle ) != NULL ) {
            if( VarIsRestrictedTrue( var_handle ) ) {
                SetVariableByHandle( var_handle, "1" );
            } else {
                SetVariableByHandle( var_handle, "0" );
            }
        }
        var_handle = NextGlobalVar( var_handle );
    }
}


void SetDefaultAutoSetValue( vhandle var_handle )
/***********************************************/
{
    const char      *cond;

    cond = VarGetAutoSetCond( var_handle );
    if( cond != NULL ) {
        if( EvalCondition( cond ) ) {
            SetVariableByHandle( var_handle, "1" );
        } else {
            SetVariableByHandle( var_handle, "0" );
        }
    }
}

static void InitAutoSetValues( void )
/***********************************/
{
    vhandle     var_handle;

    var_handle = NextGlobalVar( NO_VAR );
    while( var_handle != NO_VAR ) {
        SetDefaultAutoSetValue( var_handle );
        var_handle = NextGlobalVar( var_handle );
    }
    NeedInitAutoSetValues = false;
}


static char *CompileCondition( const char *str )
/**********************************************/
// turn token names in an expression into #<var_handle>
{
    char        buff[MAXBUF];
    vhandle     var_handle;
    char        *str2;
    char        *token;

    if( str == NULL || IS_EMPTY( str ) ) {
        return( NULL );
    }
    var_handle = NO_VAR;
    buff[0] = '\0';
    str2 = GUIStrDup( str, NULL );  // copy string so we can use STRTOK
    token = strtok( str2, " " );
    while( token != NULL ) {
        switch( token[0] ) {
        case '|':
        case '&':
        case '+':
        case '!':
        case '?':
            strcat( buff, token );
            strcat( buff, " " );
            break;
        default:
            strcat( buff, "#" );
            var_handle = AddVariable( token );
            itoa( var_handle, buff + strlen( buff ), 10 );
            strcat( buff, " " );
        }
        token = strtok( NULL, " " );
    }
    GUIMemFree( str2 );
    return( GUIStrDup( buff, NULL ) );
}

vhandle MakeDummyVar( void )
/**************************/
{
    static unsigned int counter;
    char                buffer[DUMMY_VAR_SIZE];

    sprintf( buffer, "DUMMY_VAR_%u", counter );
    counter++;
    return( AddVariable( buffer ) );
}
