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


// System includes -------------------------------------------------------

#include <wpshbttn.hpp>
#include <wcheckbx.hpp>
#include <wgroupbx.hpp>
#include <weditbox.hpp>
#include <wfiledlg.hpp>

// Project includes ------------------------------------------------------

#include "printdlg.h"
#include "prtfuncs.h"
#include "prtvars.h"
#include "prtclass.h"
#include "prttypes.h"

// Constant definitions --------------------------------------------------

const int PrintDialogWidth = 300;
const int PrintDialogHeight = 150;

// Static member initialization ------------------------------------------

bool PrintDialog::_checked[ PrintDialog::Types + 1 ] =
        { TRUE, TRUE, TRUE, TRUE };

// Methods ---------------------------------------------------------------

PrintDialog::PrintDialog( WWindow * parent, ProjectModel * project,
                          int x, int y, char * title )
/*****************************************************************/

    : WDialog( parent, WRect( x, y, PrintDialogWidth, PrintDialogHeight ),
               title )
    , _project( project )

{
}

void PrintDialog::initialize()
/****************************/
{
    WPushButton * pButton;

    pButton = new WPushButton( this, WRect( -95, 20, 90, 24 ), "&Print" );
    pButton->onClick( this, (cbw) &PrintDialog::printButton );
    pButton->show();

    pButton = new WPushButton( this, WRect( -95, 48, 90, 24 ), "Print To &Disk" );
    pButton->onClick( this, (cbw) &PrintDialog::toDiskButton );
    pButton->show();

    pButton = new WPushButton( this, WRect( -95, 76, 90, 24 ), "&Cancel" );
    pButton->onClick( this, (cbw) &PrintDialog::cancelButton );
    pButton->show();

    _checkFunctions= new WCheckBox( this, WRect( 10, 25, 140, 20 ), "Functions" );
    _checkFunctions->setCheck( _checked[ Functions ]);
    _checkFunctions->show();

    _checkClasses = new WCheckBox( this, WRect( 10, 46, 140, 20 ), "Classes" );
    _checkClasses->setCheck( _checked[ Classes ]);
    _checkClasses->show();

    _checkVariables = new WCheckBox( this, WRect( 10, 67, 140, 20 ), "Variables" );
    _checkVariables->setCheck( _checked[ Variables ]);
    _checkVariables->show();

    _checkTypes = new WCheckBox( this, WRect( 10, 89, 140, 20 ), "Types" );
    _checkTypes->setCheck( _checked[ Types ]);
    _checkTypes->show();

    WGroupBox * pGroupBox;

    pGroupBox = new WGroupBox( this, WRect( 5, 5, 180, 105), "Print Which" );
    pGroupBox->show();

    show();
}

void PrintDialog::toDiskButton( WWindow * )
/*****************************************/
{
    WFileDialog fileDialog( this );

    const char * fileName = fileDialog.getSaveFileName( NULL, "Print To Disk" );
    if( fileName != NULL ) {
        SafeFile file( fileName );
        saveToFile( file );
        show( WWinStateHide );
        setCheckFlags();
        quit( TRUE );
    }
}

void PrintDialog::printButton( WWindow * )
/****************************************/
// Callback for "Print" button
//
{
    SafeFile file( "PRINT.TMP" );

    saveToFile( file );

    show( WWinStateHide );
    setCheckFlags();
    quit( TRUE );
}

void PrintDialog::setCheckFlags()
/*******************************/
{
    _checked[ Functions ] = _checkFunctions->checked();
    _checked[ Classes ] = _checkClasses->checked();
    _checked[ Types ] = _checkTypes->checked();
    _checked[ Variables ] = _checkVariables->checked();
}

void PrintDialog::saveToFile( SafeFile & file )
/*********************************************/
{
    file.safeOpen( SafeOWrite );

    if( _checkFunctions->checked() ) {
        PrintFunctions funcs( _project );
        funcs.printAll( &file );
    }

    if( _checkClasses->checked() ) {
        PrintClasses classes( _project );
        classes.printAll( &file );
    }

    if( _checkVariables->checked() ) {
        PrintVariables vars( _project );
        vars.printAll( &file );
    }

    if( _checkTypes->checked() ) {
        PrintTypes types( _project );
        types.printAll( &file );
    }

    file.safeClose();
}

void PrintDialog::cancelButton( WWindow * )
/*****************************************/
// Callback for "Cancel" button
//
{
    show( WWinStateHide );
    quit( TRUE );
}

