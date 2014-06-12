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
* Description:  Windows Resource Compiler grammar for yacc.
*
*/

/***************************************************************************
 * As of Nov 25/94 this file contains 13 shift/reduce conflicts
 *      -3 involving Y_MINUS
 *      -5 involving Y_LANGUAGE
 *      -5 involving Y_CHARACTERISTICS
 ***************************************************************************/

/*** error tokens ***/
%token Y_SCAN_ERROR
/*** non-keyword tokens ***/
%token <string>     Y_NAME
%token <intinfo>    Y_INTEGER
%token <string>     Y_STRING
%token <string>     Y_DOS_FILENAME
/*** operator tokens ***/
%token Y_POUND_SIGN                 /* this token is intercepted by yylex */
%token Y_LPAREN
%token Y_RPAREN
%token Y_LBRACE
%token Y_RBRACE
%token Y_LSQ_BRACKET
%token Y_RSQ_BRACKET
%token Y_PLUS
%token Y_MINUS
%token Y_BITNOT
%token Y_NOT
%token Y_TIMES
%token Y_DIVIDE
%token Y_MOD
%token Y_SHIFTL
%token Y_SHIFTR
%token Y_GT
%token Y_LT
%token Y_GE
%token Y_LE
%token Y_EQ
%token Y_SINGLE_EQ
%token Y_NE
%token Y_BITAND
%token Y_BITXOR
%token Y_BITOR
%token Y_AND
%token Y_OR
%token Y_QUESTION
%token Y_COLON
%token Y_COMMA
/*** keywords tokens ***/
%token Y_ACCELERATORS
%token Y_ALT
%token Y_ASCII
%token Y_AUTOCHECKBOX
%token Y_AUTORADIOBUTTON
%token Y_AUTO3STATE
%token Y_BEGIN
%token Y_BITMAP
%token Y_BLOCK
%token Y_BUTTON
%token Y_CAPTION
%token Y_CHARACTERISTICS
%token Y_CHECKBOX
%token Y_CHECKED
%token Y_CLASS
%token Y_CODEPAGE
%token Y_COMBOBOX
%token Y_CONTROL
%token Y_CTEXT
%token Y_CURSOR
%token Y_DEFPUSHBUTTON
%token Y_DIALOG
%token Y_DIALOG_EX
%token Y_DISCARDABLE
%token Y_DLGINIT
%token Y_EDIT
%token Y_EDITTEXT
%token Y_END
%token Y_ERRTABLE
%token Y_EXSTYLE
%token Y_FILEFLAGS
%token Y_FILEFLAGSMASK
%token Y_FILEOS
%token Y_FILESUBTYPE
%token Y_FILETYPE
%token Y_FILEVERSION
%token Y_FIXED
%token Y_FONT
%token Y_GRAYED
%token Y_GROUPBOX
%token Y_HELP
%token Y_ICON
%token Y_IMPURE
%token Y_INACTIVE
%token Y_INCLUDERES
%token Y_LANGUAGE
%token Y_LISTBOX
%token Y_LOADONCALL
%token Y_LTEXT
%token Y_MENU
%token Y_MENU_EX
%token Y_MENUBARBREAK
%token Y_MENUBREAK
%token Y_MENUITEM
%token Y_MESSAGETABLE
%token Y_MOVEABLE
%token Y_NOINVERT
%token Y_OWNERDRAW
%token Y_POPUP
%token Y_POUND_PRAGMA
%token Y_PRELOAD
%token Y_PRODUCTVERSION
%token Y_PURE
%token Y_PUSHBUTTON
%token Y_RADIOBUTTON
%token Y_RCDATA
%token Y_RCINCLUDE          /* This is an old keyword that should no */
                            /* longer be used, but is used in the sample files*/
                            /* so may be used in some old .RC files. */
                            /* It is intercepted by the scanner */
%token Y_RTEXT
%token Y_SCROLLBAR
%token Y_SEPARATOR
%token Y_SHIFT
%token Y_STATE3
%token Y_STATIC
%token Y_STRINGTABLE
%token Y_STYLE
%token Y_TOOLBAR
%token Y_VALUE
%token Y_VERSION
%token Y_VERSIONINFO
%token Y_VIRTKEY

%type <maskint>         constant-expression
%type <maskint>         conditional-exp
%type <maskint>         log-or-exp
%type <maskint>         log-and-exp
%type <maskint>         bit-or-exp
%type <maskint>         bit-xor-exp
%type <maskint>         bit-and-exp
%type <maskint>         equality-exp
%type <maskint>         relational-exp
%type <maskint>         shift-exp
%type <maskint>         additive-exp
%type <maskint>         multiplicative-exp
%type <maskint>         unary-exp
%type <maskint>         primary-exp
%type <token>           keyword-name
%type <resid>           name-id
%type <resid>           type-id
%type <resid>           user-defined-type-id
%type <accflags>        acc-item-option
%type <accflags>        acc-item-options
%type <accelfullentry>  acc-item
%type <accelfullentry>  acc-items
%type <resloc>          acc-section
%type <integral>        id-value
%type <accevent>        event
%type <fullmemflags>    resource-options
%type <token>           resource-option
%type <string>          file-name
%type <integral>        fontitalic
%type <integral>        fontweight
%type <integral>        fontextra
%type <menuflags>       menu-item-options
%type <token>           menu-item-option
%type <menuptr>         menu-section
%type <menuptr>         menu-items
%type <menuitem>        menu-item
%type <popupmenuitem>   menu-popup-stmt
%type <normalmenuitem>  menu-entry-stmt
%type <normalmenuitem>  menu-entry-defn
%type <string>          menu-text
%type <integral>        menu-result
%type <langinfo>        language-stmt
%type <integral>        size-x
%type <integral>        size-y
%type <integral>        size-w
%type <integral>        size-h
%type <sizeinfo>        size-info
%type <string>          string-constant
%type <string>          string-group
%type <maskint>         style
%type <maskint>         exstyle
%type <diagopts>        style-stmt
%type <diagopts>        exstyle-stmt
%type <diagopts>        exstyle-equal-stmt
%type <diagopts>        caption-stmt
%type <diagopts>        menu-stmt
%type <diagopts>        class-stmt
%type <diagopts>        font-stmt
%type <diagopts>        diag-lang-stmt
%type <diagopts>        diag-options-stmt
%type <diagopts>        diag-version-stmt
%type <diagopts>        diag-characteristics-stmt
%type <token>           dialog-or-dialogEx
%type <dataelem>        diag-data-elements
%type <nameorord>       class-name
%type <nameorord>       ctl-class-name
%type <integral>        point-size
%type <string>          typeface
%type <diaghead>        diag-options-section
%type <diagctrllist>    diag-control-section
%type <diagctrllist>    diag-control-stmts
%type <diagctrl>        diag-control-stmt
%type <diagctrlopts>    cntl-text-options
%type <diagctrlopts>    cntl-options
%type <diagctrl>        ltext-stmt
%type <diagctrl>        rtext-stmt
%type <diagctrl>        ctext-stmt
%type <diagctrl>        autocheckbox-stmt
%type <diagctrl>        autoradiobutton-stmt
%type <diagctrl>        auto3state-stmt
%type <diagctrl>        checkbox-stmt
%type <diagctrl>        pushbutton-stmt
%type <diagctrl>        listbox-stmt
%type <diagctrl>        groupbox-stmt
%type <diagctrl>        defpushbutton-stmt
%type <diagctrl>        radiobutton-stmt
%type <diagctrl>        edittext-stmt
%type <diagctrl>        combobox-stmt
%type <diagctrl>        icon-stmt
%type <diagctrl>        scrollbar-stmt
%type <diagctrl>        control-stmt
%type <diagctrl>        state3-stmt
%type <nameorord>       icon-name
%type <diagctrlopts>    icon-parms
%type <integral>        cntl-id
%type <resid>           cntl-text
%type <integral>        string-id
%type <stritem>         string-item
%type <strtable>        string-items
%type <strtable>        string-section
%type <rawitem>         raw-data-item
%type <resloc>          user-defined-data
%type <token>           resource-type
%type <verinforoot>     fixed-ver-section
%type <verfixedoption>  fixed-ver-stmt
%type <verfixedoption>  fileversion-stmt
%type <verfixedoption>  productversion-stmt
%type <verfixedoption>  fileflagsmask-stmt
%type <verfixedoption>  fileflags-stmt
%type <verfixedoption>  fileos-stmt
%type <verfixedoption>  filetype-stmt
%type <verfixedoption>  filesubtype-stmt
%type <verfixedoption>  version
%type <verpair>         version-pair
%type <verblocknest>    variable-ver-section
%type <verblocknest>    block-stmts
%type <verblock>        block-stmt
%type <string>          block-name
%type <verblocknest>    block-body
%type <verblocknest>    value-stmts
%type <verblock>        value-stmt
%type <string>          value-name
%type <valuelist>       value-list
%type <valueitem>       value-item
%type <valueitem>       value-string-list
%type <dataelem>        raw-data-section
%type <dataelem>        raw-data-items
%type <dlghelpid>       helpId-opt
%type <integral>        menuId
%type <integral>        menuType
%type <integral>        menuState
%type <integral>        helpId
%type <toolbar>         toolbar-block
%type <toolbar>         toolbar-items
%type <integral>        toolbar-item

%start goal-symbol

%%

goal-symbol
    : resources
    | /* nothing */
    ;

resources
    : resource
    | resources resource
    ;

resource
    : normal-resource
    | string-table-resource
    | error-table-resource
    | pragma-statment
    | includeres-statement
    ;

normal-resource
    : single-line-resource
    | rcdata-resource
    | user-defined-resource
    | accelerators-resource
    | menu-resource
    | menuex-resource
    | dlg-resource
    | version-info-resource
    | language-resource
    | characteristics-stmt
    | version-stmt
    | message-table-resource
    | toolbar-resource
    ;

name-id
    : Y_NAME
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | string-constant
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | constant-expression
        {
            $$ = WResIDFromNum( $1.Value );
            if( $$ == NULL ) {
                $$ = WResIDFromNum( 0 );
                RcError( ERR_BAD_RES_ID, $1.Value );
                ErrorHasOccured = TRUE;
            }
        }
    | keyword-name
        { $$ = WResIDFromStr( SemWINTokenToString( $1 ) ); }
    ;

type-id
    : Y_NAME
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | string-constant
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | constant-expression
        {
            $$ = WResIDFromNum( $1.Value );
            if( $$ == NULL ) {
                $$ = WResIDFromNum( 0 );
                RcError( ERR_BAD_RES_TYPE, $1.Value );
                ErrorHasOccured = TRUE;
            }
        }
    ;

includeres-statement
    : Y_INCLUDERES file-name
        {
            SemWINAddResFile( $2.string );
        }
    ;

pragma-statment
    : Y_POUND_PRAGMA Y_CODEPAGE  Y_LPAREN constant-expression Y_RPAREN
      {}
    ;

keyword-name
    : Y_ACCELERATORS
        { $$ = Y_ACCELERATORS; }
    | Y_ALT
        { $$ = Y_ALT; }
    | Y_ASCII
        { $$ = Y_ASCII; }
    | Y_AUTOCHECKBOX
        { $$ = Y_AUTOCHECKBOX; }
    | Y_AUTORADIOBUTTON
        { $$ = Y_AUTORADIOBUTTON; }
    | Y_AUTO3STATE
        { $$ = Y_AUTO3STATE; }
    | Y_BEGIN
        { $$ = Y_BEGIN; }
    | Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_BLOCK
        { $$ = Y_BLOCK; }
    | Y_BUTTON
        { $$ = Y_BUTTON; }
    | Y_CAPTION
        { $$ = Y_CAPTION; }
    | Y_CHARACTERISTICS
        { $$ = Y_CHARACTERISTICS; }
    | Y_CHECKBOX
        { $$ = Y_CHECKBOX; }
    | Y_CHECKED
        { $$ = Y_CHECKED; }
    | Y_CLASS
        { $$ = Y_CLASS; }
    | Y_COMBOBOX
        { $$ = Y_COMBOBOX; }
    | Y_CODEPAGE
        { $$ = Y_CODEPAGE; }
    | Y_CONTROL
        { $$ = Y_CONTROL; }
    | Y_CTEXT
        { $$ = Y_CTEXT; }
    | Y_CURSOR
        { $$ = Y_CURSOR; }
    | Y_DEFPUSHBUTTON
        { $$ = Y_DEFPUSHBUTTON; }
    | Y_DIALOG
        { $$ = Y_DIALOG; }
    | Y_DIALOG_EX
        { $$ = Y_DIALOG_EX; }
    | Y_DISCARDABLE
        { $$ = Y_DISCARDABLE; }
    | Y_DLGINIT
        { $$ = Y_DLGINIT; }
    | Y_EDIT
        { $$ = Y_EDIT; }
    | Y_EDITTEXT
        { $$ = Y_EDITTEXT; }
    | Y_END
        { $$ = Y_END; }
    | Y_ERRTABLE
        { $$ = Y_ERRTABLE; }
    | Y_EXSTYLE
        { $$ = Y_EXSTYLE; }
    | Y_FILEFLAGS
        { $$ = Y_FILEFLAGS; }
    | Y_FILEFLAGSMASK
        { $$ = Y_FILEFLAGSMASK; }
    | Y_FILEOS
        { $$ = Y_FILEOS; }
    | Y_FILESUBTYPE
        { $$ = Y_FILESUBTYPE; }
    | Y_FILETYPE
        { $$ = Y_FILETYPE; }
    | Y_FILEVERSION
        { $$ = Y_FILEVERSION; }
    | Y_FIXED
        { $$ = Y_FIXED; }
    | Y_FONT
        { $$ = Y_FONT; }
    | Y_GRAYED
        { $$ = Y_GRAYED; }
    | Y_GROUPBOX
        { $$ = Y_GROUPBOX; }
    | Y_HELP
        { $$ = Y_HELP; }
    | Y_ICON
        { $$ = Y_ICON; }
    | Y_IMPURE
        { $$ = Y_IMPURE; }
    | Y_INACTIVE
        { $$ = Y_INACTIVE; }
    | Y_LANGUAGE
        { $$ = Y_LANGUAGE; }
    | Y_LISTBOX
        { $$ = Y_LISTBOX; }
    | Y_LOADONCALL
        { $$ = Y_LOADONCALL; }
    | Y_LTEXT
        { $$ = Y_LTEXT; }
    | Y_MENU
        { $$ = Y_MENU; }
    | Y_MENU_EX
        { $$ = Y_MENU_EX; }
    | Y_MENUBARBREAK
        { $$ = Y_MENUBARBREAK; }
    | Y_MENUBREAK
        { $$ = Y_MENUBREAK; }
    | Y_MENUITEM
        { $$ = Y_MENUITEM; }
    | Y_MESSAGETABLE
        { $$ = Y_MESSAGETABLE; }
    | Y_MOVEABLE
        { $$ = Y_MOVEABLE; }
    | Y_NOINVERT
        { $$ = Y_NOINVERT; }
    | Y_OWNERDRAW
        { $$ = Y_OWNERDRAW; }
    | Y_POPUP
        { $$ = Y_POPUP; }
    | Y_PRELOAD
        { $$ = Y_PRELOAD; }
    | Y_PRODUCTVERSION
        { $$ = Y_PRODUCTVERSION; }
    | Y_PURE
        { $$ = Y_PURE; }
    | Y_PUSHBUTTON
        { $$ = Y_PUSHBUTTON; }
    | Y_RADIOBUTTON
        { $$ = Y_RADIOBUTTON; }
    | Y_RCDATA
        { $$ = Y_RCDATA; }
    | Y_RCINCLUDE
        { $$ = Y_RCINCLUDE; }
    | Y_RTEXT
        { $$ = Y_RTEXT; }
    | Y_SCROLLBAR
        { $$ = Y_SCROLLBAR; }
    | Y_SEPARATOR
        { $$ = Y_SEPARATOR; }
    | Y_SHIFT
        { $$ = Y_SHIFT; }
    | Y_STATIC
        { $$ = Y_STATIC; }
    | Y_STATE3
        { $$ = Y_STATE3; }
    | Y_STRINGTABLE
        { $$ = Y_STRINGTABLE; }
    | Y_STYLE
        { $$ = Y_STYLE; }
    | Y_TOOLBAR
        { $$ = Y_TOOLBAR; }
    | Y_VALUE
        { $$ = Y_VALUE; }
    | Y_VERSIONINFO
        { $$ = Y_VERSIONINFO; }
    | Y_VIRTKEY
        { $$ = Y_VIRTKEY; }
    ;

comma-opt
    : Y_COMMA
    | /* nothing */
    ;

single-line-resource
    : name-id resource-type file-name
        { SemWINAddSingleLineResource( $1, $2, NULL, $3.string ); }
    | name-id resource-type resource-options file-name
        { SemWINAddSingleLineResource( $1, $2, &($3), $4.string ); }
    ;

resource-options
    : resource-option
        { $$ = SemWINAddFirstMemOption( $1 ); }
    | resource-options resource-option
        { $$ = SemWINAddMemOption( $1, $2 ); }
    ;

resource-option
    : Y_PRELOAD
        { $$ = Y_PRELOAD; }
    | Y_LOADONCALL
        { $$ = Y_LOADONCALL; }
    | Y_FIXED
        { $$ = Y_FIXED; }
    | Y_MOVEABLE
        { $$ = Y_MOVEABLE; }
    | Y_PURE
        { $$ = Y_PURE; }
    | Y_IMPURE
        { $$ = Y_IMPURE; }
    | Y_DISCARDABLE
        { $$ = Y_DISCARDABLE; }
    ;

file-name
    : Y_NAME
    | Y_DOS_FILENAME
    | string-constant
    ;

resource-type
    : Y_CURSOR
        { $$ = Y_CURSOR; }
    | Y_ICON
        { $$ = Y_ICON; }
    | Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_FONT
        { $$ = Y_FONT; }
    ;

user-defined-resource
    : name-id comma-opt user-defined-type-id user-defined-data
        {
            SemAddResourceFree( $1, $3,
                    MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, $4 );
        }
    | name-id comma-opt user-defined-type-id resource-options user-defined-data
        {
            SemWINCheckMemFlags( &($4), 0, MEMFLAG_DISCARDABLE|MEMFLAG_MOVEABLE,
                    MEMFLAG_PURE );
            SemAddResourceFree( $1, $3, $4.flags, $5 );
        }
    ;

user-defined-type-id
    :type-id
        { $$ = $1; }
    |Y_DLGINIT
        { $$ = WResIDFromNum( 0xF0 ); }
    ;

user-defined-data
    : file-name
        { $$ = SemCopyRawFile( $1.string ); RcMemFree( $1.string ); }
    | raw-data-section
        { $$ = SemFlushDataElemList( $1, TRUE ); }
    ;

raw-data-section
    : Y_BEGIN raw-data-items Y_END
       { $$ = $2; }
    | Y_LBRACE raw-data-items Y_RBRACE
       { $$ = $2; }
    ;

raw-data-items
    : raw-data-item
        { $$ = SemNewDataElemList( $1 ); }
    | raw-data-items raw-data-item
        { $$ = SemAppendDataElem( $1, $2 ); }
    | raw-data-items Y_COMMA
        { $$ = $1; }
    ;

raw-data-item
    : string-constant
        {
            $$.IsString = TRUE;
            $$.TmpStr = TRUE;
            $$.StrLen = $1.length;
            $$.Item.String = $1.string;
            $$.LongItem = $1.lstring;
        }
    | constant-expression
        { $$.IsString = FALSE; $$.Item.Num = $1.Value; $$.LongItem = $1.longVal; }
    ;

rcdata-resource
    : name-id Y_RCDATA opt-resource-info-stmts user-defined-data
        {
            SemAddResourceFree( $1, WResIDFromNum( (long)RT_RCDATA ),
                    MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, $4 );
        }
    | name-id Y_RCDATA resource-options opt-resource-info-stmts user-defined-data
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_DISCARDABLE|MEMFLAG_MOVEABLE,
                    MEMFLAG_PURE );
            SemAddResourceFree( $1, WResIDFromNum( (long)RT_RCDATA ), $3.flags, $5 );
        }
    ;

string-table-resource
    : Y_STRINGTABLE opt-resource-info-stmts string-section
        {
            SemWINMergeStrTable( $3,
                MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE );
        }
    | Y_STRINGTABLE resource-options opt-resource-info-stmts string-section
        {
            SemWINCheckMemFlags( &($2), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINMergeStrTable( $4, $2.flags );
        }
    ;

toolbar-resource
    : name-id Y_TOOLBAR resource-options constant-expression comma-opt
      constant-expression toolbar-block
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            SemWINWriteToolBar( $1, $7, $4.Value, $6.Value, $3.flags );
        }
    | name-id Y_TOOLBAR constant-expression comma-opt
      constant-expression toolbar-block
        {
            SemWINWriteToolBar( $1, $6, $3.Value, $5.Value,
                                MEMFLAG_PURE | MEMFLAG_MOVEABLE );
        }
    ;

toolbar-block
    : Y_BEGIN toolbar-items Y_END
        {
            $$ = $2;
        }
    | Y_LBRACE toolbar-items Y_RBRACE
        {
            $$ = $2;
        }
    ;

toolbar-items
    : toolbar-item
        {
            $$ = SemWINCreateToolBar();
            SemWINAddToolBarItem( $$, $1 );
        }
    | toolbar-items toolbar-item
        {
            $$ = $1;
            SemWINAddToolBarItem( $$, $2 );
        }
    ;

toolbar-item
    : Y_BUTTON constant-expression
        {
            $$ = $2.Value;
        }
    | Y_SEPARATOR
        {
            $$ = 0;
        }
    ;

message-table-resource
    : name-id Y_MESSAGETABLE file-name
        {
            SemWINAddMessageTable( $1, &$3 );
        }
    ;


error-table-resource
    : Y_ERRTABLE opt-resource-info-stmts string-section
        {
            SemWINMergeErrTable( $3,
                MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE );
        }
    | Y_ERRTABLE resource-options opt-resource-info-stmts string-section
        {
            SemWINCheckMemFlags( &($2), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINMergeErrTable( $4, $2.flags );
        }
    ;

string-section
    : Y_BEGIN string-items Y_END
        { $$ = $2; }
    | Y_LBRACE string-items Y_RBRACE
        { $$ = $2; }
    ;

string-items
    : string-item
        {
            $$ = SemWINNewStringTable();
            SemWINAddStrToStringTable( $$, $1.ItemID, $1.String );
            RcMemFree( $1.String );
        }
    | string-items string-item
        {
            SemWINAddStrToStringTable( $1, $2.ItemID, $2.String );
            $$ = $1;
            RcMemFree( $2.String );
        }
    ;

string-item
    : string-id comma-opt string-constant
        { $$.ItemID = $1; $$.String = $3.string; }
    ;

string-id
    : constant-expression
        { $$ = $1.Value; }
    ;

accelerators-resource
    : name-id Y_ACCELERATORS opt-resource-info-stmts acc-section
        {
            SemAddResourceFree( $1, WResIDFromNum( (long)RT_ACCELERATOR ),
                MEMFLAG_PURE | MEMFLAG_MOVEABLE, $4 );
        }
    | name-id Y_ACCELERATORS resource-options opt-resource-info-stmts acc-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            SemAddResourceFree( $1, WResIDFromNum( (long)RT_ACCELERATOR ),
                    $3.flags, $5 );
        }
    ;

acc-section
    : Y_BEGIN acc-items Y_END
        {
            SemWINWriteLastAccelEntry( $2 );
            $$.start = $2.startoftable;
            $$.len = SemEndResource( $2.startoftable );
        }
    | Y_LBRACE acc-items Y_RBRACE
        {
            SemWINWriteLastAccelEntry( $2 );
            $$.start = $2.startoftable;
            $$.len = SemEndResource( $2.startoftable );
        }
    ;

acc-items
    : acc-item
        { $$ = $1; $$.startoftable = SemStartResource(); }
    | acc-items acc-item
        {
            SemWINWriteAccelEntry( $1 );
            $$ = $2;
            $$.startoftable = $1.startoftable;
        }
    ;

acc-item
    : event comma-opt id-value comma-opt acc-item-options
        { $$ = SemWINMakeAccItem( $1, $3, $5 ); }
    | event comma-opt id-value
        { $$ = SemWINMakeAccItem( $1, $3, DefaultAccelFlags ); }
    ;

event
    : string-constant
        {
            $$.event = SemWINStrToAccelEvent( $1.string );
            $$.strevent = TRUE;
            RcMemFree( $1.string );
        }
    | constant-expression
        {
            $$.event = $1.Value;
            $$.strevent = FALSE;
        }
    ;

id-value
    : constant-expression
        { $$ = $1.Value; }
    ;

acc-item-options
    : acc-item-option
    | acc-item-options comma-opt acc-item-option
        {
            if ($3.flags == ACCEL_ASCII) {
                $$.flags = $1.flags & ~ACCEL_VIRTKEY;
            } else {
                $$.flags = $1.flags | $3.flags;
            }
            $$.typegiven = $1.typegiven || $3.typegiven;
        }
    ;

acc-item-option
    : Y_NOINVERT
        { $$.flags = ACCEL_NOINVERT; $$.typegiven = FALSE; }
    | Y_ALT
        { $$.flags = ACCEL_ALT; $$.typegiven = FALSE; }
    | Y_SHIFT
        { $$.flags = ACCEL_SHIFT; $$.typegiven = FALSE; }
    | Y_CONTROL
        { $$.flags = ACCEL_CONTROL; $$.typegiven = FALSE; }
    | Y_ASCII
        { $$.flags = ACCEL_ASCII; $$.typegiven = TRUE; }
    | Y_VIRTKEY
        { $$.flags = ACCEL_VIRTKEY; $$.typegiven = TRUE; }
    ;

menuex-resource
    : name-id Y_MENU_EX menu-section
        { SemWINWriteMenu( $1, MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                    $3, Y_MENU_EX ); }
    | name-id Y_MENU_EX resource-options menu-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteMenu( $1, $3.flags, $4, Y_MENU_EX );
        }
    ;

menu-resource
    : name-id Y_MENU opt-resource-info-stmts menu-section
        { SemWINWriteMenu( $1, MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                    $4, Y_MENU ); }
    | name-id Y_MENU resource-options opt-resource-info-stmts menu-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteMenu( $1, $3.flags, $5, Y_MENU );
        }
    ;

menu-section
    : Y_BEGIN menu-items Y_END
        { $$ = $2; }
    | Y_LBRACE menu-items Y_RBRACE
        { $$ = $2; }
    ;

menu-items
    : menu-item
        { $$ = SemWINNewMenu( $1 ); }
    | menu-items menu-item
        { $$ = SemWINAddMenuItem( $1, $2 ); }
    ;

menuId
    : constant-expression
      { $$ = $1.Value; }
    ;

menuType
    : constant-expression
      { $$ = $1.Value; }
    ;

menuState
    : constant-expression
      { $$ = $1.Value; }
    ;

helpId
    : constant-expression
      { $$ = $1.Value; }
    ;

menu-item
    : menu-entry-stmt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.UseUnicode = (CmdLineParms.TargetOS == RC_TARGET_OS_WIN32);
            $$.IsPopup = FALSE;
            $$.item.normal = $1;
        }
    | menu-popup-stmt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.UseUnicode = (CmdLineParms.TargetOS == RC_TARGET_OS_WIN32);
            $$.IsPopup = TRUE;
            $$.item.popup = $1;
        }
    ;

menu-popup-stmt
    : Y_POPUP menu-text comma-opt menu-section comma-opt
        {
            $$.item.type = MT_EITHER;
            $$.item.menuData.ItemFlags = MENU_POPUP;
            $$.item.menuData.ItemText = $2.string;
            $$.submenu = $4;
            $$.item.menuExData.ItemId = 0L;
            $$.item.menuExData.ItemType = 0L;
            $$.item.menuExData.ItemState = 0L;
            $$.item.menuExData.HelpId = 0L;
        }

    | Y_POPUP menu-text comma-opt menuId comma-opt menu-section comma-opt
        {
            $$.item.type = MT_MENUEX;
            $$.item.menuData.ItemFlags = MENUEX_POPUP;
            $$.item.menuData.ItemText = $2.string;
            $$.submenu = $6;
            $$.item.menuExData.ItemId = $4;
            $$.item.menuExData.ItemType = 0L;
            $$.item.menuExData.ItemState = 0L;
            $$.item.menuExData.HelpId = 0L;
        }
    | Y_POPUP menu-text comma-opt menuId comma-opt menuType comma-opt
              menu-section comma-opt
        {
            $$.item.type = MT_MENUEX;
            $$.item.menuData.ItemFlags = MENUEX_POPUP;
            $$.item.menuData.ItemText = $2.string;
            $$.submenu = $8;
            $$.item.menuExData.ItemId = $4;
            $$.item.menuExData.ItemType = $6;
            $$.item.menuExData.ItemState = 0L;
            $$.item.menuExData.HelpId = 0L;
        }
    | Y_POPUP menu-text comma-opt menuId comma-opt menuType comma-opt
              menuState comma-opt menu-section comma-opt
        {
            $$.item.type = MT_MENUEX;
            $$.item.menuData.ItemFlags = MENUEX_POPUP;
            $$.item.menuData.ItemText = $2.string;
            $$.submenu = $10;
            $$.item.menuExData.ItemId = $4;
            $$.item.menuExData.ItemType = $6;
            $$.item.menuExData.ItemState = $8;
            $$.item.menuExData.HelpId = 0L;
        }
    | Y_POPUP menu-text comma-opt menuId comma-opt menuType comma-opt
              menuState comma-opt helpId comma-opt menu-section comma-opt
        {
            $$.item.type = MT_MENUEX;
            $$.item.menuData.ItemFlags = MENUEX_POPUP;
            $$.item.menuData.ItemText = $2.string;
            $$.submenu = $12;
            $$.item.menuExData.ItemId = $4;
            $$.item.menuExData.ItemType = $6;
            $$.item.menuExData.ItemState = $8;
            $$.item.menuExData.HelpId = $10;
        }

    | Y_POPUP menu-text comma-opt menu-item-options comma-opt menu-section comma-opt
        {
            $$.item.type = MT_MENU;
            $$.item.menuData.ItemText = $2.string;
            $$.item.menuData.ItemFlags = $4 | MENU_POPUP;
            $$.submenu = $6;
        }
    ;

menu-entry-stmt
    : Y_MENUITEM menu-entry-defn
        { $$ = $2; }
    ;

menu-entry-defn
    : Y_SEPARATOR comma-opt
        {
            $$.type = MT_SEPARATOR;
            $$.menuData.ItemText = NULL;
            $$.menuData.ItemID = 0;
            $$.menuData.ItemFlags = 0;
            $$.menuExData.ItemType = MENUEX_SEPARATOR;
            $$.menuExData.ItemState = 0;
        }
    | menu-text comma-opt
        {
            $$.type = MT_MENUEX_NO_ID;
            $$.menuData.ItemText = $1.string;
            $$.menuData.ItemID = 0;
            $$.menuData.ItemFlags = 0;
            $$.menuExData.ItemType = 0L;
            $$.menuExData.ItemState = 0L;
        }

    | menu-text comma-opt menu-result comma-opt
        {
            $$.type = MT_EITHER;
            $$.menuData.ItemText = $1.string;
            $$.menuData.ItemID = $3;
            $$.menuData.ItemFlags = 0;
            $$.menuExData.ItemType = 0L;
            $$.menuExData.ItemState = 0L;
        }
    | menu-text comma-opt menu-result comma-opt menuType comma-opt
        {
            $$.type = MT_MENUEX;
            $$.menuData.ItemText = $1.string;
            $$.menuData.ItemID = $3;
            $$.menuData.ItemFlags = 0;
            $$.menuExData.ItemType = $5;
            $$.menuExData.ItemState = 0L;
        }

    | menu-text comma-opt menu-result comma-opt menuType comma-opt
                menuState comma-opt
        {
            $$.type = MT_MENUEX;
            $$.menuData.ItemText = $1.string;
            $$.menuData.ItemID = $3;
            $$.menuData.ItemFlags = 0;
            $$.menuExData.ItemType = $5;
            $$.menuExData.ItemState = $7;
        }

    | menu-text comma-opt menu-result comma-opt menu-item-options comma-opt
        {
            $$.type = MT_MENU;
            $$.menuData.ItemText = $1.string;
            $$.menuData.ItemID = $3;
            $$.menuData.ItemFlags = $5;
        }
    ;

menu-text
    : string-constant
    ;

menu-result
    : constant-expression
        { $$ = $1.Value; }
    ;

/* Note: The way we accept menu options differs from what is specified */
/* in the Windows Programmer's Reference. There they say that the Y_HELP */
/* keyword is used only with menu entry statements but then proceed to ignore */
/* it. The bit that is designated MF_HELP in windows.h only produces */
/* meaningful results if it is set for top level menu items. Every where else */
/* we issue a warnning but set the bit anyways. This is similar to what the */
/* Whitewater Resource Toolkit does. */
/* Note: the Y_BITMAP and Y_OWNERDRAW keywords may not be meaning full in this*/
/* context but we accept them anyways and set the appropriate bits assuming */
/* that the user knows what he or she is doing when they use these keyword */

menu-item-options
    : menu-item-option
        { $$ = SemWINAddFirstMenuOption( $1 ); }
    | menu-item-options comma-opt menu-item-option
        { $$ = SemWINAddMenuOption( $1, $3 ); }
    ;

menu-item-option
    : Y_CHECKED
        { $$ = Y_CHECKED; }
    | Y_GRAYED
        { $$ = Y_GRAYED; }
    | Y_INACTIVE
        { $$ = Y_INACTIVE; }
    | Y_MENUBARBREAK
        { $$ = Y_MENUBARBREAK; }
    | Y_MENUBREAK
        { $$ = Y_MENUBREAK; }
    | Y_OWNERDRAW
        { $$ = Y_OWNERDRAW; }
    | Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_HELP
        { $$ = Y_HELP; }
    ;

language-resource
    : language-stmt
        {
            SemWINSetGlobalLanguage( &$1 );
        }
    ;

opt-resource-info-stmts
    : resource-info-stmts
    | /* nothing */
    ;

resource-info-stmts
    : resource-info-stmt
    | resource-info-stmts resource-info-stmt
    ;

resource-info-stmt
    : language-stmt
        {
            SemWINSetResourceLanguage( &$1, TRUE );
        }
    | characteristics-stmt
    | version-stmt
    ;

language-stmt
    : Y_LANGUAGE constant-expression Y_COMMA constant-expression
        { $$.lang = $2.Value; $$.sublang = $4.Value; }
    ;

characteristics-stmt
    : Y_CHARACTERISTICS constant-expression
        { SemWINUnsupported( Y_CHARACTERISTICS ); }
    ;

version-stmt
    : Y_VERSION constant-expression
        { SemWINUnsupported( Y_VERSION ); }
    ;

dialog-or-dialogEx
    : Y_DIALOG
        { $$ = Y_DIALOG; }
    | Y_DIALOG_EX
        { $$ = Y_DIALOG_EX; }
    ;

helpId-opt
    : comma-opt constant-expression
       { $$.HelpId = $2.Value; $$.HelpIdDefined = TRUE; }
    | /* nothing */
       { $$.HelpId = 0; $$.HelpIdDefined = FALSE; }
    ;

dlg-resource
    : name-id dialog-or-dialogEx size-info helpId-opt diag-options-section
              diag-control-section
        {
            SemWINWriteDialogBox( $1,
               MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
               $3, $5, $6, $4, $2 );
        }
    | name-id dialog-or-dialogEx size-info helpId-opt diag-control-section
        {
            SemWINWriteDialogBox( $1,
               MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
               $3, NULL, $5, $4, $2 );
        }
    | name-id dialog-or-dialogEx exstyle-equal-stmt size-info helpId-opt
              diag-options-section diag-control-section
        {
            SemWINWriteDialogBox( $1,
               MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
               $4, SemWINDiagOptions( $6, &($3) ), $7, $5, $2 );
        }
    | name-id dialog-or-dialogEx exstyle-equal-stmt size-info helpId-opt
              diag-control-section
        {
            SemWINWriteDialogBox( $1,
                MEMFLAG_PURE|MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                $4, SemWINNewDiagOptions( &($3 ) ), $6, $5, $2 );
        }
    | name-id dialog-or-dialogEx resource-options comma-opt size-info helpId-opt
               diag-options-section diag-control-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteDialogBox( $1, $3.flags, $5, $7, $8, $6, $2 );
        }
    | name-id dialog-or-dialogEx resource-options comma-opt size-info helpId-opt
                diag-control-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteDialogBox( $1, $3.flags, $5, NULL, $7, $6, $2 );
        }
    | name-id dialog-or-dialogEx resource-options comma-opt exstyle-equal-stmt
                size-info helpId-opt diag-options-section diag-control-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteDialogBox( $1, $3.flags, $6,
                                SemWINDiagOptions( $8, &($5) ) , $9, $7, $2 );
        }
    | name-id dialog-or-dialogEx resource-options comma-opt exstyle-equal-stmt
              size-info helpId-opt diag-control-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemWINWriteDialogBox( $1, $3.flags, $6,
                               SemWINNewDiagOptions( &($5 ) ), $8, $7, $2 );
        }
    ;

size-info
    : size-x comma-opt size-y comma-opt size-w comma-opt size-h
        { $$.x = $1; $$.y = $3; $$.width = $5; $$.height = $7; }
    ;

size-x
    : constant-expression
        { $$ = $1.Value; }
    ;

size-y
    : constant-expression
        { $$ = $1.Value; }
    ;

size-w
    : constant-expression
        { $$ = $1.Value; }
    ;

size-h
    : constant-expression
        { $$ = $1.Value; }
    ;

diag-options-section
    : diag-options-stmt
        { $$ = SemWINNewDiagOptions( &($1) ); }
    | diag-options-section diag-options-stmt
        { $$ = SemWINDiagOptions( $1, &($2) ); }
    ;

diag-options-stmt
    : style-stmt
    | exstyle-stmt
    | caption-stmt
    | menu-stmt
    | class-stmt
    | font-stmt
    | diag-lang-stmt
    | diag-version-stmt
    | diag-characteristics-stmt
    ;

diag-lang-stmt
    : language-stmt
        { $$.token = Y_LANGUAGE; $$.Opt.lang = $1; }
    ;

diag-version-stmt
    : version-stmt
        { $$.token = Y_VERSION; }
    ;

diag-characteristics-stmt
    : characteristics-stmt
        { $$.token = Y_CHARACTERISTICS; }
    ;

exstyle-stmt
    : Y_EXSTYLE exstyle
        { $$.token = Y_EXSTYLE; $$.Opt.Exstyle = $2.Value; }
    ;

exstyle-equal-stmt
    : Y_EXSTYLE Y_SINGLE_EQ exstyle comma-opt
        { $$.token = Y_EXSTYLE; $$.Opt.Exstyle = $3.Value; }
    ;

exstyle
    : constant-expression
    ;

style-stmt
    : Y_STYLE style
        { $$.token = Y_STYLE; $$.Opt.Style = $2.Value; }
    ;

style
    : constant-expression
    ;

caption-stmt
    : Y_CAPTION string-constant
        {
            $$.token = Y_CAPTION;
            $$.Opt.Str = $2.string;
        }
    ;

menu-stmt
    : Y_MENU name-id
        {
            $$.token = Y_MENU;
            $$.Opt.Name = WResIDToNameOrOrd( $2 );
            RcMemFree( $2 );
        }
    ;

class-stmt
    : Y_CLASS class-name
        { $$.token = Y_CLASS; $$.Opt.Name = $2; }
    ;

class-name
    : string-constant
        { $$ = ResStrToNameOrOrd( $1.string ); RcMemFree( $1.string ); }
    | constant-expression
        { $$ = ResNumToNameOrOrd( $1.Value ); }
    ;

ctl-class-name
    : string-constant
        { $$ = ResStrToNameOrOrd( $1.string ); RcMemFree( $1.string ); }
    | Y_BUTTON
        { $$ = ResStrToNameOrOrd( "BUTTON" ); }
    | Y_COMBOBOX
        { $$ = ResStrToNameOrOrd( "COMBOBOX" ); }
    | Y_EDIT
        { $$ = ResStrToNameOrOrd( "EDIT" ); }
    | Y_LISTBOX
        { $$ = ResStrToNameOrOrd( "LISTBOX" ); }
    | Y_SCROLLBAR
        { $$ = ResStrToNameOrOrd( "SCROLLBAR" ); }
    | Y_STATIC
        { $$ = ResStrToNameOrOrd( "STATIC" ); }
    | constant-expression
        { $$ = ResNumToNameOrOrd( $1.Value ); }
    ;

font-stmt
    : Y_FONT point-size comma-opt typeface
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = 0;
            $$.Opt.Font.FontItalic = 0;
            $$.Opt.Font.FontExtra = 1;
            $$.Opt.Font.FontWeightDefined = FALSE;
            $$.Opt.Font.FontItalicDefined = FALSE;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = 0;
            $$.Opt.Font.FontExtra = 1;
            $$.Opt.Font.FontWeightDefined = TRUE;
            $$.Opt.Font.FontItalicDefined = FALSE;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight comma-opt
             fontitalic
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = $8;
            $$.Opt.Font.FontExtra = 1;
            $$.Opt.Font.FontWeightDefined = TRUE;
            $$.Opt.Font.FontItalicDefined = TRUE;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight comma-opt
             fontitalic comma-opt fontextra
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = $8;
            $$.Opt.Font.FontExtra = $10;
            $$.Opt.Font.FontWeightDefined = TRUE;
            $$.Opt.Font.FontItalicDefined = TRUE;
        }
    ;

fontextra
    : constant-expression
      { $$ = $1.Value & 0xFF; }
    ;

fontweight
    : constant-expression
      { $$ = $1.Value; }
    ;

fontitalic
    : constant-expression
      {
          /* the value stored is boolean and must be 1 or 0 */
          $$ = ( $1.Value != 0 );
      }
    ;

point-size
    : constant-expression
        { $$ = $1.Value; }
    ;

typeface
    : string-constant
    ;

diag-control-section
    : Y_BEGIN diag-control-stmts Y_END
        {
            $$ = $2;
        }
    | Y_LBRACE diag-control-stmts Y_RBRACE
        { $$ = $2; }
    | Y_BEGIN Y_END
        { $$ = SemWINEmptyDiagCtrlList(); }
    | Y_LBRACE Y_RBRACE
        { $$ = SemWINEmptyDiagCtrlList(); }
    ;

diag-data-elements
    : raw-data-section
    | /* Nothing */
      { $$ = NULL; }
    ;

diag-control-stmts
    : diag-control-stmt diag-data-elements
        { $$ = SemWINNewDiagCtrlList( $1, $2 ); }
    | diag-control-stmts diag-control-stmt diag-data-elements
        { $$ = SemWINAddDiagCtrlList( $1, $2, $3 ); }
    ;

diag-control-stmt
    : ltext-stmt
    | rtext-stmt
    | ctext-stmt
    | autocheckbox-stmt
    | autoradiobutton-stmt
    | auto3state-stmt
    | checkbox-stmt
    | pushbutton-stmt
    | listbox-stmt
    | groupbox-stmt
    | defpushbutton-stmt
    | radiobutton-stmt
    | edittext-stmt
    | combobox-stmt
    | icon-stmt
    | scrollbar-stmt
    | state3-stmt
    | control-stmt
    ;

cntl-text-options
    : string-constant cntl-options
        {
            $2.Text = ResStrToNameOrOrd( $1.string );
            RcMemFree( $1.string );
            $$ = $2;
        }
    ;

cntl-options
    : comma-opt cntl-id comma-opt size-info
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style.Mask = 0;
            $$.Text = NULL;
            $$.ExtendedStyle = 0L;
            $$.HelpId = 0L;
            $$.HelpIdDefined = FALSE;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = 0L;
            $$.HelpId = 0L;
            $$.HelpIdDefined = FALSE;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style comma-opt exstyle
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = $8.Value;
            $$.HelpId = 0L;
            $$.HelpIdDefined = FALSE;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style comma-opt
           exstyle comma-opt helpId
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = $8.Value;
            $$.HelpId = $10;
            $$.HelpIdDefined = TRUE;
        }
    ;

cntl-id
    : constant-expression
        { $$ = $1.Value; }
    ;

ltext-stmt
    : Y_LTEXT cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_LTEXT, $2 ); }
    ;

rtext-stmt
    : Y_RTEXT cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_RTEXT, $2 ); }
    ;

ctext-stmt
    : Y_CTEXT cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_CTEXT, $2 ); }
    ;

autocheckbox-stmt
    : Y_AUTOCHECKBOX cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_AUTOCHECKBOX, $2 ); }
    ;

autoradiobutton-stmt
    : Y_AUTORADIOBUTTON cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_AUTORADIOBUTTON, $2 ); }
    ;

auto3state-stmt
    : Y_AUTO3STATE cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_AUTO3STATE, $2 ); }
    ;

checkbox-stmt
    : Y_CHECKBOX cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_CHECKBOX, $2 ); }
    ;

pushbutton-stmt
    : Y_PUSHBUTTON cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_PUSHBUTTON, $2 ); }
    ;

listbox-stmt
    : Y_LISTBOX cntl-options
        { $$ = SemWINNewDiagCtrl( Y_LISTBOX, $2 ); }
    ;

groupbox-stmt
    : Y_GROUPBOX cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_GROUPBOX, $2 ); }
    ;

defpushbutton-stmt
    : Y_DEFPUSHBUTTON cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_DEFPUSHBUTTON, $2 ); }
    ;

radiobutton-stmt
    : Y_RADIOBUTTON cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_RADIOBUTTON, $2 ); }
    ;

edittext-stmt
    : Y_EDITTEXT cntl-options
        { $$ = SemWINNewDiagCtrl( Y_EDITTEXT, $2 ); }
    ;

combobox-stmt
    : Y_COMBOBOX cntl-options
        { $$ = SemWINNewDiagCtrl( Y_COMBOBOX, $2 ); }
    ;

icon-stmt
    : Y_ICON icon-name comma-opt cntl-id comma-opt icon-parms
        { $6.Text = $2; $6.ID = $4; $$ = SemWINNewDiagCtrl( Y_ICON, $6 ); }
    ;

state3-stmt
    : Y_STATE3 cntl-text-options
        { $$ = SemWINNewDiagCtrl( Y_STATE3, $2 ); }
    ;

icon-name
    : name-id
        { $$ = WResIDToNameOrOrd( $1 ); RcMemFree( $1 ); }
    ;

icon-parms
    : size-x comma-opt size-y
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = 0;          /* ignore width, height, style */
            $$.Size.height = 0;
            $$.Style.Mask = 0;
            $$.ExtendedStyle = 0L;
        }
    | size-x comma-opt size-y comma-opt style
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = 0;          /* ignore width, height */
            $$.Size.height = 0;
            $$.Style = $5;
            $$.ExtendedStyle = 0L;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = $5;         /* ignore style */
            $$.Size.height = $7;
            $$.Style.Mask = 0;
            $$.ExtendedStyle = 0L;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h comma-opt style
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = $5;
            $$.Size.height = $7;
            $$.Style = $9;
            $$.ExtendedStyle = 0L;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h comma-opt style
             comma-opt exstyle
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = $5;
            $$.Size.height = $7;
            $$.Style = $9;
            $$.ExtendedStyle = $11.Value;
        }
    ;

scrollbar-stmt
    : Y_SCROLLBAR cntl-options
        { $$ = SemWINNewDiagCtrl( Y_SCROLLBAR, $2 ); }
    ;

control-stmt
    : Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info
        {
            $$ = SemWINSetControlData( $8, $4, $10, $2, $6, 0L, NULL );
        }

    | Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info comma-opt exstyle helpId-opt
        {
            $$ = SemWINSetControlData( $8, $4, $10, $2, $6, $12.Value, &($13) );
        }
    ;

cntl-text
    : name-id
    ;

version-info-resource
    : name-id Y_VERSIONINFO fixed-ver-section variable-ver-section
        {
            SemWINWriteVerInfo( $1, MEMFLAG_MOVEABLE | MEMFLAG_PURE, $3, $4 );
        }
    | name-id Y_VERSIONINFO resource-options fixed-ver-section
                                variable-ver-section
        {
            SemWINCheckMemFlags( &($3), 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            SemWINWriteVerInfo( $1, $3.flags, $4, $5 );
        }
    ;

fixed-ver-section
    : fixed-ver-stmt
        { $$ = SemWINNewVerFixedInfo( $1 ); }
    | fixed-ver-section fixed-ver-stmt
        { $$ = SemWINAddVerFixedInfo( $1, $2 ); }
    ;

fixed-ver-stmt
    : fileversion-stmt
    | productversion-stmt
    | fileflagsmask-stmt
    | fileflags-stmt
    | fileos-stmt
    | filetype-stmt
    | filesubtype-stmt
    ;

fileversion-stmt
    : Y_FILEVERSION version
        { $2.token = Y_FILEVERSION; $$ = $2;  }
    ;

productversion-stmt
    : Y_PRODUCTVERSION version
        { $2.token = Y_PRODUCTVERSION; $$ = $2; }
    ;

fileflagsmask-stmt
    : Y_FILEFLAGSMASK constant-expression
        { $$.token = Y_FILEFLAGSMASK; $$.u.Option = $2.Value; }
    ;

fileflags-stmt
    : Y_FILEFLAGS constant-expression
        { $$.token = Y_FILEFLAGS; $$.u.Option = $2.Value; }
    ;

fileos-stmt
    : Y_FILEOS constant-expression
        { $$.token = Y_FILEOS; $$.u.Option = $2.Value; }
    ;

filetype-stmt
    : Y_FILETYPE constant-expression
        { $$.token = Y_FILETYPE; $$.u.Option = $2.Value; }
    ;

filesubtype-stmt
    : Y_FILESUBTYPE constant-expression
        { $$.token = Y_FILESUBTYPE; $$.u.Option = $2.Value; }
    ;

version
    : version-pair
        {
            $$.u.Version.High = $1;
            $$.u.Version.Low.HighWord = 0;
            $$.u.Version.Low.LowWord = 0;
        }
    | version-pair comma-opt version-pair
        { $$.u.Version.High = $1; $$.u.Version.Low = $3; }
    | version-pair comma-opt constant-expression
        {
            $$.u.Version.High = $1;
            $$.u.Version.Low.HighWord = $3.Value;
            $$.u.Version.Low.LowWord = 0;
        }
    ;

version-pair
    : constant-expression comma-opt constant-expression
        { $$.HighWord = $1.Value; $$.LowWord = $3.Value; }
    ;

variable-ver-section
    : Y_BEGIN block-stmts Y_END
        { $$ = $2; }
    | Y_LBRACE block-stmts Y_RBRACE
        { $$ = $2; }
    ;

block-stmts
    : block-stmt
        { $$ = SemWINNewBlockNest( $1 ); }
    | block-stmts block-stmt
        { $$ = SemWINAddBlockNest( $1, $2 ); }
    ;

block-stmt
    : Y_BLOCK block-name block-body
        { $$ = SemWINNameVerBlock( $2.string, $3 ); }
    ;

block-name
    : string-constant
    ;

block-body
    : Y_BEGIN value-stmts Y_END
        { $$ = $2; }
    | Y_BEGIN value-stmts block-stmts Y_END
        { $$ = SemWINMergeBlockNest( $2, $3 ); }
    | Y_BEGIN block-stmts Y_END
        { $$ = $2; }
    | Y_LBRACE value-stmts Y_RBRACE
        { $$ = $2; }
    | Y_LBRACE value-stmts block-stmts Y_RBRACE
        { $$ = SemWINMergeBlockNest( $2, $3 ); }
    | Y_LBRACE block-stmts Y_RBRACE
        { $$ = $2; }
    ;

value-stmts
    : value-stmt
        { $$ = SemWINNewBlockNest( $1 ); }
    | value-stmts value-stmt
        { $$ = SemWINAddBlockNest( $1, $2 ); }
    ;

value-stmt
    : Y_VALUE value-name comma-opt value-list
        { $$ = SemWINNewBlockVal( $2.string, $4 ); }
    ;

value-name
    : string-constant
    ;

value-list
    : value-item
        { $$ = SemWINNewVerValueList( $1 ); }
    | value-list comma-opt value-item
        { $$ = SemWINAddVerValueList( $1, $3 ); }
    ;

value-item
    : constant-expression
        { $$.IsNum = TRUE; $$.Value.Num = $1.Value; }
    | value-string-list
    ;

value-string-list
    :string-constant
        {
            $$.IsNum = FALSE;
            $$.Value.String = $1.string;
            $$.strlen = $1.length;
        }
    | value-string-list string-constant
        {
            $$.IsNum = FALSE;
            $$.strlen = strlen( $1.Value.String ) + strlen( $2.string );
            $$.Value.String = RcMemMalloc( $$.strlen + 1 );
            strcpy( $$.Value.String, $1.Value.String );
            strcat( $$.Value.String, $2.string );
            RcMemFree( $1.Value.String );
            RcMemFree( $2.string );
        }
    ;

string-constant
    : Y_STRING
    | Y_LSQ_BRACKET string-group Y_RSQ_BRACKET
        {
            $$ = $2;
        }
    ;

string-group
    : Y_STRING
    | string-group Y_STRING
        {
            $$.lstring = ( $1.lstring | $2.lstring );
            $$.length = $1.length + $2.length;
            $$.string = RcMemMalloc( $$.length + 1 );
            strcpy( $$.string, $1.string );
            strcat( $$.string, $2.string );
            RcMemFree( $1.string );
            RcMemFree( $2.string );
        }
    ;

constant-expression
    : conditional-exp
    ;

conditional-exp
    : log-or-exp
    | log-or-exp Y_QUESTION constant-expression Y_COLON conditional-exp
        { $$ = $1.Value ? $3 : $5; }
    ;

log-or-exp
    : log-and-exp
    | log-or-exp Y_OR log-and-exp
        { $$.Value = $1.Value || $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

log-and-exp
    : bit-or-exp
    | log-and-exp Y_AND bit-or-exp
        { $$.Value = $1.Value && $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

bit-or-exp
    : bit-xor-exp
    | bit-or-exp Y_BITOR bit-xor-exp
        { $$.Value = $1.Value | $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

bit-xor-exp
    : bit-and-exp
    | bit-xor-exp Y_BITXOR bit-and-exp
        { $$.Value = $1.Value ^ $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

bit-and-exp
    : equality-exp
    | bit-and-exp Y_BITAND equality-exp
        { $$.Value = $1.Value & $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

equality-exp
    : relational-exp
    | equality-exp Y_EQ relational-exp
        { $$.Value = $1.Value == $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | equality-exp Y_NE relational-exp
        { $$.Value = $1.Value != $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

relational-exp
    : shift-exp
    | relational-exp Y_GT shift-exp
        { $$.Value = $1.Value > $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | relational-exp Y_LT shift-exp
        { $$.Value = $1.Value < $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | relational-exp Y_GE shift-exp
        { $$.Value = $1.Value >= $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | relational-exp Y_LE shift-exp
        { $$.Value = $1.Value <= $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

shift-exp
    : additive-exp
    | shift-exp Y_SHIFTL additive-exp
        { $$.Value = $1.Value << $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | shift-exp Y_SHIFTR additive-exp
        { $$.Value = $1.Value >> $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

additive-exp
    : multiplicative-exp
    | additive-exp Y_PLUS multiplicative-exp
        { $$.Value = $1.Value + $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | additive-exp Y_MINUS multiplicative-exp
        { $$.Value = $1.Value - $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

multiplicative-exp
    : unary-exp
    | multiplicative-exp Y_TIMES unary-exp
        { $$.Value = $1.Value * $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | multiplicative-exp Y_DIVIDE unary-exp
        { $$.Value = $1.Value / $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    | multiplicative-exp Y_MOD unary-exp
        { $$.Value = $1.Value % $3.Value; $$.Mask = $1.Mask | $3.Mask; }
    ;

unary-exp
    : primary-exp
    | Y_MINUS unary-exp
        { $$.Value = - $2.Value; $$.Mask = $2.Mask; }
    | Y_BITNOT unary-exp
        { $$.Value = ~ $2.Value; $$.Mask = $2.Mask; }
    | Y_NOT unary-exp
        { $$.Value = ! $2.Value; $$.Mask = $2.Mask; }
    ;

primary-exp
    : Y_INTEGER
        {
            $$.Mask = $1.val; $$.Value = $1.val;
            $$.longVal = ($1.type & SCAN_INT_TYPE_LONG) != 0;
            $$.unsgVal = ($1.type & SCAN_INT_TYPE_UNSIGNED) != 0;
        }
    | Y_LPAREN constant-expression Y_RPAREN
        { $$ = $2; }
    ;

%%
