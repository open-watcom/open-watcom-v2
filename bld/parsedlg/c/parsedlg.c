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
* Description:  Parse Windows dialog resource script and 
*               convert it into OS/2 dialog resource script.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "watcom.h"
#include "parsedlg.h"

#include "clibext.h"


#define OLD_FORMAT

#define MAX_NAME_LEN    122
#define MAX_LINE_LEN    255

#define MAX_STMT_PARMS  14

#define MAX_PARM_LEN    45

#define isWSorCtrlZ(x)  (isspace(x)||(x==0x1A))

#define ADD_AFTER   1
#define ADD_BEFORE  0

#define CONV_X      54 / 46
#define CONV_Y      52 / 62

#if defined( OLD_FORMAT )
#define STR_SPC     "    "
#else
#define STR_SPC     "\t"
#endif

typedef struct _statement {
    char    name[ MAX_NAME_LEN ];
    char    text[ MAX_LINE_LEN ];
    char    ID[ MAX_NAME_LEN ];
    int     x;
    int     y;
    int     dx;
    int     dy;
    char    *parms[ MAX_STMT_PARMS ];
} statement;

typedef void (*fnx)(char*,char**,control_type,int,int,char*,char**,char**,int*);

char *usage[] = {
    "Usage: parsedlg [options] <in_file> [<out_file>]",
    "   Options:",
    "     -hide           : create dialogs as hidden, using -f=filename as input",
    "     -quiet          : suppress output",
    "     -f=filename     : a file containing a list of dialogs to be hidden",
    "     -font=font name : dialog font overwrite",
    NULL
};

char *options_text[] = {
    "f",
    "hide",
    "quiet",
    "font",
    NULL
};

struct _options {
    int     quiet;
    int     hide;
    int     font;
    char    font_name[ MAX_NAME_LEN ];
    long    font_size;
    int     flist_cnt;
    char    **flist_data;
} opt;

statement   dlg_hdr;
statement   dlg_item;
int         dialogs_cnt = 0;

char *my_fgets( char *buf, int n, FILE *fp )
/******************************************/
{
    char    *rc;
    size_t  i;
    
    if( (rc = fgets( buf, n, fp )) != NULL ) {
        for( i = strlen( buf ); i && isWSorCtrlZ( buf[ i - 1] ); --i ) {
            buf[ i - 1 ] = '\0';
        }
    }
    return( rc );
}

void disp_usage( void )
/*********************/
{
    char    **p;
    
    p = usage;
    while( *p ) {
        printf( "%s\n", *p++ );
    }
}

void process_f_option( char *fname )
/**********************************/
{
    FILE    *fp;
    char    buff1[ MAX_NAME_LEN ];
    int     i;

    if(( fname != NULL ) && ( *fname != '\0' )) {
        if( (fp = fopen( fname, "r" )) != NULL ) {
            for( opt.flist_cnt = 0; !feof( fp ); ++opt.flist_cnt )
                fgets( buff1, MAX_NAME_LEN, fp );
            fseek( fp, 0, SEEK_SET );
            opt.flist_data = malloc( opt.flist_cnt * sizeof(char *) );
            for( i = 0; i < opt.flist_cnt; ++i ) {
                my_fgets( buff1, MAX_NAME_LEN, fp );
                opt.flist_data[ i ] = malloc( strlen( buff1 ) + 1 );
                strcpy( opt.flist_data[ i ], buff1 );
            }
            fclose( fp );
        }
    }
}

int process_cmdl( int argc, char *argv[] )
/****************************************/
{
    char    *p;
    char    *o;
    int     i, j;
    size_t  len;

    opt.quiet = 0;
    opt.hide = 0;
    opt.font = 0;
    *opt.font_name = '\0';
    opt.font_size = 0;
    opt.flist_cnt = 0;
    opt.flist_data = NULL;
    for( i = 1; i < argc; ++i ) {
        p = argv[ i ];
#if defined( __UNIX__ )
        if( *p != '-' )
#else
        if(( *p != '-' ) && ( *p != '/' ))
#endif
            break;
        ++p;
        len = 0;
        for( j = 0; options_text[ j ] != NULL; ++j ) {
            o = options_text[ j ];
            len = strlen( o );
            if(( p[ len ] == '=' ) && ( strnicmp( o, p, len ) == 0 ))
                break;
            if( stricmp( o, p ) == 0 ) {
                break;
            }
        }
        switch( j ) {
        case 0: // "f"
            process_f_option( p + 2 );
            break;
        case 1: // "hide"
            opt.hide = 1;
            break;
        case 2: // "quiet"
            opt.quiet = 1;
            break;
        case 3: // "font"
            opt.font = 1;
            p += len + 1;
            opt.font_size = strtol( p, &p, 10 );
            if( *p == '.' )
                ++p;
            strcpy( opt.font_name, p );
            break;
        default:
            break;
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

char *skip_separator( char *str )
/*******************************/
{
    while(( *str == ' ' ) || ( *str == '\t' ) || ( *str == ',' ))
        ++str;
    return( str );
}

int check_statement( char *str )
/******************************/
{
    char    buff1[ MAX_LINE_LEN ];
    char    *p;
    int     i;
    
    if( *str != '\0' ) {
        while( isspace( *str ) )
            ++str;
        strcpy( buff1, str );
        strupr( buff1 );
        p = strtok( buff1, " \t," );
        for( i = 0; i < CTRL_TYPE_CNT; i++ ) {
            if( strcmp( p, control_type_win[ i ] ) == 0 ) {
                return( 1 );
            }
        }
        if( strcmp( p, "END" ) == 0 ) {
            return( 1 );
        }
    }
    return( 0 );
}
char *skip_keyword( char *str, int *plen )
/****************************************/
{
    int     flag;
    int     len;
    char    *p;
    char    *p2;
    
    flag = 0;
    while(( *str != '"' ) && !isdigit( *str ) && ( *str != '+' )
        && ( *str != '-' ) && ( *str != '\0' )) {
        if( flag == 0 ) {
            if( isalpha( *str ) ) {
                flag = 1;
            }
        } else if( flag == 1 ) {
            if(( *str == '\t' ) || ( *str == ' ' )) {
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
        for( p = str + 1; isdigit( *p ) || ((flag == 3) && isalpha( *p )); ++p ) {
            ++len;
        }
    }
    *plen = len;
    return( str );
}

int check_control_style( style idx, control_type control )
/********************************************************/
{
    switch( control ) {
    case T_LTEXT:
    case T_RTEXT:
    case T_CTEXT:
    case T_EDITTEXT:
        return( (( idx < T_CONTROL_ES_LEFT ) || ( idx > T_CONTROL_ES_OEMCONVERT )) ? 0 : 1 );
    case T_LISTBOX:
        return( (( idx < T_CONTROL_LBS_STANDARD ) || ( idx > T_CONTROL_LBS_WANTKEYBOARDINPUT )) ? 0 : 1 );
    case T_GROUPBOX:
    case T_ICON:
        return( (( idx < T_CONTROL_SS_LEFT ) || ( idx > T_CONTROL_SS_USERITEM )) ? 0 : 1 );
    case T_COMBOBOX:
        return( (( idx < T_CONTROL_CBS_SIMPLE ) || ( idx > T_CONTROL_CBS_OEMCONVERT )) ? 0 : 1 );
    case T_SCROLLBAR:
        return( (( idx < T_CONTROL_SBS_VERT ) || ( idx > T_CONTROL_SBS_SIZEBOXBOTTOMRIGHTALIGN )) ? 0 : 1 );
    case T_CONTROL:
        return( 1 );
    case T_CHECKBOX:
    case T_PUSHBUTTON:
    case T_DEFPUSHBUTTON:
    case T_RADIOBUTTON:
        return( (( idx < T_CONTROL_BS_PUSHBUTTON ) || ( idx > T_CONTROL_BS_OWNERDRAW )) ? 0 : 1 );
    default:
        return( 0 );
    }
}

void check_parm_item( char *keyword, char *parms[], control_type control, int parm_idx,
                int tab_cnt, char *str, char **win_tab, char **os2_tab, int *retval )
/************************************************************************************/
{
    parms = parms; control = control; parm_idx = parm_idx; tab_cnt = tab_cnt;
    win_tab = win_tab; os2_tab = os2_tab;

    if( strstr( str, keyword ) != NULL ) {
        *retval = 1;
    }
}

void convert_parm_table( char *keyword, char *parms[], control_type control, int parm_idx,
                   int tab_cnt, char *str, char **win_tab, char **os2_tab, int *retval )
/***************************************************************************************/
{
    int i;
    
    for( i = 0; i < tab_cnt; ++i ) {
        if( strcmp( str, win_tab[i] ) == 0 ) {
            if(( win_tab == control_style_win ) && !check_control_style( i, control ) ) {
                *(parms[ parm_idx ]) = '\0';
            } else {
                strcpy( parms[ parm_idx ], os2_tab[ i ] );
            }
            *retval = i;
            return;
        }
    }
}


control_type process_parms( char *parms[], int parms_cnt, char **win_tab,
    char **os2_tab, int tab_cnt, control_type control, fnx fn, char *keyword )
/****************************************************************************/
{
    int     i;
    int     retval;
    char    buff1[ MAX_LINE_LEN ];
    
    retval = 0;
    for( i = 0; i < parms_cnt; ++i ) {
        if( *(parms[ i ]) != '\0' ) {
            strcpy( buff1, parms[ i ] );
            if( win_tab != font_win ) 
                strupr( buff1 );
            fn( keyword, parms, control, i, tab_cnt, buff1, win_tab, os2_tab, &retval );
        }
    }
    return( retval );
}

void convert_buttons( char *ID, char *name, int flag )
/****************************************************/
{
    if( strcmp( ID, "IDOK" ) == 0 ) {
        strcpy( ID, "DID_OK" );
        if( flag ) {
            strcpy( name, "DEFPUSHBUTTON" );
        }
    } else if( strcmp( ID, "IDCANCEL" ) == 0 ) {
        strcpy( ID, "DID_CANCEL" );
    } else if(( strcmp( ID, "IDHELP" ) == 0 ) || ( strcmp( ID, "IDC_HELP" ) == 0 )) {
        strcpy( ID, "DID_HELP_BUTTON" );
    }
}

void add_parms_item( char *parms[], char *str, int after )
/********************************************************/
{
    int i;
    
    if( after ) {
        for( i = 0; i < MAX_STMT_PARMS; ++i ) {
            if( *(parms[ i ]) == '\0' ) {
                strcpy( parms[ i ], str );
                return;
            }
        }
        fprintf( stderr, "Error - number of the internal parameter overflow!\n" );
    } else {
        if( *(parms[ MAX_STMT_PARMS - 1 ]) != '\0' )
            fprintf( stderr, "Error - number of the internal parameter overflow!\n" );
        for( i = MAX_STMT_PARMS - 1; i; --i ) {
            if( *(parms[ i - 1 ]) != '\0' ) {
                strcpy( parms[ i ], parms[ i - 1 ] );
            }
        }
        strcpy( parms[ 0 ], str );
    }
}

void add_parms_list( statement *stmt, char *separators, int flag )
/****************************************************************/
{
    int     i = 0;
    char    *p;

    if( flag ) {
        for( i = 0; i < MAX_STMT_PARMS; ++i ) {
            if( *(stmt->parms[ i ]) == '\0' ) {
                break;
            }
        }
    }
    for( ; i < MAX_STMT_PARMS; ++i ) {
        p = strtok( NULL, separators );
        if( p == NULL ) {
            if( flag )
                break;
            *(stmt->parms[ i ]) = '\0';
        } else {
            strcpy( stmt->parms[ i ], p );
        }
    }
    if(( strstr( stmt->name, "TEXT" ) != NULL )
        || ( strcmp( stmt->name, "GROUPBOX" ) == 0 )
        || ( strcmp( stmt->name, "CONTROL" ) == 0 )) {
        if( !process_parms( stmt->parms, MAX_STMT_PARMS, control_style_win, 
            control_style_os2, CTRL_STYLE_CNT, 0, check_parm_item, "DT_MNEMONIC" )
            && strcmp( stmt->text, "\"\"" ) ) {
            add_parms_item( stmt->parms, "DT_MNEMONIC", ADD_AFTER );
        }
    }
    if(( strcmp( stmt->name + 1, "TEXT" ) == 0 ) && ( strlen( stmt->text ) > 10 )) {
        if( !process_parms( stmt->parms, MAX_STMT_PARMS, control_style_win,
            control_style_os2, CTRL_STYLE_CNT, 0, check_parm_item, "DT_WORDBREAK" ) ) {
            add_parms_item( stmt->parms, "DT_WORDBREAK", ADD_AFTER );
        }
    }
}

void remove_parms_item( char *parms[], char *str )
/************************************************/
{
    int i;
    
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        if( *(parms[ i ]) != '\0' ) {
            if( strcmp( parms[ i ], str ) == 0 ) {
                *(parms[ i ]) = '\0';
                return;
            }
        }
    }
}

void convert_font( char *parms[], int parms_cnt )
/***********************************************/
{
    int i;
    
    if( opt.font ) {
        for( i = 0; i < parms_cnt; ++i ) {
            strcpy( parms[ i ], opt.font_name );
        }
    } else {
        process_parms( parms, parms_cnt, font_win, font_os2, FONT_CNT,
            0, convert_parm_table, NULL );
    }
}

void out_parms_style( FILE *fo, char *parms[], char *str )
/********************************************************/
{
    int     oper_NOT;
    int     item_idx;
    int     i;
    char    *p;
    
    oper_NOT = 0;
    item_idx = 0;
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = parms[ i ];
        if(( *p != '\0' ) && !isdigit( *p ) ) {
#if defined( OLD_FORMAT )
            if( item_idx == 1 ) {
                fprintf( fo, "\n\t\t" );
            }
#endif
            if( item_idx == 0 ) {
                fprintf( fo, ", " );
#if !defined( OLD_FORMAT )
                fprintf( fo, "\n\t\t" );
                if( strcmp( str, "DIALOG" ) != 0 ) {
                    fprintf( fo, "\t" );
                }
#endif
            } else if(( item_idx == 1 ) && ( strcmp( str, "CONTROL" ) == 0 )) {
                fprintf( fo, " , " );
            } else if( oper_NOT == 0 ) {
                fprintf( fo, " | " );
            } else {
                fprintf( fo, " " );
            }
            if(( strcmp( p, "SS_WHITEFRAME" ) == 0 )
                || ( strcmp( p, "SS_BLACKFRAME" ) == 0 )
                || ( strcmp( p, "SS_GRAYFRAME" ) == 0 )) {
                fprintf( fo, "SS_BKGNDFRAME" );
            } else if(( strcmp( p, "SS_WHITERECT" ) == 0 )
                || ( strcmp( p, "SS_BLACKRECT" ) == 0 )
                || ( strcmp( p, "SS_GRAYRECT" ) == 0 )) {
                fprintf( fo, "SS_BKGNDRECT" );
            } else {
                fprintf( fo, "%s", p );
            }
            oper_NOT = ( strcmp( p, "NOT" ) == 0 ) ? 1 : 0 ;
            ++item_idx;
        }
    }
    fprintf( fo, "\n" );
}

void process_style( char *parms[], char *str )
/********************************************/
{
    char            **ptr;
    control_type    control = 0;
    int             i;
    
    if( strcmp( str, "DIALOG" ) != 0 ) {
        ptr = malloc( sizeof(char *) );
        *ptr = str;
        control = process_parms( ptr, 1, control_type_win, control_type_os2,
            CTRL_TYPE_CNT, 0, convert_parm_table, NULL );
        free( ptr );
        if( control == T_COMBOBOX ) {
            if( process_parms( parms, MAX_STMT_PARMS, control_class_win,
                control_class_os2, CTRL_NAME_CNT, control, check_parm_item, "CBS_DROPDOWNLIST" ) ) {
                if( process_parms( parms, MAX_STMT_PARMS, control_class_win, 
                    control_class_os2, CTRL_NAME_CNT, control, check_parm_item, "CBS_OWNERDRAW" ) ) {
                    strcpy( str, "CONTROL" );
                    add_parms_item( parms, "\"watcombo\"", ADD_BEFORE );
                }
            }
        } else if( control == T_CONTROL ) {
            if( process_parms( parms, MAX_STMT_PARMS, style_win[ control ], style_os2[ control ],
                style_cnt[ control ], control, check_parm_item, "BS_GROUPBOX" ) ) {
                strcpy( str, "GROUPBOX" );
                control = T_GROUPBOX;
                for( i = 0; i < MAX_STMT_PARMS; ++i ) {
                    *(parms[ i ]) = '\0';
                }
            }
        }
        process_parms( parms, MAX_STMT_PARMS, style_win[ control ], style_os2[ control ],
            style_cnt[ control ], control, convert_parm_table, NULL );
        if( control == T_CONTROL ) {
            process_parms( parms, MAX_STMT_PARMS, control_class_win, control_class_os2,
                CTRL_NAME_CNT, control, convert_parm_table, NULL );
            if( process_parms( parms, MAX_STMT_PARMS, control_class_win, control_class_os2,
                CTRL_NAME_CNT, control, check_parm_item, "WC_STATIC" ) ) {
                if( !process_parms( parms, MAX_STMT_PARMS, control_class_win, 
                    control_class_os2, CTRL_NAME_CNT, control, check_parm_item, "SS_" ) ) {
                    add_parms_item( parms, "SS_GROUPBOX", ADD_AFTER );
                }
            }
        } else {
            process_parms( parms, MAX_STMT_PARMS, control_style_win, control_style_os2,
                CTRL_STYLE_CNT, control, convert_parm_table, NULL );
        }
        if( !process_parms( parms, MAX_STMT_PARMS, control_class_win, control_class_os2,
            CTRL_NAME_CNT, control, check_parm_item, "WS_VISIBLE" ) ) {
            add_parms_item( parms, "WS_VISIBLE", ADD_AFTER );
        }
    }
    process_parms( parms, MAX_STMT_PARMS, window_style_win, window_style_os2,
        WND_STYLE_CNT, control, convert_parm_table, NULL );
}

void out_color_style( FILE *fo, statement *x )
/********************************************/
{
    int     i;
    char    *p;
    
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = x->parms[ i ];
        if(( strcmp( p, "SS_WHITEFRAME" ) == 0 )
            || ( strcmp( p, "SS_WHITERECT" ) == 0 )) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, RGB_WHITE\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, RGB_WHITE\n", STR_SPC STR_SPC );
        } else if(( strcmp( p, "SS_BLACKFRAME" ) == 0 )
            || ( strcmp( p, "SS_BLACKRECT" ) == 0 )) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, RGB_BLACK\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, RGB_BLACK\n", STR_SPC STR_SPC );
        } else if(( strcmp( p, "SS_GRAYFRAME" ) == 0 )
            || ( strcmp( p, "SS_GRAYRECT" ) == 0 )) {
            fprintf( fo, "%sPRESPARAMS PP_BACKGROUNDCOLOR, 0x00C0C0C0L\n", STR_SPC STR_SPC );
            fprintf( fo, "%sPRESPARAMS PP_FOREGROUNDCOLOR, 0x00C0C0C0L\n", STR_SPC STR_SPC );
        }
        *p = '\0';
    }
}

void get_rectangle_list( statement *x, char *separators )
/*******************************************************/
{
    char    *p;
    
    p = strtok( NULL, separators );
    if( p != NULL ) {
        x->x = atoi( p ) * CONV_X;
    }
    p = strtok( NULL, separators );
    if( p != NULL ) {
        x->y = atoi( p ) * CONV_Y;
    }
    p = strtok( NULL, separators );
    if( p != NULL ) {
        x->dx = atoi( p ) * CONV_X;
    }
    p = strtok( NULL, separators );
    if( p != NULL ) {
        x->dy = atoi( p ) * CONV_Y;
    }
}

void get_rectangle_parms( statement *x )
/**************************************/
{
    int     i;
    int     j;
    char    *p;
    
    j = 0;
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        p = x->parms[ i ];
        if( !isdigit( *p ) )
            continue;
        if(( *p == '0' ) && ( tolower( *(p + 1) ) == 'x' ))
            continue;
        if( j == 0 ) {
            x->x = atoi( p ) * CONV_X;
        } else if( j == 1 ) {
            x->y = atoi( p ) * CONV_Y;
        } else if( j == 2 ) {
            x->dx = atoi( p ) * CONV_X;
        } else if( j == 3 ) {
            x->dy = atoi( p ) * CONV_Y;
        }
        ++j;
        *p = '\0';
    }
}

int process_statement( char *line, FILE *fo )
/*******************************************/
{
    char    *separators = " ,\t|";
    char    buff1[ MAX_LINE_LEN + 1 ];
    int     len = 0;
    char    *p;
    
    if( *line == '\0' )
        return( 0 );
    strcpy( buff1, skip_separator( line ) );
    p = strtok( buff1, " ,\t|" );
    if( p != NULL )
        strcpy( dlg_item.name, p );
    if( strncmp( dlg_item.name, "END", 3 ) == 0 ) {
        fprintf( fo, "%s%s\nEND\n", STR_SPC, dlg_item.name );
        return( 1 );
    }
    if( strcmp( dlg_item.name, "CONTROL" ) == 0 ) {
        p = skip_keyword( line, &len );
        if( p != NULL ) {
            strncpy( dlg_item.text, p, len );
            dlg_item.text[ len ] = '\0';
            p += len;
        }
        p = strtok( p, separators );
        if( p != NULL ) {
            strcpy( dlg_item.ID, p );
        } else {
            strcpy( dlg_item.ID, "-1" );
        }
        convert_buttons( dlg_item.ID, dlg_item.name, 1 );
        add_parms_list( &dlg_item, separators, 0 );
        get_rectangle_parms( &dlg_item );
    } else {
        if(( strcmp( dlg_item.name, "COMBOBOX" ) == 0 )
            || ( strcmp( dlg_item.name, "LISTBOX" ) == 0 )
            || ( strcmp( dlg_item.name, "SCROLLBAR" ) == 0 )
            || ( strcmp( dlg_item.name, "EDITTEXT" ) == 0 )) {
            strcpy( dlg_item.text, "\"\"" );
            p = NULL;
        } else {
            p = skip_keyword( line, &len );
            if( p != NULL ) {
                strncpy( dlg_item.text, p, len );
                dlg_item.text[ len ] = '\0';
                p += len;
                convert_buttons( dlg_item.text, dlg_item.text, 0 );
            } else {
                strcpy( dlg_item.text, "\"\"" );
                p = NULL;
            }
        }
        p = strtok( p, separators );
        if( p != NULL ) {
            strcpy( dlg_item.ID, p );
        } else {
            strcpy( dlg_item.ID, "-1" );
        }
        convert_buttons( dlg_item.ID, dlg_item.name, 1 );
        get_rectangle_list( &dlg_item, separators );
        add_parms_list( &dlg_item, separators, 0 );
    }
    if( !strcmp( dlg_item.name, "SCROLLBAR" ) ) {
        add_parms_item( dlg_item.parms, "WC_SCROLLBAR", ADD_BEFORE );
    }
    process_style( dlg_item.parms, dlg_item.name );
    dlg_item.y = dlg_hdr.dy - dlg_item.y - dlg_item.dy;
    fprintf( fo, "%s%s", STR_SPC STR_SPC, dlg_item.name );
    if( strlen( dlg_item.name ) < 8 )
        fprintf( fo, "\t" );
    fprintf( fo, "\t" );
    if( strcmp( dlg_item.name, "LISTBOX" ) )
        fprintf( fo, "%s, ", dlg_item.text );
    fprintf( fo, "%s, %d, %d, %d, %d", dlg_item.ID, dlg_item.x,
                          dlg_item.y, dlg_item.dx, dlg_item.dy );
    out_parms_style( fo, dlg_item.parms, dlg_item.name );
    out_color_style( fo, &dlg_item );
    return( 0 );
}

void process_dialog_declaration( FILE *fi, FILE *fo, char *line )
/***************************************************************/
{
    long    font_size = 0;
    char    *font_name = NULL;
    char    *separators = " \t,|";
    int     font_set = 0;
    int     hidden_dialog = 0;
    char    *buff1;
    char    *p;
    char    **p2;
    size_t  len;
    int     sysmodal, visible;
    int     i;
    
    ++dialogs_cnt;
    if( opt.quiet == 0 ) {
        fprintf( stdout, "." );
    }
    // process DIALOG line
    buff1 = malloc( MAX_LINE_LEN );
    strcpy( buff1, skip_separator( line ) );
    p = strtok( buff1, separators );
    if( p != NULL )
        strcpy( dlg_hdr.ID, p );
    p = strtok( NULL, " ,\t" );
    while(( p != NULL ) && !isdigit( *p ) )
        p = strtok( NULL, separators );
    if( p != NULL )
        dlg_hdr.x = atoi( p ) * CONV_X;
    p = strtok( NULL, separators );
    if( p != NULL )
        dlg_hdr.y = atoi( p ) * CONV_Y;
    p = strtok( NULL, separators );
    if( p != NULL )
        dlg_hdr.dx = atoi( p ) * CONV_X;
    p = strtok( NULL, separators );
    if( p != NULL )
        dlg_hdr.dy = atoi( p ) * CONV_Y;
    dlg_hdr.y = 230 - dlg_hdr.y - dlg_hdr.dy;
    strcpy( dlg_hdr.text, "\"\"" );
    // process next lines
    my_fgets( line, MAX_LINE_LEN, fi );
    while(( *line != '\0' ) && ( strstr( line, "BEGIN" ) == NULL )) {
        strcpy( buff1, line );
        strtok( buff1, separators );
        if( strstr( line, "STYLE" ) != NULL ) {
            add_parms_list( &dlg_hdr, separators, 1 );
            len = strlen( line );
            for( p = line + len - 1; len && isspace( *p ); --len )
                *(p--) = '\0';
            if( *p == '|' ) {
                my_fgets( line, MAX_LINE_LEN, fi );
                strcpy( buff1, line );
                p = strtok( buff1, separators );
                while( p != NULL ) {
                    add_parms_item( dlg_hdr.parms, p, ADD_AFTER );
                    p = strtok( NULL, separators );
                }
            }
        } else if( (p = strstr( line, "CAPTION" )) != NULL ) {
            strcpy( dlg_hdr.text, p + 8 );
            dlg_hdr.text[ strlen( dlg_hdr.text ) ] = '\0';
        } else if( strstr( line, "FONT" ) != NULL ) {
            font_set = 1;
            p = strtok( NULL, separators );
            font_size = strtol( p, &p, 10 );
            p = strtok( NULL, "\"" );
            p = strtok( NULL, "\"" );
            if( opt.font ) {
                font_name = malloc( strlen( opt.font_name ) + 1 );
                strcpy( font_name, opt.font_name );
                if( opt.font_size != 0 ) {
                    font_size = opt.font_size;
                }
            } else {
                if( p != NULL ) {
                    font_name = malloc( strlen( p ) + 10 );
                    strcpy( font_name, p );
                    p2 = malloc( sizeof(char *) );
                    *p2 = font_name;
                    convert_font( p2, 1 );
                    free( p2 );
                }
            }
        }
        my_fgets( line, MAX_LINE_LEN, fi );
    }
    process_style( dlg_hdr.parms, "DIALOG" );
    if( font_set == 0 ) {
        font_name = malloc( 7 );
        strcpy( font_name, "Helv" );
        font_size = 10;
    }
    sysmodal = process_parms( dlg_hdr.parms, MAX_STMT_PARMS, control_class_win,
        control_class_os2, CTRL_NAME_CNT, 0, check_parm_item, "FCF_SYSMODAL" );
    if( opt.hide ) {
        for( i = 0; i < opt.flist_cnt; ++i ) {
            if( stricmp( dlg_hdr.ID, opt.flist_data[ i ] ) == 0 ) {
                hidden_dialog = 1;
                break;
            }
        }
    }
    visible = process_parms( dlg_hdr.parms, MAX_STMT_PARMS, control_class_win,
        control_class_os2, CTRL_NAME_CNT, 0, check_parm_item, "WS_VISIBLE" );
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
        out_parms_style( fo, dlg_hdr.parms, "DIALOG" );
    }
    if(( font_name != NULL ) || ( font_size != 0 )) {
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

void alloc_statement( statement *stmt )
/*************************************/
{
    int i;
    
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        stmt->parms[ i ] = malloc( MAX_PARM_LEN );
        *(stmt->parms[ i ]) = '\0';
    }
}

void free_statement( statement *stmt )
/************************************/
{
    int i;
    
    for( i = 0; i < MAX_STMT_PARMS; ++i ) {
        free( stmt->parms[ i ] );
    }
}

int main( int argc, char *argv[] )
/********************************/
{
    char    fname[ PATH_MAX ];
    FILE    *fi;
    FILE    *fo;
    char    *p;
    int     i;
    char    *line;
    char    *buff1;
    char    *buff2;
    char    *separators = " \t";
    
    i = process_cmdl( argc, argv );
    if( i == 0 ) {
        disp_usage();
        return( -1 );
    }
    strcpy( fname, argv[ i ] );
    if( strrchr( fname, '.' ) == NULL )
        strcat( fname, ".dlg" );
    fi = fopen( fname, "r" );
    if( fi == NULL ) {
        printf( "Could not open input file: %s\n", fname );
        return( -1 );
    }
    if( i + 1 < argc ) {
        strcpy( fname, argv[ i + 1 ] );
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
    alloc_statement( &dlg_item );

    line = malloc( MAX_LINE_LEN );
    
    buff1 = malloc( MAX_LINE_LEN );
    buff2 = malloc( MAX_LINE_LEN );
    
    my_fgets( line, MAX_LINE_LEN, fi );
    while( !feof( fi ) ) {
        while( !feof( fi ) ) {
            if( strstr( line, "DLGINCLUDE" ) != NULL ) {
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
                p = malloc( MAX_LINE_LEN );
                strcpy( p, line );
                fprintf( fo, "%s 1 ", strtok( p, separators ) );
                my_fgets( line, MAX_LINE_LEN, fi );
                my_fgets( line, MAX_LINE_LEN, fi );
                strcpy( p, line );
                fprintf( fo, "%s\n", strtok( p, separators ) );
                free( p );
                my_fgets( line, MAX_LINE_LEN, fi );
                my_fgets( line, MAX_LINE_LEN, fi );
            } else if( strstr( line, "DIALOG" ) != NULL ) {
                p = malloc( MAX_LINE_LEN );
                strcpy( p, line );
                process_dialog_declaration( fi, fo, p );
                strcpy( line, p );
                free( p );
            } else if( strstr( line, "BEGIN" ) != NULL ) {
                my_fgets( line, MAX_LINE_LEN, fi );
                break;
            } else {
#if !defined( OLD_FORMAT )
                if( *line != '\0' )
#endif
                fprintf( fo, "%s\n", line );
                my_fgets( line, MAX_LINE_LEN, fi );
            }
        }
        p = "";
        while( !feof( fi ) && strcmp( p, "END" ) ) {
            while( my_fgets( buff1, MAX_LINE_LEN, fi ) != NULL ) {
                if( check_statement( buff1 ) )
                    break;
                strncat( line, skip_separator( buff1 ), MAX_LINE_LEN );
            }
            process_statement( line, fo );
            strcpy( line, buff1 );
            strcpy( buff2, buff1 );
            p = strtok( buff2, separators );
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
            free( opt.flist_data[ i ] );
        free( opt.flist_data );
    }
    return( 0 );
}
