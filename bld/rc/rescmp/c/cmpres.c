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


#include <stdio.h>
#include "watcom.h"
#include "trmemcvr.h"
#include "wresall.h"
#include "global.h"
#include "bincmp.h"

static WResDirWindow LookUpResource( WResDirWindow wind1, WResDir dir2 )
/**********************************************************************/
{
    WResDirWindow   wind2;
    WResTypeInfo    *type1;
    WResResInfo     *res1;
    WResLangInfo    *lang1;
    char            *resname1;

    type1 = WResGetTypeInfo( wind1 );
    res1 = WResGetResInfo( wind1 );
    lang1 = WResGetLangInfo( wind1 );

    wind2 = WResFindResource( &(type1->TypeName), &(res1->ResName), dir2,
                              &(lang1->lang) );
    if (WResIsEmptyWindow( wind2 ) && !CmdLineParms.Quiet) {
        resname1 = WResIDToStr( &(res1->ResName) );
        printf( "Error: Resource %s (lang 0x%X SubLang 0x%X) not in file %s\n",
                        resname1, (int)lang1->lang.lang,
                        (int)lang1->lang.sublang,
                        CmdLineParms.FileName2 );
        TRMemFree( resname1 );
    }

    return( wind2 );
}

static int CompareOneResource( WResFileID handle1, WResDirWindow wind1,
                                WResFileID handle2, WResDirWindow wind2 )
/*************************************************************************/
{
    int             retcode;
    int             oldretcode;
    WResResInfo     *res1;
    WResResInfo     *res2;
    WResLangInfo    *lang1;
    WResLangInfo    *lang2;
    char *          resname1;

    oldretcode = 0;

    res1 = WResGetResInfo( wind1 );
    res2 = WResGetResInfo( wind2 );
    lang1 = WResGetLangInfo( wind1 );
    lang2 = WResGetLangInfo( wind2 );

    if (lang1->MemoryFlags != lang2->MemoryFlags) {
        if (!CmdLineParms.Quiet) {
            resname1 = WResIDToStr( &(res1->ResName) );
            printf( "Error: memory flags for resource %s are not the same\n",
                            resname1 );
            TRMemFree( resname1 );
        }
        oldretcode = 1;
    }
    if (lang1->Length != lang2->Length) {
        if (!CmdLineParms.Quiet) {
            resname1 = WResIDToStr( &(res1->ResName) );
            printf( "Error: resource %s does not have the same length\n",
                            resname1 );
            TRMemFree( resname1 );
        }
        oldretcode = 1;
    } else {
        retcode = BinaryCompare( handle1, lang1->Offset, handle2,
                        lang2->Offset, lang1->Length );
        switch (retcode) {
        case 1:
            if (!CmdLineParms.Quiet) {
                resname1 = WResIDToStr( &(res1->ResName) );
                printf( "Error: contents of resource %s are different.\n",
                                resname1 );
                TRMemFree( resname1 );
            }
            oldretcode = retcode;
            break;
        case -1:
            oldretcode = retcode;
            break;
        }
    }

    return( oldretcode );
}

int CompareResources( WResFileID handle1, WResDir dir1,
                        WResFileID handle2, WResDir dir2 )
/********************************************************/
{
    int             retcode;
    int             oldretcode;
    WResDirWindow   wind1;
    WResDirWindow   wind2;

    oldretcode = 0;

    if (WResIsEmpty( dir1 )) {
        if (WResIsEmpty( dir2 )) {
            return( 0 );
        } else {
            return( 1 );
        }
    } else {
        wind1 = WResFirstResource( dir1 );
        while( !WResIsEmptyWindow( wind1 ) ) {
            /* find the window in dir2 over the same resource */
            wind2 = LookUpResource( wind1, dir2 );

            if ( !WResIsEmptyWindow( wind2 ) ) {
                /* compare the contents of the actual resource */
                retcode = CompareOneResource( handle1, wind1, handle2, wind2 );
                switch (retcode) {
                case -1:
                    return( -1 );
                    break;
                case 1:
                    if (!CmdLineParms.CheckAll) {
                        return( 1 );
                    } else {
                        oldretcode = 1;
                    }
                    break;
                }
            } else {
                if (!CmdLineParms.CheckAll) {
                    return( 1 );
                } else {
                    oldretcode = 1;
                }
            }

            wind1 = WResNextResource( wind1, dir1 );
        } /* while */
    } /* if dir is empty */

    return( oldretcode );
}
