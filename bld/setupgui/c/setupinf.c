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
* Description:  Read and parse setup.inf file, create dialogs if necessary.
*               Also initialize other variables.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#ifndef __UNIX__
    #include <direct.h>
#endif
#include "wio.h"
#include "setup.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "guiutil.h"
#include "guistats.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "genctrl.h"
#include "dlggen.h"
#include "utils.h"
#include "setupio.h"
#include "iopath.h"
#include "watcom.h"
#include "dynarray.h"

#include "clibext.h"


#define GET36( x )          strtol( x, NULL, 36 )

#define IS_EMPTY(p)         ((p)[0] == '\0' || (p)[0] == '.' && (p)[1] == '\0')

#define IS_WS(c)            ((c) == ' ' || (c) == '\t')
#define SKIP_WS(p)          while(IS_WS(*(p))) (p)++

#define RoundUp( v, r )     (((v) + (r) - 1) & ~(unsigned long)((r)-1))

#define BUF_SIZE            8192

#define MAX_WINDOW_WIDTH    90

#define TreeNodeUni(op)     TreeNode(op, NULL, NULL)

#define NONMAGICVARS( x, y ) \
    x( IsDos, y ) \
    x( IsOS2, y ) \
    x( IsOS2DosBox, y ) \
    x( IsWin16, y ) \
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
    x( IsAlpha, y )

typedef struct a_file_info {
    VBUF                name;
    vhandle             dst_var;
    unsigned long       disk_size;
    unsigned long       disk_date;
    unsigned long       size;
    unsigned long       date;
    boolbit             in_old_dir  : 1;
    boolbit             in_new_dir  : 1;
    boolbit             read_only   : 1;
    boolbit             is_nlm      : 1;
    boolbit             is_dll      : 1;
    boolbit             executable  : 1;
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
    struct {
        union {
            struct tree_node    *node;
            vhandle             var;
            char                *str;
        } u;
    } left;
    struct tree_node        *right;
    tree_op                 op;
} tree_node;

typedef struct file_cond_info {
    tree_node   *cond;
    bool        one_uptodate;
    bool        dont_touch;
} file_cond_info;

typedef enum {
    DELETE_DIALOG,
    DELETE_FILE,
    DELETE_DIR,
} delete_type;

typedef enum {
    RS_UNDEFINED,
    RS_APPLICATION,
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
    RS_AUTOSET,
    RS_SPAWN,
    RS_RESTRICTIONS,
    RS_DELETEFILES,
    RS_FORCEDLLINSTALL,
    RS_ASSOCIATIONS
} read_state;

typedef struct dialog_parser_info { // structure used when parsing a dialog
    array_info          controls_array;
    array_info          controls_ext_array;
    int                 num_push_buttons;
    int                 num_variables;
    int                 num_radio_buttons;
    int                 max_width;
    int                 wrap_width;
    a_dialog_header     *curr_dialog;
    int                 row_num;
    int                 col_num;
} DIALOG_PARSER_INFO;

#define defvar( x, y ) vhandle x;
MAGICVARS( defvar, 0 )
NONMAGICVARS( defvar, 0 )
#undef defvar

static struct setup_info {
    unsigned long       stamp;
    char                *pm_group_file;
    char                *pm_group_name;
    char                *pm_group_iconfile;
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
    array_info          spawn;
    array_info          delete;
    array_info          fileconds;
    array_info          dlls_to_count;
    array_info          force_DLL_install;
    array_info          all_pm_groups;
    array_info          associations;
} SetupInfo;

static struct dir_info {
    char                *desc;
    int                 target;
    int                 parent;
    int                 num_files;
    int                 num_existing;
    boolbit             used            : 1;
} *DirInfo = NULL;


static struct target_info {
    char                *name;
    disk_ssize          space_needed;
    int                 num_files;
    char                *temp_disk;
    boolbit             supplemental    : 1;
    boolbit             needs_update    : 1;
} *TargetInfo = NULL;

static struct label_info {
    char                *dir;
    char                *label;
} *LabelInfo = NULL;

static struct upgrade_info {
    char                *name;
} *UpgradeInfo = NULL;

static struct association_info {
    char                *ext;
    char                *keyname;
    char                *description;
    char                *program;
    char                *condition;
    char                *iconfile;
    int                 iconindex;
} *AssociationInfo = NULL;

static file_cond_info   *FileCondInfo = NULL;

static struct file_info {
    char                *filename;
    int                 dir_index;
    int                 old_dir_index;
    int                 num_files;
    a_file_info         *files;
    union {
        file_cond_info  *p;
        int             i;
    } condition;
    boolbit             add             : 1;
    boolbit             remove          : 1;
    boolbit             supplemental    : 1;
    boolbit             core_component  : 1;
} *FileInfo = NULL;

static struct pm_info {
    char                *filename;
    char                *parameters;
    char                *desc;
    char                *iconfile;
    int                 iconindex;
    char                *condition;
    boolbit             group           : 1;
    boolbit             shadow          : 1;
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
    VBUF                full_path;
} *DLLsToCheck = NULL;

static struct force_DLL_install {
    char                *name;
} *ForceDLLInstall = NULL;

static struct all_pm_groups {
    char                *group_name;
    char                *group_file;
} *AllPMGroups = NULL;

static read_state       State;
static size_t           NoLineCount;
static size_t           *LineCountPointer = &NoLineCount;
static bool             NeedGetDiskSizes = false;
static bool             NeedInitAutoSetValues = true;
static char             *ReadBuf;
static size_t           ReadBufSize;
static char             *RawReadBuf;
static char             *RawBufPos;
static int              MaxWidthChars;
static int              CharWidth;

static vhandle GetTokenHandle( const char *p );
static void ZeroAutoSetValues( void );
static void InitAutoSetValues( void );

static void InitDlgArrays( DIALOG_PARSER_INFO *parse_dlg )
{
    InitArray( (void **)&parse_dlg->curr_dialog->controls, sizeof( gui_control_info ), &parse_dlg->controls_array );
    InitArray( (void **)&parse_dlg->curr_dialog->controls_ext, sizeof( control_info_ext ), &parse_dlg->controls_ext_array );
}

static bool BumpDlgArrays( DIALOG_PARSER_INFO *parse_dlg )
{
    return( BumpArray( &parse_dlg->controls_array ) && BumpArray( &parse_dlg->controls_ext_array ) );
}

#ifndef __UNIX__
static void toBackSlash( char *name )
{
    while( (name = strchr( name, '/' )) != NULL ) {
        *name = '\\';
    }
}
#endif

/**********************************************************************/
/*                   EXPRESSION EVALUTORS                             */
/**********************************************************************/

static tree_node *TreeNode( tree_op op, tree_node *left, tree_node *right )
/*************************************************************************/
{
    tree_node   *tree;

    tree = GUIMemAlloc( sizeof( tree_node ) );
    tree->op = op;
    tree->left.u.node = left;
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
    tree_node           *tree;

    #define STACK_SIZE  ( sizeof( stack ) / sizeof( *stack ) )

    if( str == NULL || IS_EMPTY( str ) ) {
        return( TreeNodeUni( OP_TRUE ) );
    }
    stack_top = -1;
    str2 = GUIStrDup( str, NULL );  // copy string so we can use STRTOK
    for( token = strtok( str2, " " ); token != NULL; token = strtok( NULL, " " ) ) {
        if( token[0] == '|' ) { // or together top 2 values
            --stack_top;
            if( stack_top < 0 ) {
                GUIDisplayMessage( MainWnd, "Expression stack underflow!", "Setup script", GUI_OK );
                stack_top = 0;
                break;
            }
            stack[stack_top] = TreeNode( OP_OR, stack[stack_top], stack[stack_top + 1] );
        } else if( token[0] == '&' ) { // and together top 2 values
            --stack_top;
            if( stack_top < 0 ) {
                GUIDisplayMessage( MainWnd, "Expression stack underflow!", "Setup script", GUI_OK );
                stack_top = 0;
                break;
            }
            stack[stack_top] = TreeNode( OP_AND, stack[stack_top], stack[stack_top + 1] );
        } else if( token[0] == '!' ) { // not top value
            stack[stack_top] = TreeNode( OP_NOT, stack[stack_top], NULL );
        } else if( token[0] == '?' ) {  // check for file existence
            ++stack_top;
            if( stack_top > STACK_SIZE - 1 ) {
                GUIDisplayMessage( MainWnd, "Expression stack overflow!", "Setup script", GUI_OK );
                stack_top = STACK_SIZE - 1;
            } else {
                stack[stack_top] = TreeNodeUni( OP_EXIST );
                stack[stack_top]->left.u.str = GUIStrDup( token + 1, NULL );
            }
        } else {                // push current value
            ++stack_top;
            if( stack_top > STACK_SIZE - 1 ) {
                GUIDisplayMessage( MainWnd, "Expression stack overflow!", "Setup script", GUI_OK );
                stack_top = STACK_SIZE - 1;
            } else {
                stack[stack_top] = TreeNodeUni( OP_VAR );
                stack[stack_top]->left.u.var = GetTokenHandle( token );
            }
        }
    }
    // and together whatever is left on stack
    tree = stack[stack_top];
    while( stack_top-- > 0 ) {
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
        return( SameExprTree( a->left.u.node, b->left.u.node ) && SameExprTree( a->right, b->right ) );
    case OP_NOT:
        return( SameExprTree( a->left.u.node, b->left.u.node ) );
    case OP_EXIST:
        return( stricmp( a->left.u.str, b->left.u.str ) == 0 );
    case OP_VAR:
        return( a->left.u.var == b->left.u.var );
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
        /* fall through */
    case OP_NOT:
        BurnTree( tree->left.u.node );
        break;
    case OP_EXIST:
        GUIMemFree( tree->left.u.str );
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
    tree_node   *new_tree;
    int         num;

    new_tree = BuildExprTree( str );
    num = SetupInfo.fileconds.num;
    while( --num >= 0 ) {
        if( SameExprTree( new_tree, FileCondInfo[num].cond ) ) {
            BurnTree( new_tree );
            return( num );
        }
    }
    num = SetupInfo.fileconds.num;
    if( !BumpArray( &SetupInfo.fileconds ) ) {
        BurnTree( new_tree );
        return( 0 );
    }
    FileCondInfo[num].cond = new_tree;
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

#define orvar( x, y ) x == y ||
#define IsMagicVar( v ) MAGICVARS( orvar, v ) false

bool GetOptionVarValue( vhandle var_handle )
/******************************************/
{
//    if( GetVariableBoolVal( "_Visibility_Condition_" ) ) {
    if( VisibilityCondition ) {
        return( VarGetBoolVal( var_handle ) );
    } else if( IsMagicVar( var_handle ) ) {
        // these are special - we always want their "true" values
        return( VarGetBoolVal( var_handle ) );
    } else if( VarGetBoolVal( UnInstall ) ) {
        // uninstall makes everything false
        return( false );
    } else if( VarGetBoolVal( FullInstall ) && VarGetAutoSetCond( var_handle ) != NULL ) {
        // fullinstall pretends all options are turned on
        return( true );
    } else {
        return( VarGetBoolVal( var_handle ) );
    }
}

#undef orvar

static bool EvalExprTree( tree_node *tree )
/*****************************************/
{
    bool        value;
    VBUF        tmp;

    switch( tree->op ) {
    case OP_AND:
        value = EvalExprTree( tree->left.u.node ) & EvalExprTree( tree->right );
        break;
    case OP_OR:
        value = EvalExprTree( tree->left.u.node ) | EvalExprTree( tree->right );
        break;
    case OP_NOT:
        value = !EvalExprTree( tree->left.u.node );
        break;
    case OP_EXIST:
        VbufInit( &tmp );
        ReplaceVars( &tmp, tree->left.u.str );
        value = ( access_vbuf( &tmp, F_OK ) == 0 );
        VbufFree( &tmp );
        break;
    case OP_VAR:
        value = GetOptionVarValue( tree->left.u.var );
        break;
    case OP_TRUE:
        value = true;
        break;
    case OP_FALSE:
    default:
        value = false;
        break;
    }
    return( value );
}

static bool DoEvalCondition( const char *str )
/********************************************/
{
    bool        value;
    tree_node   *tree;

    tree = BuildExprTree( str );
    value = EvalExprTree( tree );
    BurnTree( tree );
    return( value );
}

bool EvalCondition( const char *str )
/***********************************/
{
    if( str == NULL || *str == '\0' )
        return( true );
    return( DoEvalCondition( str ) );
}

static void PropagateValue( tree_node *tree, bool value )
/*******************************************************/
{
    vhandle     var_handle;

    switch( tree->op ) {
    case OP_AND:
        if( value ) {
            PropagateValue( tree->left.u.node, true );
            PropagateValue( tree->right, true );
        }
        break;
    case OP_OR:
        if( !value ) {
            PropagateValue( tree->left.u.node, false );
            PropagateValue( tree->right, false );
        }
        break;
    case OP_NOT:
        PropagateValue( tree->left.u.node, !value );
        break;
    case OP_VAR:
        if( value ) {
            var_handle = tree->left.u.var;
            if( VarGetAutoSetCond( var_handle ) != NULL ) {
                if( !VarIsRestrictedFalse( var_handle ) ) {
                    SetBoolVariableByHandle( PreviousInstall, true );
                    SetBoolVariableByHandle( var_handle, true );
                }
            }
        }
        break;
    default:
        break;
    }
}

static char *NextToken( char *buf, char delim )
/*********************************************/
// Locate the next 'token', delimited by the given character. Return a
// pointer to the next one, and trim trailing blanks off the current one.
{
    char            *p;
    char            *q;

    if( buf == NULL ) {
        return( NULL );
    }
    p = strchr( buf, delim );
    if( p != NULL ) {
        *p = '\0';
        q = p - 1;
        while( q >= buf && IS_WS( *q ) ) {
            *q = '\0';
            --q;
        }
        ++p;
        SKIP_WS( p );
    }
    return( p );
}


static char *StripBlanks( char *p )
/*********************************/
{
    char        *q;

    if( p == NULL ) {
        return p;
    }

    SKIP_WS( p );
    q = p + strlen( p ) - 1;
    while( q >= p && (IS_WS( *q ) || *q == '\n') ) {
        *q = '\0';
        --q;
    }
    return( p );
}


static char *StripQuotes( char *p )
/*********************************/
{
    size_t      len;

    if( p != NULL ) {
        len = strlen( p );
        if( len > 1 && p[0] == '"' && p[len - 1] == '"' ) {
            p[len - 1] = '\0';
            p++;
        }
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

    if( GUICharLen( UCHAR_VALUE( *p ) ) == 2 ) {
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

    if( GUICharLen( UCHAR_VALUE( *p ) ) == 2 ) {
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

static char *find_break( char *text, DIALOG_PARSER_INFO *parse_dlg, int *chwidth )
/********************************************************************************/
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
    while( *s && (*s != '\r') && (*s != '\n') )
        s++;
    len = s - text;

    winwidth = parse_dlg->wrap_width * CharWidth;
    // Use string length as cutoff to avoid overflow on width
    if( len < 2 * parse_dlg->wrap_width ) {
        width = GUIGetExtentX( MainWnd, text, len );
        if( width < winwidth ) {
            *chwidth = (width / CharWidth) + 1;
            if( parse_dlg->max_width < *chwidth )
                parse_dlg->max_width = *chwidth;
            return( text + len );
        }
    }
    if( parse_dlg->max_width < parse_dlg->wrap_width )
        parse_dlg->max_width = parse_dlg->wrap_width;
    *chwidth = parse_dlg->max_width;
    br = text;
    for( e = text;; ) {
        if( *e == '\0' )
            return( text );
        if( *e == '\\' && *( e + 1 ) == 'n' )
            return( e );
        n = e + GUICharLen( UCHAR_VALUE( *e ) );
        width = GUIGetExtentX( MainWnd, text, n - text );
        if( width >= winwidth )
            break;
        // is this a good place to break?
        if( IS_WS( *e ) ) { // English
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

static bool dialog_static( char *next, DIALOG_PARSER_INFO *parse_dlg )
/********************************************************************/
{
    char                *line;
    int                 len;
    VBUF                text;
    bool                rc = true;
    vhandle             var_handle;

    VbufInit( &text );
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    if( line != NULL ) {
        VbufConcStr( &text, line );
    }
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        // dummy_var allows control to have an id - used by dynamic visibility feature
        var_handle = MakeDummyVar();
        len = VbufLen( &text );
        if( len > 0 ) {
            AddInstallName( &text );
            len = VbufLen( &text );
        }
        set_dlg_dynamstring( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1,
            VbufString( &text ), VarGetId( var_handle ), parse_dlg->col_num, parse_dlg->row_num, len );
        if( len > 0 ) {
            if( parse_dlg->max_width < parse_dlg->col_num + len ) {
                parse_dlg->max_width = parse_dlg->col_num + len;
            }
        }
    } else {
        rc = false;
    }
    VbufFree( &text );
    return( rc );
}

static char *textwindow_wrap( char *text, DIALOG_PARSER_INFO *parse_dlg, bool convert_newline, bool license_file )
/****************************************************************************************************************/
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
    break_candidate = find_break( orig_index, parse_dlg, &chwidth );
    for( ; *orig_index != '\0'; orig_index++ ) {
        if( new_line ) {
            SKIP_WS( orig_index );
        }

        if( convert_newline && *orig_index == '\\' && *(orig_index + 1) == 'n' ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            orig_index++;
            break_candidate = find_break( orig_index + 1, parse_dlg, &chwidth );
        } else if( !convert_newline && *orig_index == '\r' ) {
        } else if( !convert_newline && *orig_index == '\n' ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            break_candidate = find_break( orig_index + 1, parse_dlg, &chwidth );
        } else if( break_candidate != NULL && orig_index == break_candidate ) {
            *(new_index++) = '\r';
            *(new_index++) = '\n';
            *(new_index++) = *break_candidate;
            break_candidate = find_break( orig_index + 1, parse_dlg, &chwidth );
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

static bool dialog_textwindow( char *next, DIALOG_PARSER_INFO *parse_dlg, bool license_file )
/*******************************************************************************************/
{
    char                *line;
    char                *text;
    VBUF                file_name;
    unsigned int        rows;
    bool                rc = true;
    file_handle         fh;
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
            VbufInit( &file_name );
            VbufConcStr( &file_name, line + 1 );
            fh = FileOpen( &file_name, "rb" );
            if( fh != NULL ) {
                FileStat( &file_name, &buf );
                text = GUIMemAlloc( buf.st_size + 1 );  // 1 for terminating null
                if( text != NULL ) {
                    FileRead( fh, text, buf.st_size );
                    text[buf.st_size] = '\0';
                }
                FileClose( fh );
            }
            VbufFree( &file_name );
            //VERY VERY SLOW!!!!  Don't use large files!!!
            // bottleneck is the find_break function
            text = textwindow_wrap( text, parse_dlg, false, license_file );
        } else {
            text = GUIStrDup( line, NULL );
            text = textwindow_wrap( text, parse_dlg, true, false );
        }

        line = next; next = NextToken( line, ',' );
        line = next; next = NextToken( line, ',' );
        if( EvalCondition( line ) && text != NULL ) {
            line = next; next = NextToken( line, ',' );
            // condition for visibility (dynamic)
            parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_textwindow( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1,
                text, VarGetId( var_handle ), C0, parse_dlg->row_num, parse_dlg->max_width + 2 - C0,
                rows - parse_dlg->row_num, GUI_VSCROLL );
#if defined( __DOS__ )
            parse_dlg->curr_dialog->rows += rows + 2;
            parse_dlg->row_num += rows + 2;
#else
            parse_dlg->curr_dialog->rows += rows;
            parse_dlg->row_num += rows;
#endif
        } else {
            rc = false;
        }
        GUIMemFree( text );
    }
    return( rc );
}

static bool dialog_dynamic( char *next, DIALOG_PARSER_INFO *parse_dlg )
/*********************************************************************/
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
        parse_dlg->curr_dialog->pVariables[parse_dlg->num_variables] = var_handle;
        parse_dlg->curr_dialog->pConditions[parse_dlg->num_variables] = NULL;
        parse_dlg->num_variables++;
        len = strlen( text );
        line = next;
        next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        if( parse_dlg->max_width < len )
            parse_dlg->max_width = len;
        if( parse_dlg->max_width < 60 )
            parse_dlg->max_width = 60;
        set_dlg_dynamstring( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1,
                             text, VarGetId( var_handle ), C0, parse_dlg->row_num, parse_dlg->max_width );
    } else {
        rc = false;
    }
    GUIMemFree( vbl_name );
    GUIMemFree( text );
    return( rc );
}


static bool dialog_pushbutton( char *next, DIALOG_PARSER_INFO *parse_dlg )
/************************************************************************/
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
        parse_dlg->num_push_buttons += 1;
        def_ret = false;
        if( *line_start == '.' ) {
            ++line_start;
            def_ret = true;
        }
        var_handle = GetVariableByName( line_start );
        id = set_dlg_push_button( var_handle, line_start, parse_dlg->curr_dialog->controls,
                                  parse_dlg->controls_array.num - 1, parse_dlg->row_num,
                                  parse_dlg->num_push_buttons, W / BW - 1, W, BW );
        if( def_ret ) {
            parse_dlg->curr_dialog->ret_val = IdToDlgState( id );
        }
        if( parse_dlg->max_width < parse_dlg->num_push_buttons * ( ( 3 * BW ) / 2 ) )
            parse_dlg->max_width = parse_dlg->num_push_buttons * ( ( 3 * BW ) / 2 );
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
    } else {
        rc = false;
    }
    return( rc );
}

static bool dialog_edit_button( char *next, DIALOG_PARSER_INFO *parse_dlg )
/*************************************************************************/
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
    VBUF                buff;
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    var_handle = AddVariable( vbl_name );
    line = next; next = NextToken( line, ',' );
    val = NULL;
    VbufInit( &buff );
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
                    if( GetRegString( HKEY_CURRENT_USER, section, value, &buff ) ) {
                        val = VbufString( &buff );
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
    if( val == NULL || val[0] == '\0' )
        val = VarGetStrVal( var_handle );
    SetVariableByHandle( var_handle, val );
    if( VariablesFile != NULL ) {
        ReadVariablesFile( VarGetName( var_handle ) );
    }
    line = next; next = NextToken( line, '"' );
    line = next; next = NextToken( line, '"' );
    ReplaceVars( &buff, line );
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
        parse_dlg->curr_dialog->pVariables[parse_dlg->num_variables] = var_handle;
        parse_dlg->curr_dialog->pConditions[parse_dlg->num_variables] = NULL;
        parse_dlg->num_variables++;
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        var_handle_2 = MakeDummyVar();
        SetVariableByHandle( var_handle_2, dialog_name );
        set_dlg_push_button( var_handle_2, button_text, parse_dlg->curr_dialog->controls,
                             parse_dlg->controls_array.num - 1, parse_dlg->row_num, 4, 4, W, BW );
        BumpDlgArrays( parse_dlg );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_edit( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1, VbufString( &buff ),
                      VarGetId( var_handle ), C0, parse_dlg->row_num, BW );
        if( VbufLen( &buff ) > 0 ) {
            BumpDlgArrays( parse_dlg );
            // condition for visibility (dynamic)
            parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls + 2].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_dynamstring( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1, VbufString( &buff ),
                                 VarGetId( var_handle ), C0, parse_dlg->row_num, VbufLen( &buff ) );
        }
        if( parse_dlg->max_width < 2 * VbufLen( &buff ) ) {
            parse_dlg->max_width = 2 * VbufLen( &buff );
        }
    } else {
        rc = false;
    }
    GUIMemFree( vbl_name );
    VbufFree( &buff );
    return( rc );
}

static bool dialog_other_button( char *next, DIALOG_PARSER_INFO *parse_dlg )
/**************************************************************************/
{
    char                *line;
    char                *button_text;
    char                *next_copy;
    char                *text;
    char                *dialog_name;
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
        set_dlg_push_button( var_handle, button_text, parse_dlg->curr_dialog->controls,
                             parse_dlg->controls_array.num - 1, parse_dlg->row_num, 4, 4, W, BW );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        if( text != NULL ) {
            BumpDlgArrays( parse_dlg );
            // condition for visibility (dynamic)
            parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            parse_dlg->col_num = 1;
            dialog_static( next_copy, parse_dlg );
        }
    } else {
        rc = false;
    }
    GUIMemFree( next_copy );
    return( rc );
}


static vhandle dialog_set_variable( DIALOG_PARSER_INFO *parse_dlg, const char *vbl_name,
                                    const char *init_cond )
/**************************************************************************************/
{
    vhandle     var_handle;

    var_handle = AddVariable( vbl_name );
    parse_dlg->curr_dialog->pVariables[parse_dlg->num_variables] = var_handle;
    if( init_cond != NULL ) {
        if( *init_cond == '\0' ) {
            init_cond = NULL;
        } else {
            if( SkipDialogs ) {
                if( stricmp( init_cond, "true" ) == 0 ) {
                    SetBoolVariableByHandle( var_handle, true );
                } else if( stricmp( init_cond, "false" ) == 0 ) {
                    SetBoolVariableByHandle( var_handle, false );
                } else {
                    SetVariableByHandle( var_handle, init_cond );
                }
            }
        }
    }
    parse_dlg->curr_dialog->pConditions[parse_dlg->num_variables] = GUIStrDup( init_cond, NULL );
    parse_dlg->num_variables++;
    return( var_handle );
}


static bool dialog_radiobutton( char *next, DIALOG_PARSER_INFO *parse_dlg )
/*************************************************************************/
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
        var_handle = dialog_set_variable( parse_dlg, vbl_name, init_cond );
        parse_dlg->num_radio_buttons += 1;
        len = strlen( text ) + 4; // room for button
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_radio( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1,
                       parse_dlg->num_radio_buttons, text, VarGetId( var_handle ), C0, parse_dlg->row_num, len );
        if( parse_dlg->max_width < len ) {
            parse_dlg->max_width = len;
        }
    } else {
        rc = false;
    }
    GUIMemFree( init_cond );
    GUIMemFree( vbl_name );
    GUIMemFree( text );
    return( rc );
}


static bool dialog_checkbox( char *next, DIALOG_PARSER_INFO *parse_dlg, bool detail_button )
/******************************************************************************************/
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

    dlg_var_handle = NO_VAR;
    dialog_name = NULL;
    button_text = "";
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
        var_handle = dialog_set_variable( parse_dlg, vbl_name, init_cond );
        len = strlen( text ) + 4; // room for button
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_check( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1, text,
                       VarGetId( var_handle ), parse_dlg->col_num, parse_dlg->row_num, len );
        if( parse_dlg->col_num == C0 ) {
            // 1st check-box on line
            if( parse_dlg->max_width < len )
                parse_dlg->max_width = len;
            parse_dlg->col_num += len + 1;    // update col_num for next time
        } else {
            // 2nd check-box
            if( len < parse_dlg->col_num - 1 )
                len = parse_dlg->col_num - 1;
            if( parse_dlg->max_width < 2 * len + 1 ) {    // add 1 for space
                parse_dlg->max_width = 2 * len + 1;
            }
        }
        if( detail_button ) {
            BumpDlgArrays( parse_dlg );
            parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            set_dlg_push_button( dlg_var_handle, button_text, parse_dlg->curr_dialog->controls,
                                 parse_dlg->controls_array.num - 1, parse_dlg->row_num, 4, 4, W, BW );
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


static bool dialog_editcontrol( char *next, DIALOG_PARSER_INFO *parse_dlg )
/*************************************************************************/
{
    char                *line;
    char                *vbl_name;
    const char          *val;
    char                *section;
#if defined( __NT__ )
    char                *value;
#endif
    vhandle             var_handle;
    VBUF                buff;
    bool                rc = true;

    line = next; next = NextToken( line, ',' );
    vbl_name = GUIStrDup( line, NULL );
    var_handle = AddVariable( vbl_name );
    line = next; next = NextToken( line, ',' );
    val = NULL;
    VbufInit( &buff );
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
                    if( GetRegString( HKEY_CURRENT_USER, section, value, &buff ) ) {
                        val = VbufString( &buff );
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
    ReplaceVars( &buff, line );
    line = next; next = NextToken( line, ',' );
    line = next; next = NextToken( line, ',' );
    if( EvalCondition( line ) ) {
        parse_dlg->curr_dialog->pVariables[parse_dlg->num_variables] = var_handle;
        parse_dlg->curr_dialog->pConditions[parse_dlg->num_variables] = NULL;
        parse_dlg->num_variables++;
        line = next; next = NextToken( line, ',' );
        // condition for visibility (dynamic)
        parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls].pVisibilityConds = GUIStrDup( line, NULL );
        set_dlg_edit( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1,
                      VbufString( &buff ), VarGetId( var_handle ), C0, parse_dlg->row_num, W );
        if( VbufLen( &buff ) > 0 ) {
            BumpDlgArrays( parse_dlg );
            // condition for visibility (dynamic)
            parse_dlg->curr_dialog->controls_ext[parse_dlg->curr_dialog->num_controls + 1].pVisibilityConds = GUIStrDup( line, NULL );
            // dummy_var allows control to have an id - used by dynamic visibility feature
            var_handle = MakeDummyVar();
            set_dlg_dynamstring( parse_dlg->curr_dialog->controls, parse_dlg->controls_array.num - 1, VbufString( &buff ),
                                 VarGetId( var_handle ), C0, parse_dlg->row_num, VbufLen( &buff ) );
        }
        if( parse_dlg->max_width < 2 * VbufLen( &buff ) ) {
            parse_dlg->max_width = 2 * VbufLen( &buff );
        }
    } else {
        rc = false;
    }
    VbufFree( &buff );
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
    VBUF                buff;

    // Remove leading and trailing white-space.
    line = StripBlanks( line );

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
            static DIALOG_PARSER_INFO   parse_dlg;
            bool                        added = false;

            next = NextToken( line, '=' );
            if( stricmp( line, "name" ) == 0 ) {
                // new dialog
                memset( &parse_dlg, 0, sizeof( DIALOG_PARSER_INFO ) );
                parse_dlg.curr_dialog = AddNewDialog( next );
                InitDlgArrays( &parse_dlg );
                parse_dlg.wrap_width = MaxWidthChars;
            } else if( stricmp( line, "condition" ) == 0 ) {
                parse_dlg.curr_dialog->condition = CompileCondition( next );
            } else if( stricmp( line, "title" ) == 0 ) {
                parse_dlg.curr_dialog->title = GUIStrDup( next, NULL );
            } else if( stricmp( line, "any_check" ) == 0 ) {
                parse_dlg.curr_dialog->any_check = AddVariable( next );
            } else if( stricmp( line, "width" ) == 0 ) {
                int         wrap_width;
                wrap_width = atoi( next );
                if( wrap_width > 0 && wrap_width <= MaxWidthChars ) {
                    parse_dlg.wrap_width = wrap_width;
                }
            } else if( stricmp( line, "vis_condition" ) == 0 ) {
                line = next; next = NextToken( line, ',' );
                if( !EvalCondition( next ) ) {
                    line = NULL;
                }
                parse_dlg.curr_dialog->controls_ext[parse_dlg.curr_dialog->num_controls - 1].pVisibilityConds = GUIStrDup( line, NULL );
            } else {
                // add another control to current dialog
                if( !BumpDlgArrays( &parse_dlg ) ) {
                    SetupError( "IDS_NOMEMORY" );
                    exit( 1 );
                }
                if( stricmp( line, "static_text" ) == 0 ) {
                    parse_dlg.col_num = C0;
                    added = dialog_static( next, &parse_dlg );
                } else if( stricmp( line, "dynamic_text" ) == 0 ) {
                    added = dialog_dynamic( next, &parse_dlg );
                } else if( stricmp( line, "other_button" ) == 0 ) {
                    added = dialog_other_button( next, &parse_dlg );
                } else if( stricmp( line, "edit_button" ) == 0 ) {
                    added = dialog_edit_button( next, &parse_dlg );
                } else if( stricmp( line, "push_button" ) == 0 ) {
                    added = dialog_pushbutton( next, &parse_dlg );
                    if( added ) {
                        parse_dlg.row_num -= 1;
                    }
                } else if( stricmp( line, "radio_button" ) == 0 ) {
                    added = dialog_radiobutton( next, &parse_dlg );
                } else if( stricmp(line, "check_box") == 0 ) {
                    parse_dlg.col_num = C0;
                    added = dialog_checkbox( next, &parse_dlg, false );
                } else if( stricmp(line, "detail_check") == 0 ) {
                    parse_dlg.col_num = C0;
                    added = dialog_checkbox( next, &parse_dlg, true );
                } else if( stricmp(line, "check_box_continue") == 0 ) {
                    parse_dlg.row_num -= 1;
                    added = dialog_checkbox( next, &parse_dlg, false );
                } else if( stricmp(line, "edit_control") == 0 ) {
                    added = dialog_editcontrol( next, &parse_dlg );
                } else if( stricmp(line, "text_window") == 0 ) {
                    added = dialog_textwindow( next, &parse_dlg, false );
                } else if( stricmp(line, "text_window_license") == 0 ) {
                    added = dialog_textwindow( next, &parse_dlg, true );
                }
                if( added ) {
                    parse_dlg.row_num += 1;
                    // in case this was the last control, set some values
                    parse_dlg.curr_dialog->pVariables[parse_dlg.num_variables] = NO_VAR;
                    parse_dlg.curr_dialog->pConditions[parse_dlg.num_variables] = NULL;
                    parse_dlg.curr_dialog->num_controls = parse_dlg.controls_array.num;
                    parse_dlg.curr_dialog->num_push_buttons = parse_dlg.num_push_buttons;
                    parse_dlg.curr_dialog->rows = parse_dlg.row_num  + HEIGHT_BORDER;
                    if( parse_dlg.num_push_buttons != 0 ) {
                        parse_dlg.curr_dialog->rows += 1;
                    }
                    parse_dlg.curr_dialog->cols = parse_dlg.max_width + WIDTH_BORDER;
                } else {
                    parse_dlg.controls_array.num--;
                    parse_dlg.controls_ext_array.num--;
                }
            }
        }
        break;
    case RS_APPLICATION:
        next = NextToken( line, '=' );
        if( stricmp( line, "DefPMGroup" ) == 0 ) {
            line = next; next = NextToken( line, ',' );
            if( SetupInfo.pm_group_file != NULL ) {
                GUIMemFree( SetupInfo.pm_group_file );
                SetupInfo.pm_group_file = NULL;
            }
            SetupInfo.pm_group_file = GUIStrDup( line, NULL );
            line = next; next = NextToken( line, ',' );
            if( SetupInfo.pm_group_name != NULL ) {
                GUIMemFree( SetupInfo.pm_group_name );
                SetupInfo.pm_group_name = NULL;
            }
            VbufInit( &buff );
            ReplaceVars( &buff, line );
            SetupInfo.pm_group_name = GUIStrDup( VbufString( &buff ), NULL );
            VbufFree( &buff );
            num = SetupInfo.all_pm_groups.num;
            if( !BumpArray( &SetupInfo.all_pm_groups ) )
                return( false );
            AllPMGroups[num].group_name = GUIStrDup( SetupInfo.pm_group_name, NULL );
            AllPMGroups[num].group_file = GUIStrDup( SetupInfo.pm_group_file, NULL );
            if( SetupInfo.pm_group_iconfile != NULL ) {
                GUIMemFree( SetupInfo.pm_group_iconfile );
                SetupInfo.pm_group_iconfile = NULL;
            }
            SetupInfo.pm_group_iconfile = GUIStrDup( next, NULL );
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
#ifndef __UNIX__
        toBackSlash( DirInfo[num].desc );
#endif
        break;
    case RS_FILES:
        {
            int         num_files;
            a_file_info *file;
            char        *p;

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
            num_files = GET36( line );
            if( num_files == 0 ) {
                FileInfo[num].files = NULL;
            } else {
                FileInfo[num].files = GUIMemAlloc( num_files * sizeof( a_file_info ) );
                if( FileInfo[num].files == NULL ) {
                    return( false );
                }
            }
            FileInfo[num].supplemental = false;
            FileInfo[num].core_component = false;
            FileInfo[num].num_files = num_files;
            VbufInit( &buff );
            file = FileInfo[num].files;
            for( ; num_files-- > 0; ) {
                line = next; next = NextToken( line, ',' );
                p = NextToken( line, '!' );
                VbufInit( &file->name );
                VbufConcStr( &file->name, line );
                VbufSplitpath( &file->name, NULL, NULL, NULL, &buff );
                file->is_nlm = VbufCompExt( &buff, "nlm", true ) == 0;
                file->is_dll = VbufCompExt( &buff, "dll", true ) == 0;
                line = p; p = NextToken( line, '!' );
                file->size = GET36( line ) * 512UL;
                if( p != NULL && *p != '\0' && *p != '!' ) {
                    file->date = GET36( p );
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
                        FileInfo[num].supplemental = true;
                    } else if( *p == 'k' ) {
                        FileInfo[num].core_component = true;
                    }
                }
                file++;
            }
            VbufFree( &buff );
            line = next; next = NextToken( line, ',' );
            FileInfo[num].dir_index = GET36( line ) - 1;
            line = next; next = NextToken( line, ',' );
            FileInfo[num].old_dir_index = GET36( line );
            if( FileInfo[num].old_dir_index != -1 ) {
                FileInfo[num].old_dir_index--;
            }
            line = next; next = NextToken( line, ',' );
            FileInfo[num].condition.i = NewFileCond( line );
        }
        break;
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
        PMInfo[num].group = ( strcmp( line, "GROUP" ) == 0 );
        PMInfo[num].shadow = ( line[0] == '+' );
        if( PMInfo[num].shadow ) {
            PMInfo[num].filename = GUIStrDup( line + 1, NULL );
        } else {
            PMInfo[num].filename = GUIStrDup( line, NULL );
        }
        line = next; next = NextToken( line, ',' );
        PMInfo[num].parameters = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        VbufInit( &buff );
        ReplaceVars( &buff, line );
        PMInfo[num].desc = GUIStrDup( VbufString( &buff ), NULL );
        VbufFree( &buff );
        if( PMInfo[num].group ) {
            AllPMGroups[SetupInfo.all_pm_groups.num].group_name = GUIStrDup( PMInfo[num].desc, NULL );
            AllPMGroups[SetupInfo.all_pm_groups.num].group_file = GUIStrDup( PMInfo[num].parameters, NULL );
            if( !BumpArray( &SetupInfo.all_pm_groups ) ) {
                return( false );
            }
        }
        PMInfo[num].iconfile = NULL;
        PMInfo[num].iconindex = 0;
        if( next != NULL ) {
            line = next; next = NextToken( line, ',' );
            PMInfo[num].iconfile = GUIStrDup( line, NULL );
            if( next != NULL ) {
                line = next; next = NextToken( line, ',' );
                PMInfo[num].iconindex = atoi( line );
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
        TargetInfo[num].supplemental = false;
        if( next != NULL && stricmp( next, "supplemental" ) == 0 ) {
            TargetInfo[num].supplemental = true;
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
            return( false );
        next = NextToken( line, '=' );
        AssociationInfo[num].ext = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].keyname = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].description = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].program = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].iconfile = GUIStrDup( line, NULL );
        line = next; next = NextToken( line, ',' );
        AssociationInfo[num].iconindex = strtol( line, NULL, 10 );
        AssociationInfo[num].condition = CompileCondition( next );
        break;
    /*
     * For now Setup Error Messages, Status line Messages and Misc Messages
     * are treated as global symbolic variables just like
     * regular Error Messages
     */
    case RS_SETUPERRORMESSAGE:
    case RS_STATUSLINEMESSAGE:
    case RS_MISCMESSAGE:
    case RS_ERRORMESSAGE:
#ifdef LICENSE
    case RS_LICENSEMESSAGE:
#endif
        {
            char    *p;

            next = NextToken( line, '=' );
            next = StripBlanks( next );
            next = StripQuotes( next );
            for( ; (p = strstr( next, "\\n" )) != NULL; ) {
                *p = '\n';
                memmove( p + 1, p + 2, strlen( p + 2 ) + 1 );
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
    VBUF        buff;
    size_t      dir_end;
    struct stat buf;
    int         j;
    int         k;
    bool        found;
    bool        supp;
    a_file_info *file;

    if( dir_index == -1 )
        return( false );
    found = false;
    VbufInit( &buff );
    SimDirNoEndSlash( dir_index, &buff );
    if( access_vbuf( &buff, F_OK ) == 0 ) {
        VbufAddDirSep( &buff );
        dir_end = VbufLen( &buff );
        supp = TargetInfo[DirInfo[FileInfo[i].dir_index].target].supplemental;
        if( supp ) {
            // don't turn off supplemental bit if file is already marked
            FileInfo[i].supplemental = supp;
        }
        file = FileInfo[i].files;
        for( j = 0; j < FileInfo[i].num_files; ++j, file++ ) {
            file->disk_size = 0;
            file->disk_date = 0;
            file->read_only = false;
            if( VbufLen( &file->name ) == 0 )
                continue;
            VbufSetVbufAt( &buff, &file->name, dir_end );
            if( access_vbuf( &buff, F_OK ) == 0 ) {
                stat_vbuf( &buff, &buf );
                found = true;
                file->disk_size = buf.st_size;
                file->disk_date = (unsigned long)buf.st_mtime;
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
    }
    VbufFree( &buff );
    return( found );
}

static bool GetDiskSizes( void )
/******************************/
{
    int         i;
    int         j;
    long        status_amount;
    long        status_curr;
    bool        zeroed;
    bool        rc = true;
    bool        asked;
    bool        dont_touch;
    bool        uninstall;

    status_amount = 0;
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        status_amount += FileInfo[i].num_files;
    }
    StatusShow( true );
    StatusLines( STAT_CHECKING, "" );
    SetBoolVariableByHandle( PreviousInstall, false );
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
    uninstall = VarGetBoolVal( UnInstall );
    if( uninstall ) {
        // if uninstalling - remove all files, don't prompt
        asked = true;
    } else {
        asked = false;
    }
    for( i = 0; i < SetupInfo.files.num; ++i ) {
        if( FileInfo[i].condition.p->one_uptodate &&
            FileInfo[i].num_files != 0 &&
            !FileInfo[i].supplemental &&
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


static char *readLine( file_handle fh, char *buffer, size_t length )
/******************************************************************/
{
    static int      raw_buf_size;
    char            *line_start;
    size_t          len;
    bool            done;

    done = false;
    do {
        // Read data into raw buffer if it's empty
        if( RawBufPos == NULL ) {
            raw_buf_size = FileRead( fh, RawReadBuf, BUF_SIZE );
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
               ((size_t)( RawBufPos - line_start ) < length) ) {
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


static int PrepareSetupInfo( file_handle fh, pass_type pass )
/***********************************************************/
{
    int                 result;
    gui_mcursor_handle  old_cursor;
    bool                done;
    size_t              len;
    char                *p;

    RawBufPos = NULL;       // reset buffer position
    LineCountPointer = &NoLineCount;
    old_cursor = GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
    result = SIM_INIT_NOERROR;
    if( pass == PRESCAN_FILE ) {
        State = RS_UNDEFINED;
    }

    // Read file in blocks, break up into lines
    done = false;
    for( ;; ) {
        len = 0;
        for( ;; ) {
            if( readLine( fh, ReadBuf + len, ReadBufSize - len ) == NULL ) {
                done = true;
                break;
            }
            // Eliminate leading blanks on continued lines
            if( len > 0 ) {
                p = ReadBuf + len;
                SKIP_WS( p );
                memmove( ReadBuf + len, p, strlen( p ) + 1 );
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
        if( State == RS_TERMINATE ) {
            break;
        }
    }
    GUIResetMouseCursor( old_cursor );
    return( result );
}

bool CheckForceDLLInstall( const VBUF *name )
/*******************************************/
{
    int         i;
    size_t      len;
    const char  *dllname;

    len = VbufLen( name );
    dllname = VbufString( name );
    for( i = 0; i < SetupInfo.force_DLL_install.num; i++ ) {
        if( stristr( ForceDLLInstall[i].name, dllname, len ) != NULL ) {
            return( true );
        }
    }
    return( false );
}

long SimInit( const VBUF *inf_name )
/**********************************/
{
    long                result;
    file_handle         fh;
    struct stat         stat_buf;
    int                 i;
    gui_text_metrics    metrics;

    memset( &SetupInfo, 0, sizeof( struct setup_info ) );
    FileStat( inf_name, &stat_buf );
    SetupInfo.stamp = (unsigned long)stat_buf.st_mtime;

#define setvar( x, y ) x = AddVariable( #x );
    MAGICVARS( setvar, 0 )
    NONMAGICVARS( setvar, 0 )
#undef setvar

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
    fh = FileOpen( inf_name, "rb" );
    if( fh == NULL ) {
        GUIMemFree( ReadBuf );
        GUIMemFree( RawReadBuf );
        return( SIM_INIT_NOFILE );
    }
    SetVariableByName_vbuf( "SetupInfFile", inf_name );
    result = PrepareSetupInfo( fh, PRESCAN_FILE );
    FileClose( fh );
    fh = FileOpen( inf_name, "rb" );
    if( fh == NULL ) {
        GUIMemFree( ReadBuf );
        GUIMemFree( RawReadBuf );
        return( SIM_INIT_NOFILE );
    }
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
    InitArray( (void **)&SpawnInfo, sizeof( struct spawn_info ), &SetupInfo.spawn );
    InitArray( (void **)&DeleteInfo, sizeof( struct spawn_info ), &SetupInfo.delete );
    InitArray( (void **)&FileCondInfo, sizeof( struct file_cond_info ), &SetupInfo.fileconds );
    InitArray( (void **)&DLLsToCheck, sizeof( struct dlls_to_check ), &SetupInfo.dlls_to_count );
    InitArray( (void **)&ForceDLLInstall, sizeof( struct force_DLL_install ), &SetupInfo.force_DLL_install );
    InitArray( (void **)&AllPMGroups, sizeof( struct all_pm_groups ), &SetupInfo.all_pm_groups );
    InitArray( (void **)&AssociationInfo, sizeof( struct association_info ), &SetupInfo.associations );
#if defined( GUI_IS_GUI )
    SetDialogFont();
#endif
    GUIGetTextMetrics( MainWnd, &metrics );
    GUIGetDlgTextMetrics( &metrics );
    CharWidth = metrics.avg.x;
    MaxWidthChars = GUIScale.x / CharWidth - 4 * WIDTH_BORDER;
    if( MaxWidthChars > MAX_WINDOW_WIDTH )  {
        MaxWidthChars = MAX_WINDOW_WIDTH;
    }
    result = PrepareSetupInfo( fh, FINAL_SCAN );
    FileClose( fh );
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

const char *SimDeleteName( int i )
/********************************/
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

void SimTargetDir( int i, VBUF *buff )
/************************************/
{
    // same as SimTargetDirName, only expand macros
    ReplaceVars( buff, GetVariableStrVal( TargetInfo[i].name ) );
}

void SimTargetDirName( int i, VBUF *buff )
/****************************************/
{
    // same as SimTargetDir, only don't expand macros
    ReplaceVars( buff, TargetInfo[i].name );
}

bool SimTargetNeedsUpdate( int i )
/********************************/
{
    return( TargetInfo[i].needs_update );
}

disk_ssize SimTargetSpaceNeeded( int i )
/**************************************/
{
    return( TargetInfo[i].space_needed );
}

int SimGetTargNumFiles( int i )
/*****************************/
{
    return( TargetInfo[i].num_files );
}

void SimSetTargTempDisk( int i, char disk )
/*****************************************/
{
    *TargetInfo[i].temp_disk = disk;
}

const char *SimGetTargTempDisk( int i )
/*************************************/
{
    return( TargetInfo[i].temp_disk );
}

/*
 * =======================================================================
 * API to DirInfo[]
 * =======================================================================
 */

int SimDirTargNum( int i )
/************************/
{
    return( DirInfo[i].target );
}

int SimDirParent( int i )
/***********************/
{
    return( DirInfo[i].parent );
}

int SimNumDirs( void )
/********************/
{
    return( SetupInfo.dirs.num );
}

void SimDirNoEndSlash( int i, VBUF *buff )
/****************************************/
{
    SimTargetDir( DirInfo[i].target, buff );
    if( !IS_EMPTY( DirInfo[i].desc ) ) {
        VbufAddDirSep( buff );
        VbufConcStr( buff, DirInfo[i].desc );
    }
}

bool SimDirUsed( int i )
/**********************/
{
    return( DirInfo[i].used );
}

void SimGetDir( int i, VBUF *buff )
/*********************************/
{
    SimDirNoEndSlash( i, buff );
    VbufAddDirSep( buff );
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

void SimGetFileDesc( int parm, VBUF *buff )
/*****************************************/
{
    if( FileInfo[parm].num_files == 0 ) {
        VbufSetStr( buff, FileInfo[parm].filename );
    } else {
        VbufSetVbuf( buff, &FileInfo[parm].files[0].name );
    }
}

void SimGetFileName( int parm, VBUF *buff )
/*****************************************/
{
    VbufSetStr( buff, FileInfo[parm].filename );
}

long SimFileSize( int parm )
/**************************/
{
    long        size;
    int         len;

    size = 0;
    len = FileInfo[parm].num_files;
    while( len-- > 0 ) {
        size += FileInfo[parm].files[len].size;
    }
    return( size );
}


long SimSubFileSize( int parm, int subfile )
/******************************************/
{
    return( FileInfo[parm].files[subfile].size );
}


void SimFileDir( int parm, VBUF *buff )
/*************************************/
{
    SimGetDir( FileInfo[parm].dir_index, buff );
}

int SimFileDirNum( int parm )
/***************************/
{
    return( FileInfo[parm].dir_index );
}

bool SimFileOldDir( int parm, VBUF *buff )
/****************************************/
{
    if( FileInfo[parm].old_dir_index == -1 ) {
        VbufRewind( buff );
        return( false );
    }
    SimGetDir( FileInfo[parm].old_dir_index, buff );
    return( true );
}

int SimNumSubFiles( int parm )
/****************************/
{
    return( FileInfo[parm].num_files );
}

bool SimSubFileInOldDir( int parm, int subfile )
/**********************************************/
{
    return( FileInfo[parm].files[subfile].in_old_dir );
}

bool SimSubFileInNewDir( int parm, int subfile )
/**********************************************/
{
    return( FileInfo[parm].files[subfile].in_new_dir );
}

bool SimSubFileReadOnly( int parm, int subfile )
/**********************************************/
{
    return( FileInfo[parm].files[subfile].read_only );
}

bool SimSubFileExecutable( int parm, int subfile )
/************************************************/
{
    return( FileInfo[parm].files[subfile].executable );
}

bool SimSubFileIsNLM( int parm, int subfile )
/*******************************************/
{
    return( FileInfo[parm].files[subfile].is_nlm );
}

bool SimSubFileIsDLL( int parm, int subfile )
/*******************************************/
{
    return( FileInfo[parm].files[subfile].is_dll );
}

bool SimSubFileNewer( int parm, int subfile )
/*******************************************/
{
    return( FileInfo[parm].files[subfile].disk_date > FileInfo[parm].files[subfile].date );
}

time_t SimSubFileDate( int parm, int subfile )
/********************************************/
{
    return( FileInfo[parm].files[subfile].date );
}

bool SimSubFileExists( int parm, int subfile )
/********************************************/
{
    return( SimSubFileInOldDir( parm, subfile ) || SimSubFileInNewDir( parm, subfile ) );
}

void SimSubFileName( int parm, int subfile, VBUF *buff )
/******************************************************/
{
    VbufSetVbuf( buff, &FileInfo[parm].files[subfile].name );
}

vhandle SimSubFileVar( int parm, int subfile )
/********************************************/
{
    return( FileInfo[parm].files[subfile].dst_var );
}

bool SimFileUpToDate( int parm )
/******************************/
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

void SimGetPMApplGroupFile( VBUF *buff )
/**************************************/
{
    VbufRewind( buff );
    if( SetupInfo.pm_group_file != NULL ) {
        VbufConcStr( buff, SetupInfo.pm_group_file );
    }
}

void SimGetPMApplGroupIconFile( VBUF *buff )
/******************************************/
{
    VbufRewind( buff );
    if( SetupInfo.pm_group_iconfile != NULL ) {
        VbufConcStr( buff, SetupInfo.pm_group_iconfile );
    }
}

void SimGetPMApplGroupName( VBUF *buff )
/**************************************/
{
    VbufRewind( buff );
    if( SetupInfo.pm_group_name != NULL ) {
        VbufConcStr( buff, SetupInfo.pm_group_name );
    }
}

bool SimIsPMApplGroupDefined( void )
/**********************************/
{
    return( SetupInfo.pm_group_name != NULL );
}

int SimGetPMsNum( void )
/**********************/
{
    return( SetupInfo.pm_files.num );
}

static int SimFindDirForFile( const VBUF *buff )
/**********************************************/
{
    int         i;
    int         j;

    for( i = 0; i < SetupInfo.files.num; i++ ) {
        for( j = 0; j < FileInfo[i].num_files; ++j ) {
            if( VbufCompVbuf( &FileInfo[i].files[j].name, buff, true ) == 0 ) {
                return( FileInfo[i].dir_index );
            }
        }
    }
    return( -1 );
}


int SimGetPMProgInfo( int parm, VBUF *buff )
/******************************************/
{
    VbufSetStr( buff, PMInfo[parm].filename );
    return( SimFindDirForFile( buff ) );
}

bool SimPMIsShadow( int parm )
/****************************/
{
    return( PMInfo[parm].shadow );
}

bool SimPMIsGroup( int parm )
/***************************/
{
    return( PMInfo[parm].group );
}

void SimGetPMParms( int parm, VBUF *buff )
/****************************************/
{
    VbufSetStr( buff, PMInfo[parm].parameters );
}

void SimGetPMDesc( int parm, VBUF *buff )
/***************************************/
{
    VbufSetStr( buff, PMInfo[parm].desc );
}

int SimGetPMIconInfo( int parm, VBUF *iconfile, int *iconindex )
/**************************************************************/
{
    if( PMInfo[parm].iconfile == NULL ) {
        ReplaceVars( iconfile, PMInfo[parm].filename );
    } else {
        VbufSetStr( iconfile, PMInfo[parm].iconfile );
    }
    *iconindex = PMInfo[parm].iconindex;
    return( SimFindDirForFile( iconfile ) );
}

bool SimCheckPMCondition( int parm )
/**********************************/
{
    return( EvalCondition( PMInfo[parm].condition ) );
}

int SimGetPMGroupsNum( void )
/***************************/
{
    return( SetupInfo.all_pm_groups.num );
}

void SimGetPMGroupName( int parm, VBUF *buff )
/********************************************/
{
    VbufSetStr( buff, AllPMGroups[parm].group_name );
}

void SimGetPMGroupFile( int parm, VBUF *buff )
/********************************************/
{
    VbufSetStr( buff, AllPMGroups[parm].group_file );
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

void SimProfInfo( int parm, VBUF *app_name, VBUF *key_name, VBUF *value,
                         VBUF *file_name, VBUF *hive_name )
/**********************************************************************/
{
    VbufSetStr( app_name, ProfileInfo[parm].app_name );
    VbufSetStr( key_name, ProfileInfo[parm].key_name );
    VbufSetStr( value, ProfileInfo[parm].value );
    VbufRewind( file_name );
    if( ProfileInfo[parm].file_name != NULL ) {
        VbufConcStr( file_name, ProfileInfo[parm].file_name );
    }
    VbufRewind( hive_name );
    if( ProfileInfo[parm].hive_name != NULL ) {
        VbufConcStr( hive_name, ProfileInfo[parm].hive_name );
    }
}

bool SimCheckProfCondition( int parm )
/************************************/
{
    return( EvalCondition( ProfileInfo[parm].condition ) );
}

/*
 * =======================================================================
 * API to AutoExecInfo[] EnvironmentInfo[] ConfigInfo[]
 * =======================================================================
 */

static append_mode SimGetConfigStringsFrom( struct config_info *array, int i,
                                            VBUF *cfg_var, VBUF *cfg_val )
/****************************************************************************/
{
    append_mode append;
    char        *p;

    ReplaceVars( cfg_val, array[i].value );
    append = AM_OVERWRITE;
    p = array[i].var;
    if( *p == '+' ) {
        ++p;
        if( *p == '+' ) {
            ++p;
            append = AM_BEFORE;
        } else {
            append = AM_AFTER;
        }
    }
    VbufSetStr( cfg_var, p );
    return( append );
}

int SimNumAutoExec( void )
/************************/
{
    return( SetupInfo.autoexec.num );
}

append_mode SimGetAutoExecStrings( int i, VBUF *cfg_var, VBUF *cfg_val )
/**********************************************************************/
{
    return( SimGetConfigStringsFrom( AutoExecInfo, i, cfg_var, cfg_val ) );
}

bool SimCheckAutoExecCondition( int parm )
/****************************************/
{
    return( EvalCondition( AutoExecInfo[parm].condition ) );
}

int SimNumConfig( void )
/**********************/
{
    return( SetupInfo.config.num );
}


append_mode SimGetConfigStrings( int i, VBUF *cfg_var, VBUF *cfg_val )
/********************************************************************/
{
    return( SimGetConfigStringsFrom( ConfigInfo, i, cfg_var, cfg_val ) );
}

bool SimCheckConfigCondition( int parm )
/**************************************/
{
    return( EvalCondition( ConfigInfo[parm].condition ) );
}

int SimNumEnvironment( void )
/***************************/
{
    return( SetupInfo.environment.num );
}


append_mode SimGetEnvironmentStrings( int i, VBUF *cfg_var, VBUF *cfg_val )
/*************************************************************************/
{
    return( SimGetConfigStringsFrom( EnvironmentInfo, i, cfg_var, cfg_val ) );
}

bool SimCheckEnvironmentCondition( int parm )
/*******************************************/
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

void SimGetAssociationExt( int parm, VBUF *buff )
/***********************************************/
{
    VbufSetStr( buff, AssociationInfo[parm].ext );
}

void SimGetAssociationKeyName( int parm, VBUF *buff )
/***************************************************/
{
    VbufSetStr( buff, AssociationInfo[parm].keyname );
}

void SimGetAssociationProgram( int parm, VBUF *buff )
/***************************************************/
{
    VbufSetStr( buff, AssociationInfo[parm].program );
}

void SimGetAssociationDescription( int parm, VBUF *buff )
/*******************************************************/
{
    VbufSetStr( buff, AssociationInfo[parm].description );
}

void SimGetAssociationIconFileName( int parm, VBUF *buff )
/********************************************************/
{
    VbufSetStr( buff, AssociationInfo[parm].iconfile );
}

int SimGetAssociationIconIndex( int parm )
/****************************************/
{
    return( AssociationInfo[parm].iconindex );
}

bool SimCheckAssociationCondition( int parm )
/*******************************************/
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

void SimGetLabelDir( int parm, VBUF *buff )
/*****************************************/
{
    VbufSetStr( buff, LabelInfo[parm].dir );
}

void SimGetLabelLabel( int parm, VBUF *buff )
/*******************************************/
{
    VbufSetStr( buff, LabelInfo[parm].label );
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

const char *SimGetUpgradeName( int parm )
/***************************************/
{
    return( UpgradeInfo[parm].name );
}

/*
 * =======================================================================
 *
 * =======================================================================
 */

const char *SimGetTargetDriveLetter( int parm, VBUF *buff )
/*********************************************************/
{
    char temp[_MAX_PATH];

    SimTargetDir( parm, buff );
    if( VbufLen( buff ) == 0 ) {
        getcwd( temp, sizeof( temp ) );
        VbufSetStr( buff, temp );
    } else if( VbufString( buff )[0] != '\\' || VbufString( buff )[1] != '\\' ) {
        if( VbufString( buff )[0] == '\\' && VbufString( buff )[1] != '\\' ) {
            getcwd( temp, sizeof( temp ) );
            VbufPrepStr( buff, temp );
        } else if( VbufString( buff )[1] != ':' ) {
            getcwd( temp, sizeof( temp ) );
            VbufSetStr( buff, temp );
        }
    }
    return( VbufString( buff ) );
}


static void MarkUsed( int dir_index )
/***********************************/
{
    int         parent;

    DirInfo[dir_index].used = true;
    for( parent = DirInfo[dir_index].parent; parent != -1; parent = DirInfo[parent].parent ) {
        DirInfo[parent].used = true;
    }
}

#if defined ( __NT__ )
void CheckDLLCount( const char *install_name )
/********************************************/
{
    // Takes care of DLL usage counts in the Win95/WinNT registry;
    // removes DLLs if their usage count goes to zero and the user
    // agrees to delete them.
    int         i;

    /* unused parameters */ (void)install_name;

    for( i = 0; i < SetupInfo.dlls_to_count.num; i++ ) {
        if( FileInfo[DLLsToCheck[i].index].core_component ) {
            continue;
        }
        if( VarGetBoolVal( UnInstall ) || FileInfo[DLLsToCheck[i].index].remove
          || ( !FileInfo[DLLsToCheck[i].index].add && GetVariableBoolVal( "ReInstall" ) ) ) {
            if( DecrementDLLUsageCount( &DLLsToCheck[i].full_path ) == 0 ) {
                if( MsgBoxVbuf( MainWnd, "IDS_REMOVE_DLL", GUI_YES_NO, &DLLsToCheck[i].full_path ) == GUI_RET_YES ) {
                    FileInfo[DLLsToCheck[i].index].add = false;
                    FileInfo[DLLsToCheck[i].index].remove = true;
                }
            }
        } else if( FileInfo[DLLsToCheck[i].index].add ) {
            IncrementDLLUsageCount( &DLLsToCheck[i].full_path );
        }
    }
}

static bool CheckDLLSupplemental( int i, const VBUF *filename )
/*************************************************************/
{
    bool    ok;

    // if ( supplemental is_dll & ) then we want to
    // keep a usage count of this dll.  Store its full path for later.
    ok = true;
    if( FileInfo[i].supplemental ) {
        VBUF    ext;

        VbufInit( &ext );
        VbufSplitpath( filename, NULL, NULL, NULL, &ext );
        if( VbufCompExt( &ext, "dll", true ) == 0 ) {
            VBUF        file_desc;
            VBUF        dir;
            VBUF        dst_path;
            bool        flag;
            int         m;

            VbufInit( &dst_path );
            VbufInit( &dir );
            VbufInit( &file_desc );
            SimFileDir( i, &dir );
            SimGetFileDesc( i, &file_desc );
            VbufMakepath( &dst_path, NULL, &dir, &file_desc, NULL );
            VbufFree( &file_desc );
            VbufFree( &dir );

            flag = false;
            for( m = 0; m < SetupInfo.dlls_to_count.num; m++ ) {
                if( VbufCompVbuf( &dst_path, &DLLsToCheck[m].full_path, true ) == 0 ) {
                    flag = true;
                    break;
                }
            }
            if( !flag ) {
                ok = BumpArray( &SetupInfo.dlls_to_count );
                if( ok ) {
                    VbufSetVbuf( &DLLsToCheck[SetupInfo.dlls_to_count.num - 1].full_path, &dst_path );
                    DLLsToCheck[SetupInfo.dlls_to_count.num - 1].index = i;
                }
            }
            VbufFree( &dst_path );
        }
        VbufFree( &ext );
    }
    return( ok );
}
#endif

void SimCalcAddRemove( void )
/***************************/
{
    int                 i;
    int                 j;
    int                 k;
    int                 targ_index = 0;
    int                 dir_index;
    unsigned            cs; /* cluster size */
    bool                previous;
    bool                add;
    bool                uninstall;
    bool                remove;
    vhandle             reinstall;
    bool                ok;

    // for each file that will be installed, total the size
    if( NeedInitAutoSetValues ) {
        InitAutoSetValues();
    }

    previous = VarGetBoolVal( PreviousInstall );
    uninstall = VarGetBoolVal( UnInstall );
    // look for existence of ReInstall variable - use this to decide
    // if we should remove unchecked components (wanted for SQL installs)
    reinstall = GetVariableByName( "ReInstall" );
    if( reinstall != NO_VAR ) {
        // it is defined, treat same as PreviousInstall
        previous = VarGetBoolVal( reinstall );
    }
    ok = true;
    for( i = 0; ok && i < SetupInfo.files.num; ++i ) {
        dir_index = FileInfo[i].dir_index;
        targ_index = DirInfo[dir_index].target;
        add = EvalExprTree( FileInfo[i].condition.p->cond );
        if( FileInfo[i].supplemental ) {
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
            DirInfo[dir_index].num_files += FileInfo[i].num_files;
        }
        TargetInfo[targ_index].num_files += FileInfo[i].num_files;
        cs = GetClusterSize( *TargetInfo[targ_index].temp_disk );
        FileInfo[i].remove = remove;
        FileInfo[i].add = add;
        for( k = 0; k < FileInfo[i].num_files; ++k ) {
            a_file_info *file = &FileInfo[i].files[k];
            if( file->size == 0 )
                continue;
            if( file->disk_size != 0 ) {
                DirInfo[dir_index].num_existing++;
                if( !TargetInfo[targ_index].supplemental ) {
                    SetBoolVariableByHandle( PreviousInstall, true );
                }
            }

#if defined( __NT__ )
            // if ( supplemental is_dll & ) then we want to
            // keep a usage count of this dll.  Store its full path for later.
            ok = CheckDLLSupplemental( i, &file->name );
            if( !ok ) {
                break;
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
    if( ok ) {
        /* Estimate space used for directories. Be generous. */
        if( !uninstall ) {
            for( i = 0; i < SetupInfo.target.num; ++i ) {
                cs = GetClusterSize( *TargetInfo[i].temp_disk );
                for( j = 0; j < SetupInfo.dirs.num; ++j ) {
                    if( DirInfo[j].target != i )
                        continue;
                    if( !DirInfo[j].used )
                        continue;
                    if( DirInfo[j].num_files <= DirInfo[j].num_existing )
                        continue;
                    TargetInfo[i].space_needed += RoundUp( ((( DirInfo[j].num_files - DirInfo[j].num_existing ) / 10) + 1) * 1024UL, cs);
                }
            }
        }
    }
}


bool SimCalcTargetSpaceNeeded( void )
/***********************************/
{
    int                 i;
    gui_mcursor_handle  old_cursor;
    const char          *temp;
    VBUF                temp_path;
    bool                ok;

    /* assume power of 2 */

    if( NeedGetDiskSizes ) {
        if( !GetDiskSizes() )
            return( false );
        NeedGetDiskSizes = false;
    }
    old_cursor = GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
    /*
     * Reset Targets info
     */
    ok = true;
    VbufInit( &temp_path );
    for( i = 0; i < SetupInfo.target.num; ++i ) {
        temp = SimGetTargetDriveLetter( i, &temp_path );
        if( temp == NULL ) {
            ok = false;
            break;
        }
        strcpy( TargetInfo[i].temp_disk, temp );
        TargetInfo[i].space_needed = 0;
        TargetInfo[i].num_files = 0;
        TargetInfo[i].needs_update = false;
    }
    VbufFree( &temp_path );
    /*
     * Reset Dirs info
     */
    if( ok ) {
        for( i = 0; i < SetupInfo.dirs.num; ++i ) {
            DirInfo[i].used = false;
            DirInfo[i].num_existing = 0;
            DirInfo[i].num_files = 0;
        }
    }
    /*
     * setup Targets and Dirs info
     */
    if( ok ) {
        SimCalcAddRemove();
    }
    GUIResetMouseCursor( old_cursor );
    return( ok );
}


/* ********** Free up all structures associated with this file ******* */


static void FreeSetupInfoVal( void )
/**********************************/
{
    if( SetupInfo.pm_group_file != NULL ) {
        GUIMemFree( SetupInfo.pm_group_file );
        SetupInfo.pm_group_file = NULL;
    }
    if( SetupInfo.pm_group_name != NULL ) {
        GUIMemFree( SetupInfo.pm_group_name );
        SetupInfo.pm_group_name = NULL;
    }
    if( SetupInfo.pm_group_iconfile != NULL ) {
        GUIMemFree( SetupInfo.pm_group_iconfile );
        SetupInfo.pm_group_iconfile = NULL;
    }
}


static void FreeTargetVal( void )
/*******************************/
{
    int i;

    if( TargetInfo != NULL ) {
        for( i = 0; i < SetupInfo.target.num; i++ ) {
            GUIMemFree( TargetInfo[i].name );
            GUIMemFree( TargetInfo[i].temp_disk );
        }
        GUIMemFree( TargetInfo );
        TargetInfo = NULL;
        SetupInfo.target.num = 0;
    }
}


static void FreeDirInfo( void )
/*****************************/
{
    int i;

    if( DirInfo != NULL ) {
        for( i = 0; i < SetupInfo.dirs.num; i++ ) {
            GUIMemFree( DirInfo[i].desc );
        }
        GUIMemFree( DirInfo );
        DirInfo = NULL;
        SetupInfo.dirs.num = 0;
    }
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
                VbufFree( &FileInfo[i].files[j].name );
            }
            GUIMemFree( FileInfo[i].files );
        }
        GUIMemFree( FileInfo );
        FileInfo = NULL;
        SetupInfo.files.num = 0;
    }
}

static void FreeDLLsToCheck( void )
/*********************************/
{
    int i;

    if( DLLsToCheck != NULL ) {
        for( i = 0; i < SetupInfo.dlls_to_count.num; i++ ) {
            VbufFree( &DLLsToCheck[i].full_path );
        }
        GUIMemFree( DLLsToCheck );
        DLLsToCheck = NULL;
        SetupInfo.dlls_to_count.num = 0;
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
        SetupInfo.fileconds.num = 0;
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
        SetupInfo.force_DLL_install.num = 0;
    }
}

static void FreeSpawnInfo( void )
/*******************************/
{
    int i;

    if( SpawnInfo != NULL ) {
        for( i = 0; i < SetupInfo.spawn.num; i++ ) {
            GUIMemFree( SpawnInfo[i].command );
            GUIMemFree( SpawnInfo[i].condition );
        }
        GUIMemFree( SpawnInfo );
        SpawnInfo = NULL;
        SetupInfo.spawn.num = 0;
    }
}


static void FreeDeleteInfo( void )
/*******************************/
{
    int i;

    if( DeleteInfo != NULL ) {
        for( i = 0; i < SetupInfo.delete.num; i++ ) {
            GUIMemFree( DeleteInfo[i].name );
        }
        GUIMemFree( DeleteInfo );
        DeleteInfo = NULL;
        SetupInfo.delete.num = 0;
    }
}


static void FreePMInfo( void )
/****************************/
{
    int i;

    if( PMInfo != NULL ) {
        for( i = 0; i < SetupInfo.pm_files.num; i++ ) {
            GUIMemFree( PMInfo[i].desc );
            GUIMemFree( PMInfo[i].filename );
            GUIMemFree( PMInfo[i].parameters );
            GUIMemFree( PMInfo[i].iconfile );
            GUIMemFree( PMInfo[i].condition );
        }
        GUIMemFree( PMInfo );
        PMInfo = NULL;
        SetupInfo.pm_files.num = 0;
    }
}


static void FreeProfileInfo( void )
/*********************************/
{
    int i;

    if( ProfileInfo != NULL ) {
        for( i = 0; i < SetupInfo.profile.num; i++ ) {
            GUIMemFree( ProfileInfo[i].hive_name );
            GUIMemFree( ProfileInfo[i].app_name );
            GUIMemFree( ProfileInfo[i].key_name );
            GUIMemFree( ProfileInfo[i].value );
            GUIMemFree( ProfileInfo[i].file_name );
            GUIMemFree( ProfileInfo[i].condition );
        }
        GUIMemFree( ProfileInfo );
        ProfileInfo = NULL;
        SetupInfo.profile.num = 0;
    }
}


static void FreeOneConfigInfo( array_info *info, struct config_info *array )
/**************************************************************************/
{
    size_t  i;

    for( i = 0; i < info->num; i++ ) {
        GUIMemFree( array[i].var );
        GUIMemFree( array[i].value );
        GUIMemFree( array[i].condition );
    }
    GUIMemFree( array );
    info->num = 0;
}

static void FreeEnvironmentInfo( void )
{
    if( EnvironmentInfo != NULL ) {
        FreeOneConfigInfo( &SetupInfo.environment, EnvironmentInfo );
        EnvironmentInfo = NULL;
    }
}

static void FreeAutoExecInfo( void )
{
    if( AutoExecInfo != NULL ) {
        FreeOneConfigInfo( &SetupInfo.autoexec, AutoExecInfo );
        AutoExecInfo = NULL;
    }
}

static void FreeConfigInfo( void )
{
    if( ConfigInfo != NULL ) {
        FreeOneConfigInfo( &SetupInfo.config, ConfigInfo );
        ConfigInfo = NULL;
    }
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
        SetupInfo.upgrade.num = 0;
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
        SetupInfo.label.num = 0;
    }
}

static void FreeAllPMGroups( void )
/*********************************/
{
    int i;

    if( AllPMGroups != NULL ) {
        for( i = 0; i < SetupInfo.all_pm_groups.num; i++ ) {
            GUIMemFree( AllPMGroups[i].group_name );
            GUIMemFree( AllPMGroups[i].group_file );
        }
        GUIMemFree( AllPMGroups );
        AllPMGroups = NULL;
        SetupInfo.all_pm_groups.num = 0;
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
            GUIMemFree( AssociationInfo[i].description );
            GUIMemFree( AssociationInfo[i].program );
            GUIMemFree( AssociationInfo[i].iconfile );
            GUIMemFree( AssociationInfo[i].condition );
        }
        GUIMemFree( AssociationInfo );
        AssociationInfo = NULL;
        SetupInfo.associations.num = 0;
    }
}

void FreeAllStructs( void )
/*************************/
{
    FreeTargetVal();
    FreeDirInfo();
    FreeFileInfo();
    FreeFileCondInfo();
    FreePMInfo();
    FreeProfileInfo();
    FreeEnvironmentInfo();
    FreeAutoExecInfo();
    FreeConfigInfo();
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


bool SimGetSpawnCommand( int i, VBUF *buff )
/******************************************/
{
    if( SpawnInfo[i].command == NULL || SpawnInfo[i].command[0] == '\0' ) {
        VbufRewind( buff );
        return( true );
    }
    ReplaceVars( buff, SpawnInfo[i].command );
    return( false );
}

bool SimEvalSpawnCondition( int i )
/*********************************/
{
    return( EvalCondition( SpawnInfo[i].condition ) );
}

when_time SimWhen( int i )
/************************/
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

    for( var_handle = NextGlobalVar( NO_VAR ); var_handle != NO_VAR; var_handle = NextGlobalVar( var_handle ) ) {
        if( VarGetAutoSetCond( var_handle ) != NULL ) {
            SetBoolVariableByHandle( var_handle, VarIsRestrictedTrue( var_handle ) );
        }
    }
}


void SetDefaultAutoSetValue( vhandle var_handle )
/***********************************************/
{
    const char      *cond;

    cond = VarGetAutoSetCond( var_handle );
    if( cond != NULL ) {
        SetBoolVariableByHandle( var_handle, EvalCondition( cond ) );
    }
}

static void InitAutoSetValues( void )
/***********************************/
{
    vhandle     var_handle;

    for( var_handle = NextGlobalVar( NO_VAR ); var_handle != NO_VAR; var_handle = NextGlobalVar( var_handle ) ) {
        SetDefaultAutoSetValue( var_handle );
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
    for( token = strtok( str2, " " ); token != NULL; token = strtok( NULL, " " ) ) {
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
