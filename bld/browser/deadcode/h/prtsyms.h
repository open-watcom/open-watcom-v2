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


#ifndef _PRINT_SYMBOLS_H
#define _PRINT_SYMBOLS_H

#include "strtable.h"
#include "projmodl.h"
#include "keysym.h"
#include "safefile.h"

typedef void (WObject::* CBPrintSymbol) ( Symbol * );
typedef void (WObject::* CBPrintHeading) ( SafeFile & );

/*----------------------------------------------------------------------
  A virtual base class for printing sets of symbols from the browser.
  ----------------------------------------------------------------------*/
class PrintSymbols {
public:
                        PrintSymbols( ProjectModel * project );
                        ~PrintSymbols();

  /*--------------------------------------------------------------------
      Defines a different method than the default one that will print
      a single symbol out to a file.
    --------------------------------------------------------------------*/
    virtual void        setPrintCallbacks( WObject * client,
                                           CBPrintSymbol cbPrintSymbol,
                                           CBPrintHeading cbPrintHeading );

  /*--------------------------------------------------------------------
      Prints all the symbols out to the printer, or to a file if one
      is specified.
    --------------------------------------------------------------------*/
    virtual void        printAll( SafeFile * = NULL );

  /*--------------------------------------------------------------------
      Prints the symbols given in the list to the printer, or to a file
      if one is specified.
    --------------------------------------------------------------------*/
    virtual void        printList( WVList &, SafeFile * = NULL );

protected:
  /*--------------------------------------------------------------------
      The default print method for a symbol.
    --------------------------------------------------------------------*/
    virtual void        printSymbol( Symbol * pSym );

  /*--------------------------------------------------------------------
      Print the headings for the symbol
    --------------------------------------------------------------------*/
    virtual void        printHeading( SafeFile & file ) = 0;

    StringTable         _strTable;
    ProjectModel *      _project;
    WVList              _symbols;
    CBPrintSymbol       _cbPrintSymbol;
    CBPrintHeading      _cbPrintHeading;
    KeySymbol           _key;
    WObject *           _client;

private:
};

#endif
