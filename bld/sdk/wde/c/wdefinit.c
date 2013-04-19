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


#include "wdeglbl.h"
#include <stdio.h>
#include "wderesin.h"
#include "wdeobjid.h"
#include "wdefbase.h"
#include "wdefdiag.h"
#include "wdefcntl.h"
#include "wdefcust.h"
#include "wdefbutt.h"
#include "wdefcbox.h"
#include "wdefedit.h"
#include "wdeflbox.h"
#include "wdefstat.h"
#include "wdefscrl.h"
#include "wdefsbar.h"
#include "wdeflvw.h"
#include "wdeftvw.h"
#include "wdeftabc.h"
#include "wdefani.h"
#include "wdefupdn.h"
#include "wdeftrak.h"
#include "wdefprog.h"
#include "wdefhtky.h"
#include "wdefhdr.h"
#include "wdemain.h"
#include "wdemem.h"
#include "wdefinit.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    Bool    (*init)( Bool );
    void    (*fini)( void );
    OBJPTR  (CALLBACK *create)( OBJPTR , RECT *, OBJPTR );
} WdeObjectRoutinesType;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static CREATE_TABLE *WdeCreateTable = NULL;

static WdeObjectRoutinesType WdeObjectRoutines[] = {
    { WdeBaseInit,    WdeBaseFini,    WdeBaseCreate      },
    { WdeDialogInit,  WdeDialogFini,  WdeDialogCreate    },
    { WdeControlInit, WdeControlFini, WdeControlCreate   },
    { WdeButtonInit,  WdeButtonFini,  WdePButtonCreate   },
    { NULL,           NULL,           WdeCButtonCreate   },
    { NULL,           NULL,           WdeRButtonCreate   },
    { NULL,           NULL,           WdeGButtonCreate   },
    { WdeStaticInit,  WdeStaticFini,  WdeFrameCreate     },
    { NULL,           NULL,           WdeTextCreate      },
    { NULL,           NULL,           WdeIconCreate      },
    { WdeEditInit,    WdeEditFini,    WdeEditCreate      },
    { WdeLBoxInit,    WdeLBoxFini,    WdeLBoxCreate      },
    { WdeCBoxInit,    WdeCBoxFini,    WdeCBoxCreate      },
    { WdeScrollInit,  WdeScrollFini,  WdeHScrollCreate   },
    { NULL,           NULL,           WdeVScrollCreate   },
    { NULL,           NULL,           WdeSizeBoxCreate   },
    { WdeSBarInit,    WdeSBarFini,    WdeSBarCreate      },
    { WdeLViewInit,   WdeLViewFini,   WdeLViewCreate     },
    { WdeTViewInit,   WdeTViewFini,   WdeTViewCreate     },
    { WdeTabCInit,    WdeTabCFini,    WdeTabCCreate      },
    { WdeAniCInit,    WdeAniCFini,    WdeAniCCreate      },
    { WdeUpDnInit,    WdeUpDnFini,    WdeUpDnCreate      },
    { WdeTrakInit,    WdeTrakFini,    WdeTrakCreate      },
    { WdeProgInit,    WdeProgFini,    WdeProgCreate      },
    { WdeHtKyInit,    WdeHtKyFini,    WdeHtKyCreate      },
    { WdeHdrInit,     WdeHdrFini,     WdeHdrCreate       },
    { WdeCustomInit,  WdeCustomFini,  WdeCustomCreate1   },
    { NULL,           NULL,           WdeCustomCreate2   },
    { NULL,           NULL,           NULL               }
};

CREATE_TABLE *WdeGetCreateTable( void )
{
    return( WdeCreateTable );
}

Bool WdeInitCreateTable( void )
{
    int         i;
    Bool        first_inst;
    HINSTANCE   inst;

    first_inst = WdeIsFirstInst();
    if( WdeCreateTable == NULL ) {
        inst = WdeGetAppInstance();
        WdeCreateTable = (CREATE_TABLE *)WdeMemAlloc( sizeof( FARPROC ) * NUM_OBJECTS );
        for( i = 0; WdeObjectRoutines[i].create != NULL; i++ ) {
            if( WdeObjectRoutines[i].init != NULL ) {
                if( !WdeObjectRoutines[i].init( first_inst ) ) {
                    return( FALSE );
                }
            }
            (*WdeCreateTable)[i] = (CREATE_RTN)MakeProcInstance( (FARPROC)WdeObjectRoutines[i].create, inst );
        }
    }

    return( TRUE );
}

void WdeFiniCreateTable( void )
{
    int i;

    if( WdeCreateTable != NULL ) {
        for( i = 0; WdeObjectRoutines[i].create != NULL; i++ ) {
            if( WdeObjectRoutines[i].fini != NULL ) {
                WdeObjectRoutines[i].fini();
            }
#ifndef __NT__
            FreeProcInstance( (FARPROC)(*WdeCreateTable)[i] );
#endif
        }
        WdeMemFree( WdeCreateTable );
        WdeCreateTable = NULL;
    }
}
