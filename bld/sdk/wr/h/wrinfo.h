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


#ifndef WRINFO_INCLUDED
#define WRINFO_INCLUDED

#include "wresall.h"
#include "wrglbl.h"
#include "wrmemi.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WR_DONT_KNOW,
    WR_WIN16M_RES,
    WR_WIN16W_RES,
    WR_WINNTM_RES,
    WR_WINNTW_RES,
    WR_WIN16_EXE,
    WR_WINNT_EXE,
    WR_WIN16_DLL,
    WR_WINNT_DLL,
    WR_WIN_BITMAP,
    WR_WIN_ICON,
    WR_WIN_CURSOR,
    WR_WIN_RC_STR,
    WR_WIN_RC_MENU,
    WR_WIN_RC_ACCEL,
    WR_WIN_RC_DLG,
    WR_WIN_RC,
    WR_INVALID_FILE
} WRFileType;

typedef struct WRInfoStruct {
    WRFileType  save_type;
    char        *save_name;
    WRFileType  file_type;
    char        *file_name;
    char        *tmp_file;
    WResDir     dir;
    WRFileType  internal_type;
    char        *internal_filename;
} WRInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRDLLENTRY extern WRInfo * WRAPI WRAllocWRInfo( void );
WRDLLENTRY extern void     WRAPI WRFreeWRInfo( WRInfo * );
WRDLLENTRY extern void     WRAPI WRFreeWResDirData( WResDir );
WRDLLENTRY extern int      WRAPI WRCountZeroLengthResources( WResDir dir );
WRDLLENTRY extern int      WRAPI WRGetInternalRESName( char *filename, char *newname );

#endif
