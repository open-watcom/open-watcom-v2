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


#include "wsyshelp.hpp"
#include "wbrw.h"

#define BRH_SELECT_ROOTS        HLP_MANIPULATING_THE_TREE_VIEWS
#define BRH_QUERY_CONFIGURATION HLP_GLOBAL_SYMBOL_QUERIES
#define BRH_EDIT_MODULE_LIST    HLP_WORKING_WITH_BROWSER_MODULE_FILES
#define BRH_REGULAR_EXPRESSIONS HLP_REGULAR_EXPRESSIONS_FOR_FIND_AND_QUERY
#define BRH_INHERITANCE_OPTIONS HLP_DEFINING_GRAPH_VIEW_LEGENDS
#define BRH_CALL_TREE_OPTIONS   HLP_DEFINING_GRAPH_VIEW_LEGENDS
#define BRH_LINE_OPTIONS        HLP_DEFINING_GRAPH_VIEW_LEGENDS
#define BRH_REPORT_OPTIONS      0       // NYI
#define BRH_FILE_FILTER         HLP_GLOBAL_SYMBOL_QUERIES
#define BRH_QUERY_FILTER        HLP_GLOBAL_SYMBOL_QUERIES

#define BRH_LIST_VIEW           HLP_THE_LIST_VIEW
#define BRH_INHERITANCE_VIEW    HLP_THE_CLASS_INHERITANCE_VIEW
#define BRH_CALL_TREE_VIEW      HLP_THE_FUNCTION_CALL_TREE_VIEW
#define BRH_DETAIL_VIEW         HLP_VIEWING_DETAIL_INFORMATION
#define BRH_SOURCE_REFERENCES   HLP_SOURCE_REFERENCES
#define BRH_SYMBOL_REFERENCES   HLP_SYMBOL_REFERENCES
#define BRH_ENUMERATOR_STYLES   HLP_ENUMERATOR_STYLES
#define BRH_MEMBER_FILTERS      HLP_MEMBER_FILTERS_FOR_CLASSES
#define BRH_NEW_DATABASE_FILE   HLP_CREATING_A_BROWSER_DATABASE_FILE_WITHIN_THE_BROWSER
