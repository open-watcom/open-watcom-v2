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

#include "chfile.h"
#include "dialog.h"
#include "bind.h"

Dialog * CurrDialog = NULL;
Binding * CurrBinding = NULL;

int main( int argc, char * argv[] ) {

try {

    if( argc != 5 ) {
        printf( "Usage: prsdlg <dlg file> <bnd file> <output header> <output source>\n" );
        return 0;
    }

    DialogParser dlgparse( argv[1] );
    dlgparse.yyparse();

    if( CurrDialog ) {
        BindingParser bndparse( argv[2] );
        bndparse.yyparse();
    }

    if( CurrDialog && CurrBinding ) {
        CurrBinding->bind( CurrDialog, argv[3], argv[4] );
    }

} catch( FileExcept oops ) {
    static const char * act[] = { "open", "close", "read", "write", "seek", "tell", "stat" };

    printf( "Error %sing %s: %d -- %s\n", act[ oops._action ], oops._fileName, oops._error, oops._message );
}
    return 0;
}

