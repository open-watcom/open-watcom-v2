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
* Description:  Windows dialog related semantic actions.
*
****************************************************************************/


/*
 * The data structure for DIALOGEX's is as follows:
 *
 * struct DialogSettings {
 *     WORD       wVersion;      // Always 00 01
 *     WORD       wAlwaysFF;     // Always FF FF
 *     DWORD      HelpId;
 *     DWORD      ExStyle;
 *     DWORD      Style;
 *     WORD       NumberOfItems;
 *     WORD       x;
 *     WORD       y;
 *     WORD       cx;
 *     WORD       cy;
 *     [Name/Ord] MenuName;
 *     [Name/Ord] ClassName;
 *     WCHAR      szCaption[]
 *     WORD       fontHeight;
 *     WORD       fontWeight;
 *     WORD       italic;
 *     WCHAR      szFontName[];
 * }
 *
 * struct ControlData {
 *     DWORD      HelpId;
 *     DWORD      ExStyle;
 *     DWORD      Style;
 *     WORD       x;
 *     WORD       y;
 *     WORD       cx;
 *     WORD       cy;
 *     DWORD      id;
 *     [Name/Ord] classId;
 *     [Name/Ord] text;
 *     WORD       numBytes;  // Number of bytes of data elements that follow.
 * }
 *
 *
 *
 */

#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"


static FullDialogBoxHeader *NewDialogBoxHeader( void )
/****************************************************/
{
    FullDialogBoxHeader     *diaghead;

    diaghead = MemAllocSafe( sizeof( FullDialogBoxHeader ) );

    if( CmdLineParms.iswin32 ) {
        diaghead->Head.Style = 0;
        diaghead->Head.ExtendedStyle = 0;
        diaghead->Head.NumOfItems = 0;
        diaghead->Head.SizeInfo.x = 0;
        diaghead->Head.SizeInfo.y = 0;
        diaghead->Head.SizeInfo.width = 0;
        diaghead->Head.SizeInfo.height = 0;
        diaghead->Head.MenuName = NULL;
        diaghead->Head.ClassName = NULL;
        diaghead->Head.Caption = NULL;
        diaghead->Head.PointSize = 0;
        diaghead->Head.FontName = NULL;

        diaghead->Head.FontWeight = 0;
        diaghead->Head.FontItalic = 0;
        diaghead->Head.FontCharset = DEFAULT_CHARSET;
        diaghead->Head.HelpId = 0;
        diaghead->Head.FontWeightDefined = false;
        diaghead->Head.FontItalicDefined = false;
        diaghead->Head.FontCharsetDefined = false;
    } else {
        diaghead->Head.Style = 0;
        diaghead->Head.NumOfItems = 0;
        diaghead->Head.SizeInfo.x = 0;
        diaghead->Head.SizeInfo.y = 0;
        diaghead->Head.SizeInfo.width = 0;
        diaghead->Head.SizeInfo.height = 0;
        diaghead->Head.MenuName = NULL;
        diaghead->Head.ClassName = NULL;
        diaghead->Head.Caption = NULL;
        diaghead->Head.PointSize = 0;
        diaghead->Head.FontName = NULL;
    }

    diaghead->StyleGiven = false;

    return( diaghead );
} /* NewDialogBoxHeader */

static void AddDialogOption( DialogBoxHeader *head, FullDialogOptions *opt )
/**************************************************************************/
{
    switch( opt->token ) {
    case Y_STYLE:
        head->Style |= opt->Opt.Style;
        break;
    case Y_MENU:
        if( head->MenuName != NULL ) {
            MemFree( head->MenuName );
        }
        head->MenuName = opt->Opt.Name;
        break;
    case Y_CLASS:
        if( head->ClassName != NULL ) {
            MemFree( head->ClassName );
        }
        head->ClassName = opt->Opt.Name;
        break;
    case Y_FONT:
        if( CmdLineParms.winver < 30 ) {
            RcWarning( RCMSG_DLGFONT_WIN2X );
        } else {
            head->Style |= DS_SETFONT;
            head->PointSize = opt->Opt.Font.PointSize;
            if( head->FontName != NULL ) {
                MemFree( head->FontName );
            }
            head->FontName = opt->Opt.Font.FontName;
        }
        break;
    case Y_CAPTION:
        head->Style |= WS_CAPTION;
        if( head->Caption != NULL ) {
            MemFree( head->Caption );
        }
        head->Caption = opt->Opt.Str;
        break;
    case Y_EXSTYLE:
        RcWarning( RCMSG_NT_KEYWORD, SemWINTokenToString( opt->token ) );
        break;
    }
} /* AddDialogOptions */

static void AddDialogOption32( DialogBoxHeader *head, FullDialogOptions *opt )
/****************************************************************************/
{
    switch( opt->token ) {
    case Y_STYLE:
        head->Style |= opt->Opt.Style;
        break;
    case Y_EXSTYLE:
        head->ExtendedStyle |= opt->Opt.Exstyle;
        break;
    case Y_MENU:
        if( head->MenuName != NULL ) {
            MemFree( head->MenuName );
        }
        head->MenuName = opt->Opt.Name;
        break;
    case Y_CLASS:
        if( head->ClassName != NULL ) {
            MemFree( head->ClassName );
        }
        head->ClassName = opt->Opt.Name;
        break;
    case Y_FONT:
        head->Style |= DS_SETFONT;
        head->PointSize = opt->Opt.Font.PointSize;
        if( head->FontName != NULL ) {
            MemFree( head->FontName );
        }
        head->FontName = opt->Opt.Font.FontName;
        head->FontItalic = opt->Opt.Font.FontItalic;
        head->FontWeight = opt->Opt.Font.FontWeight;
        head->FontCharset = opt->Opt.Font.FontCharset;
        head->FontItalicDefined = opt->Opt.Font.FontItalicDefined;
        head->FontWeightDefined = opt->Opt.Font.FontWeightDefined;
        head->FontCharsetDefined = opt->Opt.Font.FontCharsetDefined;
        break;
    case Y_CAPTION:
        head->Style |= WS_CAPTION;
        if( head->Caption != NULL ) {
            MemFree( head->Caption );
        }
        head->Caption = opt->Opt.Str;
        break;
    case Y_LANGUAGE:
        SemWINSetResourceLanguage( &opt->Opt.lang, true );
        break;
    }
} /* AddDialogOptions32 */

FullDialogBoxHeader *SemWINDialogOptions( FullDialogBoxHeader *diaghead, FullDialogOptions *opt )
/***********************************************************************************************/
{
    if( diaghead == NULL ) {
        diaghead = NewDialogBoxHeader();
    }
    if( CmdLineParms.iswin32 ) {
        AddDialogOption32( &diaghead->Head, opt );
    } else {
        AddDialogOption( &diaghead->Head, opt );
    }
    if( opt->token == Y_STYLE ) {
        diaghead->StyleGiven = true;
    }

    return( diaghead );
}

FullDialogCtrlListWIN *SemWINEmptyDialogCtrlList( void )
/******************************************************/
{
    FullDialogCtrlListWIN   *newlist;

    newlist = MemAllocSafe( sizeof( FullDialogCtrlListWIN ) );
    newlist->head = NULL;
    newlist->tail = NULL;
    newlist->numctrls = 0;
    return( newlist );
}

FullDialogCtrlListWIN *SemWINAddDialogCtrlList( FullDialogCtrlListWIN *list,
                    FullDialogBoxControlWIN *diagctrl, DataElemList *dataList )
/**********************************************************************/
{
    if( list == NULL ) {
        list = SemWINEmptyDialogCtrlList();
    }
    if( diagctrl != NULL ) {
        diagctrl->dataListHead = dataList;
        ResAddLLItemAtEnd( (void **)&(list->head), (void **)&(list->tail), diagctrl );
        list->numctrls++;
    }
    return( list );
} /* SemWINAddDialogCtrlList */

static FullDialogBoxControlWIN *semInitDialogCtrl( void )
/*******************************************************/
{
    FullDialogBoxControlWIN     *diagctrl;

    diagctrl = MemAllocSafe( sizeof( FullDialogBoxControlWIN ) );
    diagctrl->next = NULL;
    diagctrl->prev = NULL;

    return( diagctrl );
} /* semInitDialogCtrl */

/*
 * These are the default styles used for all dialog box control statmens
 * except the control statement (see rc.y for it).  The HI style contains
 * all the WS_ styles that are applicable (the high word) and the LO style
 * contains the styles that are particular to the class of the control
 */

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

FullDialogBoxControlWIN *SemWINNewDialogCtrl( YYTOKENTYPE token, FullDialogCtrlOptionsWIN opts )
/**********************************************************************************************/
{
    FullDialogBoxControlWIN *diagctrl;
    uint_32                 style_mask; /* for the style of the control */
    uint_32                 style_value;
    uint_32                 defstyle_hi = 0;
    uint_32                 defstyle_lo = 0;
    uint_32                 style_hi;
    uint_32                 style_lo;
    uint_16                 class = 0;
    uint_16                 tmp_mask;
    ResNameOrOrdinal        *cclass;

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
        if( CmdLineParms.iswin32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTO3STATE_HI;
            defstyle_lo = DEF_AUTO3STATE_LO;
        } else {
            RcWarning( RCMSG_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    case Y_AUTOCHECKBOX:
        if( CmdLineParms.iswin32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTOCHECKBOX_HI;
            defstyle_lo = DEF_AUTOCHECKBOX_LO;
        } else {
            RcWarning( RCMSG_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    case Y_AUTORADIOBUTTON:
        if( CmdLineParms.iswin32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_AUTORADIOBUTTON_HI;
            defstyle_lo = DEF_AUTORADIOBUTTON_LO;
        } else {
            RcWarning( RCMSG_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
//    case Y_PUSHBOX:
//      class = CLASS_BUTTON;
//      defstyle_hi = DEF_PUSHBOX_HI;
//      defstyle_lo = DEF_PUSHBOX_LO;
//      break;
    case Y_STATE3:
        if( CmdLineParms.iswin32 ) {
            class = CLASS_BUTTON;
            defstyle_hi = DEF_STATE3_HI;
            defstyle_lo = DEF_STATE3_LO;
        } else {
            RcWarning( RCMSG_NT_KEYWORD, SemWINTokenToString( token ) );
            return( NULL );
        }
        break;
    }

    diagctrl = semInitDialogCtrl();
    cclass = ResNumToNameOrOrdinal( class );

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
    /*
     * for the high word use the bits that were mentioned from style_value
     * and all the other bits from defstyle_hi
     */
    style_hi = (style_mask & style_value) | (~style_mask & defstyle_hi);

    if( CmdLineParms.iswin32 ) {
        diagctrl->ctrl.ID = opts.ID;
        diagctrl->ctrl.SizeInfo = opts.SizeInfo;
        diagctrl->ctrl.Text = opts.Text;
        diagctrl->ctrl.ClassID = cclass;
        diagctrl->ctrl.Style = (style_lo & LO_WORD) | (style_hi & HI_WORD);
        /*
         * ExtraBytes and ExtendStyle are 0 for all controls
         * that RC understands
         */
        diagctrl->ctrl.ExtraBytes = 0;

        diagctrl->ctrl.ExtendedStyle = opts.ExtendedStyle;
        diagctrl->ctrl.HelpId = opts.HelpId;
    } else {
        diagctrl->ctrl.ID = opts.ID;
        diagctrl->ctrl.SizeInfo = opts.SizeInfo;
        diagctrl->ctrl.Text = opts.Text;
        diagctrl->ctrl.ClassID = cclass;
        diagctrl->ctrl.Style = (style_lo & LO_WORD) | (style_hi & HI_WORD);
        /*
         * ExtraBytes is 0 for all controls that RC understands
         */
        diagctrl->ctrl.ExtraBytes = 0;
    }

    return( diagctrl );
} /* SemWINNewDialogCtrl */


static void SemFreeDialogCtrlList( FullDialogCtrlListWIN *list )
/**************************************************************/
{
    FullDialogBoxControlWIN     *diagctrl;

    while( (diagctrl = list->head) != NULL ) {
        list->head = diagctrl->next;
        /*
         * free the contents of pointers within the structure
         */
        if( CmdLineParms.iswin32 ) {
            if( diagctrl->ctrl.ClassID != NULL ) {
                MemFree( diagctrl->ctrl.ClassID );
            }
            if( diagctrl->ctrl.Text != NULL ) {
                MemFree( diagctrl->ctrl.Text );
            }
        } else {
            if( diagctrl->ctrl.ClassID != NULL ) {
                MemFree( diagctrl->ctrl.ClassID );
            }
            if( diagctrl->ctrl.Text != NULL ) {
                MemFree( diagctrl->ctrl.Text );
            }
        }
        MemFree( diagctrl );
    }
    MemFree( list );

} /* SemFreeDialogCtrlList */

static void SemFreeDialogHeader( FullDialogBoxHeader *diaghead )
/**************************************************************/
{
    if( CmdLineParms.iswin32 ) {
        if( diaghead->Head.MenuName != NULL ) {
            MemFree( diaghead->Head.MenuName );
        }
        if( diaghead->Head.ClassName != NULL ) {
            MemFree( diaghead->Head.ClassName );
        }
        if( diaghead->Head.Caption != NULL ) {
            MemFree( diaghead->Head.Caption );
        }
        if( diaghead->Head.FontName != NULL ) {
            MemFree( diaghead->Head.FontName );
        }
    } else {
        if( diaghead->Head.MenuName != NULL ) {
            MemFree( diaghead->Head.MenuName );
        }
        if( diaghead->Head.ClassName != NULL ) {
            MemFree( diaghead->Head.ClassName );
        }
        if( diaghead->Head.Caption != NULL ) {
            MemFree( diaghead->Head.Caption );
        }
        if( diaghead->Head.FontName != NULL ) {
            MemFree( diaghead->Head.FontName );
        }
    }

    MemFree( diaghead );
} /* SemFreeDialogHeader */

static bool SemWriteDialogCtrlList( FullDialogCtrlListWIN *list, int *err_code, bool is_dialogex )
/************************************************************************************************/
{
    bool                        error;
    FullDialogBoxControlWIN     *diagctrl;

    error = false;
    for( diagctrl = list->head; diagctrl != NULL && !error; diagctrl = diagctrl->next ) {
        if( is_dialogex ) {
            error = ResWriteDialogBoxExControl( &(diagctrl->ctrl), CurrResFile.fp );
            if( diagctrl->dataListHead != NULL ) {
                SemFlushDataElemList( diagctrl->dataListHead, false );
            }
        } else if( CmdLineParms.iswin32 ) {
            error = ResWriteDialogBoxControl32( &(diagctrl->ctrl), CurrResFile.fp );
        } else {
            error = ResWriteDialogBoxControl( &(diagctrl->ctrl), CurrResFile.fp );
        }
    }
    *err_code = LastWresErr();
    return( error );
} /* SemWriteDialogCtrlList */

static size_t SemCountBytes( DataElemList *list )
/***********************************************/
{
    DataElemList      *travptr;
    size_t            bytes;
    unsigned          i;

    bytes = 0;
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

static void SemCheckDialogBox( FullDialogBoxHeader *diaghead, bool is_dialogex,
                               help_id HelpId, FullDialogCtrlListWIN *ctrls )
/*****************************************************************************/
{
    FullDialogBoxControlWIN *diagctrl;

    if( CmdLineParms.iswin32 ) {
        if( is_dialogex ) {
            diaghead->Head.HelpId = HelpId;
        } else {
            for( diagctrl = ctrls->head; diagctrl != NULL; diagctrl = diagctrl->next ) {
                if( diagctrl->dataListHead != NULL ) {
                    SemFreeDataElemList( diagctrl->dataListHead );
                    RcError( RCMSG_DATA_ELEMENTS );
                }
            }
        }
    }
}

void SemWINWriteDialogBox( WResID *res_id, ResMemFlags res_flags,
                    DialogSizeInfo SizeInfo, FullDialogBoxHeader *diaghead,
                    FullDialogCtrlListWIN *ctrls, help_id HelpId,
                    bool is_dialogex )
/*************************************************************************/
{
    ResLocation              loc;
    int                      err_code = 0;
    int                      error;
    FullDialogBoxControlWIN  *diagctrl;

    error = 0;
    if( diaghead == NULL ) {
        diaghead = NewDialogBoxHeader();
    }
    SemCheckDialogBox( diaghead, is_dialogex, HelpId, ctrls );
    if( CmdLineParms.iswin32 ) {
        if( is_dialogex ) {
            for( diagctrl = ctrls->head; diagctrl != NULL; diagctrl = diagctrl->next ) {
                if( diagctrl->dataListHead != NULL ) {
                    diagctrl->ctrl.ExtraBytes = SemCountBytes( diagctrl->dataListHead );
                }
            }
        }
        if( !diaghead->StyleGiven ) {
            diaghead->Head.Style |= (WS_POPUP | WS_BORDER | WS_SYSMENU);
        }

        diaghead->Head.NumOfItems = ctrls->numctrls;
        diaghead->Head.SizeInfo = SizeInfo;
        /*
         * pad the start of the resource so that padding within the resource
         * is easier
         */
        if( ResWritePadDWord( CurrResFile.fp ) ) {
            error = 1;
        }
    } else {
        if( !diaghead->StyleGiven ) {
            diaghead->Head.Style |= (WS_POPUP | WS_BORDER | WS_SYSMENU);
        }
        /*
         * Win16 resources stores resource count in one byte thus
         * limiting number of controls to 255.
         */
        if( ctrls->numctrls > 255 ) {
            error = 2;
        } else {
            diaghead->Head.NumOfItems = ctrls->numctrls & 0xFF;
            diaghead->Head.SizeInfo = SizeInfo;
        }
    }
    if( !error
      && !ErrorHasOccured ) {
        loc.start = SemStartResource();
        if( is_dialogex ) {
            if( ResWriteDialogBoxExHeader( &(diaghead->Head), CurrResFile.fp ) ) {
                error = 1;
            }
        } else if( CmdLineParms.iswin32 ) {
            if( ResWriteDialogBoxHeader32( &(diaghead->Head), CurrResFile.fp ) ) {
                error = 1;
            }
        } else {
            if( ResWriteDialogBoxHeader( &(diaghead->Head), CurrResFile.fp ) ) {
                error = 1;
            }
        }
        if( !error
          && ctrls->head != NULL ) {
            error = SemWriteDialogCtrlList( ctrls, &err_code, is_dialogex );
        }
        if( !error ) {
            loc.len = SemEndResource( loc.start );
            SemAddResourceAndFree( res_id, WResIDFromNum( RESOURCE2INT( RT_DIALOG ) ), res_flags, loc );
        }
    } else {
        MemFree( res_id );
    }
    if( error ) {
        if( error == 2 ) {
            RcError( RCMSG_WIN16_TOO_MANY_CONTROLS, ctrls->numctrls );
        } else {
            err_code = LastWresErr();
            RcError( RCMSG_WRITTING_RES, CurrResFile.filename, strerror( err_code )  );
        }
        ErrorHasOccured = true;
    }
    SemFreeDialogHeader( diaghead );
    SemFreeDialogCtrlList( ctrls );

} /* SemWINWriteDialogBox */


FullDialogBoxControlWIN *SemWINSetControlData( IntMask ctrlstyle,
         uint_16 cntlid, DialogSizeInfo SizeInfo, WResID *cntltext_id,
         ResNameOrOrdinal *ctlclassname, uint_32 exstyle, help_id HelpId )
/*************************************************************************/
{
    FullDialogBoxControlWIN *diagctrl;
    uint_32                 mask;
    uint_32                 value;
    uint_32                 style;

    mask = ctrlstyle.Mask;
    value = ctrlstyle.Value;
    style = (mask & value) | (~mask & (WS_CHILD|WS_VISIBLE));

    diagctrl = semInitDialogCtrl();
    if( CmdLineParms.iswin32 ) {
        diagctrl->ctrl.ID = cntlid;
        diagctrl->ctrl.SizeInfo = SizeInfo;
        diagctrl->ctrl.Text = WResIDToNameOrOrdinal( cntltext_id );
        MemFree( cntltext_id );
        diagctrl->ctrl.ClassID = ResNormalizeControlClass( ctlclassname );
        diagctrl->ctrl.Style = style;
        /*
         * ExtraBytes and ExtendStyle are 0 for all controls
         * that RC understands
         */
        diagctrl->ctrl.ExtraBytes = 0;
        diagctrl->ctrl.ExtendedStyle = exstyle;
        diagctrl->ctrl.HelpId = HelpId;

        MemFree( ctlclassname );
    } else {
        diagctrl->ctrl.ID = cntlid;
        diagctrl->ctrl.SizeInfo = SizeInfo;
        diagctrl->ctrl.Text = WResIDToNameOrOrdinal( cntltext_id );
        MemFree( cntltext_id );
        diagctrl->ctrl.ClassID = ResNormalizeControlClass( ctlclassname );
        diagctrl->ctrl.Style = style;
        /*
         * ExtraBytes is 0 for all controls that RC understands
         */
        diagctrl->ctrl.ExtraBytes = 0;
        MemFree( ctlclassname );
    }

    return( diagctrl );
}
