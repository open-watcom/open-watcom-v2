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

#include <wstring.hpp>

// Project includes ------------------------------------------------------

#include "strtable.h"
#include "referenc.h"
#include "prtsyms.h"

// Constant definitions --------------------------------------------------

// Methods ---------------------------------------------------------------

PrintSymbols::PrintSymbols( ProjectModel * project )
    : _project( project )
    , _cbPrintSymbol( NULL )
    , _cbPrintHeading( NULL )
    , _client( NULL )
    , _strTable( 3, 39, 1, 39 )
/**************************************************/
// - uses default methods for printing symbols and heading
// - defines the string table as having 3 cols of width 39, 1, and 39 chars
{
}

PrintSymbols::~PrintSymbols()
/*******************************/
{
    _symbols.deleteContents();
}

void PrintSymbols::setPrintCallbacks( WObject * client,
                                      CBPrintSymbol cbPrintSymbol,
                                      CBPrintHeading cbPrintHeading )
/*******************************************************************/
// Set the callbacks used to print each symbol and the heading.
// If NULL, then the default method is used.
{
    _client = client;
    _cbPrintSymbol = cbPrintSymbol;
    _cbPrintHeading = cbPrintHeading;
}

void PrintSymbols::printAll( SafeFile * pFile )
/***********************************************/
// If pFile is NULL, then we print to the printer.
//
// If pFile is not NULL, then pFile must be a pointer to a SafeFile which
// is opened for writing; the information is written to the file.  The file
// is not closed.
{
    _project->findSymbols( &_symbols, &_key );
    printList( _symbols, pFile );

    if( pFile != NULL ) {
        WString tmp( "" );
        pFile->safePuts( tmp );
    }
}

void PrintSymbols::printList( WVList & symList, SafeFile * pFile )
/******************************************************************/
{
    int i;
    bool printToDisk;

    printToDisk = ( pFile != NULL );
    if( !printToDisk ) {
        pFile = new SafeFile( "PRINT.TMP" );
        pFile->safeOpen( SafeOWrite );
    }

    _strTable.setOutputCallback( pFile, (CBStringTable) SafeFile::safePuts );

    if( _cbPrintHeading == NULL ) {
        printHeading( *pFile );
    } else {
        (_client->*_cbPrintHeading)( *pFile );
    }

    _project->findSymbols( &_symbols, &_key );
    if( _cbPrintSymbol == NULL ) {
        for( i = 0; i < symList.count(); i += 1 ) {
            printSymbol( (Symbol *) _symbols[ i ]);
        }
    } else {
        for( i = 0; i < symList.count(); i += 1 ) {
            (_client->*_cbPrintSymbol)( (Symbol *) _symbols[ i ]);
        }
    }

    if( !printToDisk ) {
        pFile->safeClose();
        delete pFile;
    }
}

void PrintSymbols::printSymbol( Symbol * pSym )
/*********************************************/
{
    WString tmpStr;
    WVList users;
    char * symName;

    _project->findRefSyms( &users, pSym );

    symName = pSym->scopedName();

    if( users.count() > 0 ) {
        int i;

        _strTable.outputRow( symName, EmptyString,
                             ( (Symbol *) users[ 0 ])->scopedName() );
        for( i = 1; i < users.count(); i += 1 ) {
            _strTable.outputRow( EmptyString, EmptyString,
                                 ( (Symbol *) users[ i ])->scopedName() );
        }
    } else {
        _strTable.outputRow( symName, EmptyString, EmptyString );
    }

    users.deleteContents();
}

