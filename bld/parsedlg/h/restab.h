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
* Description:  windows -> os2 dialog items conversion rules definition
*
****************************************************************************/


#if TABLE == 1
/**************************************
  control type items conversion
**************************************/
pick ( LTEXT,         LTEXT         )
pick ( RTEXT,         RTEXT         )
pick ( CTEXT,         CTEXT         )
pick ( CHECKBOX,      CHECKBOX      )
pick ( PUSHBUTTON,    PUSHBUTTON    )
pick ( LISTBOX,       LISTBOX       )
pick ( GROUPBOX,      GROUPBOX      )
pick ( DEFPUSHBUTTON, DEFPUSHBUTTON )
pick ( RADIOBUTTON,   RADIOBUTTON   )
pick ( EDITTEXT,      EDITTEXT      )
pick ( COMBOBOX,      COMBOBOX      )
pick ( ICON,          ICON          )
pick ( SCROLLBAR,     CONTROL       )
pick ( CONTROL,       CONTROL       )

#elif TABLE == 2
/**************************************
  control class items conversion
**************************************/
pick ( BUTTON,        WC_BUTTON         )
pick ( COMBOBOX,      WC_COMBOBOX       )
pick ( EDIT,          WC_ENTRYFIELD     )
pick ( LISTBOX,       WC_LISTBOX        )
pick ( SCROLLBAR,     WC_SCROLLBAR      )
pick ( STATIC,        WC_STATIC         )

#elif TABLE == 3
/**************************************
  dialog font name conversion
**************************************/
pick ( MS_SANS_SERIF, MS Sans Serif, Helv            )
pick ( MS_SERIF,      MS Serif,      Tms Rmn         )
pick ( TIMES,         Times,         Times New Roman )
pick ( ARIAL,         Arial,         Helvetica       )

#elif TABLE == 4
/**************************************
  window style items conversion
**************************************/
pick1( DS_LOCALEDIT          )    
pick1( DS_MODALFRAME         )        
pick1( DS_NOIDLEMSG          )    
pick ( DS_SYSMODAL,          FCF_SYSMODAL     )
pick1( WS_BORDER             )
pick ( WS_CAPTION,           FCF_TITLEBAR     )
pick1( WS_CHILD              )
pick1( WS_CHILDWINDOW        )        
pick ( WS_CLIPCHILDREN,      WS_CLIPCHILDREN  )
pick ( WS_CLIPSIBLINGS,      WS_CLIPSIBLINGS  )         
pick ( WS_DISABLED,          WS_DISABLED      )
pick ( WS_DLGFRAME,          FCF_DLGBORDER    )
pick ( WS_GROUP,             WS_GROUP         )
pick ( WS_HSCROLL,           FCF_HORZSCROLL   )
pick ( WS_ICONIC,            FCF_ICON         )
pick ( WS_MAXIMIZE,          WS_MAXIMIZED     )
pick ( WS_MAXIMIZEBOX,       WS_MAXBUTTON     )  
pick ( WS_MINIMIZE,          WS_MINIMIZED     )
pick ( WS_MINIMIZEBOX,       WS_MINBUTTON     )  
pick1( WS_OVERLAPPED         )        
pick1( WS_OVERLAPPEDWINDOW   )                    
pick1( WS_POPUP              )
pick1( WS_POPUPWINDOW        )        
pick ( WS_SIZEBOX,           FCF_SIZEBORDER   )
pick ( WS_SYSMENU,           FCF_SYSMENU      )
pick ( WS_TABSTOP,           WS_TABSTOP       )
pick ( WS_THICKFRAME,        FCF_SIZEBORDER   )    
pick ( WS_VISIBLE,           WS_VISIBLE       )
pick1( WS_VSCROLL            )

#elif TABLE == 5
/**************************************
  style items conversion
**************************************/
pick ( LTEXT,         WS_TABSTOP,                   WS_TABSTOP    )
pick ( LTEXT,         WS_GROUP,                     WS_GROUP      )
pick ( RTEXT,         WS_TABSTOP,                   WS_TABSTOP    )
pick ( RTEXT,         WS_GROUP,                     WS_GROUP      )
pick ( CTEXT,         WS_TABSTOP,                   WS_TABSTOP    )
pick ( CTEXT,         WS_GROUP,                     WS_GROUP      )
pick ( CHECKBOX,      WS_TABSTOP,                   WS_TABSTOP    )
pick ( CHECKBOX,      WS_GROUP,                     WS_GROUP      )
pick ( PUSHBUTTON,    WS_TABSTOP,                   WS_TABSTOP    )
pick ( PUSHBUTTON,    WS_GROUP,                     WS_GROUP      )
pick ( PUSHBUTTON,    WS_DISABLED,                  WS_DISABLED   )
pick1( LISTBOX,       WS_BORDER                     )
pick1( LISTBOX,       WS_VSCROLL                    )
pick ( GROUPBOX,      WS_TABSTOP,                   WS_TABSTOP    )
pick ( GROUPBOX,      WS_DISABLED,                  WS_DISABLED   )
pick ( DEFPUSHBUTTON, WS_TABSTOP,                   WS_TABSTOP    )
pick ( DEFPUSHBUTTON, WS_GROUP,                     WS_GROUP      )
pick ( DEFPUSHBUTTON, WS_DISABLED,                  WS_DISABLED   )
pick ( RADIOBUTTON,   WS_TABSTOP,                   WS_TABSTOP    )
pick ( RADIOBUTTON,   WS_GROUP,                     WS_GROUP      )
pick ( RADIOBUTTON,   WS_DISABLED,                  WS_DISABLED   )
pick ( EDITTEXT,      WS_TABSTOP,                   WS_TABSTOP    )
pick ( EDITTEXT,      WS_GROUP,                     WS_GROUP      )
pick ( EDITTEXT,      WS_DISABLED,                  WS_DISABLED   )
pick1( EDITTEXT,      WS_VSCROLL                    )
pick1( EDITTEXT,      WS_HSCROLL                    )
pick ( COMBOBOX,      WS_TABSTOP,                   WS_TABSTOP    )
pick ( COMBOBOX,      WS_GROUP,                     WS_GROUP      )
pick ( COMBOBOX,      WS_DISABLED,                  WS_DISABLED   )
pick1( COMBOBOX,      WS_VSCROLL                    )
pick ( ICON,          SS_ICON,                      SS_ICON       )
pick ( SCROLLBAR,     WS_TABSTOP,                   WS_TABSTOP    )
pick ( SCROLLBAR,     WS_GROUP,                     WS_GROUP      )
pick ( SCROLLBAR,     WS_DISABLED,                  WS_DISABLED   )
pick ( CONTROL,       BS_PUSHBUTTON,                BS_PUSHBUTTON       )
pick ( CONTROL,       BS_DEFPUSHBUTTON,             BS_DEFAULT          )
pick ( CONTROL,       BS_CHECKBOX,                  BS_CHECKBOX         )
pick ( CONTROL,       BS_AUTOCHECKBOX,              BS_AUTOCHECKBOX     )
pick ( CONTROL,       BS_RADIOBUTTON,               BS_RADIOBUTTON      )
pick ( CONTROL,       BS_AUTORADIOBUTTON,           BS_AUTORADIOBUTTON  )
pick1( CONTROL,       BS_LEFTTEXT                   )
pick ( CONTROL,       BS_3STATE,                    BS_3STATE           )
pick ( CONTROL,       BS_AUTO3STATE,                BS_AUTO3STATE       )
pick1( CONTROL,       BS_GROUPBOX                   )
pick ( CONTROL,       BS_OWNERDRAW,                 BS_USERBUTTON       )
pick ( CONTROL,       CBS_SIMPLE,                   CBS_SIMPLE          )
pick ( CONTROL,       CBS_DROPDOWN,                 CBS_DROPDOWN        )
pick1( CONTROL,       CBS_OWNERDRAWFIXED            )
pick1( CONTROL,       CBS_OWNERDRAWVARIABLE         )
pick1( CONTROL,       CBS_AUTOHSCROLL               )
pick1( CONTROL,       CBS_SORT                      )
pick1( CONTROL,       CBS_HASSTRINGS                )
pick1( CONTROL,       CBS_OEMCONVERT                )
pick ( CONTROL,       ES_LEFT,                      ES_LEFT             )
pick ( CONTROL,       ES_CENTER,                    ES_CENTER           )
pick ( CONTROL,       ES_RIGHT,                     ES_RIGHT            )
pick1( CONTROL,       ES_LOWERCASE                  )
pick1( CONTROL,       ES_UPPERCASE                  )
pick ( CONTROL,       ES_PASSWORD,                  ES_UNREADABLE       )
pick ( CONTROL,       ES_MULTILINE,                 WC_MLE              )
pick ( CONTROL,       ES_AUTOVSCROLL,               ES_AUTOSCROLL       )
pick ( CONTROL,       ES_AUTOHSCROLL,               ES_AUTOSCROLL       )
pick1( CONTROL,       ES_NOHIDESEL                  )
pick1( CONTROL,       ES_OEMCONVERT                 )
pick ( CONTROL,       LBS_STANDARD,                 WC_LISTBOX          )
pick ( CONTROL,       LBS_EXTENDEDSEL,              LS_EXTENDEDSEL      )
pick1( CONTROL,       LBS_HASSTRINGS                )
pick1( CONTROL,       LBS_NOTIFY                    )
pick ( CONTROL,       LBS_MULTIPLESEL,              LS_MULTIPLESEL      )
pick1( CONTROL,       LBS_MULTICOLUMN               )
pick1( CONTROL,       LBS_NOINTEGRALHEIGHT          )
pick1( CONTROL,       LBS_SORT                      )
pick1( CONTROL,       LBS_NOREDRAW                  )
pick ( CONTROL,       LBS_OWNERDRAWFIXED,           LS_OWNERDRAW        )
pick ( CONTROL,       LBS_OWNERDRAWVARIABLE,        LS_OWNERDRAW        )
pick ( CONTROL,       LBS_USETABSTOPS,              ES_AUTOTAB          )
pick1( CONTROL,       LBS_WANTKEYBOARDINPUT         )
pick ( CONTROL,       SBS_VERT,                     SBS_VERT            )
pick1( CONTROL,       SBS_RIGHTALIGN                )
pick1( CONTROL,       SBS_LEFTALIGN                 )
pick ( CONTROL,       SBS_HORZ,                     SBS_HORZ            )
pick1( CONTROL,       SBS_TOPALIGN                  )
pick1( CONTROL,       SBS_BOTTOMALIGN               )
pick ( CONTROL,       SBS_SIZEBOX,                  SBS_AUTOSIZE        )
pick ( CONTROL,       SBS_SIZEBOXTOPLEFTALIGN,      SBS_AUTOSIZE        )
pick ( CONTROL,       SBS_SIZEBOXBOTTOMRIGHTALIGN,  SBS_AUTOSIZE        )
pick ( CONTROL,       SS_LEFT,                      SS_TEXT | DT_LEFT   )
pick ( CONTROL,       SS_CENTER,                    SS_TEXT | DT_CENTER )
pick ( CONTROL,       SS_RIGHT,                     SS_TEXT | DT_RIGHT  )
pick ( CONTROL,       SS_LEFTNOWORDWRAP,            SS_TEXT | DT_WORDBREAK | DT_TOP | DT_LEFT )
pick1( CONTROL,       SS_SIMPLE                     )
pick1( CONTROL,       SS_NOPREFIX                   )
pick ( CONTROL,       SS_ICON,                      SS_ICON             )
pick ( CONTROL,       SS_BLACKRECT,                 SS_BLACKRECT        )
pick ( CONTROL,       SS_GRAYRECT,                  SS_GRAYRECT         )
pick ( CONTROL,       SS_WHITERECT,                 SS_WHITERECT        )
pick ( CONTROL,       SS_BLACKFRAME,                SS_BLACKFRAME       )
pick ( CONTROL,       SS_GRAYFRAME,                 SS_GRAYFRAME        )
pick ( CONTROL,       SS_WHITEFRAME,                SS_WHITEFRAME       )
pick ( CONTROL,       SS_USERITEM,                  SS_USERITEM         )

#endif
