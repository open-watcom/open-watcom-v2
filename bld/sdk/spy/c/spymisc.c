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
* Description:  Spy miscellaneous functions.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <commdlg.h>
#include "spyexmsg.h"

/* These styles are included in Win16 headers, but not Win32 headers. */
#ifndef CS_KEYCVTWINDOW
    #define CS_KEYCVTWINDOW 0x0004
#endif
#ifndef CS_NOKEYCVT
    #define CS_NOKEYCVT     0x0100
#endif

typedef struct {
    char    *name;
    DWORD   flags;
    DWORD   mask;
} style_info;

typedef struct {
    char        *class_name;
    style_info  *style_array;
    WORD        *style_array_size;
} class_styles;

static style_info __near StyleArray[] = {
    { "WS_POPUP",           WS_POPUP,           WS_POPUP            },
    { "WS_CHILD",           WS_CHILD,           WS_CHILD            },
    { "WS_OVERLAPPED",      WS_OVERLAPPED,      WS_POPUP | WS_CHILD },
    { "WS_BORDER",          WS_BORDER,          WS_BORDER           },
    { "WS_CAPTION",         WS_CAPTION,         WS_CAPTION          },
    { "WS_SYSMENU",         WS_SYSMENU,         WS_SYSMENU          },
    { "WS_THICKFRAME",      WS_THICKFRAME,      WS_THICKFRAME       },
    { "WS_MINIMIZEBOX",     WS_MINIMIZEBOX,     WS_MINIMIZEBOX      },
    { "WS_MAXIMIZEBOX",     WS_MAXIMIZEBOX,     WS_MAXIMIZEBOX      },
    { "WS_MINIMIZE",        WS_MINIMIZE,        WS_MINIMIZE         },
    { "WS_VISIBLE",         WS_VISIBLE,         WS_VISIBLE          },
    { "WS_DISABLED",        WS_DISABLED,        WS_DISABLED         },
    { "WS_CLIPSIBLINGS",    WS_CLIPSIBLINGS,    WS_CLIPSIBLINGS     },
    { "WS_CLIPCHILDREN",    WS_CLIPCHILDREN,    WS_CLIPCHILDREN     },
    { "WS_MAXIMIZE",        WS_MAXIMIZE,        WS_MAXIMIZE         },
    { "WS_DLGFRAME",        WS_DLGFRAME,        WS_DLGFRAME         },
    { "WS_VSCROLL",         WS_VSCROLL,         WS_VSCROLL          },
    { "WS_HSCROLL",         WS_HSCROLL,         WS_HSCROLL          },
    { "WS_GROUP",           WS_GROUP,           WS_GROUP            },
    { "WS_TABSTOP",         WS_TABSTOP,         WS_TABSTOP          }
};

static WORD StyleArraySize = sizeof( StyleArray ) / sizeof( style_info );

static style_info __near ButtonStyleArray[] = {
    { "BS_PUSHBUTTON",      BS_PUSHBUTTON,      BS_TYPEMASK         },
    { "BS_DEFPUSHBUTTON",   BS_DEFPUSHBUTTON,   BS_TYPEMASK         },
    { "BS_CHECKBOX",        BS_CHECKBOX,        BS_TYPEMASK         },
    { "BS_AUTOCHECKBOX",    BS_AUTOCHECKBOX,    BS_TYPEMASK         },
    { "BS_RADIOBUTTON",     BS_RADIOBUTTON,     BS_TYPEMASK         },
    { "BS_3STATE",          BS_3STATE,          BS_TYPEMASK         },
    { "BS_AUTO3STATE",      BS_AUTO3STATE,      BS_TYPEMASK         },
    { "BS_GROUPBOX",        BS_GROUPBOX,        BS_TYPEMASK         },
    { "BS_USERBUTTON",      BS_USERBUTTON,      BS_TYPEMASK         },
    { "BS_AUTORADIOBUTTON", BS_AUTORADIOBUTTON, BS_TYPEMASK         },
    { "BS_OWNERDRAW",       BS_OWNERDRAW,       BS_TYPEMASK         },
#ifndef __NT__
    { "BS_LEFTTEXT",        BS_LEFTTEXT,        BS_LEFTTEXT         }
#else
    { "BS_LEFTTEXT",        BS_LEFTTEXT,        BS_LEFTTEXT         },
    { "BS_TEXT",            BS_TEXT,            BS_ICON | BS_BITMAP },
    { "BS_ICON",            BS_ICON,            BS_ICON | BS_BITMAP },
    { "BS_BITMAP",          BS_BITMAP,          BS_ICON | BS_BITMAP },
    { "BS_LEFT",            BS_LEFT,            BS_LEFT | BS_RIGHT  },
    { "BS_RIGHT",           BS_RIGHT,           BS_LEFT | BS_RIGHT  },
    { "BS_CENTER",          BS_CENTER,          BS_LEFT | BS_RIGHT  },
    { "BS_TOP",             BS_TOP,             BS_TOP | BS_BOTTOM  },
    { "BS_BOTTOM",          BS_BOTTOM,          BS_TOP | BS_BOTTOM  },
    { "BS_VCENTER",         BS_VCENTER,         BS_TOP | BS_BOTTOM  },
    { "BS_PUSHLIKE",        BS_PUSHLIKE,        BS_PUSHLIKE         },
    { "BS_MULTILINE",       BS_MULTILINE,       BS_MULTILINE        },
    { "BS_NOTIFY",          BS_NOTIFY,          BS_NOTIFY           },
    { "BS_FLAT",            BS_FLAT,            BS_FLAT             }
#endif
};

static WORD ButtonStyleArraySize = sizeof( ButtonStyleArray ) / sizeof( style_info );

static style_info __near EditStyleArray[] = {
    { "ES_LEFT",        ES_LEFT,        ES_CENTER | ES_RIGHT },
    { "ES_CENTER",      ES_CENTER,      ES_CENTER | ES_RIGHT },
    { "ES_RIGHT",       ES_RIGHT,       ES_CENTER | ES_RIGHT },
    { "ES_MULTILINE",   ES_MULTILINE,   ES_MULTILINE         },
    { "ES_UPPERCASE",   ES_UPPERCASE,   ES_UPPERCASE         },
    { "ES_LOWERCASE",   ES_LOWERCASE,   ES_LOWERCASE         },
    { "ES_PASSWORD",    ES_PASSWORD,    ES_PASSWORD          },
    { "ES_AUTOVSCROLL", ES_AUTOVSCROLL, ES_AUTOVSCROLL       },
    { "ES_AUTOHSCROLL", ES_AUTOHSCROLL, ES_AUTOHSCROLL       },
    { "ES_NOHIDESEL",   ES_NOHIDESEL,   ES_NOHIDESEL         },
    { "ES_OEMCONVERT",  ES_OEMCONVERT,  ES_OEMCONVERT        },
    { "ES_READONLY",    ES_READONLY,    ES_READONLY          },
#ifndef __NT__
    { "ES_WANTRETURN",  ES_WANTRETURN,  ES_WANTRETURN        }
#else
    { "ES_WANTRETURN",  ES_WANTRETURN,  ES_WANTRETURN        },
    { "ES_NUMBER",      ES_NUMBER,      ES_NUMBER            }
#endif
};

static WORD EditStyleArraySize = sizeof( EditStyleArray ) / sizeof( style_info );

static style_info __near StaticStyleArray[] = {
    { "SS_LEFT",            SS_LEFT,            SS_TYPEMASK        },
    { "SS_CENTER",          SS_CENTER,          SS_TYPEMASK        },
    { "SS_RIGHT",           SS_RIGHT,           SS_TYPEMASK        },
    { "SS_ICON",            SS_ICON,            SS_TYPEMASK        },
    { "SS_BLACKRECT",       SS_BLACKRECT,       SS_TYPEMASK        },
    { "SS_GRAYRECT",        SS_GRAYRECT,        SS_TYPEMASK        },
    { "SS_WHITERECT",       SS_WHITERECT,       SS_TYPEMASK        },
    { "SS_BLACKFRAME",      SS_BLACKFRAME,      SS_TYPEMASK        },
    { "SS_GRAYFRAME",       SS_GRAYFRAME,       SS_TYPEMASK        },
    { "SS_WHITEFRAME",      SS_WHITEFRAME,      SS_TYPEMASK        },
#ifdef __NT__
    { "SS_USERITEM",        SS_USERITEM,        SS_TYPEMASK        },
#endif
    { "SS_SIMPLE",          SS_SIMPLE,          SS_TYPEMASK        },
    { "SS_LEFTNOWORDWRAP",  SS_LEFTNOWORDWRAP,  SS_TYPEMASK        },
#ifndef __NT__
    { "SS_NOPREFIX",        SS_NOPREFIX,        SS_NOPREFIX        }
#else
    { "SS_OWNERDRAW",       SS_OWNERDRAW,       SS_TYPEMASK        },
    { "SS_BITMAP",          SS_BITMAP,          SS_TYPEMASK        },
    { "SS_ENHMETAFILE",     SS_ENHMETAFILE,     SS_TYPEMASK        },
    { "SS_ETCHEDHORZ",      SS_ETCHEDHORZ,      SS_TYPEMASK        },
    { "SS_ETCHEDVERT",      SS_ETCHEDVERT,      SS_TYPEMASK        },
    { "SS_ETCHEDFRAME",     SS_ETCHEDFRAME,     SS_TYPEMASK        },
    { "SS_REALSIZECONTROL", SS_REALSIZECONTROL, SS_REALSIZECONTROL },
    { "SS_NOPREFIX",        SS_NOPREFIX,        SS_NOPREFIX        },
    { "SS_NOTIFY",          SS_NOTIFY,          SS_NOTIFY          },
    { "SS_CENTERIMAGE",     SS_CENTERIMAGE,     SS_CENTERIMAGE     },
    { "SS_RIGHTJUST",       SS_RIGHTJUST,       SS_RIGHTJUST       },
    { "SS_REALSIZEIMAGE",   SS_REALSIZEIMAGE,   SS_REALSIZEIMAGE   },
    { "SS_SUNKEN",          SS_SUNKEN,          SS_SUNKEN          },
    { "SS_EDITCONTROL",     SS_EDITCONTROL,     SS_EDITCONTROL     },
    { "SS_ENDELLIPSIS",     SS_ENDELLIPSIS,     SS_ELLIPSISMASK    },
    { "SS_PATHELLIPSIS",    SS_PATHELLIPSIS,    SS_ELLIPSISMASK    },
    { "SS_WORDELLIPSIS",    SS_WORDELLIPSIS,    SS_ELLIPSISMASK    }
#endif
};

static WORD StaticStyleArraySize = sizeof( StaticStyleArray ) / sizeof( style_info );

static style_info __near ListBoxStyleArray[] = {
    { "LBS_NOTIFY",             LBS_NOTIFY,             LBS_NOTIFY            },
    { "LBS_SORT",               LBS_SORT,               LBS_SORT              },
    { "LBS_NOREDRAW",           LBS_NOREDRAW,           LBS_NOREDRAW          },
    { "LBS_MULTIPLESEL",        LBS_MULTIPLESEL,        LBS_MULTIPLESEL       },
    { "LBS_OWNERDRAWFIXED",     LBS_OWNERDRAWFIXED,     LBS_OWNERDRAWFIXED    },
    { "LBS_OWNERDRAWVARIABLE",  LBS_OWNERDRAWVARIABLE,  LBS_OWNERDRAWVARIABLE },
    { "LBS_HASSTRINGS",         LBS_HASSTRINGS,         LBS_HASSTRINGS        },
    { "LBS_USETABSTOPS",        LBS_USETABSTOPS,        LBS_USETABSTOPS       },
    { "LBS_NOINTEGRALHEIGHT",   LBS_NOINTEGRALHEIGHT,   LBS_NOINTEGRALHEIGHT  },
    { "LBS_MULTICOLUMN",        LBS_MULTICOLUMN,        LBS_MULTICOLUMN       },
    { "LBS_WANTKEYBOARDINPUT",  LBS_WANTKEYBOARDINPUT,  LBS_WANTKEYBOARDINPUT },
    { "LBS_EXTENDEDSEL",        LBS_EXTENDEDSEL,        LBS_EXTENDEDSEL       },
#ifndef __NT__
    { "LBS_DISABLENOSCROLL",    LBS_DISABLENOSCROLL,    LBS_DISABLENOSCROLL   }
#else
    { "LBS_DISABLENOSCROLL",    LBS_DISABLENOSCROLL,    LBS_DISABLENOSCROLL   },
    { "LBS_NODATA",             LBS_NODATA,             LBS_NODATA            },
    { "LBS_NOSEL",              LBS_NOSEL,              LBS_NOSEL             },
    { "LBS_COMBOBOX",           LBS_COMBOBOX,           LBS_COMBOBOX          }
#endif
};

static WORD ListBoxStyleArraySize = sizeof( ListBoxStyleArray ) / sizeof( style_info );

static style_info __near ComboBoxStyleArray[] = {
    { "CBS_SIMPLE",             CBS_SIMPLE,             CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_DROPDOWN",           CBS_DROPDOWN,           CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_DROPDOWNLIST",       CBS_DROPDOWNLIST,       CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_OWNERDRAWFIXED",     CBS_OWNERDRAWFIXED,     CBS_OWNERDRAWFIXED        },
    { "CBS_OWNERDRAWVARIABLE",  CBS_OWNERDRAWVARIABLE,  CBS_OWNERDRAWVARIABLE     },
    { "CBS_AUTOHSCROLL",        CBS_AUTOHSCROLL,        CBS_AUTOHSCROLL           },
    { "CBS_OEMCONVERT",         CBS_OEMCONVERT,         CBS_OEMCONVERT            },
    { "CBS_SORT",               CBS_SORT,               CBS_SORT                  },
    { "CBS_HASSTRINGS",         CBS_HASSTRINGS,         CBS_HASSTRINGS            },
    { "CBS_NOINTEGRALHEIGHT",   CBS_NOINTEGRALHEIGHT,   CBS_NOINTEGRALHEIGHT      },
#ifndef __NT__
    { "CBS_DISABLENOSCROLL",    CBS_DISABLENOSCROLL,    CBS_DISABLENOSCROLL       }
#else
    { "CBS_DISABLENOSCROLL",    CBS_DISABLENOSCROLL,    CBS_DISABLENOSCROLL       },
    { "CBS_UPPERCASE",          CBS_UPPERCASE,          CBS_UPPERCASE             },
    { "CBS_LOWERCASE",          CBS_LOWERCASE,          CBS_LOWERCASE             }
#endif
};

static WORD ComboBoxStyleArraySize = sizeof( ComboBoxStyleArray ) / sizeof( style_info );

static style_info __near DialogStyleArray[] = {
    { "DS_ABSALIGN",        DS_ABSALIGN,        DS_ABSALIGN              },
    { "DS_SYSMODAL",        DS_SYSMODAL,        DS_SYSMODAL              },
    { "DS_LOCALEDIT",       DS_LOCALEDIT,       DS_LOCALEDIT             },
#ifndef __NT__
    { "DS_SETFONT",         DS_SETFONT,         DS_SETFONT               }
#else
    { "DS_SETFONT",         DS_SETFONT,         DS_SETFONT | DS_FIXEDSYS },
    { "DS_SETFOREGROUND",   DS_SETFOREGROUND,   DS_SETFOREGROUND         },
    { "DS_3DLOOK",          DS_3DLOOK,          DS_3DLOOK                },
    { "DS_FIXEDSYS",        DS_FIXEDSYS,        DS_SETFONT | DS_FIXEDSYS },
    { "DS_NOFAILCREATE",    DS_NOFAILCREATE,    DS_NOFAILCREATE          },
    { "DS_CONTROL",         DS_CONTROL,         DS_CONTROL               },
    { "DS_CENTER",          DS_CENTER,          DS_CENTER                },
    { "DS_CENTERMOUSE",     DS_CENTERMOUSE,     DS_CENTERMOUSE           },
    { "DS_CONTEXTHELP",     DS_CONTEXTHELP,     DS_CONTEXTHELP           },
    { "DS_SHELLFONT",       DS_SHELLFONT,       DS_SETFONT | DS_FIXEDSYS }
#endif
};

static WORD DialogStyleArraySize = sizeof( DialogStyleArray ) / sizeof( style_info );

#ifdef __NT__
static style_info __near AnimateStyleArray[] = {
    { "ACS_CENTER",         ACS_CENTER,         ACS_CENTER      },
    { "ACS_TRANSPARENT",    ACS_TRANSPARENT,    ACS_TRANSPARENT },
    { "ACS_AUTOPLAY",       ACS_AUTOPLAY,       ACS_AUTOPLAY    },
    { "ACS_TIMER",          ACS_TIMER,          ACS_TIMER       }
};

static WORD AnimateStyleArraySize = sizeof( AnimateStyleArray ) / sizeof( style_info );

static style_info __near DateTimeStyleArray[] = {
    { "DTS_UPDOWN",             DTS_UPDOWN,         DTS_UPDOWN         },
    { "DTS_SHOWNONE",           DTS_SHOWNONE,       DTS_SHOWNONE       },
    { "DTS_SHORTDATECENTURYFORMAT", DTS_SHORTDATECENTURYFORMAT,
      DTS_SHORTDATECENTURYFORMAT },
    { "DTS_LONGDATEFORMAT",     DTS_LONGDATEFORMAT, DTS_LONGDATEFORMAT },
    { "DTS_TIMEFORMAT",         DTS_TIMEFORMAT,     DTS_TIMEFORMAT     },
    { "DTS_SHORTDATEFORMAT",    DTS_SHORTDATEFORMAT,
      DTS_SHORTDATECENTURYFORMAT | DTS_LONGDATEFORMAT | DTS_TIMEFORMAT },
    { "DTS_APPCANPARSE",        DTS_APPCANPARSE,    DTS_APPCANPARSE    },
    { "DTS_RIGHTALIGN",         DTS_RIGHTALIGN,     DTS_RIGHTALIGN     }
};

static WORD DateTimeStyleArraySize = sizeof( DateTimeStyleArray ) / sizeof( style_info );

static style_info __near MonthCalStyleArray[] = {
    { "MCS_DAYSTATE",           MCS_DAYSTATE,           MCS_DAYSTATE         },
    { "MCS_MULTISELECT",        MCS_MULTISELECT,        MCS_MULTISELECT      },
    { "MCS_WEEKNUMBERS",        MCS_WEEKNUMBERS,        MCS_WEEKNUMBERS      },
    { "MCS_NOTODAYCIRCLE",      MCS_NOTODAYCIRCLE,      MCS_NOTODAYCIRCLE    },
    { "MCS_NOTODAY",            MCS_NOTODAY,            MCS_NOTODAY          },
    { "MCS_NOTRAILINGDATES",    MCS_NOTRAILINGDATES,    MCS_NOTRAILINGDATES  },
    { "MCS_SHORTDAYSOFWEEK",    MCS_SHORTDAYSOFWEEK,    MCS_SHORTDAYSOFWEEK  },
    { "MCS_NOSELCHANGEONNAV",   MCS_NOSELCHANGEONNAV,   MCS_NOSELCHANGEONNAV }
};

static WORD MonthCalStyleArraySize = sizeof( MonthCalStyleArray ) / sizeof( style_info );

static style_info __near ProgressBarStyleArray[] = {
    { "PBS_SMOOTH",         PBS_SMOOTH,         PBS_SMOOTH        },
    { "PBS_VERTICAL",       PBS_VERTICAL,       PBS_VERTICAL      },
    { "PBS_MARQUEE",        PBS_MARQUEE,        PBS_MARQUEE       },
    { "PBS_SMOOTHREVERSE",  PBS_SMOOTHREVERSE,  PBS_SMOOTHREVERSE }
};

static WORD ProgressBarStyleArraySize = sizeof( ProgressBarStyleArray ) /
                                        sizeof( style_info );

static style_info __near RebarStyleArray[] = {
    { "RBS_TOOLTIPS",           RBS_TOOLTIPS,           RBS_TOOLTIPS        },
    { "RBS_VARHEIGHT",          RBS_VARHEIGHT,          RBS_VARHEIGHT       },
    { "RBS_BANDBORDERS",        RBS_BANDBORDERS,        RBS_BANDBORDERS     },
    { "RBS_FIXEDORDER",         RBS_FIXEDORDER,         RBS_FIXEDORDER      },
    { "RBS_REGISTERDROP",       RBS_REGISTERDROP,       RBS_REGISTERDROP    },
    { "RBS_AUTOSIZE",           RBS_AUTOSIZE,           RBS_AUTOSIZE        },
    { "RBS_VERTICALGRIPPER",    RBS_VERTICALGRIPPER,    RBS_VERTICALGRIPPER },
    { "RBS_DBLCLKTOGGLE",       RBS_DBLCLKTOGGLE,       RBS_DBLCLKTOGGLE    }
};

static WORD RebarStyleArraySize = sizeof( RebarStyleArray ) / sizeof( style_info );

static style_info __near StatusBarStyleArray[] = {
    { "SBARS_SIZEGRIP", SBARS_SIZEGRIP, SBARS_SIZEGRIP },
    { "SBARS_TOOLTIPS", SBARS_TOOLTIPS, SBARS_TOOLTIPS }
};

static WORD StatusBarStyleArraySize = sizeof( StatusBarStyleArray ) /
                                      sizeof( style_info );

static style_info __near ToolbarStyleArray[] = {
    { "TBSTYLE_TOOLTIPS",       TBSTYLE_TOOLTIPS,       TBSTYLE_TOOLTIPS     },
    { "TBSTYLE_WRAPABLE",       TBSTYLE_WRAPABLE,       TBSTYLE_WRAPABLE     },
    { "TBSTYLE_ALTDRAG",        TBSTYLE_ALTDRAG,        TBSTYLE_ALTDRAG      },
    { "TBSTYLE_FLAT",           TBSTYLE_FLAT,           TBSTYLE_FLAT         },
    { "TBSTYLE_LIST",           TBSTYLE_LIST,           TBSTYLE_LIST         },
    { "TBSTYLE_CUSTOMERASE",    TBSTYLE_CUSTOMERASE,    TBSTYLE_CUSTOMERASE  },
    { "TBSTYLE_REGISTERDROP",   TBSTYLE_REGISTERDROP,   TBSTYLE_REGISTERDROP },
    { "TBSTYLE_TRANSPARENT",    TBSTYLE_TRANSPARENT,    TBSTYLE_TRANSPARENT  }
};

static WORD ToolbarStyleArraySize = sizeof( ToolbarStyleArray ) / sizeof( style_info );

static style_info __near ToolTipsStyleArray[] = {
    { "TTS_ALWAYSTIP",      TTS_ALWAYSTIP,      TTS_ALWAYSTIP      },
    { "TTS_NOPREFIX",       TTS_NOPREFIX,       TTS_NOPREFIX       },
    { "TTS_NOANIMATE",      TTS_NOANIMATE,      TTS_NOANIMATE      },
    { "TTS_NOFADE",         TTS_NOFADE,         TTS_NOFADE         },
    { "TTS_BALLOON",        TTS_BALLOON,        TTS_BALLOON        },
    { "TTS_CLOSE",          TTS_CLOSE,          TTS_CLOSE          },
    { "TTS_USEVISUALSTYLE", TTS_USEVISUALSTYLE, TTS_USEVISUALSTYLE }
};

static WORD ToolTipsStyleArraySize = sizeof( ToolTipsStyleArray ) / sizeof( style_info );

static style_info __near TrackBarStyleArray[] = {
    { "TBS_AUTOTICKS",          TBS_AUTOTICKS,          TBS_AUTOTICKS        },
    { "TBS_VERT",               TBS_VERT,               TBS_VERT             },
    { "TBS_LEFT",               TBS_VERT | TBS_LEFT,    TBS_VERT | TBS_LEFT  },
    { "TBS_RIGHT",              TBS_VERT,
      TBS_VERT | TBS_LEFT | TBS_BOTH | TBS_NOTICKS },
    { "TBS_HORZ",               TBS_HORZ,               TBS_VERT             },
    { "TBS_TOP",                TBS_TOP,                TBS_VERT | TBS_TOP   },
    { "TBS_BOTTOM",             TBS_HORZ,
      TBS_VERT | TBS_TOP | TBS_BOTH | TBS_NOTICKS },
    { "TBS_BOTH",               TBS_BOTH,               TBS_BOTH             },
    { "TBS_NOTICKS",            TBS_NOTICKS,            TBS_NOTICKS          },
    { "TBS_ENABLESELRANGE",     TBS_ENABLESELRANGE,     TBS_ENABLESELRANGE   },
    { "TBS_FIXEDLENGTH",        TBS_FIXEDLENGTH,        TBS_FIXEDLENGTH      },
    { "TBS_NOTHUMB",            TBS_NOTHUMB,            TBS_NOTHUMB          },
    { "TBS_TOOLTIPS",           TBS_TOOLTIPS,           TBS_TOOLTIPS         },
    { "TBS_REVERSED",           TBS_REVERSED,           TBS_REVERSED         },
    { "TBS_DOWNISLEFT",         TBS_DOWNISLEFT,         TBS_DOWNISLEFT       },
    { "TBS_NOTIFYBEFOREMOVE",   TBS_NOTIFYBEFOREMOVE,   TBS_NOTIFYBEFOREMOVE },
    { "TBS_TRANSPARENTBKGND",   TBS_TRANSPARENTBKGND,   TBS_TRANSPARENTBKGND }
};

static WORD TrackBarStyleArraySize = sizeof( TrackBarStyleArray ) / sizeof( style_info );

static style_info __near UpDownStyleArray[] = {
    { "UDS_WRAP",           UDS_WRAP,           UDS_WRAP        },
    { "UDS_SETBUDDYINT",    UDS_SETBUDDYINT,    UDS_SETBUDDYINT },
    { "UDS_ALIGNRIGHT",     UDS_ALIGNRIGHT,     UDS_ALIGNRIGHT  },
    { "UDS_ALIGNLEFT",      UDS_ALIGNLEFT,      UDS_ALIGNLEFT   },
    { "UDS_AUTOBUDDY",      UDS_AUTOBUDDY,      UDS_AUTOBUDDY   },
    { "UDS_ARROWKEYS",      UDS_ARROWKEYS,      UDS_ARROWKEYS   },
    { "UDS_HORZ",           UDS_HORZ,           UDS_HORZ        },
    { "UDS_NOTHOUSANDS",    UDS_NOTHOUSANDS,    UDS_NOTHOUSANDS },
    { "UDS_HOTTRACK",       UDS_HOTTRACK,       UDS_HOTTRACK    }
};

static WORD UpDownStyleArraySize = sizeof( UpDownStyleArray ) / sizeof( style_info );

static style_info __near HeaderStyleArray[] = {
    { "HDS_BUTTONS",    HDS_BUTTONS,    HDS_BUTTONS    },
    { "HDS_HOTTRACK",   HDS_HOTTRACK,   HDS_HOTTRACK   },
    { "HDS_HIDDEN",     HDS_HIDDEN,     HDS_HIDDEN     },
    { "HDS_DRAGDROP",   HDS_DRAGDROP,   HDS_DRAGDROP   },
    { "HDS_FULLDRAG",   HDS_FULLDRAG,   HDS_FULLDRAG   },
    { "HDS_FILTERBAR",  HDS_FILTERBAR,  HDS_FILTERBAR  },
    { "HDS_FLAT",       HDS_FLAT,       HDS_FLAT       },
    { "HDS_CHECKBOXES", HDS_CHECKBOXES, HDS_CHECKBOXES },
    { "HDS_NOSIZING",   HDS_NOSIZING,   HDS_NOSIZING   },
    { "HDS_OVERFLOW",   HDS_OVERFLOW,   HDS_OVERFLOW   }
};

static WORD HeaderStyleArraySize = sizeof( HeaderStyleArray ) / sizeof( style_info );

static style_info __near ListViewStyleArray[] = {
    { "LVS_ICON",               LVS_ICON,               LVS_TYPEMASK        },
    { "LVS_REPORT",             LVS_REPORT,             LVS_TYPEMASK        },
    { "LVS_SMALLICON",          LVS_SMALLICON,          LVS_TYPEMASK        },
    { "LVS_LIST",               LVS_LIST,               LVS_TYPEMASK        },
    { "LVS_SINGLESEL",          LVS_SINGLESEL,          LVS_SINGLESEL       },
    { "LVS_SHOWSELALWAYS",      LVS_SHOWSELALWAYS,      LVS_SHOWSELALWAYS   },
    { "LVS_SORTASCENDING",      LVS_SORTASCENDING,      LVS_SORTASCENDING   },
    { "LVS_SORTDESCENDING",     LVS_SORTDESCENDING,     LVS_SORTDESCENDING  },
    { "LVS_SHAREIMAGELISTS",    LVS_SHAREIMAGELISTS,    LVS_SHAREIMAGELISTS },
    { "LVS_NOLABELWRAP",        LVS_NOLABELWRAP,        LVS_NOLABELWRAP     },
    { "LVS_AUTOARRANGE",        LVS_AUTOARRANGE,        LVS_AUTOARRANGE     },
    { "LVS_EDITLABELS",         LVS_EDITLABELS,         LVS_EDITLABELS      },
    { "LVS_OWNERDRAWFIXED",     LVS_OWNERDRAWFIXED,     LVS_OWNERDRAWFIXED  },
    { "LVS_ALIGNTOP",           LVS_ALIGNTOP,           LVS_ALIGNMASK       },
    { "LVS_ALIGNLEFT",          LVS_ALIGNLEFT,          LVS_ALIGNMASK       },
    { "LVS_OWNERDATA",          LVS_OWNERDATA,          LVS_OWNERDATA       },
    { "LVS_NOSCROLL",           LVS_NOSCROLL,           LVS_NOSCROLL        },
    { "LVS_NOCOLUMNHEADER",     LVS_NOCOLUMNHEADER,     LVS_NOCOLUMNHEADER  },
    { "LVS_NOSORTHEADER",       LVS_NOSORTHEADER,       LVS_NOSORTHEADER    }
};

static WORD ListViewStyleArraySize = sizeof( ListViewStyleArray ) / sizeof( style_info );

static style_info __near TabControlStyleArray[] = {
    { "TCS_SCROLLOPPOSITE",     TCS_SCROLLOPPOSITE,         TCS_SCROLLOPPOSITE        },
    { "TCS_BOTTOM",             TCS_BOTTOM | TCS_VERTICAL,  TCS_BOTTOM | TCS_VERTICAL },
    { "TCS_RIGHT",              TCS_RIGHT,                  TCS_BOTTOM | TCS_VERTICAL },
    { "TCS_MULTISELECT",        TCS_MULTISELECT,            TCS_MULTISELECT           },
    { "TCS_FLATBUTTONS",        TCS_FLATBUTTONS,            TCS_FLATBUTTONS           },
    { "TCS_FORCEICONLEFT",      TCS_FORCEICONLEFT,          TCS_FORCEICONLEFT         },
    { "TCS_FORCELABELLEFT",     TCS_FORCELABELLEFT,         TCS_FORCELABELLEFT        },
    { "TCS_HOTTRACK",           TCS_HOTTRACK,               TCS_HOTTRACK              },
    { "TCS_VERTICAL",           TCS_VERTICAL,               TCS_VERTICAL              },
    { "TCS_BUTTONS",            TCS_BUTTONS,                TCS_BUTTONS               },
    { "TCS_TABS",               TCS_TABS,                   TCS_BUTTONS               },
    { "TCS_MULTILINE",          TCS_MULTILINE,              TCS_MULTILINE             },
    { "TCS_SINGLELINE",         TCS_SINGLELINE,             TCS_MULTILINE             },
    { "TCS_RIGHTJUSTIFY",       TCS_MULTILINE,
      TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_RAGGEDRIGHT },
    { "TCS_FIXEDWIDTH",         TCS_FIXEDWIDTH,             TCS_FIXEDWIDTH            },
    { "TCS_RAGGEDRIGHT",        TCS_RAGGEDRIGHT,            TCS_RAGGEDRIGHT           },
    { "TCS_FOCUSONBUTTONDOWN",  TCS_FOCUSONBUTTONDOWN,      TCS_FOCUSONBUTTONDOWN     },
    { "TCS_OWNERDRAWFIXED",     TCS_OWNERDRAWFIXED,         TCS_OWNERDRAWFIXED        },
    { "TCS_TOOLTIPS",           TCS_TOOLTIPS,               TCS_TOOLTIPS              },
    { "TCS_FOCUSNEVER",         TCS_FOCUSNEVER,             TCS_FOCUSNEVER            }
};

static WORD TabControlStyleArraySize = sizeof( TabControlStyleArray ) /
                                       sizeof( style_info );

static style_info __near TreeViewStyleArray[] = {
    { "TVS_HASBUTTONS",         TVS_HASBUTTONS,         TVS_HASBUTTONS      },
    { "TVS_HASLINES",           TVS_HASLINES,           TVS_HASLINES        },
    { "TVS_LINESATROOT",        TVS_LINESATROOT,        TVS_LINESATROOT     },
    { "TVS_EDITLABELS",         TVS_EDITLABELS,         TVS_EDITLABELS      },
    { "TVS_DISABLEDRAGDROP",    TVS_DISABLEDRAGDROP,    TVS_DISABLEDRAGDROP },
    { "TVS_SHOWSELALWAYS",      TVS_SHOWSELALWAYS,      TVS_SHOWSELALWAYS   },
    { "TVS_RTLREADING",         TVS_RTLREADING,         TVS_RTLREADING      },
    { "TVS_NOTOOLTIPS",         TVS_NOTOOLTIPS,         TVS_NOTOOLTIPS      },
    { "TVS_CHECKBOXES",         TVS_CHECKBOXES,         TVS_CHECKBOXES      },
    { "TVS_TRACKSELECT",        TVS_TRACKSELECT,        TVS_TRACKSELECT     },
    { "TVS_SINGLEEXPAND",       TVS_SINGLEEXPAND,       TVS_SINGLEEXPAND    },
    { "TVS_INFOTIP",            TVS_INFOTIP,            TVS_INFOTIP         },
    { "TVS_FULLROWSELECT",      TVS_FULLROWSELECT,      TVS_FULLROWSELECT   },
    { "TVS_NOSCROLL",           TVS_NOSCROLL,           TVS_NOSCROLL        },
    { "TVS_NONEVENHEIGHT",      TVS_NONEVENHEIGHT,      TVS_NONEVENHEIGHT   },
    { "TVS_NOHSCROLL",          TVS_NOHSCROLL,          TVS_NOHSCROLL       }
};

static WORD TreeViewStyleArraySize = sizeof( TreeViewStyleArray ) /
                                     sizeof( style_info );
#endif

static class_styles __near ClassStyles[] = {
    { "button",             ButtonStyleArray,       &ButtonStyleArraySize      },
    { "edit",               EditStyleArray,         &EditStyleArraySize        },
    { "static",             StaticStyleArray,       &StaticStyleArraySize      },
    { "listbox",            ListBoxStyleArray,      &ListBoxStyleArraySize     },
    { "combobox",           ComboBoxStyleArray,     &ComboBoxStyleArraySize    },
#ifndef __NT__
    { "#32770",             DialogStyleArray,       &DialogStyleArraySize      }
#else
    { "#32770",             DialogStyleArray,       &DialogStyleArraySize      },
    { ANIMATE_CLASS,        AnimateStyleArray,      &AnimateStyleArraySize     },
    { DATETIMEPICK_CLASS,   DateTimeStyleArray,     &DateTimeStyleArraySize    },
    { MONTHCAL_CLASS,       MonthCalStyleArray,     &MonthCalStyleArraySize    },
    { PROGRESS_CLASS,       ProgressBarStyleArray,  &ProgressBarStyleArraySize },
    { REBARCLASSNAME,       RebarStyleArray,        &RebarStyleArraySize       },
    { STATUSCLASSNAME,      StatusBarStyleArray,    &StatusBarStyleArraySize   },
    { TOOLBARCLASSNAME,     ToolbarStyleArray,      &ToolbarStyleArraySize     },
    { TOOLTIPS_CLASS,       ToolTipsStyleArray,     &ToolTipsStyleArraySize    },
    { TRACKBAR_CLASS,       TrackBarStyleArray,     &TrackBarStyleArraySize    },
    { UPDOWN_CLASS,         UpDownStyleArray,       &UpDownStyleArraySize      },
    { WC_HEADER,            HeaderStyleArray,       &HeaderStyleArraySize      },
    { WC_LISTVIEW,          ListViewStyleArray,     &ListViewStyleArraySize    },
    { WC_TABCONTROL,        TabControlStyleArray,   &TabControlStyleArraySize  },
    { WC_TREEVIEW,          TreeViewStyleArray,     &TreeViewStyleArraySize    }
#endif
};

static WORD ClassStylesSize = sizeof( ClassStyles ) / sizeof( class_styles );

static style_info __near ExStyleArray[] = {
    { "WS_EX_DLGMODALFRAME",    WS_EX_DLGMODALFRAME,    WS_EX_DLGMODALFRAME   },
    { "WS_EX_NOPARENTNOTIFY",   WS_EX_NOPARENTNOTIFY,   WS_EX_NOPARENTNOTIFY  },
    { "WS_EX_TOPMOST",          WS_EX_TOPMOST,          WS_EX_TOPMOST         },
    { "WS_EX_ACCEPTFILES",      WS_EX_ACCEPTFILES,      WS_EX_ACCEPTFILES     },
#ifndef __NT__
    { "WS_EX_TRANSPARENT",      WS_EX_TRANSPARENT,      WS_EX_TRANSPARENT     }
#else
    { "WS_EX_TRANSPARENT",      WS_EX_TRANSPARENT,      WS_EX_TRANSPARENT     },
    { "WS_EX_MDICHILD",         WS_EX_MDICHILD,         WS_EX_MDICHILD        },
    { "WS_EX_TOOLWINDOW",       WS_EX_TOOLWINDOW,       WS_EX_TOOLWINDOW      },
    { "WS_EX_WINDOWEDGE",       WS_EX_WINDOWEDGE,       WS_EX_WINDOWEDGE      },
    { "WS_EX_CLIENTEDGE",       WS_EX_CLIENTEDGE,       WS_EX_CLIENTEDGE      },
    { "WS_EX_CONTEXTHELP",      WS_EX_CONTEXTHELP,      WS_EX_CONTEXTHELP     },
    { "WS_EX_RIGHT",            WS_EX_RIGHT,            WS_EX_RIGHT           },
    { "WS_EX_RTLREADING",       WS_EX_RTLREADING,       WS_EX_RTLREADING      },
    { "WS_EX_LEFTSCROLLBAR",    WS_EX_LEFTSCROLLBAR,    WS_EX_LEFTSCROLLBAR   },
    { "WS_EX_CONTROLPARENT",    WS_EX_CONTROLPARENT,    WS_EX_CONTROLPARENT   },
    { "WS_EX_STATICEDGE",       WS_EX_STATICEDGE,       WS_EX_STATICEDGE      },
    { "WS_EX_APPWINDOW",        WS_EX_APPWINDOW,        WS_EX_APPWINDOW       },
    { "WS_EX_LAYERED",          WS_EX_LAYERED,          WS_EX_LAYERED         },
    { "WS_EX_NOINHERITLAYOUT",  WS_EX_NOINHERITLAYOUT,  WS_EX_NOINHERITLAYOUT },
    { "WS_EX_LAYOUTRTL",        WS_EX_LAYOUTRTL,        WS_EX_LAYOUTRTL       },
    { "WS_EX_COMPOSITED",       WS_EX_COMPOSITED,       WS_EX_COMPOSITED      },
    { "WS_EX_NOACTIVATE",       WS_EX_NOACTIVATE,       WS_EX_NOACTIVATE      }
#endif
};

static WORD ExStyleArraySize = sizeof( ExStyleArray ) / sizeof( style_info );

static style_info __near ClassStyleArray[] = {
    { "CS_VREDRAW",         CS_VREDRAW,         CS_VREDRAW         },
    { "CS_HREDRAW",         CS_HREDRAW,         CS_HREDRAW         },
    { "CS_KEYCVTWINDOW",    CS_KEYCVTWINDOW,    CS_KEYCVTWINDOW    },
    { "CS_DBLCLKS",         CS_DBLCLKS,         CS_DBLCLKS         },
    { "CS_OWNDC",           CS_OWNDC,           CS_OWNDC           },
    { "CS_CLASSDC",         CS_CLASSDC,         CS_CLASSDC         },
    { "CS_PARENTDC",        CS_PARENTDC,        CS_PARENTDC        },
    { "CS_NOKEYCVT",        CS_NOKEYCVT,        CS_NOKEYCVT        },
    { "CS_NOCLOSE",         CS_NOCLOSE,         CS_NOCLOSE         },
    { "CS_SAVEBITS",        CS_SAVEBITS,        CS_SAVEBITS        },
    { "CS_BYTEALIGNCLIENT", CS_BYTEALIGNCLIENT, CS_BYTEALIGNCLIENT },
    { "CS_BYTEALIGNWINDOW", CS_BYTEALIGNWINDOW, CS_BYTEALIGNWINDOW },
    { "CS_GLOBALCLASS",     CS_GLOBALCLASS,     CS_GLOBALCLASS     }
};

static WORD ClassStyleArraySize = sizeof( ClassStyleArray ) / sizeof( style_info );

/*
 * GetHexStr - convert a number to a hex string, padded out with 0's
 */
void GetHexStr( LPSTR res, DWORD num, int padlen )
{
    char        tmp[10];
    int         i;
    int         j,k;

    ultoa( num, tmp, 16 );
    i = strlen( tmp );
    k = 0;
    for( j = i; j < padlen; j++ ) {
        res[k++] = '0';
    }
    for( j = 0; j < i; j++ ) {
        res[k++] = tmp[j];
    }

} /* GetHexStr */

/*
 * IsMyWindow - check if a specific window belongs to our task
 */
BOOL IsMyWindow( HWND hwnd )
{

    if( hwnd == NULL ) {
        return( FALSE );
    }
    if( GetWindowTask( hwnd ) == MyTask ) {
        if( hwnd == GetDesktopWindow() ) {
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* IsMyWindow */

/*
 * GetWindowName - set up a window name string
 */
void GetWindowName( HWND hwnd, char *str )
{
    int         len;
    char        name[64];

    if( hwnd == NULL ) {
        strcpy( str, GetRCString( STR_NONE ) );
        return;
    }
    len = GetWindowText( hwnd, name, sizeof( name ) );
    name[len] = 0;
    if( len == 0 ) {
        GetHexStr( str, (UINT)hwnd, 4 );
        str[4] = 0;
    } else {
        sprintf( str, "%0*x: %s", UINT_STR_LEN, (UINT)hwnd, name );
    }

} /* GetWindowName */

/*
 * GetWindowStyleString - get string corrosponding to window style bits
 */
void GetWindowStyleString( HWND hwnd, char *str, char *sstr )
{
    UINT        id;
    DWORD       style;
    DWORD       exstyle;
    char        tmp[40];
    int         len;
    char        *rcstr;
    int         i;
    int         j;

    style = GetWindowLong( hwnd, GWL_STYLE );
    exstyle = GetWindowLong( hwnd, GWL_EXSTYLE );

    GetHexStr( str, style, 8 );
    str[8] = 0;
    sstr[0] = 0;

    if( style & WS_CHILD ) {
        id = GET_ID( hwnd );
        rcstr = GetRCString( STR_CHILD_ID );
        sprintf( tmp, rcstr, id, UINT_STR_LEN, id );
        strcat( str, tmp );
    }
    for( i = 0; i < StyleArraySize; i++ ) {
        if( (style & StyleArray[i].mask) == StyleArray[i].flags ) {
            strcat( sstr, StyleArray[i].name );
            strcat( sstr, " " );
        }
    }

    len = GetClassName( hwnd, tmp, sizeof( tmp ) );
    tmp[len] = 0;
    for( i = 0; i < ClassStylesSize; i++ ) {
        if( !stricmp( tmp, ClassStyles[i].class_name ) ) {
            for( j = 0; j < *ClassStyles[i].style_array_size; j++ ) {
                if( (style & ClassStyles[i].style_array[j].mask) ==
                    ClassStyles[i].style_array[j].flags ) {
                    strcat( sstr, ClassStyles[i].style_array[j].name );
                    strcat( sstr, " " );
                }
            }
        }
    }

    for( i = 0; i < ExStyleArraySize; i++ ) {
        if( (exstyle & ExStyleArray[i].mask) == ExStyleArray[i].flags ) {
            strcat( sstr, ExStyleArray[i].name );
            strcat( sstr, " " );
        }
    }

} /* GetWindowStyleString */

#ifndef __NT__
    #define STYLE_TYPE          WORD
    #define STYLE_HEX_LEN       4
#else
    #define STYLE_TYPE          DWORD
    #define STYLE_HEX_LEN       8
#endif

/*
 * GetClassStyleString - get a string corrosponding to class style bits
 */
void GetClassStyleString( HWND hwnd, char *str, char *sstr )
{
    STYLE_TYPE          style;
    int                 i;

    style = GET_CLASS_STYLE( hwnd );

    GetHexStr( str, style, STYLE_HEX_LEN  );
    str[STYLE_HEX_LEN] = 0;
    sstr[0] = 0;

    for( i = 0; i < ClassStyleArraySize; i++ ) {
        if( (style & ClassStyleArray[i].mask) == ClassStyleArray[i].flags ) {
            strcat( sstr, ClassStyleArray[i].name );
            strcat( sstr, " " );
        }
    }

} /* GetClassStyleString */

/*
 * DumpToComboBox - dump a string of space separated items to a combo box
 */
void DumpToComboBox( char *str, HWND cb )
{
    char        tmp[128];
    int         i;

    SendMessage( cb, CB_RESETCONTENT, 0, 0L );
    while( *str != 0 ) {
        i = 0;
        while( *str != ' ' ) {
            tmp[i++] = *str;
            str++;
        }
        tmp[i] = 0;
        if( i != 0 ) {
            str++;
            SendMessage( cb, CB_ADDSTRING, 0, (LONG) (LPSTR) tmp );
        }
    }
    SendMessage( cb, CB_SETCURSEL, 0, 0L );

} /* DumpToComboBox */

/*
 * FormatSpyMessage - make a spy message string
 */
void FormatSpyMessage( char *msg, LPMSG pmsg, char *res )
{
    memset( res,' ', SPYOUT_LENGTH );
    strcpy( res, msg );
    res[strlen( msg )] = ' ';
    GetHexStr( &res[SPYOUT_HWND], (DWORD) pmsg->hwnd, SPYOUT_HWND_LEN );
    GetHexStr( &res[SPYOUT_MSG], pmsg->message, SPYOUT_MSG_LEN );
    GetHexStr( &res[SPYOUT_WPARAM], pmsg->wParam, SPYOUT_WPARAM_LEN );
    GetHexStr( &res[SPYOUT_LPARAM], pmsg->lParam, SPYOUT_LPARAM_LEN );
    res[SPYOUT_LENGTH] = 0;

} /* FormatSpyMessage */

/*
 * SetSpyState - set current spy state
 */
void SetSpyState( spystate ss )
{
    char        str[130];
    char        *rcstr;
    int         len;

    SpyState = ss;
    len = GetWindowText( SpyMainWindow, &str[1], sizeof( str ) - 2 );
    switch( ss ) {
    case OFF:
        if( str[ 1 ] != '<' ) {
            str[ 0 ] = '<';
            str[ len + 1 ] = '>';
            str[ len + 2 ] = 0;
            SetWindowText( SpyMainWindow, str );
        }
        rcstr = GetRCString( STR_SPY_ON );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING,
                    SPY_OFFON, rcstr );
        break;
    case ON:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING,
                    SPY_OFFON, rcstr );
        if( str[ 1 ] == '<' ) {
            str[ len ] = 0;
            SetWindowText( SpyMainWindow, &str[ 2 ] );
        }
        break;
    case NEITHER:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_GRAYED | MF_STRING,
                    SPY_OFFON, rcstr );
        if( str[ 1 ] == '<' ) {
            str[ len ] = 0;
            SetWindowText( SpyMainWindow, &str[ 2 ] );
        }
        break;
    }
    SetOnOffTool( ss );

} /* SetSpyState */

static char filterList[] = "File (*.*)" \
                           "\0" \
                           "*.*" \
                           "\0\0";

/*
 * GetFileName - get a file name using common dialog stuff
 */
BOOL GetFileName( char *ext, int type, char *fname )
{
    OPENFILENAME        of;
    BOOL                rc;

    fname[0] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = SpyMainWindow;
    of.lpstrFilter = (LPSTR)filterList;
    of.lpstrDefExt = ext;
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.Flags = OFN_HIDEREADONLY;
    if( type == FILE_OPEN ) {
        rc = GetOpenFileName( &of );
    } else {
        rc = GetSaveFileName( &of );
    }
    return( rc );

} /* GetFileName */

BOOL InitGblStrings( void ) {

    DWORD       heading;
    DWORD       heading_uline;

    SpyName = AllocRCString( STR_APP_NAME );
    if( SpyName == NULL ) {
        return( FALSE );
    }

#ifdef __NT__
    heading = STR_HEADINGS_NT;
    heading_uline = STR_HEADING_UNDERLINE_NT;
#else
    heading = STR_HEADINGS_WIN;
    heading_uline = STR_HEADING_UNDERLINE_WIN;
#endif
    TitleBar = AllocRCString( heading );
    TitleBarLen = strlen( TitleBar + 1 );
    if( TitleBar == NULL ) {
        return( FALSE );
    }

    TitleBarULine = AllocRCString( heading_uline );
    if( TitleBarULine == NULL ) {
        return( FALSE );
    }
    return( TRUE );

} /* InitGblStrings */

