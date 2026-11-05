/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parse Windows dialog resource script and
*               convert it into OS/2 dialog resource script.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "bool.h"
#include "watcom.h"
#include "parsedlg.h"

#include "clibext.h"


#define OLD_FORMAT

#define MAX_NAME_LEN    122
#define MAX_LINE_LEN    255
#define MAX_STMT_LEN    20
#define MAX_SYM_LEN     30
#define MAX_PARM_LEN    45

#define MAX_STMT_PARMS  14

#define MAX_OPT_LEN     6

#define ADD_AFTER   true
#define ADD_BEFORE  false
#define ADD_OVERWR  false

#define CONV_X      54 / 46
#define CONV_Y      52 / 62

#if defined( OLD_FORMAT )
#define STR_SPC     "    "
#else
#define STR_SPC     "\t"
#endif

#define DEFAULT_FONT_NAME   "Helv"
#define DEFAULT_FONT_SIZE   10

#define SEPARATORS1     " \t,|"
#define SEPARATORS2     " \t,"
#define SEPARATORS3     " \t"

typedef struct _statement {
    char    name[MAX_NAME_LEN];
    char    text[MAX_LINE_LEN];
    char    ID[MAX_NAME_LEN];
    int     x;
    int     y;
    int     dx;
    int     dy;
    char    *parms[MAX_STMT_PARMS];
} statement;

static const char   UsageText[] = {
    "Usage: parsedlg [options] <in_file> [<out_file>]\0"
    "   Options:\0"
    "     -hide           : create dialogs as hidden, using -f=filename as input\0"
    "     -quiet          : suppress output\0"
    "     -f=filename     : a file containing a list of dialogs to be hidden\0"
    "     -font=font name : dialog font overwrite\0"
};

struct _options {
    bool    quiet;
    bool    hide;
    bool    font;
    char    font_name[MAX_NAME_LEN];
    long    font_size;
    int     flist_cnt;
    char    **flist_data;
} opt;

statement   dlg_hdr;
statement   dlg_item;
int         dialogs_cnt = 0;

static char *my_fgets( char *buf, int max_len, FILE *fp )
/*******************************************************/
{
    char    *rc;
    size_t  i;

    if( (rc = fgets( buf, max_len, fp )) != NULL ) {
        i = strlen( buf );
        while( i-- > 0
          && isspace( ((unsigned char *)buf)[i] ) ) {
            buf[i] = '\0';
        }
    }
    return( rc );
}

static void disp_usage( void )
/****************************/
{
    const char  *p;

    for( p = UsageText; *p != '\0'; ) {
        puts( p );
        while( *p++ != '\0' ) {
            /* nothing */;
        }
    }
}

static void process_f_option( const char *fname )
/***********************************************/
{
    FILE    *fp;
    char    buff1[MAX_NAME_LEN];
    int     i;

    if( ( fname != NULL )
      && ( *fname != '\0' ) ) {
        if( (fp = fopen( fname, "r" )) != NULL ) {
            for( opt.flist_cnt = 0; !feof( fp ); ++opt.flist_cnt )
                fgets( buff1, sizeof( buff1 ), fp );
            fseek( fp, 0, SEEK_SET );
            opt.flist_data = malloc( opt.flist_cnt * sizeof( char * ) );
            for( i = 0; i < opt.flist_cnt; ++i ) {
                my_fgets( buff1, sizeof( buff1 ), fp );
                opt.flist_data[i] = malloc( strlen( buff1 ) + 1 );
                strcpy( opt.flist_data[i], buff1 );
            }
            fclose( fp );
        }
    }
}

static int process_cmdl( int argc, char *argv[] )
/***********************************************/
{
    char            *p;
    char            opt_buf[MAX_OPT_LEN + 1];
    int             i;
    int             j;
    int             c;

    opt.quiet = false;
    opt.hide = false;
    opt.font = false;
    *opt.font_name = '\0';
    opt.font_size = 0;
    opt.flist_cnt = 0;
    opt.flist_data = NULL;
    for( i = 1; i < argc; ++i ) {
        p = argv[i];
#if defined( __UNIX__ )
        if( *p != '-' )
#else
        if( ( *p != '-' )
          && ( *p != '/' ) )
#endif
            break;
        ++p;
        j = 0;
        while( j < MAX_OPT_LEN
          && (c = *(unsigned char *)p) != '\0'
          && c != '=' ) {
            opt_buf[j++] = (char)tolower( c );
            ++p;
        }
        opt_buf[j] = '\0';
        if( strcmp( opt_buf, "f" ) == 0 ) {
            if( *p == '=' ) {
                p++;
                process_f_option( p );
            }
        } else if( strcmp( opt_buf, "hide" ) == 0 ) {
            opt.hide = true;
        } else if( strcmp( opt_buf, "quiet" ) == 0 ) {
            opt.quiet = true;
        } else if( strcmp( opt_buf, "font" ) == 0 ) {
            if( *p == '=' ) {
                p++;
                opt.font = true;
                opt.font_size = strtol( p, &p, 10 );
                if( *p == '.' )
                    ++p;
                strcpy( opt.font_name, p );
            }
        }
    }
    if( i == argc ) {
        return( 0 );
    } else if( argc - i > 2 ) {
        return( 0 );
    } else {
        return( i );
    }
}

static const char *skip_separators2( const char *str )
/****************************************************/
{
    while( ( *str == ' ' )
      || ( *str == '\t' )
      || ( *str == ',' ) ) {
        ++str;
    }
    return( str );
}

static void get_statement_name( const char *str, char *stmt_buf )
/***************************************************************/
{
    int     i;
    int     c;

    i = 0;
    if( str != NULL ) {
        while( i < MAX_STMT_LEN
          && (c = ((unsigned char *)str)[i]) != '\0'
          && strchr( SEPARATORS1, c ) == NULL ) {
            stmt_buf[i++] = (char)toupper( c );
        }
    }
    stmt_buf[i] = '\0';
}

static control_type check_control( const char *str, const char *extra_str )
/*************************************************************************/
{
    char            stmt_buf[MAX_STMT_LEN + 1];
    control_type    control;

    get_statement_name( str, stmt_buf );
    for( control = 0; control < CTRL_TYPE_CNT; control++ ) {
        if( strcmp( stmt_buf, control_type_win[control] ) == 0 ) {
            return( control );
        }
    }
    if( extra_str != NULL && strcmp( stmt_buf, extra_str ) == 0 ) {
        return( control );
    }
    return( T_NONE );
}

static bool is_control( const char *str )
/***************************************/
{
    if( *str != '\0' ) {
        while( isspace( *str ) )
            ++str;
        if( check_control( str, "END" ) != T_NONE ) {
            return( true );
        }
    }
    return( false );
}

static char *skip_keyword( char *str, int *plen )
/***********************************************/
{
    int     flag;
    int     len;
    char    *p;
    char    *p2;

    flag = 0;
    while( ( *str != '"' )
      && !isdigit( *str )
      && ( *str != '+' )
      && ( *str != '-' )
      && ( *str != '\0' ) ) {
        if( flag == 0 ) {
            if( isalpha( *str ) ) {
                flag = 1;
            }
        } else if( flag == 1 ) {
            if( ( *str == '\t' )
              || ( *str == ' ' ) ) {
                flag = 2;
            }
        } else if( flag == 2 ) {
            if( isalpha( *str ) ) {
                flag = 3;
                break;
            }
        }
        ++str;
    }
    len = 0;
    if( *str == '"' ) {
        len = 2;
        for( p = str + 1; *p != '"'; ++p ) {
            if( *p == '&' ) {
                if( *(p + 1) != '&' ) {
                    *p = '~';
                } else {
                    for( p2 = p; *p2 != '\0'; ++p2 ) {
                        *p2 = *(p2 + 1);
                    }
                }
            }
            ++len;
        }
    } else if( *str != '\0' ) {
        len = 1;
        for( p = str + 1; isdigit( *p ) || ( ( flag == 3 ) && isalpha( *p ) ); ++p ) {
            ++len;
        }
    }
    *plen = len;
    return( str );
}

static bool check_control_style( style idx, control_type control )
/****************************************************************/
{
    switch( control ) {
    case T_LTEXT:
    case T_RTEXT:
    case T_CTEXT:
    case T_EDITTEXT:
        return( ( idx >= T_CONTROL_ES_LEFT ) && ( idx <= T_CONTROL_ES_OEMCONVERT ) );
    case T_LISTBOX:
        return( ( idx >= T_CONTROL_LBS_STANDARD ) && ( idx <= T_CONTROL_LBS_WANTKEYBOARDINPUT ) );
    case T_GROUPBOX:
    case T_ICON:
        return( ( idx >= T_CONTROL_SS_LEFT ) && ( idx <= T_CONTROL_SS_USERITEM ) );
    case T_COMBOBOX:
        return( ( idx >= T_CONTROL_CBS_SIMPLE ) && ( idx <= T_CONTROL_CBS_OEMCONVERT ) );
    case T_SCROLLBAR:
        return( ( idx >= T_CONTROL_SBS_VERT ) && ( idx <= T_CONTROL_SBS_SIZEBOXBOTTOMRIGHTALIGN ) );
    case T_CONTROL:
        return( true );
    case T_CHECKBOX:
    case T_PUSHBUTTON:
    case T_DEFPUSHBUTTON:
    case T_RADIOBUTTON:
        return( ( idx >= T_CONTROL_BS_PUSHBUTTON ) && ( idx <= T_CONTROL_BS_OWNERDRAW ) );
    default:
        return( false );
    }
}

static bool process_parms_check( char *parms[], char *symbol )
/************************************************************/
{
    int     i;
    char    *parm;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        parm = parms[i];
        if( *parm != '\0'
          && strstr( parm, symbol ) != NULL ) {
            return( true );
        }
    }
    return( false );
}

static void process_parms_convert( char *parms[], const char * const * win_tab,
    const char * const * os2_tab, int tab_cnt, control_type control )
/*****************************************************************************/
{
    int     i;
    int     j;
    char    sym_buf[MAX_SYM_LEN + 1];
    char    *parm;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        parm = parms[i];
        if( *parm != '\0' ) {
            if( win_tab == control_class_win ) {
                get_statement_name( parm, sym_buf );
            } else {
                strncpy( sym_buf, parm, MAX_SYM_LEN );
                sym_buf[MAX_SYM_LEN] = '\0';
            }
            for( j = 0; j < tab_cnt; ++j ) {
                if( strcmp( sym_buf, win_tab[j] ) == 0 ) {
                    if( ( win_tab == control_style_win )
                      && !check_control_style( j, control ) ) {
                        *parm = '\0';
                    } else {
                        strcpy( parm, os2_tab[j] );
                    }
                    break;
                }
            }
        }
    }
}

static void convert_buttons( char *ID, char *name, int flag )
/***********************************************************/
{
    if( strcmp( ID, "IDOK" ) == 0 ) {
        strcpy( ID, "DID_OK" );
        if( flag ) {
            strcpy( name, "DEFPUSHBUTTON" );
        }
    } else if( strcmp( ID, "IDCANCEL" ) == 0 ) {
        strcpy( ID, "DID_CANCEL" );
    } else if( ( strcmp( ID, "IDHELP" ) == 0 )
      || ( strcmp( ID, "IDC_HELP" ) == 0 ) ) {
        strcpy( ID, "DID_HELP_BUTTON" );
    }
}

static void add_parms_item( char *parms[], const char *str, bool after )
/**********************************************************************/
{
    int     i;
    char    *parm;

    if( after ) {
        for( i = 0; i < MAX_STMT_PARMS; ++i ) {
            parm = parms[i];
            if( *parm == '\0' ) {
                strcpy( parm, str );
                return;
            }
        }
        fprintf( stderr, "Error - number of the internal parameter overflow!\n" );
    } else {
        if( *parms[MAX_STMT_PARMS - 1] != '\0' )
            fprintf( stderr, "Error - number of the internal parameter overflow!\n" );
        for( i = MAX_STMT_PARMS - 1; i; --i ) {
            if( *parms[i - 1] != '\0' ) {
                strcpy( parms[i], parms[i - 1] );
            }
        }
        strcpy( parms[0], str );
    }
}

static void add_parms_list( statement *stmt, bool after )
/*******************************************************/
{
    int     i;
    char    *p;

    i = 0;
    if( after ) {
        for( ; i < MAX_STMT_PARMS; ++i ) {
            if( *stmt->parms[i] == '\0' ) {
                break;
            }
        }
    }
    for( ; i < MAX_STMT_PARMS; ++i ) {
        p = strtok( NULL, SEPARATORS1 );
        if( p == NULL ) {
            if( after )
                break;
            *stmt->parms[i] = '\0';
        } else {
            strcpy( stmt->parms[i], p );
        }
    }
    if( ( strcmp( stmt->name + 1, "TEXT" ) == 0 )
      || ( strcmp( stmt->name, "GROUPBOX" ) == 0 )
      || ( strcmp( stmt->name, "CONTROL" ) == 0 ) ) {
        if( !process_parms_check( stmt->parms, "DT_MNEMONIC" )
          && strcmp( stmt->text, "\"\"" ) ) {
            add_parms_item( stmt->parms, "DT_MNEMONIC", ADD_AFTER );
        }
    }
    if( ( strcmp( stmt->name + 1, "TEXT" ) == 0 )
      && ( strlen( stmt->text ) > 10 ) ) {
        if( !process_parms_check( stmt->parms, "DT_WORDBREAK" ) ) {
            add_parms_item( stmt->parms, "DT_WORDBREAK", ADD_AFTER );
        }
    }
}

#if 0
static void remove_parms_item( char *parms[], const char *str )
/*************************************************************/
{
    int     i;
    char    *parm;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        parm = parms[i];
        if( *parm != '\0' ) {
            if( strcmp( parm, str ) == 0 ) {
                *parm = '\0';
                return;
            }
        }
    }
}
#endif

static void convert_font( char *parms[], int parms_cnt )
/******************************************************/
{
    int     i;
    int     j;
    char    *parm;

    if( opt.font ) {
        for( i = 0; i < parms_cnt; ++i ) {
            strcpy( parms[i], opt.font_name );
        }
    } else {
        for( i = 0; i < parms_cnt; ++i ) {
            parm = parms[i];
            if( *parm != '\0' ) {
                for( j = 0; j < FONT_CNT; ++j ) {
                    if( strcmp( parm, font_win[j] ) == 0 ) {
                        strcpy( parm, font_os2[j] );
                        break;
                    }
                }
            }
        }
    }
}

static void out_parms_style( FILE *fo, statement *stmt )
/******************************************************/
{
    bool    oper_NOT;
    int     item_idx;
    int     i;
    char    *p;

    oper_NOT = false;
    item_idx = 0;
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = stmt->parms[i];
        if( ( *p != '\0' )
          && !isdigit( *p ) ) {
#if defined( OLD_FORMAT )
            if( item_idx == 1 ) {
                fprintf( fo, "\n\t\t" );
            }
#endif
            if( item_idx == 0 ) {
                fprintf( fo, ", " );
#if !defined( OLD_FORMAT )
                fprintf( fo, "\n\t\t" );
                if( strcmp( stmt->name, "DIALOG" ) != 0 ) {
                    fprintf( fo, "\t" );
                }
#endif
            } else if( ( item_idx == 1 )
              && ( strcmp( stmt->name, "CONTROL" ) == 0 ) ) {
                fprintf( fo, " , " );
            } else if( !oper_NOT ) {
                fprintf( fo, " | " );
            } else {
                fprintf( fo, " " );
            }
            if( ( strcmp( p, "SS_WHITEFRAME" ) == 0 )
              || ( strcmp( p, "SS_BLACKFRAME" ) == 0 )
              || ( strcmp( p, "SS_GRAYFRAME" ) == 0 ) ) {
                fprintf( fo, "SS_BKGNDFRAME" );
            } else if( ( strcmp( p, "SS_WHITERECT" ) == 0 )
              || ( strcmp( p, "SS_BLACKRECT" ) == 0 )
              || ( strcmp( p, "SS_GRAYRECT" ) == 0 ) ) {
                fprintf( fo, "SS_BKGNDRECT" );
            } else {
                fprintf( fo, "%s", p );
            }
            oper_NOT = ( strcmp( p, "NOT" ) == 0 );
            ++item_idx;
        }
    }
    fprintf( fo, "\n" );
}

static void process_style( statement *stmt )
/******************************************/
{
    control_type    control;
    int             i;
    const char      *new_str;

    new_str = NULL;
    control = check_control( stmt->name, NULL );
    if( control != T_NONE ) {
        new_str = control_type_os2[control];
    }
    if( control == T_COMBOBOX ) {
        if( process_parms_check( stmt->parms, "CBS_DROPDOWNLIST" ) ) {
            if( process_parms_check( stmt->parms, "CBS_OWNERDRAW" ) ) {
                new_str = "CONTROL";
                add_parms_item( stmt->parms, "\"watcombo\"", ADD_BEFORE );
            }
        }
    } else if( control == T_CONTROL ) {
        if( process_parms_check( stmt->parms, "BS_GROUPBOX" ) ) {
            new_str = "GROUPBOX";
            control = T_GROUPBOX;
            for( i = 0; i < MAX_STMT_PARMS; ++i ) {
                *stmt->parms[i] = '\0';
            }
        }
    }
    if( new_str != NULL )
        strcpy( stmt->name, new_str );
    if( control != T_NONE )
        process_parms_convert( stmt->parms, style_win[control], style_os2[control], style_cnt[control], control );
    if( control == T_CONTROL ) {
        process_parms_convert( stmt->parms, control_class_win, control_class_os2, CTRL_NAME_CNT, control );
        if( process_parms_check( stmt->parms, "WC_STATIC" ) ) {
            if( !process_parms_check( stmt->parms, "SS_" ) ) {
                add_parms_item( stmt->parms, "SS_GROUPBOX", ADD_AFTER );
            }
        }
    } else {
        process_parms_convert( stmt->parms, control_style_win, control_style_os2, CTRL_STYLE_CNT, control );
    }
    if( !process_parms_check( stmt->parms, "WS_VISIBLE" ) ) {
        add_parms_item( stmt->parms, "WS_VISIBLE", ADD_AFTER );
    }
    process_parms_convert( stmt->parms, window_style_win, window_style_os2, WND_STYLE_CNT, control );
}

static void out_color_style( FILE *fo, statement *stmt )
/******************************************************/
{
    int     i;
    char    *p;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = stmt->parms[i];
        if( ( strcmp( p, "SS_WHITEFRAME" ) == 0 )
          || ( strcmp( p, "SS_WHITERECT" ) == 0 ) ) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, RGB_WHITE\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, RGB_WHITE\n", STR_SPC STR_SPC );
        } else if( ( strcmp( p, "SS_BLACKFRAME" ) == 0 )
          || ( strcmp( p, "SS_BLACKRECT" ) == 0 ) ) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, RGB_BLACK\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, RGB_BLACK\n", STR_SPC STR_SPC );
        } else if( ( strcmp( p, "SS_GRAYFRAME" ) == 0 )
          || ( strcmp( p, "SS_GRAYRECT" ) == 0 ) ) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, 0x00C0C0C0L\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, 0x00C0C0C0L\n", STR_SPC STR_SPC );
        }
        *p = '\0';
    }
}

static void get_rectangle_list( statement *stmt )
/***********************************************/
{
    char    *p;

    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL ) {
        stmt->x = atoi( p ) * CONV_X;
    }
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL ) {
        stmt->y = atoi( p ) * CONV_Y;
    }
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL ) {
        stmt->dx = atoi( p ) * CONV_X;
    }
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL ) {
        stmt->dy = atoi( p ) * CONV_Y;
    }
}

static void get_rectangle_parms( statement *stmt )
/************************************************/
{
    int     i;
    int     j;
    char    *p;

    j = 0;
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = stmt->parms[i];
        if( !isdigit( *p ) )
            continue;
        if( ( *p == '0' )
          && ( tolower( *(p + 1) ) == 'x' ) )
            continue;
        if( j == 0 ) {
            stmt->x = atoi( p ) * CONV_X;
        } else if( j == 1 ) {
            stmt->y = atoi( p ) * CONV_Y;
        } else if( j == 2 ) {
            stmt->dx = atoi( p ) * CONV_X;
        } else if( j == 3 ) {
            stmt->dy = atoi( p ) * CONV_Y;
        }
        ++j;
        *p = '\0';
    }
}

static bool process_statement( char *line, FILE *fo )
/***************************************************/
{
    char    buff1[MAX_LINE_LEN + 1];
    int     len;
    char    *p;

    if( *line == '\0' )
        return( false );
    strcpy( buff1, skip_separators2( line ) );
    p = strtok( buff1, SEPARATORS1 );
    get_statement_name( p, dlg_item.name );
    if( strcmp( dlg_item.name, "END" ) == 0 ) {
        fprintf( fo, "%s%s\nEND\n", STR_SPC, dlg_item.name );
        return( true );
    }
    if( strcmp( dlg_item.name, "CONTROL" ) == 0 ) {
        p = skip_keyword( line, &len );
        if( p != NULL ) {
            strncpy( dlg_item.text, p, len );
            dlg_item.text[len] = '\0';
            p += len;
        }
        p = strtok( p, SEPARATORS1 );
        if( p != NULL ) {
            strcpy( dlg_item.ID, p );
        } else {
            strcpy( dlg_item.ID, "-1" );
        }
        convert_buttons( dlg_item.ID, dlg_item.name, 1 );
        add_parms_list( &dlg_item, ADD_OVERWR );
        get_rectangle_parms( &dlg_item );
    } else {
        if( ( strcmp( dlg_item.name, "COMBOBOX" ) == 0 )
          || ( strcmp( dlg_item.name, "LISTBOX" ) == 0 )
          || ( strcmp( dlg_item.name, "SCROLLBAR" ) == 0 )
          || ( strcmp( dlg_item.name, "EDITTEXT" ) == 0 ) ) {
            strcpy( dlg_item.text, "\"\"" );
            p = NULL;
        } else {
            p = skip_keyword( line, &len );
            if( p != NULL ) {
                strncpy( dlg_item.text, p, len );
                dlg_item.text[len] = '\0';
                p += len;
                convert_buttons( dlg_item.text, dlg_item.text, 0 );
            } else {
                strcpy( dlg_item.text, "\"\"" );
                p = NULL;
            }
        }
        p = strtok( p, SEPARATORS1 );
        if( p != NULL ) {
            strcpy( dlg_item.ID, p );
        } else {
            strcpy( dlg_item.ID, "-1" );
        }
        convert_buttons( dlg_item.ID, dlg_item.name, 1 );
        get_rectangle_list( &dlg_item );
        add_parms_list( &dlg_item, ADD_OVERWR );
    }
    if( strcmp( dlg_item.name, "SCROLLBAR" ) == 0 ) {
        add_parms_item( dlg_item.parms, "WC_SCROLLBAR", ADD_BEFORE );
    }
    process_style( &dlg_item );
    dlg_item.y = dlg_hdr.dy - dlg_item.y - dlg_item.dy;
    fprintf( fo, "%s%s", STR_SPC STR_SPC, dlg_item.name );
    if( strlen( dlg_item.name ) < 8 )
        fprintf( fo, "\t" );
    fprintf( fo, "\t" );
    if( strcmp( dlg_item.name, "LISTBOX" ) )
        fprintf( fo, "%s, ", dlg_item.text );
    fprintf( fo, "%s, %d, %d, %d, %d", dlg_item.ID, dlg_item.x,
                          dlg_item.y, dlg_item.dx, dlg_item.dy );
    out_parms_style( fo, &dlg_item );
    out_color_style( fo, &dlg_item );
    return( false );
}

static void process_dialog_declaration( FILE *fi, FILE *fo, char *line, int max_len )
/***********************************************************************************/
{
    long    font_size;
    char    *font_name;
    bool    font_set;
    bool    hidden_dialog;
    char    *buff1;
    char    *p;
    char    **p2;
    size_t  len;
    bool    sysmodal;
    bool    visible;
    int     i;
    char    stmt_buf[MAX_STMT_LEN + 1];

    font_set = false;
    font_size = 0;
    font_name = NULL;
    hidden_dialog = false;
    ++dialogs_cnt;
    if( !opt.quiet ) {
        fprintf( stdout, "." );
    }
    // process DIALOG line
    buff1 = malloc( MAX_LINE_LEN );
    strcpy( buff1, skip_separators2( line ) );
    p = strtok( buff1, SEPARATORS1 );
    if( p != NULL )
        strcpy( dlg_hdr.ID, p );
    strtok( NULL, SEPARATORS2 );    /* skip DIALOG */
    p = strtok( NULL, SEPARATORS2 );
    while( ( p != NULL )
      && !isdigit( *p ) ) {
        p = strtok( NULL, SEPARATORS1 );
    }
    if( p != NULL )
        dlg_hdr.x = atoi( p ) * CONV_X;
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL )
        dlg_hdr.y = atoi( p ) * CONV_Y;
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL )
        dlg_hdr.dx = atoi( p ) * CONV_X;
    p = strtok( NULL, SEPARATORS1 );
    if( p != NULL )
        dlg_hdr.dy = atoi( p ) * CONV_Y;
    dlg_hdr.y = 230 - dlg_hdr.y - dlg_hdr.dy;
    strcpy( dlg_hdr.text, "\"\"" );
    // process next lines
    my_fgets( line, max_len, fi );
    while( *line != '\0' ) {
        strcpy( buff1, line );
        p = strtok( buff1, SEPARATORS1 );
        get_statement_name( p, stmt_buf );
        if( strcmp( stmt_buf, "BEGIN" ) == 0 ) {
            break;
        } else if( strcmp( stmt_buf, "STYLE" ) == 0 ) {
            add_parms_list( &dlg_hdr, ADD_AFTER );
            len = strlen( line );
            for( p = line + len - 1; len && isspace( *p ); --len )
                *(p--) = '\0';
            if( *p == '|' ) {
                my_fgets( line, max_len, fi );
                strcpy( buff1, line );
                p = strtok( buff1, SEPARATORS1 );
                while( p != NULL ) {
                    add_parms_item( dlg_hdr.parms, p, ADD_AFTER );
                    p = strtok( NULL, SEPARATORS1 );
                }
            }
        } else if( strcmp( stmt_buf, "CAPTION" ) == 0 ) {
            p = strtok( NULL, "" );
            strcpy( dlg_hdr.text, p );
            dlg_hdr.text[strlen( dlg_hdr.text )] = '\0';
        } else if( strcmp( stmt_buf, "FONT" ) == 0 ) {
            font_set = true;
            p = strtok( NULL, SEPARATORS1 );
            font_size = strtol( p, &p, 10 );
            p = strtok( NULL, "\"" );
            p = strtok( NULL, "\"" );
            if( opt.font ) {
                if( font_name != NULL )
                    free( font_name );
                font_name = malloc( strlen( opt.font_name ) + 1 );
                strcpy( font_name, opt.font_name );
                if( opt.font_size != 0 ) {
                    font_size = opt.font_size;
                }
            } else {
                if( p != NULL ) {
                    if( font_name != NULL )
                        free( font_name );
                    font_name = malloc( strlen( p ) + 10 );
                    strcpy( font_name, p );
                    p2 = malloc( sizeof( char * ) );
                    *p2 = font_name;
                    convert_font( p2, 1 );
                    free( p2 );
                }
            }
        }
        my_fgets( line, max_len, fi );
    }
    process_parms_convert( dlg_hdr.parms, window_style_win, window_style_os2, WND_STYLE_CNT, T_NONE );
    if( !font_set ) {
        font_name = malloc( sizeof( DEFAULT_FONT_NAME ) );
        strcpy( font_name, DEFAULT_FONT_NAME );
        font_size = DEFAULT_FONT_SIZE;
    }
    sysmodal = process_parms_check( dlg_hdr.parms, "FCF_SYSMODAL" );
    if( opt.hide ) {
        for( i = 0; i < opt.flist_cnt; ++i ) {
            if( stricmp( dlg_hdr.ID, opt.flist_data[i] ) == 0 ) {
                hidden_dialog = true;
                break;
            }
        }
    }
    visible = process_parms_check( dlg_hdr.parms, "WS_VISIBLE" );
    fprintf( fo, "DLGTEMPLATE %s\n", dlg_hdr.ID );
    fprintf( fo, "BEGIN\n%sDIALOG %s, %s, %d, %d, %d, %d, ", STR_SPC, dlg_hdr.text,
               dlg_hdr.ID, dlg_hdr.x, dlg_hdr.y, dlg_hdr.dx, dlg_hdr.dy );
    if( hidden_dialog ) {
        fprintf( fo, "FS_BORDER | NOT FS_DLGBORDER | NOT WS_VISIBLE\n" );
    } else {
#if defined( OLD_FORMAT )
        if( sysmodal && visible ) {
            fprintf( fo, "FS_SYSMODAL\n\t\t| WS_VISIBLE" );
        } else if( sysmodal ) {
            fprintf( fo, "FS_SYSMODAL\n\t\t" );
        } else if( visible ) {
            fprintf( fo, "WS_VISIBLE\n\t\t" );
        } else {
            fprintf( fo, "0L\n\t\t" );
        }
#else
        if( sysmodal && visible ) {
            fprintf( fo, "\n\t\tFS_SYSMODAL | WS_VISIBLE" );
        } else if( sysmodal ) {
            fprintf( fo, "\n\t\tFS_SYSMODAL" );
        } else if( visible ) {
            fprintf( fo, "\n\t\tWS_VISIBLE" );
        } else {
            fprintf( fo, "\n\t\t0L" );
        }
#endif
        out_parms_style( fo, &dlg_hdr );
    }
    if( font_name != NULL ) {
        fprintf( fo, "%sPRESPARAMS PP_FONTNAMESIZE, ", STR_SPC );
        if( font_size != 0 ) {
            fprintf( fo, "\"%ld.%s\"\n", font_size, font_name );
        } else {
            fprintf( fo, "\"%s\"\n", font_name );
        }
        free( font_name );
    }
    fprintf( fo, "%sBEGIN\n", STR_SPC );
    free( buff1 );
}

static void alloc_statement( statement *stmt )
/********************************************/
{
    int i;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        stmt->parms[i] = malloc( MAX_PARM_LEN );
        *stmt->parms[i] = '\0';
    }
}

static void free_statement( statement *stmt )
/*******************************************/
{
    int i;

    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        free( stmt->parms[i] );
    }
}

int main( int argc, char *argv[] )
/********************************/
{
    char    fname[PATH_MAX];
    FILE    *fi;
    FILE    *fo;
    char    *p;
    int     i;
    char    *line;
    char    *buff1;
    char    *buff2;
    size_t  len;
    char    stmt_buf[MAX_STMT_LEN + 1];

    i = process_cmdl( argc, argv );
    if( i == 0 ) {
        disp_usage();
        return( -1 );
    }
    len = strlen( argv[i] );
    if( len > PATH_MAX - 1 )
        len = PATH_MAX - 1;
    memcpy( fname, argv[i], len );
    fname[len] = '\0';
    if( strrchr( fname, '.' ) == NULL )
        strcat( fname, ".dlg" );
    fi = fopen( fname, "r" );
    if( fi == NULL ) {
        printf( "Could not open input file: %s\n", fname );
        return( -1 );
    }
    if( i + 1 < argc ) {
        len = strlen( argv[i + 1] );
        if( len > PATH_MAX - 1 )
            len = PATH_MAX - 1;
        memcpy( fname, argv[i + 1], len );
        fname[len] = '\0';
        if( strrchr( fname, '.' ) == NULL ) {
            strcat( fname, ".dlg" );
        }
    } else {
        strcpy( fname, "os2dlg.dlg" );
    }
    fo = fopen( fname, "w" );
    if( fo == NULL ) {
        printf( "Could not open input file: %s\n", fname );
        return( -1 );
    }

    alloc_statement( &dlg_hdr );
    strcpy( dlg_hdr.name, "DIALOG" );
    alloc_statement( &dlg_item );

    line = malloc( MAX_LINE_LEN );

    buff1 = malloc( MAX_LINE_LEN );
    buff2 = malloc( MAX_LINE_LEN );

    my_fgets( line, MAX_LINE_LEN, fi );
    while( !feof( fi ) ) {
        while( !feof( fi ) ) {
            strcpy( buff1, line );
            p = buff1;
            while( isspace( *(unsigned char *)p ) )
                p++;
            p = strtok( p, SEPARATORS3 );
            get_statement_name( p, stmt_buf );
            if( strcmp( stmt_buf, "DLGINCLUDE" ) == 0 ) {
                /**********************
                 * source file format:
                 *
                 * DLGINCLUDE
                 * BEGIN
                 * filename
                 * END
                 *
                 * converted to:
                 *
                 * DLGINCLUDE 1 filename
                 */
                strcpy( buff1, line );
                fprintf( fo, "%s 1 ", strtok( buff1, SEPARATORS3 ) );
                my_fgets( line, MAX_LINE_LEN, fi );
                my_fgets( line, MAX_LINE_LEN, fi );
                strcpy( buff1, line );
                fprintf( fo, "%s\n", strtok( buff1, SEPARATORS3 ) );
                my_fgets( line, MAX_LINE_LEN, fi );
                my_fgets( line, MAX_LINE_LEN, fi );
            } else if( strcmp( stmt_buf, "BEGIN" ) == 0 ) {
                my_fgets( line, MAX_LINE_LEN, fi );
                break;
            } else {
                if( p != NULL )
                    p = strtok( NULL, SEPARATORS3 );
                get_statement_name( p, stmt_buf );
                if( strcmp( stmt_buf, "DIALOG" ) == 0 ) {
                    strcpy( buff1, line );
                    process_dialog_declaration( fi, fo, buff1, MAX_LINE_LEN );
                    strcpy( line, buff1 );
                } else {
#if !defined( OLD_FORMAT )
                    if( *line != '\0' )
#endif
                    fprintf( fo, "%s\n", line );
                    my_fgets( line, MAX_LINE_LEN, fi );
                }
            }
        }
        p = "";
        while( !feof( fi ) ) {
            get_statement_name( p, stmt_buf );
            if( strcmp( stmt_buf, "END" ) == 0 )
                break;
            while( my_fgets( buff1, MAX_LINE_LEN, fi ) != NULL ) {
                if( is_control( buff1 ) )
                    break;
                strncat( line, skip_separators2( buff1 ), MAX_LINE_LEN );
            }
            process_statement( line, fo );
            strcpy( line, buff1 );
            strcpy( buff2, buff1 );
            p = strtok( buff2, SEPARATORS3 );
        }
        if( !feof( fi ) ) {
            fprintf( fo, "%sEND\n", STR_SPC );
        }
    }
    free( buff2 );
    free( buff1 );

    free( line );

    free_statement( &dlg_item );
    free_statement( &dlg_hdr );
    if( fi != NULL )
        fclose( fi );
    if( fo != NULL ) {
#if defined( OLD_FORMAT )
        fprintf( fo, "\n" );
#endif
        fclose( fo );
    }
    if( !opt.quiet )
        fprintf( stdout, "\nParsed %d dialogs.\n", dialogs_cnt );
    if( opt.flist_data ) {
        for( i = 0; i < opt.flist_cnt; i++ )
            free( opt.flist_data[i] );
        free( opt.flist_data );
    }
    return( 0 );
}
