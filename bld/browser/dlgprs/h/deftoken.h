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


/* define the tokens for the scanner.  These must be sorted by name. */

/* :%s/^\#define[ \t]T_([a-zA-Z0-9]*)[ \t]+.*$/DEFTOKEN( "\u\1", \t\tT_\1 )/ */
/* !G sort */

DEFTOKEN( "BEGIN",              T_Begin )
DEFTOKEN( "CAPTION",            T_Caption )
DEFTOKEN( "CHECKBOX",           T_CheckBox )
DEFTOKEN( "COMBOBOX",           T_ComboBox )
DEFTOKEN( "CONTROL",            T_Control )
DEFTOKEN( "CTEXT",              T_CText )
DEFTOKEN( "DEFPUSHBUTTON",      T_DefPushButton )
DEFTOKEN( "DIALOG",             T_Dialog )
DEFTOKEN( "DISCARDABLE",        T_Discardable )
DEFTOKEN( "DLGINCLUDE",         T_DialogInclude )
DEFTOKEN( "EDITTEXT",           T_EditText )
DEFTOKEN( "END",                T_End )
DEFTOKEN( "FIXED",              T_Fixed )
DEFTOKEN( "FONT",               T_Font )
DEFTOKEN( "GROUPBOX",           T_GroupBox )
DEFTOKEN( "ICON",               T_Icon )
DEFTOKEN( "IMPURE",             T_Impure )
DEFTOKEN( "LISTBOX",            T_ListBox )
DEFTOKEN( "LTEXT",              T_LText )
DEFTOKEN( "MOVEABLE",           T_Moveable )
DEFTOKEN( "PRELOAD",            T_Preload )
DEFTOKEN( "PUSHBUTTON",         T_PushButton )
DEFTOKEN( "RADIOBUTTON",        T_RadioButton )
DEFTOKEN( "RCDATA",             T_RCData )
DEFTOKEN( "RTEXT",              T_RText )
DEFTOKEN( "SCROLLBAR",          T_ScrollBar )
DEFTOKEN( "STYLE",              T_Style )
DEFTOKEN( "class",              T_Class )
