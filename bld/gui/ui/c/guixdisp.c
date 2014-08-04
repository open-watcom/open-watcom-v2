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


#include <stdlib.h>
#include <string.h>
#include "guiwind.h"
#include "guidlg.h"
#include "guistr.h"
#include "clibext.h"

/* buttons and icons that can be in the dialog */
typedef enum {
    NO_CONTROL  = 0x000,
    ABORT       = 0x001,
    CANCEL      = 0x002,
    IGNORE      = 0x004,
    NO          = 0x008,
    OK          = 0x010,
    RETRY       = 0x020,
    YES         = 0x040,
    EXCLAMATION = NO_CONTROL,
    QUESTION    = NO_CONTROL,
    INFORMATION = NO_CONTROL,
    STOP        = NO_CONTROL
} control_types;

/* information about the controls needed for each gui_message_type */
typedef struct message_types {
    gui_message_type    type;
    int                 num_controls;
    control_types       controls;
} message_types;

/* control definition for each control that can be in the dialog */
typedef struct control_pairs {
    gui_control_info    control;
    control_types       type;
} control_pairs;

/* struct used to describe string broken into segments that can be displayed
 * one per line */
typedef struct string_info {
    char        *text;
    int         length;
} string_info;

#define TEXT_ROW        1
#define TEXT_START_COL  2
#define ICON_ROW        1
#define BUTTON_ROW      3
#define BUTTON_WIDTH    7

/* control definition for each control that can be in the dialog */
static control_pairs MessageControls[] = {
 {  DLG_BUTTON( NULL,           GUI_RET_ABORT,  0, 0, BUTTON_WIDTH ),   ABORT       },
 {  DLG_BUTTON( NULL,           GUI_RET_CANCEL, 0, 0, BUTTON_WIDTH ),   CANCEL      },
 {  DLG_BUTTON( NULL,           GUI_RET_IGNORE, 0, 0, BUTTON_WIDTH ),   IGNORE      },
 {  DLG_BUTTON( NULL,           GUI_RET_NO,     0, 0, BUTTON_WIDTH ),   NO          },
 {  DLG_DEFBUTTON( NULL,        GUI_RET_OK,     0, 0, BUTTON_WIDTH ),   OK          },
 {  DLG_BUTTON( NULL,           GUI_RET_RETRY,  0, 0, BUTTON_WIDTH ),   RETRY       },
 {  DLG_BUTTON( NULL,           GUI_RET_YES,    0, 0, BUTTON_WIDTH ),   YES         },
 {  DLG_STRING( "!",                            0, 0, 1 ),              EXCLAMATION },
 {  DLG_STRING( "?",                            0, 0, 1 ),              QUESTION    },
 {  DLG_STRING( "i",                            0, 0, 1 ),              INFORMATION },
 {  DLG_STRING( NULL,                           0, 0, 5 ),              STOP        }
};
#define NUM_CONTROL_TYPES ( sizeof( MessageControls ) / sizeof( control_pairs ) )

/* static text controls used for displaying message */
static gui_control_info StaticMessage = DLG_STRING( NULL, TEXT_START_COL, TEXT_ROW, 0 );

/* information about the controls needed for each gui_message_type */
static message_types ControlsNeeded[] = {
  { GUI_ABORT_RETRY_IGNORE,     3,      ABORT | IGNORE | RETRY  },
  { GUI_EXCLAMATION,            1,      EXCLAMATION | OK        },
  { GUI_INFORMATION,            1,      INFORMATION | OK        },
  { GUI_QUESTION,               1,      QUESTION | OK           },
  { GUI_STOP,                   1,      STOP | OK               },
  { GUI_OK,                     1,      OK                      },
  { GUI_OK_CANCEL,              2,      OK | CANCEL             },
  { GUI_RETRY_CANCEL,           2,      RETRY | CANCEL          },
  { GUI_YES_NO,                 2,      YES | NO                },
  { GUI_YES_NO_CANCEL,          3,      YES | NO | CANCEL       },
  { GUI_SYSTEMMODAL,            0,      NO_CONTROL                      }
};
#define NUM_STYLES ( sizeof( ControlsNeeded ) / sizeof( message_types ) )

static bool MessagesInitialized = FALSE;

static void InitMessageControls( void )
{
    int j;

    for( j = 0; j < NUM_CONTROL_TYPES; j++ ) {
        switch( MessageControls[j].type ) {
            case ABORT:
                MessageControls[j].control.text = LIT( XAbort );
                break;
            case CANCEL:
                MessageControls[j].control.text = LIT( Cancel );
                break;
            case IGNORE:
                MessageControls[j].control.text = LIT( XIgnore );
                break;
            case NO:
                MessageControls[j].control.text = LIT( XNo );
                break;
            case OK:
                MessageControls[j].control.text = LIT( OK );
                break;
            case RETRY:
                MessageControls[j].control.text = LIT( XRetry );
                break;
            case YES:
                MessageControls[j].control.text = LIT( XYes );
                break;
            case STOP:
                MessageControls[j].control.text = LIT( Stop_Bang );
                break;
            default :
                break;
        }
    }
}

/*
 * DisplayMessage - callback function for dialog box
 */

bool DisplayMessage( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_message_return *ret;
    unsigned            id;

    ret = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case GUI_RET_ABORT :
        case GUI_RET_CANCEL :
        case GUI_RET_IGNORE :
        case GUI_RET_NO :
        case GUI_RET_OK :
        case GUI_RET_RETRY :
        case GUI_RET_YES :
            *ret = id;
            GUICloseDialog( gui );
            return( TRUE );
            break;
        default :
            break;
        }
    default :
        break;
    }
    return( TRUE );
}

/*
 * GUIStrnDup -- duplicate the string text up to length characters
 */

bool GUIStrnDup( char * text, char ** new, int length )
{
    int str_len;

    if( text == NULL ) {
        *new = NULL;
        return( TRUE );
    } else {
        str_len = strlen( text );
        if( str_len < length ) {
            length = str_len;
        }
        *new = (char *)GUIMemAlloc( length + 1 );
        if( *new == NULL ) {
            return( FALSE );
        }
        strncpy( *new, text, length );
        (*new)[length] = '\0';
    }
    return( TRUE );
}


/*
* TabFilter -- Expand tabs to blanks
*/
char *TabFilter( char *message )
{
    char *new_message;
    char *start;
    int  tab_pos;

#define TAB_SIZE 4
                                /* allocate another chunk of memory since */
                                /* reallocating space for string literals is a no no */
    new_message = ( char *)GUIMemAlloc( strlen(message)+1 );
    strcpy( new_message, message );
    for( ; ; ){
        tab_pos = strcspn(new_message, "\t");
        if( tab_pos == strlen(new_message) ) break;   /* no more tabs */
        new_message = ( char *)GUIMemRealloc(new_message, strlen(new_message)+TAB_SIZE+1 );
        start = new_message + tab_pos;                /* don't forget the NULL */
        memmove(start+TAB_SIZE, start+1, strlen( start+1 )+1 );
        strnset( start, ' ', TAB_SIZE);
    }
    return( new_message );
}


/*
 * GetNumStringControls -- divide string up into segments that can be
 *                         displayed one per line.  Attempt to break
 *                         lines at spaces.
 */

static bool GetNumStringControls( int *num_controls, char *old_message,
                                  string_info **info )
{
    gui_rect            scale;
    gui_text_metrics    metrics;
    int                 max_width;
    char                *tmp_n;              /* temp newline character */
    int                 len;
    char                *start;
    char                *end_char;
    char                *message;
    char                *new_message;

    new_message = TabFilter( old_message );     /* expand out tab characters */
    message = new_message;

    if( message == NULL ) {
        *num_controls = 0;
        return( FALSE );
    }
    GUIGetScale( &scale );
    GUIGetDlgTextMetrics( &metrics );
    if( strlen( old_message ) > 256 ) {
    // If message is long, go to wider box
        max_width = ( 7 * ( scale.width / metrics.avg.x ) ) / 8;
    } else {
        max_width = ( 2 * ( scale.width / metrics.avg.x ) ) / 3;
    }
    *info           = NULL;
    tmp_n           = NULL;
    *num_controls   = 0;

    for( ;message != tmp_n; ) {
        tmp_n = message + strcspn(message, "\n\r");

        if( tmp_n-message < max_width ) {
            start   = message;
            len     = tmp_n-start;
            if( *tmp_n == '\0' ) {
                message = tmp_n;      /* at the end of original string */
            } else {
                message = tmp_n+1;   /* skip over newline */
            }
        } else {                     /* search back for space */
            for(end_char = message+max_width; (message < end_char) && (*end_char != ' '); --end_char );
            if( end_char == message ) {
                start   = message;           /* no spaces found */
                len     = max_width;
                message += max_width;
            } else {
                start   = message;
                len     = end_char-start+1;
                message = end_char+1;      /* skip over blank */
            }
        }                                  /* add new line to error box */
        (*num_controls)+=1;
        *info = ( string_info * )GUIMemRealloc( *info, sizeof( string_info ) *
                                            (*num_controls) );
        if( *info == NULL ) return( FALSE );
        (*info)[*num_controls - 1].length = len;
        if( !GUIStrnDup( start, &((*info)[*num_controls - 1].text), len ) ) {
            return( FALSE );
        }
    } /* for */

    GUIMemFree( new_message );     /* allocated in TabFilter routine */
    return( TRUE );
}

/*
 * UpdateCols -- increase number of columns needed according to size
 *               and location of control
 */

static int UpdateCols( gui_control_info *control_info, int cols )
{
    if( cols < control_info->rect.x - DLG_COL_0 + control_info->rect.width - DLG_COL_0 )
        cols = control_info->rect.x - DLG_COL_0 + control_info->rect.width - DLG_COL_0;
    return( cols );
}

/*
 * AdjustVert -- adjust the vertical position of buttons and icons according
 *               to the number of lines of text ( also adjusts cols )
 */

int AdjustVert( int *cols, control_types controls_to_use,
                gui_control_info *control_info, int num_controls,
                int num_string_controls )
{
    int num_buttons;
    int i;
    int j;

    if( !MessagesInitialized ) {
        InitMessageControls();
        MessagesInitialized = TRUE;
    }

    i = num_string_controls;
    num_buttons = 0;
    for( j = 0; j < NUM_CONTROL_TYPES; j++ ) {
        if( ( i < num_controls ) &&
            ( controls_to_use & MessageControls[j].type ) ) {
            memcpy( &control_info[i], &MessageControls[j].control,
                    sizeof( gui_control_info ) );
            switch( control_info[i].control_class ) {
            case GUI_PUSH_BUTTON :
            case GUI_DEFPUSH_BUTTON :
                num_buttons ++;
                control_info[i].rect.y = DLG_ROW( BUTTON_ROW +
                                            num_string_controls  - 1 );
                break;
            case GUI_STATIC :
                control_info[i].rect.y = DLG_ROW( ICON_ROW +
                                    ( num_string_controls - 1 ) / 2 );
                break;
            default :
                break;
            }
            *cols = UpdateCols( &control_info[i], *cols );
            i++;
        }
    }
    return( num_buttons );
}

/*
 * CentreButtons -- centre the buttons horizontally
 */

static void CentreButtons( int cols, int num_buttons,
                           gui_control_info *control_info, int num_controls )
{
    int button_number;
    int space_per_button;
    int i;

    button_number = 0;
    if( num_buttons > 0 ) {
        space_per_button = cols / num_buttons;
    }
    for( i = 0; i < num_controls; i++ ) {
        switch( control_info[i].control_class ) {
        case GUI_PUSH_BUTTON :
        case GUI_DEFPUSH_BUTTON :
            button_number++;
            control_info[i].rect.x = DLG_COL( space_per_button
              * button_number - ( ( space_per_button + BUTTON_WIDTH ) / 2 ) );
            break;
        default :
            break;
        }
    }
}

/*
 * GUIDisplayMessage -- display the message, return the user's response
 */

gui_message_return GUIDisplayMessage( gui_window *wnd,
                                      char *message, char *caption,
                                      gui_message_type type )
{
    int                 rows;
    int                 cols;
    gui_control_info    *control_info;
    int                 num_controls;
    int                 num_string_controls;
    gui_message_return  ret;
    int                 i;
    control_types       controls_to_use;
    int                 mess_length;
    int                 capt_length;
    string_info         *strings;
    int                 num_buttons;

    wnd = wnd;
    if( message != NULL ) {
        mess_length = strlen( message );
    } else {
        mess_length = 0;
    }
    if( caption != NULL ) {
        capt_length = strlen( caption );
    } else {
        capt_length = 0;
    }

    /* figure out the number of icon and button controls and which ones */
    num_controls = 0;
    controls_to_use = 0;
    for( i = 0; i < NUM_STYLES; i++ ) {
        if( type & ControlsNeeded[i].type ) {
            num_controls += ControlsNeeded[i].num_controls;
            controls_to_use |= ControlsNeeded[i].controls;
        }
    }

    /* figure out how manu GUI_STATIC controls are required for the text */
    num_string_controls = 0;
    if( !GetNumStringControls( &num_string_controls, message, &strings ) ) {
        return( GUI_RET_CANCEL );
    }

    num_controls += num_string_controls;
    control_info = (gui_control_info *)GUIMemAlloc( sizeof( gui_control_info ) *
                                                 num_controls );
    if( control_info == NULL ) {
        return( GUI_RET_CANCEL );
    }

    cols = capt_length + 2;

    /* create GUI_STATIC controls, as many as required */
    if( num_string_controls > 0 ) {
        for( i = 0; i < num_string_controls; i++ ) {
            uiyield();
            StaticMessage.text = strings[i].text;
            StaticMessage.rect.width = DLG_COL( strings[i].length );
            StaticMessage.rect.y = DLG_ROW( TEXT_ROW + i );
            memcpy( &control_info[i], &StaticMessage, sizeof( gui_control_info ) );
            cols = UpdateCols( &control_info[i], cols );
        }
    }
    rows = num_string_controls + BUTTON_ROW + 1;

    /* adjust the vertical position of buttons and icons according to the
     * number of lines of text ( also adjusts cols )
     */
    num_buttons = AdjustVert( &cols, controls_to_use, control_info, num_controls,
                              num_string_controls );

    if( cols < ( num_buttons * ( BUTTON_WIDTH + 4 ) ) ) {
        cols = num_buttons * ( BUTTON_WIDTH + 4 ) ;
    }
    /* centre the buttons horizontally */
    CentreButtons( cols, num_buttons, control_info, num_controls );
    ret = GUI_RET_CANCEL; /* default -- if escape hit */
    GUIDlgOpen( caption, rows, cols, control_info, num_controls,
                &DisplayMessage, &ret );
    for( i = 0; i < num_string_controls; i++ ) {
        GUIMemFree( strings[i].text );
    }
    GUIMemFree( strings );
    GUIMemFree( control_info );
    return( ret );
}
