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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _UIDIALOG_H
#define _UIDIALOG_H

#include "uigadget.h"       /* for list_box structure element */
#include "uivedit.h"
#include "uiledit.h"

typedef enum {
    HOT_CHAR    = 0x00ff,
    HOT_DEFAULT = 0x0100,
    HOT_CURRENT = 0x0200,
    HOT_ACTIVE  = 0x0400,
    HOT_HIDDEN  = 0x0800,
    HOT_NO_KEY  = 0x1000,
} a_hot_spot_flags;

typedef enum {
    FLD_NONE,
    FLD_VOID,
    FLD_FRAME,
    FLD_LABEL,
    FLD_TEXT,          /* background text must be first */
    FLD_HOT,           /* hot spot must be first        */
    FLD_EDIT,
    FLD_INVISIBLE_EDIT,
    FLD_LISTBOX,
    FLD_PULLDOWN,
    FLD_COMBOBOX,
    FLD_CHECK,
    FLD_RADIO,
    FLD_EDIT_MLE
} a_field_type;

typedef bool (UIPICKGETTEXT)( const void *data_handle, unsigned item, char *buff, unsigned buff_len );

typedef struct an_edit_control {
    char            *buffer;
    size_t          length;
} an_edit_control;

typedef struct a_list_info {
    VSCREEN         *vs;
    SAREA           area;
    a_gadget        gadget;
    unsigned        line;
    unsigned        row;
    UIATTR          uiattr;         // attribute for highlighted item
} a_list_info;

typedef struct a_choice {
    unsigned        choice;
    unsigned        num_choices;
} a_choice;

typedef struct a_num_field {          /* display is choice+diff */
    a_choice        c;
    int             diff;
    char            *special;     /* non numeric displayed for choice==0 */
} a_num_field;

typedef struct a_hot_spot {
    char                *str;
    ui_event            event;
    int                 row;
    int                 startcol;
    unsigned            length;
    a_hot_spot_flags    flags;
} a_hot_spot;

typedef struct a_toggle {
    a_choice        c;
    char            **strings;
} a_toggle;

typedef struct a_list {
    unsigned        choice;
    const void      *data_handle;
    UIPICKGETTEXT   *get;
    a_list_info     *box;
} a_list;

typedef struct a_check {
    bool            val;
    bool            def;
    char            *str;   // without button
    char            hotkey; // will be set by UI
} a_check;

#define _checked( a ) ( a->val != a->def )

typedef struct a_combo_box {
    a_list          list;
    an_edit_control edit;
    bool            perm;
} a_combo_box;

typedef struct a_radio_group {
    int             value;
    char            *caption;      // used for printing and reading
    int             def;           // default value (used as above)
} a_radio_group;

typedef struct a_radio {
    int             value;
    char            *str;        // without button
    a_radio_group   *group;
    char            hotkey;      // will be set by UI
} a_radio;

typedef struct vfield {
    SAREA           area;
    a_field_type    typ;
    union {
        void            *ptr;
        char            *str;
        a_radio         *radio;
        a_check         *check;
        a_combo_box     *combo;
        a_list          *list;
        an_edit_control *edit;
        a_hot_spot      *hs;
    } u;
} VFIELD;

typedef struct a_dialog {
    VSCREEN         *vs;
    unsigned        field;
    VFIELD          *fields;        // pointer to VFIELDs
    VFIELD          *other;         // prev VFIELD or moused but no tab
    VFIELD          *curr;          // current VFIELD
    VFIELD          *first;         // first VFIELD
    a_ui_edit       *edit_data;
    boolbit         dirty       :1;
    boolbit         moving      :1;
} a_dialog;

#ifdef __cplusplus
extern "C" {
#endif

extern void             uiposnhotspots( VSCREEN *, VFIELD * );
extern void             uiprinthotspots( VSCREEN *, VFIELD * );
extern void             uioffhotspots( VSCREEN *, VFIELD * );
extern ui_event         uihotkeyfilter( a_dialog *, ui_event );
extern ui_event         uihotspotfilter( VSCREEN *, VFIELD *, ui_event );
extern char             uihotspot( VSCREEN *, const char *str, SAREA *parea, a_hot_spot_flags flags );
extern void             uidisplayhotspot( VSCREEN *w, VFIELD *vfield );
extern char             uidrawhottext( VSCREEN *, const char *str, SAREA *parea, ATTR attr, ATTR hotattr, bool hidden, bool no_hotkey, bool centre_text );

extern unsigned         ui_split_line( char **, char *, unsigned );
extern VSCREEN          *uiinitdialog( const char *title, ATTR, char **, uisize, uisize, int, int );
extern void             uifinidialog( VSCREEN * );

extern a_dialog         *uibegdialog( const char *title, VFIELD *, unsigned, unsigned, int, int );
extern bool             uigetdialogarea( a_dialog *ui_dlg_info, SAREA *area );
extern ui_event         uiprocessdialogevent( ui_event, a_dialog * );
extern void             uireinitdialog( a_dialog *, VFIELD *);
extern ui_event         uidialog( a_dialog * );
extern void             uienddialog( a_dialog * );
extern void             uifreedialog( a_dialog *ui_dlg_info );
extern void             uiprintfield( a_dialog *, VFIELD * );
extern void             uimovefield( a_dialog *, VFIELD *, int row_diff, int col_diff );
extern bool             uiresizedialog( a_dialog *ui_dlg_info, SAREA *new_area );
extern void             uiredrawdialog( a_dialog *ui_dlg_info );
extern bool             uidialogisdirty( a_dialog *);
extern void             uidialogsetdirty( a_dialog *, bool );
extern void             uidialogsetcurr( a_dialog *, VFIELD *);
extern void             uidialogexitcurr( a_dialog * );
extern void             uidialogchangefield( a_dialog * );
extern bool             uiisdefaulthotspot( VFIELD *, ui_event );
extern ui_event         uilistbox( ui_event , a_list *, bool );
extern void             uiupdateedit( a_dialog *, VFIELD * );
extern void             uiboxpushlist( void );
extern void             uiboxpoplist( void );
extern void             uimovelistbox( a_list *, int row_diff, int col_diff );
extern unsigned         uiendlistbox( a_list * );
extern void             uipaintlistbox( a_list * );
extern a_list_info      *uibeglistbox( VSCREEN *, SAREA *, a_list * );
extern unsigned         uilistsize( a_list * );
extern UIPICKGETTEXT    uigetlistelement;
extern void             uiupdatelistbox( a_list * );
extern void             uiupdatecombobox( a_combo_box * );

/*
 * functions in uidlgfcn.c
 */
extern void             uioncheckbox( a_dialog *, VFIELD * );
extern void             uioffcheckbox( a_dialog *, VFIELD * );
extern void             uiselectradio( a_dialog *, VFIELD * );
extern void             uiselectlist( a_dialog *, VFIELD *, unsigned choice );
extern void             uiselectcombo( a_dialog *, VFIELD *, unsigned choice );

/*
 * functions in uidialcb.c
 */

extern ui_event         uidialogcallback( a_dialog *, ui_event );

#ifdef __cplusplus
}
#endif

#endif
