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


#include "rcdef.h"
#define G_OPENER                "+"
#define G_CLOSER                "-"
#define G_BREAK                 "!"
#define G_DIMBREAK              "."
#define G_SHAFT                 "="
#define G_POINTS                ">"
#define G_UNPOINTS              "<"
#define G_OSOURCE               "S"
#define G_CSOURCE               "s"
#define G_OASSEMBLY             "A"
#define G_CASSEMBLY             "a"
#define G_READ                  "r"
#define G_WRITE                 "w"
#define G_CODEHERE              " "

#define BRACKET1( x ) "[" x "]"
#define BRACKET2( x ) "|" x "|"

#define ARROW1( x )   "=" x ">"
#define ARROW2( x )   "-" x "-"

#define GADGET_FIRST_PRIMARY GADGET_OPEN
#define GADGET_FIRST_SECONDARY SECONDARY_GADGET_OPEN

pick( GADGET_OPEN, BITMAP_CLOSE1, BITMAP_CLOSE2,
      BRACKET1( G_CLOSER ), BRACKET2( G_CLOSER ),
      MENU_LIT( HELP_GadgetCLOSE ) )
pick( GADGET_CLOSED, BITMAP_OPEN1, BITMAP_OPEN2,
      BRACKET1( G_OPENER ), BRACKET2( G_OPENER ),
      MENU_LIT( HELP_GadgetOPEN ) )
pick( GADGET_POINTS, BITMAP_POINTS1, BITMAP_POINTS2,
      BRACKET1( G_POINTS ), BRACKET2( G_POINTS ),
      MENU_LIT( HELP_GadgetPOINTS ) )
pick( GADGET_NOBREAK, BITMAP_CODEHER1, BITMAP_CODEHER2,
      BRACKET1( G_CODEHERE ), BRACKET2( G_CODEHERE ),
      MENU_LIT( HELP_SetBreak ) )
pick( GADGET_BREAK, BITMAP_BRKHERE1, BITMAP_BRKHERE2,
      BRACKET1( G_BREAK ), BRACKET2( G_BREAK ),
      MENU_LIT( HELP_DisableBreak ) )
pick( GADGET_DIMBREAK, BITMAP_DIMBRK1, BITMAP_DIMBRK2,
      BRACKET1( G_DIMBREAK ), BRACKET2( G_DIMBREAK ),
      MENU_LIT( HELP_RemoveBreak ) )
pick( GADGET_CURRLINE, BITMAP_CURRLIN1, BITMAP_CURRLIN2,
      ARROW1( G_SHAFT ), ARROW2( G_SHAFT ),
      MENU_LIT( HELP_SetBreak ) )
pick( GADGET_CURRBREAK, BITMAP_CURRBRK1, BITMAP_CURRBRK2,
      ARROW1( G_BREAK ), ARROW2( G_BREAK ),
      MENU_LIT( HELP_DisableBreak ) )
pick( GADGET_CURRDIMBREAK, BITMAP_CURRDIM1, BITMAP_CURRDIM2,
      ARROW1( G_DIMBREAK ), ARROW2( G_DIMBREAK ),
      MENU_LIT( HELP_RemoveBreak ) )
pick( GADGET_UNPOINTS, BITMAP_UNPOINT1, BITMAP_UNPOINT2,
      BRACKET1( G_UNPOINTS ), BRACKET2( G_UNPOINTS ),
      MENU_LIT( HELP_GadgetUNPOINTS ) )
pick( GADGET_OPEN_SOURCE, BITMAP_OPENSRC1, BITMAP_OPENSRC2,
      BRACKET1( G_OSOURCE ), BRACKET2( G_OSOURCE ),
      MENU_LIT( HELP_GadgetSOURCE ) )
pick( GADGET_CLOSED_SOURCE, BITMAP_CLSSRC1, BITMAP_CLSSRC2,
      BRACKET1( G_CSOURCE ), BRACKET2( G_CSOURCE ),
      MENU_LIT( HELP_GadgetSOURCE ) )
pick( GADGET_OPEN_ASSEMBLY, BITMAP_OPENASM1, BITMAP_OPENASM2,
      BRACKET1( G_OASSEMBLY ), BRACKET2( G_OASSEMBLY ),
      MENU_LIT( HELP_GadgetASSEMBLY ) )
pick( GADGET_CLOSED_ASSEMBLY, BITMAP_CLSASM1, BITMAP_CLSASM2,
      BRACKET1( G_CASSEMBLY ), BRACKET2( G_CASSEMBLY ),
      MENU_LIT( HELP_GadgetASSEMBLY ) )
pick( GADGET_READ, BITMAP_READ1, BITMAP_READ2,
      BRACKET1( G_READ ), BRACKET2( G_READ ),
      MENU_LIT( HELP_GadgetREAD ) )
pick( GADGET_WRITE, BITMAP_WRITE1, BITMAP_WRITE2,
      BRACKET1( G_WRITE ), BRACKET2( G_WRITE ),
      MENU_LIT( HELP_GadgetWRITE ) )
pick( GADGET_INHERIT_OPEN, BITMAP_ICLOSE1, BITMAP_ICLOSE2,
      BRACKET1( G_CLOSER ), BRACKET2( G_CLOSER ),
      MENU_LIT( HELP_GadgetCLOSE ) )
pick( GADGET_INHERIT_CLOSED, BITMAP_IOPEN1, BITMAP_IOPEN2,
      BRACKET1( G_OPENER ), BRACKET2( G_OPENER ),
      MENU_LIT( HELP_GadgetOPEN ) )
pick( GADGET_BAD_POINTS, BITMAP_BPOINTS1, BITMAP_BPOINTS2,
      BRACKET1( G_POINTS ), BRACKET2( G_POINTS ),
      MENU_LIT( HELP_GadgetPOINTS ) )

#undef pick

#undef pick
