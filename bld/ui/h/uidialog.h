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
    FLD_VOID,
    FLD_FRAME,
    FLD_LABEL,
    FLD_TEXT,          /* background text      * must be */
    FLD_HOT,           /* hot spot             * first   */
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
    unsigned        length;
} an_edit_control;

typedef struct a_list_info {
    VSCREEN         *vs;
    SAREA           area;
    a_gadget        gadget;
    unsigned        line;
    unsigned        row;
    ATTR            attr;       // attribute for highlighted item
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
    EVENT               event;
    int                 row;
    int                 startcol;
    int                 length;
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
    unsigned        dirty:1;
    unsigned        moving:1;
} a_dialog;

#ifdef __cplusplus
    extern "C" {
#endif
extern void uiposnhotspots(VSCREEN *,VFIELD *);
extern void uiprinthotspots(VSCREEN *,VFIELD *);
extern void uioffhotspots(VSCREEN *,VFIELD *);
extern EVENT uihotkeyfilter( a_dialog *, EVENT );
extern EVENT uihotspotfilter( VSCREEN *, VFIELD *, EVENT );
extern char uihotspot( VSCREEN *, char *str, SAREA *parea, a_hot_spot_flags flags );
extern void uidisplayhotspot( VSCREEN *w, VFIELD *vfield );
extern char uidrawhottext( VSCREEN *, char *str, SAREA *parea, ATTR attr, ATTR hotattr, bool hidden, bool no_hotkey, bool centre_text );

extern unsigned int ui_split_line(char **,char *,unsigned int );
extern void *uiinitdialog(const char *title, ATTR, char **, unsigned int, int, int, int );
extern void uifinidialog(void *);

extern a_dialog *uibegdialog( const char *title, VFIELD *, ORD, ORD, int, int );
extern bool uigetdialogarea( a_dialog *ui_dlg_info, SAREA *area );
extern EVENT uiprocessdialogevent( EVENT, a_dialog * );
extern void uireinitdialog( a_dialog *, VFIELD *);
extern EVENT uidialog(a_dialog *);
extern void uienddialog(a_dialog *);
extern void uifreedialog( a_dialog *ui_dlg_info );
extern void uiprintfield(a_dialog *, VFIELD *);
extern void uimovefield( a_dialog *, VFIELD *, int row_diff, int col_diff );
extern bool uiresizedialog( a_dialog *ui_dlg_info, SAREA *new_area );
extern void uiredrawdialog( a_dialog *ui_dlg_info );
extern bool uidialogisdirty( a_dialog *);
extern void uidialogsetdirty( a_dialog *, bool );
extern void uidialogsetcurr( a_dialog *, VFIELD *);
extern void uidialogexitcurr( a_dialog * );
extern void uidialogchangefield( a_dialog * );
extern bool uiisdefaulthotspot( VFIELD *, EVENT );
extern EVENT uilistbox( EVENT , struct a_list *, bool );
extern void uiupdateedit( a_dialog *, VFIELD * );
extern void uiboxpushlist( void );
extern void uiboxpoplist( void );
extern void uimovelistbox( struct a_list *, int row_diff, int col_diff );
extern unsigned uiendlistbox( struct a_list * );
extern void uipaintlistbox( struct a_list * );
extern struct a_list_info *uibeglistbox( VSCREEN *, struct sarea *,
                                         struct a_list * );
extern int  uilistsize( struct a_list * );
extern UIPICKGETTEXT uigetlistelement;
extern void uiupdatelistbox( struct a_list * );
extern void uiupdatecombobox( struct a_combo_box * );
extern void *openstream(char *,char *,char **);
extern void closestream(void *);

/*
 * functions in uidlgfcn.c
 */
extern void uioncheckbox( a_dialog *, VFIELD * );
extern void uioffcheckbox( a_dialog *, VFIELD * );
extern void uiselectradio( a_dialog *, VFIELD * );
extern void uiselectlist( a_dialog *, VFIELD *, unsigned choice );
extern void uiselectcombo( a_dialog *, VFIELD *, unsigned choice );

/*
 * functions in uidialcb.c
 */

extern EVENT uidialogcallback( a_dialog *, EVENT );

#ifdef __cplusplus
}
#endif

#endif
