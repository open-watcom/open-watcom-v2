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


#include <unistd.h>
#include <sys/stat.h>
#include <process.h>

#include "common.h"
#include "aui.h"
#include "guidlg.h"
#include "wpaui.h"
#include "dip.h"
#include "msg.h"
#include "pathlist.h"

//#include "dlgsamp.def"
//#include "wpdriver.def"
//#include "utils.def"
//#include "msg.def"
extern void OpenSample(void);
extern void ReplaceExt(char *path,char *addext);
extern void ErrorMsg(char *msg,... );


extern char     SamplePath[_MAX_PATH];


static char * sampFilterList = {
    "Sample Files (*.smp)\0*.smp\0"
    ALLFILES
};



extern bint WPSampFound()
/***********************/
{
    struct stat     file_stat;
    char            buffer[_MAX_PATH2];
    char *          ext;

    if( stat( SamplePath, &file_stat ) != -1 ) return( B_TRUE );
    if( SamplePath[0] == NULLCHAR ) return( B_FALSE );
    _splitpath2( SamplePath, buffer, NULL, NULL, NULL, &ext );
    if( *ext != NULLCHAR ) return( B_FALSE );
    ReplaceExt( SamplePath, ".smp" );
    if( stat( SamplePath, &file_stat ) != -1 ) return( B_TRUE );
    return( B_FALSE );
}



extern void DlgOpenSample()
/*************************/
{
    for( ;; ) {
        if( !DlgFileBrowse( LIT( Enter_Sample ), sampFilterList, SamplePath,
                            sizeof( SamplePath ), 0 ) ) break;
        if( WPSampFound() ) {
            OpenSample();
            break;
        }
        ErrorMsg( LIT( File_Does_Not_Exist ), SamplePath );
    }
}
