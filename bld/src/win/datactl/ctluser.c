#include <windows.h>
#include "ctltype.h"

/**************************************************************
 Define the constants below only for the field types you
 need. Comment out the ones you don't need. */

#define CTL_CHECK_DEF
#define CTL_RADIO_DEF
#define CTL_TEXT_DEF
#define CTL_COMBO_DEF
#define CTL_DCOMBO_DEF
#define CTL_INT_DEF
#define CTL_FLOAT_DEF
#define CTL_RINT_DEF
#define CTL_RFLOAT_DEF


/**************************************************************
 Add your own user controls to the data structure below. */


ctl_action Ctl_user_actions[]={
    { NULL, NULL, NULL }
};


/******* Leave this stuff alone. To alter the data controls that
         are desired, change the '#defines' at the top of this file */

extern BOOL ctl_check_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_radio_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_text_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_combo_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_dcombo_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_int_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_float_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_rint_start( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_rfloat_start( ctl_elt *, HANDLE, HWND, void * );

extern BOOL ctl_check_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_radio_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_text_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_combo_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_dcombo_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_int_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_float_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_rint_finish( ctl_elt *, HANDLE, HWND, void * );
extern BOOL ctl_rfloat_finish( ctl_elt *, HANDLE, HWND, void * );

extern BOOL ctl_check_modified( ctl_elt *, int, LONG );
extern BOOL ctl_text_modified( ctl_elt *, int, LONG );
extern BOOL ctl_combo_modified( ctl_elt *, int, LONG );
extern BOOL ctl_radio_modified( ctl_elt *, int, LONG );

ctl_action Ctl_int_actions[]={
#ifdef CTL_CHECK_DEF
    { ctl_check_start, ctl_check_finish, ctl_check_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_RADIO_DEF
    { ctl_radio_start, ctl_radio_finish, ctl_radio_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_TEXT_DEF
    { ctl_text_start, ctl_text_finish, ctl_text_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_COMBO_DEF
    { ctl_combo_start, ctl_combo_finish, ctl_combo_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_DCOMBO_DEF
    { ctl_dcombo_start, ctl_dcombo_finish, ctl_combo_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_INT_DEF
    { ctl_int_start, ctl_int_finish, ctl_text_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_FLOAT_DEF
    { ctl_float_start, ctl_float_finish, ctl_text_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_RINT_DEF
    {ctl_int_start, ctl_rint_finish, ctl_text_modified },
#else
    { NULL, NULL, NULL },
#endif
#ifdef CTL_RFLOAT_DEF
    { ctl_float_start, ctl_rfloat_finish, ctl_text_modified },
#else
    { NULL, NULL, NULL },
#endif
};
