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


#include <windows.h>

#include "wdeglbl.h"
#include "wde_rc.h"
#include "wdetmenu.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *bmp;
    HBITMAP  hbmp;
    WORD     id;
} WdeToolBitMapType;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeToolBitMapType WdeMenuBitmaps[] =
{
    { "SelMode"       , NULL, IDM_SELECT_MODE   }
,   { "StikMode"      , NULL, IDM_STICKY_TOOLS  }
,   { "DiagMode"      , NULL, IDM_DIALOG_TOOL   }
,   { "PushMode"      , NULL, IDM_PBUTTON_TOOL  }
,   { "RadMode"       , NULL, IDM_RBUTTON_TOOL  }
,   { "ChekMode"      , NULL, IDM_CBUTTON_TOOL  }
,   { "GrpMode"       , NULL, IDM_GBUTTON_TOOL  }
,   { "TextMode"      , NULL, IDM_TEXT_TOOL     }
,   { "FramMode"      , NULL, IDM_FRAME_TOOL    }
,   { "IconMode"      , NULL, IDM_ICON_TOOL     }
,   { "EditMode"      , NULL, IDM_EDIT_TOOL     }
,   { "ListMode"      , NULL, IDM_LISTBOX_TOOL  }
,   { "CombMode"      , NULL, IDM_COMBOBOX_TOOL }
,   { "HScrMode"      , NULL, IDM_HSCROLL_TOOL  }
,   { "VScrMode"      , NULL, IDM_VSCROLL_TOOL  }
,   { "SBoxMode"      , NULL, IDM_SIZEBOX_TOOL  }
#ifndef __NT__
,   { "Custom1"       , NULL, IDM_CUSTOM1_TOOL  }
,   { "Custom2"       , NULL, IDM_CUSTOM2_TOOL  }
#endif
,   { "AlignLeft"     , NULL, IDM_FMLEFT        }
,   { "AlignRight"    , NULL, IDM_FMRIGHT       }
,   { "AlignTop"      , NULL, IDM_FMTOP         }
,   { "AlignBottom"   , NULL, IDM_FMBOTTOM      }
,   { "AlignHCentre"  , NULL, IDM_FMHCENTRE     }
,   { "AlignVCentre"  , NULL, IDM_FMVCENTRE     }
,   { "SameWidth"     , NULL, IDM_SAME_WIDTH    }
,   { "SameHeight"    , NULL, IDM_SAME_HEIGHT   }
,   { "SameSize"      , NULL, IDM_SAME_SIZE     }
,   { "SelMode"       , NULL, IDM_SELECT_MODE   }
,   { "DiagMode"      , NULL, IDM_DIALOG_TOOL   }
,   { "PushMode"      , NULL, IDM_PBUTTON_TOOL  }
,   { "RadMode"       , NULL, IDM_RBUTTON_TOOL  }
,   { "ChekMode"      , NULL, IDM_CBUTTON_TOOL  }
,   { "GrpMode"       , NULL, IDM_GBUTTON_TOOL  }
,   { "TextMode"      , NULL, IDM_TEXT_TOOL     }
,   { "FramMode"      , NULL, IDM_FRAME_TOOL    }
,   { "IconMode"      , NULL, IDM_ICON_TOOL     }
,   { "EditMode"      , NULL, IDM_EDIT_TOOL     }
,   { "ListMode"      , NULL, IDM_LISTBOX_TOOL  }
,   { "CombMode"      , NULL, IDM_COMBOBOX_TOOL }
,   { "HScrMode"      , NULL, IDM_HSCROLL_TOOL  }
,   { "VScrMode"      , NULL, IDM_VSCROLL_TOOL  }
,   { "SBoxMode"      , NULL, IDM_SIZEBOX_TOOL  }
,   { "Custom1"       , NULL, IDM_CUSTOM1_TOOL  }
,   { "Custom2"       , NULL, IDM_CUSTOM2_TOOL  }
,   { NULL            , NULL, -1                }
};

void WdeInitToolMenu ( HINSTANCE inst, HMENU menu )
{
    int i;

    for ( i=0; WdeMenuBitmaps[i].bmp; i++ ) {
        WdeMenuBitmaps[i].hbmp = LoadBitmap ( inst, WdeMenuBitmaps[i].bmp );
        if ( WdeMenuBitmaps[i].hbmp != NULL ) {
            ModifyMenu ( menu, WdeMenuBitmaps[i].id,
                         MF_BYCOMMAND | MF_BITMAP,
                         WdeMenuBitmaps[i].id,
                         (LPCSTR) WdeMenuBitmaps[i].hbmp );
        }
    }
}

void WdeFiniToolMenu ( void )
{
    int i;

    for ( i=0; WdeMenuBitmaps[i].bmp; i++ ) {
        if ( WdeMenuBitmaps[i].hbmp != NULL ) {
            DeleteObject ( WdeMenuBitmaps[i].hbmp );
        }
    }
}

