/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Generic dialog functions.
*
****************************************************************************/


/*
*   COMMENTS: Functions contolling the abstract type "a_dialog_header"
*             The only reference to the FirstDialog is through these
*             functions.
*/

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "wio.h"
#include "setup.h"
#include "guiutil.h"
#include "setupinf.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "genvbl.h"
#include "utils.h"
#include "gendlg.h"
#include "dlggen.h"

#include "clibext.h"


static a_dialog_header *FirstDialog = NULL;
static a_dialog_header *LastDialog = NULL;

a_dialog_header *FindDialogByName( const char *name )
/***************************************************/
{
    a_dialog_header *dlg;

    for( dlg = FirstDialog; dlg != NULL; dlg = dlg->next ) {
        if( stricmp( dlg->name, name ) == 0 ) {
            break;
        }
    }
    return( dlg );
}

static void FreeDialog( a_dialog_header *dlg )
/********************************************/
{
    int i;

    GUIMemFree( dlg->name );
    GUIMemFree( dlg->condition );
    GUIMemFree( dlg->title );
    if( !dlg->def_dlg ) {            /* free non-default controls */
        for( i = 0; dlg->pVariables[i] != NO_VAR; ++i ) {
            if( dlg->pConditions[i] != NULL ) {
                GUIMemFree( dlg->pConditions[i] );
            }
        }
        for( i = 0; i < dlg->num_controls; i++ ) {
            GUIMemFree( (void *)dlg->controls[i].text );
            if( dlg->controls_ext[i].pVisibilityConds != NULL ) {
                GUIMemFree( dlg->controls_ext[i].pVisibilityConds );
            }
        }
        GUIMemFree( dlg->controls_ext );
        GUIMemFree( dlg->controls );
    }
    GUIMemFree( dlg );
}

a_dialog_header *AddNewDialog( const char *name )
/***********************************************/
// Add new dialogs to front of linked list.
// Delete default dialogs if specified.
{
    a_dialog_header *old_dlg;
    a_dialog_header *new_dlg;

    new_dlg = (a_dialog_header *)GUIMemAlloc( sizeof( a_dialog_header ) );
    memset( new_dlg, '\0', sizeof( *new_dlg ) );
    new_dlg->name = GUIStrDup( name, NULL );
    new_dlg->adjusted = false;
    new_dlg->def_dlg = false;
    new_dlg->defaults_set = false;
    new_dlg->ret_val = DLG_NEXT;
    new_dlg->any_check = NO_VAR;
    new_dlg->pVariables[0] = NO_VAR;
    new_dlg->pConditions[0] = NULL;

    /* check if old dialog existed */
    old_dlg = FindDialogByName( name );
    if( old_dlg != NULL ) {
        new_dlg->next = old_dlg->next;
        new_dlg->prev = old_dlg->prev;
        if( new_dlg->next != NULL ) {
            new_dlg->next->prev = new_dlg;
        }
        if( new_dlg->prev != NULL ) {
            new_dlg->prev->next = new_dlg;
        }
        if( FirstDialog == NULL ) {
            FirstDialog = new_dlg;
            LastDialog = new_dlg;
        }
        if( FirstDialog == old_dlg ) {
            FirstDialog = new_dlg;
        }
        if( LastDialog == old_dlg ) {
            LastDialog = new_dlg;
        }
        FreeDialog( old_dlg );        // replace old default dialog
    } else {
        new_dlg->prev = LastDialog;
        new_dlg->next = NULL;
        if( FirstDialog == NULL ) {
            FirstDialog = new_dlg;
            LastDialog = new_dlg;
        } else {
            LastDialog->next = new_dlg;
        }
        LastDialog = new_dlg;
    }

    return( new_dlg );
}

bool CheckDialog( const char *name )
/**********************************/
{
    a_dialog_header     *dlg;

    dlg = FindDialogByName( name );
    if( dlg == NULL ) {
        return( false );
    }
    return( EvalCondition( dlg->condition ) );
}

dlg_state DoDialogByPointer( gui_window *parent, a_dialog_header *dlg )
/*********************************************************************/
{
    if( !EvalCondition( dlg->condition ) ) {
        return( dlg->ret_val );
    }
    return( GenericDialog( parent, dlg ) );
}

dlg_state DoDialogWithParent( gui_window *parent, const char *name )
/******************************************************************/
{
    a_dialog_header     *dlg;
    dlg_state           return_state;

    dlg = FindDialogByName( name );
    if( dlg == NULL ) {
        return( DLG_CANCEL );
    }
    return_state = DoDialogByPointer( parent, dlg );
    if( return_state == DLG_CANCEL ) {
        // This block is a kludgy hack which allows a dialog to
        // return DLG_DONE when the ESC key is pressed, if it has a
        // DONE button rather than a CANCEL button.
        int   i;
        bool  can = false;
        bool  done = false;

        for( i = 0; i < dlg->num_controls; i++ ) {
            if( dlg->controls[i].id == CTL_CANCEL ) {
                can = true;
                break;
            }
            if( dlg->controls[i].id == CTL_DONE ) {
                done = true;
            }
        }
        if( !can && done ) {
            return_state = DLG_DONE;
        }
    }
    return( return_state );
}

dlg_state DoDialog( const char *name )
/************************************/
{
    dlg_state result;

    if( SkipDialogs ) {
        result = DLG_NEXT;
    } else {
        result = DoDialogWithParent( NULL, name );
    }
    return result;
}

void FreeDefaultDialogs( void )
/*****************************/
{
    a_dialog_header *dlg;
    a_dialog_header *next;

    for( dlg = FirstDialog; dlg != NULL; dlg = next ) {
        next = dlg->next;
        FreeDialog( dlg );
    }
    FirstDialog = NULL;
    LastDialog = NULL;
}

