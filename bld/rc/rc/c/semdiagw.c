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
* Description:  Windows dialog related semantic actions.
*
****************************************************************************/


/*
    The data structure for DIALOGEX's is as follows:

struct DialogSettings {
    WORD       wVersion;      // Always 00 01
    WORD       wAlwaysFF;     // Always FF FF
    DWORD      HelpId;
    DWORD      ExStyle;
    DWORD      Style;
    WORD       NumberOfItems;
    WORD       x;
    WORD       y;
    WORD       cx;
    WORD       cy;
    [Name/Ord] MenuName;
    [Name/Ord] ClassName;
    WCHAR      szCaption[]
    WORD       fontHeight;
    WORD       fontWeight;
    WORD       italic;
    WCHAR      szFontName[];
}

struct ControlData {
    DWORD      HelpId;
    DWORD      ExStyle;
    DWORD      Style;
    WORD       x;
    WORD       y;
    WORD       cx;
    WORD       cy;
    DWORD      id;
    [Name/Ord] classId;
    [Name/Ord] text;
    WORD       numBytes;  // Number of bytes of data elements that follow.
}



*/


#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"


static FullDialogBoxHeader *NewDialogBoxHeader( void )
/****************************************************/
{
    FullDialogBoxHeader     *newheader;

    newheader = RCALLOC( sizeof( FullDialogBoxHeader ) );

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
        newheader->Win32 = false;
        newheader->u.Head.Style = 0L;
        newheader->u.Head.NumOfItems = 0;
        newheader->u.Head.Size.x = 0;
        newheader->u.Head.Size.y = 0;
        newheader->u.Head.Size.width = 0;
        newheader->u.Head.Size.height = 0;
        newheader->u.Head.MenuName = NULL;
        newheader->u.Head.ClassName = NULL;
        newheader->u.Head.Caption = NULL;
        newheader->u.Head.PointSize = 0;
        newheader->u.Head.FontName = NULL;
    } else {
        newheader->Win32 = true;
        newheader->u.Head32.Head.Style = 0L;
        newheader->u.Head32.Head.ExtendedStyle = 0L;
        newheader->u.Head32.Head.NumOfItems = 0;
        newheader->u.Head32.Head.Size.x = 0;
        newheader->u.Head32.Head.Size.y = 0;
        newheader->u.Head32.Head.Size.width = 0;
        newheader->u.Head32.Head.Size.height = 0;
        newheader->u.Head32.Head.MenuName = NULL;
        newheader->u.Head32.Head.ClassName = NULL;
        newheader->u.Head32.Head.Caption = NULL;
        newheader->u.Head32.Head.PointSize = 0;
        newheader->u.Head32.Head.FontName = NULL;

        newheader->u.Head32.ExHead.FontWeight = 0;
        newheader->u.Head32.ExHead.FontItalic = 0;
        newheader->u.Head32.ExHead.FontExtra = 1;
        newheader->u.Head32.ExHead.HelpId = 0L;
        newheader->u.Head32.ExHead.FontWeightDefined = false;
        newheader->u.Head32.ExHead.FontItalicDefined = false;
    }

    newheader->StyleGiven = false;

    return( newheader );
} /* NewDialogBoxHeader */

FullDialogBoxHeader *SemWINNewDiagOptions( FullDialogOptions *opt )
/*****************************************************************/
{
    FullDialogBoxHeader *newheader;

    newheader = NewDialogBoxHeader();

    return( SemWINDiagOptions( newheader, opt ) );
} /* SemWINNewDiagOptions */

static void AddDiagOption( DialogBoxHeader *head, FullDialogOptions *opt )
/************************************************************************/
{
    switch( opt->token ) {
    case Y_STYLE:
        head->Style |= opt->Opt.Style;
        break;
    case Y_MENU:
        if( head->MenuName != NULL ) {
            RCFREE( head->MenuName );
        }
        head->MenuName = opt->Opt.Name;
        break;
    case Y_CLASS:
        if( head->ClassName != NULL ) {
            RCFREE( head->ClassName );
        }
        head->ClassName = opt->Opt.Name;
        break;
    case Y_FONT:
        head->Style |= DS_SETFONT;
        head->PointSize = opt->Opt.Font.PointSize;
        if( head->FontName != NULL ) {
            RCFREE( head->FontName );
        }
        head->FontName = opt->Opt.Font.FontName;
        break;
    case Y_CAPTION:
        head->Style |= WS_CAPTION;
        if( head->Caption != NULL ) {
            RCFREE( head->Caption );
        }
        head->Caption = opt->Opt.Str;
        break;
    case Y_EXSTYLE:
        RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( opt->token ) );
        break;
    }
} /* AddDiagOptions */

static void AddDiagOption32( DlgHeader32 *head,
                             FullDialogOptions *opt )
/***************************************************/
{
    switch( opt->token ) {
    case Y_STYLE:
        head->Head.Style |= opt->Opt.Style;
        break;
    case Y_EXSTYLE:
        head->Head.ExtendedStyle |= opt->Opt.Exstyle;
        break;
    case Y_MENU:
        if( head->Head.MenuName != NULL ) {
            RCFREE( head->Head.MenuName );
        }
        head->Head.MenuName = opt->Opt.Name;
        break;
    case Y_CLASS:
        if( head->Head.ClassName != NULL ) {
            RCFREE( head->Head.ClassName );
        }
        head->Head.ClassName = opt->Opt.Name;
        break;
    case Y_FONT:
        head->Head.Style |= DS_SETFONT;
        head->Head.PointSize = opt->Opt.Font.PointSize;
        if( head->Head.FontName != NULL ) {
            RCFREE( head->Head.FontName );
        }
        head->Head.FontName = opt->Opt.Font.FontName;
        head->ExHead.FontItalic = opt->Opt.Font.FontItalic;
        head->ExHead.FontWeight = opt->Opt.Font.FontWeight;
        head->ExHead.FontExtra = opt->Opt.Font.FontExtra;
        head->ExHead.FontItalicDefined = opt->Opt.Font.FontItalicDefined;
        head->ExHead.FontWeightDefined = opt->Opt.Font.FontWeightDefined;
        break;
    case Y_CAPTION:
        head->Head.Style |= WS_CAPTION;
        if( head->Head.Caption != NULL ) {
            RCFREE( head->Head.Caption );
        }
        head->Head.Caption = opt->Opt.Str;
        break;
    case Y_LANGUAGE:
        SemWINSetResourceLanguage( &opt->Opt.lang, true );
        break;
    }
} /* AddDiagOptions32 */

FullDialogBoxHeader *SemWINDiagOptions( FullDialogBoxHeader *head,
                                             FullDialogOptions *opt )
/********************************************************************/
{
    if( head->Win32 ) {
        AddDiagOption32( &head->u.Head32, opt );
    } else {
        AddDiagOption( &head->u.Head, opt );
    }
    if( opt->token == Y_STYLE ) {
        head->StyleGiven = true;
    }

    return( head );
}

FullDiagCtrlList *SemWINEmptyDiagCtrlList( void )
/***********************************************/
{
    FullDiagCtrlList    *newlist;

    newlist = RCALLOC( sizeof( FullDiagCtrlList ) );
    newlist->head = NULL;
    newlist->tail = NULL;
    newlist->numctrls = 0;
    return( newlist );
}

FullDiagCtrlList *SemWINNewDiagCtrlList( FullDialogBoxControl *ctrl,
                                             DataElemList *list )
/******************************************************************/
{
    FullDiagCtrlList    *newlist;

    newlist = SemWINEmptyDiagCtrlList();
    return( SemWINAddDiagCtrlList( newlist, ctrl, list ) );
} /* SemWINNewDiagCtrlList */

FullDiagCtrlList *SemWINAddDiagCtrlList( FullDiagCtrlList *list,
                    FullDialogBoxControl *ctrl, DataElemList *dataList )
/**********************************************************************/
{
    if( ctrl != NULL ) {
        ctrl->dataListHead = dataList;
        ResAddLLItemAtEnd( (void **) &(list->head), (void **) &(list->tail), ctrl );
        list->numctrls++;
    }
    return( list );
} /* SemWINAddDiagCtrlList */

static FullDialogBoxControl *semInitDiagCtrl( void )
/*****************************************************/
{
    FullDialogBoxControl        *newctrl;

    newctrl = RCALLOC( sizeof( FullDialogBoxControl ) );
    newctrl->next = NULL;
    newctrl->prev = NULL;
    newctrl->Win32 = (CmdLineParms.TargetOS == RC_TARGET_OS_WIN32);

    return( newctrl );
} /* semInitDiagCtrl */

/* These are the default styles used for all dialog box control statmens */
/* except the control statement (see rc.y for it).  The HI style contains */
/* all the WS_ styles that are applicable (the high word) and the LO style */
/* contains the styles that are particular to the class of the control */

#define DEF_LTEXT_HI            (WS_CHILD|WS_VISIBLE|WS_GROUP)
#define DEF_LTEXT_LO            (SS_LEFT)
#define DEF_RTEXT_HI            (WS_CHILD|WS_VISIBLE|WS_GROUP)
#define DEF_RTEXT_LO            (SS_RIGHT)
#define DEF_CTEXT_HI            (WS_CHILD|WS_VISIBLE|WS_GROUP)
#define DEF_CTEXT_LO            (SS_CENTER)
#define DEF_AUTO3STATE_HI       (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_AUTO3STATE_LO       (BS_AUTO3STATE)
#define DEF_AUTOCHECKBOX_HI     (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_AUTOCHECKBOX_LO     (BS_AUTOCHECKBOX)
#define DEF_AUTORADIOBUTTON_HI  (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_AUTORADIOBUTTON_LO  (BS_AUTORADIOBUTTON)
#define DEF_CHECKBOX_HI         (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_CHECKBOX_LO         (BS_CHECKBOX)
#define DEF_PUSHBUTTON_HI       (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_PUSHBUTTON_LO       (BS_PUSHBUTTON)
//#define       DEF_PUSHBOX_HI          (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
//#define       DEF_PUSHBOX_LO          (BS_PUSHBOX)
#define DEF_LISTBOX_HI          (WS_CHILD|WS_VISIBLE|WS_BORDER)
#define DEF_LISTBOX_LO          (LBS_NOTIFY)
#define DEF_GROUPBOX_HI         (WS_CHILD|WS_VISIBLE)
#define DEF_GROUPBOX_LO         (BS_GROUPBOX)
#define DEF_DEFPUSHBUTTON_HI    (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_DEFPUSHBUTTON_LO    (BS_DEFPUSHBUTTON)
#define DEF_RADIOBUTTON_HI      (WS_CHILD|WS_VISIBLE)
#define DEF_RADIOBUTTON_LO      (BS_RADIOBUTTON)
#define DEF_EDITTEXT_HI         (WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER)
#define DEF_EDITTEXT_LO         (ES_LEFT)
#define DEF_COMBOBOX_HI         (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_COMBOBOX_LO         (CBS_SIMPLE)
#define DEF_ICON_HI             (WS_CHILD|WS_VISIBLE)
#define DEF_ICON_LO             (SS_ICON)
#define DEF_SCROLLBAR_HI        (WS_CHILD|WS_VISIBLE)
#define DEF_SCROLLBAR_LO        (SBS_HORZ)
#define DEF_STATE3_HI           (WS_CHILD|WS_VISIBLE|WS_TABSTOP)
#define DEF_STATE3_LO           (BS_3STATE)

#define LO_WORD     0x0000ffff
#define HI_WORD     0xffff0000

FullDialogBoxControl *SemWINNewDiagCtrl( YYTOKENTYPE token,
                                FullDiagCtrlOptions opts )
/********************************************************/
{
    FullDialogBoxControl    *newctrl;
    uint_32                 style_mask; /* for the style of the control */
    uint_32                 style_value;
    uint_32                 defstyle_hi = 0;
    uint_32                 defstyle_lo = 0;
    uint_32                 style_hi;
    uint_32                 style_lo;
    uint_16                 class = 0;
    uint_16                 tmp_mask;
    ControlClass            *cont_class;

    switch( token ) {
    case Y_LTEXT:
        class = CLASS_STATIC;
        defstyle_hi = DEF_LTEXT_HI;
        defstyle_lo = DEF_LTEXT_LO;
        break;
    case Y_RTEXT:
        class = CLASS_STATIC;
        defstyle_hi = DEF_RTEXT_HI;
        defstyle_lo = DEF_RTEXT_LO;
        break;
    case Y_CTEXT:
        class = CLASS_STATIC;
        defstyle_hi = DEF_CTEXT_HI;
        defstyle_lo = DEF_CTEXT_LO;
        break;
    case Y_CHECKBOX:
        class = CLASS_BUTTON;
        defstyle_hi = DEF_CHECKBOX_HI;
        defstyle_lo = DEF_CHECKBOX_LO;
        break;
    case Y_PUSHBUTTON:
        class = CLASS_BUTTON;
        defstyle_hi = DEF_PUSHBUTTON_HI;
        defstyle_lo = DEF_PUSHBUTTON_LO;
        break;
    case Y_LISTBOX:
        class = CLASS_LISTBOX;
        defstyle_hi = DEF_LISTBOX_HI;
        defstyle_lo = DEF_LISTBOX_LO;
        break;
    case Y_GROUPBOX:
        class = CLASS_BUTTON;
        defstyle_hi = DEF_GROUPBOX_HI;
        defstyle_lo = DEF_GROUPBOX_LO;
        break;
    case Y_DEFPUSHBUTTON:
        class = CLASS_BUTTON;
        defstyle_hi = DEF_DEFPUSHBUTTON_HI;
        defstyle_lo = DEF_DEFPUSHBUTTON_LO;
        break;
    case Y_RADIOBUTTON:
        class = CLASS_BUTTON;
        defstyle_hi = DEF_RADIOBUTTON_HI;
        defstyle_lo = DEF_RADIOBUTTON_LO;
        break;
    case Y_EDITTEXT:
        class = CLASS_EDIT;
        defstyle_hi = DEF_EDITTEXT_HI;
        defstyle_lo = DEF_EDITTEXT_LO;
        break;
    case Y_COMBOBOX:
        class = CLASS_COMBOBOX;
        defstyle_hi = DEF_COMBOBOX_HI;
        defstyle_lo = DEF_COMBOBOX_LO;
        break;
    case Y_ICON:
        class = CLASS_STATIC;
        defstyle_hi = DEF_ICON_HI;
        defstyle_lo = DEF_ICON_LO;
        break;
    case Y_SCROLLBAR:
        class = CLASS_SCROLLBAR;
        defstyle_hi = DEF_SCROLLBAR_HI;
        defstyle_lo = DEF_SCROLLBAR_LO;
        break;
    case Y_AUTO3STATE:
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTO3STATE_HI;
            defstyle_lo = DEF_AUTO3STATE_LO;
        } else {
            RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    case Y_AUTOCHECKBOX:
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTOCHECKBOX_HI;
            defstyle_lo = DEF_AUTOCHECKBOX_LO;
        } else {
            RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    case Y_AUTORADIOBUTTON:
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTORADIOBUTTON_HI;
            defstyle_lo = DEF_AUTORADIOBUTTON_LO;
        } else {
            RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
//    case Y_PUSHBOX:
//      class = CLASS_BUTTON;
//      defstyle_hi = DEF_PUSHBOX_HI;
//      defstyle_lo = DEF_PUSHBOX_LO;
//      break;
    case Y_STATE3:
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_STATE3_HI;
            defstyle_lo = DEF_STATE3_LO;
        } else {
            RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    }

    newctrl = semInitDiagCtrl();
    cont_class = ResNumToControlClass( class );

    style_mask = opts.Style.Mask;
    style_value = opts.Style.Value;

    switch( token ) {
    case Y_COMBOBOX:
        style_lo = defstyle_lo;
        if( style_mask & LO_WORD ) {
            style_lo = style_value;
        }
        break;
    default:
        tmp_mask = style_mask & LO_WORD;
        style_lo = (tmp_mask & style_value) | (~tmp_mask & defstyle_lo);
        break;
    }

    /* for the high word use the bits that were mentioned from style_value */
    /* and all the other bits from defstyle_hi */
    style_hi = (style_mask & style_value) | (~style_mask & defstyle_hi);

    if( newctrl->Win32 ) {
        newctrl->u.ctrl32.ID = opts.ID;
        newctrl->u.ctrl32.Size = opts.Size;
        newctrl->u.ctrl32.Text = opts.Text;
        newctrl->u.ctrl32.ClassID = cont_class;
        newctrl->u.ctrl32.Style = (style_lo & LO_WORD) | (style_hi & HI_WORD);
        /* ExtraBytes and ExtendStyle are 0 for all controls */
        /* that RC understands */
        newctrl->u.ctrl32.ExtraBytes = 0;

        newctrl->u.ctrl32.ExtendedStyle = opts.ExtendedStyle;
        newctrl->u.ctrl32.HelpId = opts.HelpId;
        newctrl->u.ctrl32.HelpIdDefined = opts.HelpIdDefined;
    } else {
        newctrl->u.ctrl.ID = opts.ID;
        newctrl->u.ctrl.Size = opts.Size;
        newctrl->u.ctrl.Text = opts.Text;
        newctrl->u.ctrl.ClassID = cont_class;
        newctrl->u.ctrl.Style = (style_lo & LO_WORD) | (style_hi & HI_WORD);
        /* ExtraBytes is 0 for all controls that RC understands */
        newctrl->u.ctrl.ExtraBytes = 0;
    }

    return( newctrl );
} /* SemWINNewDiagCtrl */


static void SemFreeDiagCtrlList( FullDiagCtrlList *list )
/*******************************************************/
{
    FullDialogBoxControl        *ctrl;
    FullDialogBoxControl        *oldctrl;

    ctrl = list->head;
    while( ctrl != NULL ) {
        /* free the contents of pointers within the structure */
        if( ctrl->Win32 ) {
            if( ctrl->u.ctrl32.ClassID != NULL ) {
                RCFREE( ctrl->u.ctrl32.ClassID );
            }
            if( ctrl->u.ctrl32.Text != NULL ) {
                RCFREE( ctrl->u.ctrl32.Text );
            }
        } else {
            if( ctrl->u.ctrl.ClassID != NULL ) {
                RCFREE( ctrl->u.ctrl.ClassID );
            }
            if( ctrl->u.ctrl.Text != NULL ) {
                RCFREE( ctrl->u.ctrl.Text );
            }
        }

        oldctrl = ctrl;
        ctrl = ctrl->next;

        RCFREE( oldctrl );
    }

    RCFREE( list );
} /* SemFreeDiagCtrlList */

static void SemFreeDialogHeader( FullDialogBoxHeader *head )
/**********************************************************/
{
    if( head->Win32 ) {
        if( head->u.Head32.Head.MenuName != NULL ) {
            RCFREE( head->u.Head32.Head.MenuName );
        }
        if( head->u.Head32.Head.ClassName != NULL ) {
            RCFREE( head->u.Head32.Head.ClassName );
        }
        if( head->u.Head32.Head.Caption != NULL ) {
            RCFREE( head->u.Head32.Head.Caption );
        }
        if( head->u.Head32.Head.FontName != NULL ) {
            RCFREE( head->u.Head32.Head.FontName );
        }
    } else {
        if( head->u.Head.MenuName != NULL ) {
            RCFREE( head->u.Head.MenuName );
        }
        if( head->u.Head.ClassName != NULL ) {
            RCFREE( head->u.Head.ClassName );
        }
        if( head->u.Head.Caption != NULL ) {
            RCFREE( head->u.Head.Caption );
        }
        if( head->u.Head.FontName != NULL ) {
            RCFREE( head->u.Head.FontName );
        }
    }

    RCFREE( head );
} /* SemFreeDialogHeader */

static bool SemWriteDiagCtrlList( FullDiagCtrlList *list, int *err_code,
                                 YYTOKENTYPE tokentype )
/*********************************************************************/
{
    bool                        error;
    FullDialogBoxControl        *ctrl;
    DialogBoxExControl32        controlex;
    DialogBoxControl32          control;

    error = false;
    for( ctrl = list->head; ctrl != NULL && !error; ctrl = ctrl->next ) {
        if( ctrl->Win32 ) {
            if( tokentype == Y_DIALOG ) {
                control.Style = ctrl->u.ctrl32.Style;
                control.ExtendedStyle = ctrl->u.ctrl32.ExtendedStyle;
                control.Size = ctrl->u.ctrl32.Size;
                control.ID = ctrl->u.ctrl32.ID;
                control.ClassID = ctrl->u.ctrl32.ClassID;
                control.Text = ctrl->u.ctrl32.Text;
                control.ExtraBytes = ctrl->u.ctrl32.ExtraBytes;
                error = ResWriteDialogBoxControl32( &control, CurrResFile.handle );
            } else if( tokentype == Y_DIALOG_EX ) {
                controlex.HelpId = ctrl->u.ctrl32.HelpId;
                controlex.ExtendedStyle = ctrl->u.ctrl32.ExtendedStyle;
                controlex.Style = ctrl->u.ctrl32.Style;
                controlex.Size = ctrl->u.ctrl32.Size;
                controlex.ID = ctrl->u.ctrl32.ID;
                controlex.ClassID = ctrl->u.ctrl32.ClassID;
                controlex.Text = ctrl->u.ctrl32.Text;
                controlex.ExtraBytes = ctrl->u.ctrl32.ExtraBytes;
                error = ResWriteDialogExControl32( &controlex, CurrResFile.handle);
                if( ctrl->dataListHead != NULL ) {
                    SemFlushDataElemList( ctrl->dataListHead, false );
                }
            }
            if( !error ) {
                error = ResPadDWord( CurrResFile.handle );
            }
        } else {
            error = ResWriteDialogBoxControl( &(ctrl->u.ctrl), CurrResFile.handle );
        }
    }
    *err_code = LastWresErr();
    return( error );
} /* SemWriteDiagCtrlList */

static uint_16 SemCountBytes( DataElemList *list )
/************************************************/
{
    DataElemList      *travptr;
    uint_16           bytes = 0;
    int               i;

    for( travptr = list; travptr != NULL; travptr = travptr->next ) {
        for( i = 0; i < travptr->count; i++ ) {
            if( travptr->data[i].IsString ) {
                bytes += travptr->data[i].StrLen;
            } else {
                bytes += 2;
            }
        }
    }

    return( bytes );
}

static void SemCheckDialogBox( FullDialogBoxHeader *head, YYTOKENTYPE tokentype,
                               DlgHelpId dlghelp, FullDiagCtrlList *ctrls )
/***************************************************************************/
{
    FullDialogBoxControl    *travptr;

    if( head->Win32 ) {
        if( tokentype == Y_DIALOG && dlghelp.HelpIdDefined ) {
            RcError( ERR_DIALOG_HELPID );
        } else if( tokentype == Y_DIALOG_EX && dlghelp.HelpIdDefined ) {
            head->u.Head32.ExHead.HelpId = dlghelp.HelpId;
        }
        if( tokentype == Y_DIALOG ) {
            if( head->u.Head32.ExHead.FontItalicDefined ) {
                RcError( ERR_FONT_ITALIC );
            }
            if( head->u.Head32.ExHead.FontWeightDefined ) {
                RcError( ERR_FONT_WEIGHT );
            }
            for( travptr = ctrls->head; travptr != NULL; travptr = travptr->next ) {
                if( travptr->u.ctrl32.HelpIdDefined ) {
                    RcError( ERR_DIALOG_CONTROL_HELPID );
                }
                if( travptr->dataListHead != NULL ) {
                    SemFreeDataElemList( travptr->dataListHead );
                    RcError( ERR_DATA_ELEMENTS );
                }
            }
        }
    }
}

void SemWINWriteDialogBox( WResID *name, ResMemFlags flags,
                    DialogSizeInfo size, FullDialogBoxHeader *head,
                    FullDiagCtrlList *ctrls, DlgHelpId dlghelp,
                    YYTOKENTYPE tokentype )
/******************************************************************/
{
    ResLocation              loc;
    int                      err_code = 0;
    bool                     error;
    FullDialogBoxControl     *travptr;

    error = false;
    if( head == NULL ) {
        head = NewDialogBoxHeader();
    }
    SemCheckDialogBox( head, tokentype, dlghelp, ctrls );
    if( head->Win32 ) {
        if( tokentype != Y_DIALOG ) {
            for( travptr = ctrls->head; travptr != NULL; travptr = travptr->next ) {
                if( travptr->dataListHead != NULL ) {
                    travptr->u.ctrl32.ExtraBytes = SemCountBytes( travptr->dataListHead );
                }
            }
        }
        if( !head->StyleGiven ) {
            head->u.Head32.Head.Style |= (WS_POPUP | WS_BORDER | WS_SYSMENU);
        }

        head->u.Head32.Head.NumOfItems = ctrls->numctrls;
        head->u.Head32.Head.Size = size;
        /* pad the start of the resource so that padding within the resource */
        /* is easier */
        error = ResPadDWord( CurrResFile.handle );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }
    } else {
        if( !head->StyleGiven ) {
            head->u.Head.Style |= (WS_POPUP | WS_BORDER | WS_SYSMENU);
        }
        /* Win16 resources stores resource count in one byte thus
           limiting number of controls to 255. */
        if( ctrls->numctrls > 255 ) {
            RcError( ERR_WIN16_TOO_MANY_CONTROLS, ctrls->numctrls );
            goto CustomError;
        }
        head->u.Head.NumOfItems = ctrls->numctrls & 0xFF;
        head->u.Head.Size = size;
    }
    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();

        if( head->Win32 ) {
            if( tokentype == Y_DIALOG ) {
                error = ResWriteDialogBoxHeader32( &(head->u.Head32.Head), CurrResFile.handle );
            } else if( tokentype == Y_DIALOG_EX ) {
                error = ResWriteDialogExHeader32( &(head->u.Head32.Head), &(head->u.Head32.ExHead), CurrResFile.handle );
            }
            if( !error ) {
                error = ResPadDWord( CurrResFile.handle );
            }
        } else {
            error = ResWriteDialogBoxHeader( &(head->u.Head), CurrResFile.handle );
        }
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        if( ctrls->head == NULL ) {
            if( head->Win32 ) {
                error = ResPadDWord( CurrResFile.handle );
            }
        } else {
            error = SemWriteDiagCtrlList( ctrls, &err_code, tokentype );
        }
        if( error )
            goto OutputWriteError;

        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( (long)RT_DIALOG ), flags, loc );
    } else {
        RCFREE( name );
    }

    SemFreeDialogHeader( head );
    SemFreeDiagCtrlList( ctrls );

    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code )  );
CustomError:
    ErrorHasOccured = true;
    SemFreeDialogHeader( head );
    SemFreeDiagCtrlList( ctrls );
    return;
} /* SemWINWriteDialogBox */


FullDialogBoxControl *SemWINSetControlData( IntMask ctrlstyle,
         uint_16 cntlid, DialogSizeInfo sizeinfo, WResID *cntltext,
         ResNameOrOrdinal *ctlclassname, uint_32 exstyle, DlgHelpId *help )
/*************************************************************************/
{
    FullDialogBoxControl   *control;
    uint_32                mask;
    uint_32                value;
    uint_32                style;

    mask = ctrlstyle.Mask;
    value = ctrlstyle.Value;
    style = (mask & value) | (~mask & (WS_CHILD|WS_VISIBLE));

    control = semInitDiagCtrl();
    if( control->Win32 ) {
        control->u.ctrl32.ID = cntlid;
        control->u.ctrl32.Size = sizeinfo;
        control->u.ctrl32.Text = WResIDToNameOrOrd( cntltext );
        RCFREE( cntltext );
        control->u.ctrl32.ClassID = ResNameOrOrdToControlClass( ctlclassname );
        control->u.ctrl32.Style = style;

        /* ExtraBytes and ExtendStyle are 0 for all controls */
        /* that RC understands */

        control->u.ctrl32.ExtraBytes = 0;
        control->u.ctrl32.ExtendedStyle = exstyle;

        if( help != NULL ) {
            control->u.ctrl32.HelpId = help->HelpId;
            control->u.ctrl32.HelpIdDefined = help->HelpIdDefined;
        } else {
            control->u.ctrl32.HelpId = 0;
            control->u.ctrl32.HelpIdDefined = false;
        }
        RCFREE( ctlclassname );
    } else {
        control->u.ctrl.ID = cntlid;
        control->u.ctrl.Size = sizeinfo;
        control->u.ctrl.Text = WResIDToNameOrOrd( cntltext );
        RCFREE( cntltext );
        control->u.ctrl.ClassID = ResNameOrOrdToControlClass( ctlclassname );
        control->u.ctrl.Style = style;
        /* ExtraBytes is 0 for all controls that RC understands */
        control->u.ctrl.ExtraBytes = 0;
        RCFREE( ctlclassname );
    }

    return( control );
}
