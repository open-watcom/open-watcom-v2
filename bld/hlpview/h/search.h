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


#ifndef _SEARCH_H_INCLUDED_
#define _SEARCH_H_INCLUDED_

#include "stdui.h"
#include "helpidx.h"
#include "helpio.h"
#include "uidialog.h"

typedef struct HelpHdl {
    FILE        *fp;
    HelpHeader  header;
    uint_16     *itemindex;
    char        *def_topic;
    char        *desc_str;
} *HelpHdl;

typedef struct {
    HelpHdl             hdl;
    unsigned            page;
    unsigned            entry;
    unsigned long       offset;
} HelpSrchInfo;

extern char             *HelpFindNext( HelpSrchInfo *info );
extern unsigned         HelpFindFirst( HelpHdl hdl, const char *name, HelpSrchInfo *info );
extern HelpHdl          InitHelpSearch( FILE *fp );
extern void             FiniHelpSearch( HelpHdl hdl );
extern char             *HelpFindPrev( HelpSrchInfo *info );
extern unsigned long    HelpGetOffset( HelpSrchInfo cursor );
extern unsigned long    HelpFindTopicOffset( HelpHdl hdl, const char *topic );
extern char             *HelpGetIndexedTopic( HelpHdl hdl, unsigned index );
extern char             *GetDefTopic( HelpHdl hdl );
extern char             *GetDescrip( HelpHdl hdl );

extern char             *HelpSearch( HelpHdl hdl );
extern void             HelpDialogCallBack( a_dialog *info );
extern void             SearchDlgFini( void );
#endif
