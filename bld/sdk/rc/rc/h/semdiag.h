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


#ifndef SEMDIAG_INCLUDED
#define SEMDIAG_INCLUDED

#include "watcom.h"
#include "wresall.h"
#include "semraw.h"

/* condition inclution to prevent recursive inclusion */
#ifndef SEMSTRUCT_INCLUDED
    #include "semantic.h"
#endif

typedef struct FullDialogOptions {
    uint_8      token;
    union {
        ResNameOrOrdinal *  Name;
        char *              Str;
        DialogStyle         Style;
        DialogExstyle       Exstyle;
        WResLangType        lang;
        struct {
            uint_16         PointSize;
            char *          FontName;
            uint_16         FontWeight;
            uint_8          FontItalic;
            uint_8          FontExtra;  /* don't know what this is but
                                         * MS rc accepts it and it appears in
                                         * some samples */
            char            FontWeightDefined;
            char            FontItalicDefined;
        } Font;
    } Opt;
} FullDialogOptions;

typedef struct DlgHeader32 {
    DialogBoxHeader32    Head;
    DialogExHeader32     ExHead;
} DlgHeader32;

typedef struct FullDialogBoxHeader {
    uint_8              Win32;
    union {
        DialogBoxHeader     Head;
        DlgHeader32         Head32;
    } u;
    uint_8              StyleGiven;
} FullDialogBoxHeader;

typedef struct DlgControl32 {
    uint_32               Style;
    uint_32               ExtendedStyle;
    DialogSizeInfo        Size;
    uint_32               ID;
    ControlClass         *ClassID;
    ResNameOrOrdinal     *Text;
    uint_16               ExtraBytes;       /* should be 0 */
    uint_32               HelpId;           /* only used for Dialogex */
    char                  HelpIdDefined;    /* only used for Dialogex */
}DlgControl32;

typedef struct FullDialogBoxControl {
    struct FullDialogBoxControl *   next;
    struct FullDialogBoxControl *   prev;
    uint_8                          Win32;
    union {
        DialogBoxControl            ctrl;
        DlgControl32                ctrl32;
    } u;
    DataElemList                *   dataListHead;
} FullDialogBoxControl;

typedef struct FullDiagCtrlList {
    FullDialogBoxControl *          head;
    FullDialogBoxControl *          tail;
    uint_8                          numctrls;
} FullDiagCtrlList;

typedef struct FullDiagCtrlOptions {
    DialogSizeInfo          Size;
    IntMask                 Style;
    uint_16                 ID;
    ResNameOrOrdinal *      Text;
    uint_32                 ExtendedStyle;
    uint_32                 HelpId;
    int                     HelpIdDefined;
} FullDiagCtrlOptions;

typedef struct DlgHelpId {
    uint_32     HelpId;
    char        HelpIdDefined;
} DlgHelpId;

extern FullDialogBoxHeader * SemNewDiagOptions( FullDialogOptions * opt );
extern FullDialogBoxHeader * SemDiagOptions( FullDialogBoxHeader * head,
                    FullDialogOptions * opt );
extern FullDiagCtrlList * SemNewDiagCtrlList( FullDialogBoxControl * ctrl,
                    DataElemList * );
extern FullDiagCtrlList * SemAddDiagCtrlList( FullDiagCtrlList * list,
                    FullDialogBoxControl * ctrl, DataElemList * );
extern FullDialogBoxControl * SemNewDiagCtrl( uint_8 token,
                    FullDiagCtrlOptions opts );
extern FullDialogBoxControl * SemInitDiagCtrl( void );
extern void SemWriteDialogBox( WResID * name, ResMemFlags, DialogSizeInfo,
                    FullDialogBoxHeader *, FullDiagCtrlList *, DlgHelpId,
                    uint_16 );
extern FullDiagCtrlList * SemEmptyDiagCtrlList( void );
extern FullDialogBoxControl * SemSetControlData( IntMask, unsigned long,
         DialogSizeInfo, WResID *, ResNameOrOrdinal *, uint_32, DlgHelpId * );

#endif
