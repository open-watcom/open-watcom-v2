/***************************************************************************
 * OS/2 Resource Compiler Grammar
 * Worked out with much blood, sweat & tears, no thanks to IBM's atrocious
 * "documentation". Reverse engineering rules.
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
%token Y_ACCELTABLE
%token Y_ALT
%token Y_ASSOCTABLE
%token Y_AUTOCHECKBOX
%token Y_AUTORADIOBUTTON
%token Y_BEGIN
%token Y_BITMAP
%token Y_CHAR
%token Y_CHARTABLE
%token Y_CHECKBOX
%token Y_CODEPAGE
%token Y_COMBOBOX
%token Y_CONTAINER
%token Y_CONTROL
%token Y_CTEXT
%token Y_CTLDATA
%token Y_CURSOR
%token Y_DEFAULTICON
%token Y_DEFPUSHBUTTON
%token Y_DIALOG
%token Y_DISCARDABLE
%token Y_DLGINCLUDE
%token Y_DLGTEMPLATE
%token Y_EDITTEXT
%token Y_END
%token Y_ENTRYFIELD
%token Y_FIXED
%token Y_FKAITEM
%token Y_FKALONG
%token Y_FKAROW
%token Y_FKASHORT
%token Y_FONT
%token Y_FONTDIR
%token Y_FRAME
%token Y_GROUPBOX
%token Y_HELP
%token Y_HELPITEM
%token Y_HELPSUBITEM
%token Y_HELPSUBTABLE
%token Y_HELPTABLE
%token Y_ICON
%token Y_IMPURE
%token Y_KEYTABLE
%token Y_LISTBOX
%token Y_LOADONCALL
%token Y_LONEKEY
%token Y_LTEXT
%token Y_MENU
%token Y_MENUITEM
%token Y_MESSAGETABLE
%token Y_MLE
%token Y_MOVEABLE
%token Y_NONSHARED
%token Y_NOT_KEYWORD
%token Y_NOTEBOOK
%token Y_POINTER
%token Y_POUND_PRAGMA
%token Y_PRELOAD
%token Y_PRESPARAMS
%token Y_PSZ
%token Y_PURE
%token Y_PUSHBUTTON
%token Y_RADIOBUTTON
%token Y_RCDATA
%token Y_RCINCLUDE          /* This is an old keyword that should no */
                            /* longer be used, but is used in the sample files*/
                            /* so may be used in some old .RC files. */
                            /* It is intercepted by the scanner */
%token Y_RESOURCE
%token Y_RTEXT
%token Y_SCANCODE
%token Y_SEGALIGN
%token Y_SEPARATOR
%token Y_SHARED
%token Y_SHIFT
%token Y_SLIDER
%token Y_SPINBUTTON
%token Y_STRINGTABLE
%token Y_SUBITEMSIZE
%token Y_SUBMENU
%token Y_SYSCOMMAND
%token Y_VALUESET
%token Y_VIRTUALKEY
%token Y_VKEYTABLE
%token Y_WINDOW
%token Y_WINDOWTEMPLATE

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
%type <integral>        id-value
%type <fullresflags>    resource-options
%type <optint>          resource-option
%type <string>          file-name
%type <acceltable>      acc-section
%type <accevent>        acc-event
%type <integral>        acc-cmd
%type <accflags>        acc-item-option
%type <accflags>        acc-item-options
%type <accelfullentry>  acc-item
%type <acceltable>      acc-items
%type <helptable>       help-table-section
%type <helptable>       help-items
%type <helpfullentry>   help-item
%type <helpsubtable>    help-subitems
%type <helpsubtable>    help-subtable-section
%type <dataelem>        help-subitem
%type <menuflags>       menu-item-options
%type <token>           menu-item-option
%type <menuptr>         menu-section
%type <menuptr>         submenu-section
%type <menuptr>         menu-items
%type <menufull>        menu-item
%type <menuitem>        menu-entry-stmt
%type <menuitem>        menu-entry-defn
%type <menufull>        submenu-entry-stmt
%type <string>          menu-text
%type <integral>        size-x
%type <integral>        size-y
%type <integral>        size-w
%type <integral>        size-h
%type <sizeinfo>        size-info
%type <string>          string-constant
%type <string>          string-group
%type <diagopts>        menu-stmt
%type <diagopts>        font-stmt
%type <diagopts>        diag-options-stmt
%type <token>           dialogtemplate
%type <dataelem>        diag-data-elements
%type <nameorord>       ctl-class-name
%type <token>           dialog-or-frame
%type <diagctrllist>    diag-control-section
%type <diagctrllist>    diag-control-stmts
%type <diagctrl>        diag-control-stmt
%type <diagctrl>        dialog-stmt
%type <diagctrlopts>    cntl-text-options
%type <diagctrlopts>    cntl-options
%type <diagctrlopts>    icon-parms
%type <presparams>      presparam-stmt
%type <presparamlist>   presparam-list
%type <diagctrl>        autocheckbox-stmt
%type <diagctrl>        autoradiobutton-stmt
%type <diagctrl>        checkbox-stmt
%type <diagctrl>        combobox-stmt
%type <diagctrl>        container-stmt
%type <diagctrl>        ctext-stmt
%type <diagctrl>        defpushbutton-stmt
%type <diagctrl>        edittext-stmt
%type <diagctrl>        groupbox-stmt
%type <diagctrl>        listbox-stmt
%type <diagctrl>        ltext-stmt
%type <diagctrl>        mle-stmt
%type <diagctrl>        notebook-stmt
%type <diagctrl>        pushbutton-stmt
%type <diagctrl>        radiobutton-stmt
%type <diagctrl>        rtext-stmt
%type <diagctrl>        slider-stmt
%type <diagctrl>        spinbutton-stmt
%type <diagctrl>        valueset-stmt
%type <diagctrl>        icon-stmt
%type <diagctrl>        control-stmt
%type <nameorord>       control-name
%type <nameorord>       presparam-name
%type <integral>        cntl-id
%type <nameorord>       cntl-text
%type <maskint>         cntl-style
%type <maskint>         frame-style
%type <integral>        string-id
%type <stritem>         string-item
%type <strtable>        string-items
%type <strtable>        string-section
%type <rawitem>         raw-data-item
%type <rawitem>         raw-numeric-data-item
%type <rawitem>         rc-data-item
%type <resloc>          user-defined-data
%type <resloc>          rc-data
%type <token>           resource-type
%type <dataelem>        control-data-section
%type <dataelem>        raw-data-section
%type <dataelem>        raw-data-items
%type <dataelem>        raw-numeric-data-items
%type <dataelem>        rc-data-items
%type <dataelem>        rc-data-section
%type <integral>        menu-id
%type <integral>        menuitem-style
%type <integral>        menuitem-attrib

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
    | message-table-resource
    | pragma-statment
    | codepage-statement
    ;

normal-resource
    : single-line-resource
    | rcdata-resource
    | user-defined-resource
    | menu-resource
    | accel-table-resource
    | help-table-resource
    | help-subtable-resource
    | dlg-template
    | dlginclude-resource
    ;

name-id
    : Y_NAME
        /* OS/2 accepts quoted strings and numbers only, not bare names */
        {
            $$ = WResIDFromNum( 0 );
            RcError( ERR_SYMBOL_NOT_DEFINED, $1.string );
            ErrorHasOccured = TRUE;
        }
    | string-constant
        /* OS/2 accepts quoted strings and numbers only, not bare names */
        {
            $$ = WResIDFromNum( 0 );
            RcError( ERR_SYNTAX_STR, $1.string );
            ErrorHasOccured = TRUE;
        }
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
        { $$ = WResIDFromStr( SemOS2TokenToString( $1 ) ); }
    ;

type-id
    : Y_NAME
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | string-constant
        { $$ = WResIDFromStr( $1.string ); RcMemFree( $1.string ); }
    | constant-expression
        { $$ = WResIDFromNum( $1.Value ); }
    ;

pragma-statment
    : Y_POUND_PRAGMA Y_CODEPAGE  Y_LPAREN constant-expression Y_RPAREN
        { SemOS2SetCodepage( $4.Value ); }
    ;

codepage-statement
    : Y_CODEPAGE constant-expression
        { SemOS2SetCodepage( $2.Value ); }
    ;

keyword-name
    : Y_ACCELTABLE
        { $$ = Y_ACCELTABLE; }
    | Y_ALT
        { $$ = Y_ALT; }
    | Y_ASSOCTABLE
        { $$ = Y_ASSOCTABLE; }
    | Y_AUTOCHECKBOX
        { $$ = Y_AUTOCHECKBOX; }
    | Y_AUTORADIOBUTTON
        { $$ = Y_AUTORADIOBUTTON; }
    | Y_BEGIN
        { $$ = Y_BEGIN; }
    | Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_CHAR
        { $$ = Y_CHAR; }
    | Y_CHARTABLE
        { $$ = Y_CHARTABLE; }
    | Y_CHECKBOX
        { $$ = Y_CHECKBOX; }
    | Y_CODEPAGE
        { $$ = Y_CODEPAGE; }
    | Y_COMBOBOX
        { $$ = Y_COMBOBOX; }
    | Y_CONTAINER
        { $$ = Y_CONTAINER; }
    | Y_CONTROL
        { $$ = Y_CONTROL; }
    | Y_CTEXT
        { $$ = Y_CTEXT; }
    | Y_CTLDATA
        { $$ = Y_CTLDATA; }
    | Y_CURSOR
        { $$ = Y_CURSOR; }
    | Y_DEFAULTICON
        { $$ = Y_DEFAULTICON; }
    | Y_DEFPUSHBUTTON
        { $$ = Y_DEFPUSHBUTTON; }
    | Y_DIALOG
        { $$ = Y_DIALOG; }
    | Y_DISCARDABLE
        { $$ = Y_DISCARDABLE; }
    | Y_DLGINCLUDE
        { $$ = Y_DLGINCLUDE; }
    | Y_DLGTEMPLATE
        { $$ = Y_DLGTEMPLATE; }
    | Y_EDITTEXT
        { $$ = Y_EDITTEXT; }
    | Y_END
        { $$ = Y_END; }
    | Y_ENTRYFIELD
        { $$ = Y_ENTRYFIELD; }
    | Y_FIXED
        { $$ = Y_FIXED; }
    | Y_FKAITEM
        { $$ = Y_FKAITEM; }
    | Y_FKALONG
        { $$ = Y_FKALONG; }
    | Y_FKAROW
        { $$ = Y_FKASHORT; }
    | Y_FKASHORT
        { $$ = Y_FKASHORT; }
    | Y_FONT
        { $$ = Y_FONT; }
    | Y_FONTDIR
        { $$ = Y_FONTDIR; }
    | Y_FRAME
        { $$ = Y_FRAME; }
    | Y_GROUPBOX
        { $$ = Y_GROUPBOX; }
    | Y_HELP
        { $$ = Y_HELP; }
    | Y_HELPITEM
        { $$ = Y_HELPITEM; }
    | Y_HELPSUBITEM
        { $$ = Y_HELPSUBITEM; }
    | Y_HELPTABLE
        { $$ = Y_HELPTABLE; }
    | Y_ICON
        { $$ = Y_ICON; }
    | Y_IMPURE
        { $$ = Y_IMPURE; }
    | Y_KEYTABLE
        { $$ = Y_KEYTABLE; }
    | Y_LISTBOX
        { $$ = Y_LISTBOX; }
    | Y_LOADONCALL
        { $$ = Y_LOADONCALL; }
    | Y_LONEKEY
        { $$ = Y_LONEKEY; }
    | Y_LTEXT
        { $$ = Y_LTEXT; }
    | Y_MENU
        { $$ = Y_MENU; }
    | Y_MENUITEM
        { $$ = Y_MENUITEM; }
    | Y_MESSAGETABLE
        { $$ = Y_MESSAGETABLE; }
    | Y_MLE
        { $$ = Y_MLE; }
    | Y_MOVEABLE
        { $$ = Y_MOVEABLE; }
    | Y_NONSHARED
        { $$ = Y_NONSHARED; }
    | Y_NOTEBOOK
        { $$ = Y_NOTEBOOK; }
    | Y_POINTER
        { $$ = Y_POINTER; }
    | Y_PRELOAD
        { $$ = Y_PRELOAD; }
    | Y_PRESPARAMS
        { $$ = Y_PRESPARAMS; }
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
    | Y_RESOURCE
        { $$ = Y_RESOURCE; }
    | Y_RTEXT
        { $$ = Y_RTEXT; }
    | Y_SCANCODE
        { $$ = Y_SCANCODE; }
    | Y_SEGALIGN
        { $$ = Y_SEGALIGN; }
    | Y_SEPARATOR
        { $$ = Y_SEPARATOR; }
    | Y_SHARED
        { $$ = Y_SHARED; }
    | Y_SHIFT
        { $$ = Y_SHIFT; }
    | Y_SLIDER
        { $$ = Y_SLIDER; }
    | Y_SPINBUTTON
        { $$ = Y_SPINBUTTON; }
    | Y_STRINGTABLE
        { $$ = Y_STRINGTABLE; }
    | Y_SUBITEMSIZE
        { $$ = Y_SUBITEMSIZE; }
    | Y_SUBMENU
        { $$ = Y_SUBMENU; }
    | Y_SYSCOMMAND
        { $$ = Y_SYSCOMMAND; }
    | Y_VALUESET
        { $$ = Y_VALUESET; }
    | Y_VIRTUALKEY
        { $$ = Y_VIRTUALKEY; }
    | Y_VKEYTABLE
        { $$ = Y_VKEYTABLE; }
    | Y_WINDOW
        { $$ = Y_WINDOW; }
    | Y_WINDOWTEMPLATE
        { $$ = Y_WINDOWTEMPLATE; }
    ;

comma-opt
    : Y_COMMA
    | /* nothing */
    ;

dlginclude-resource
    : Y_DLGINCLUDE name-id file-name
        { SemOS2AddDlgincResource( $2, $3.string ); }
    ;

single-line-resource
    : resource-type name-id file-name
        { SemOS2AddSingleLineResource( $2, $1, NULL, $3.string ); }
    | resource-type name-id resource-options file-name
        { SemOS2AddSingleLineResource( $2, $1, &($3), $4.string ); }
    | Y_DEFAULTICON file-name
        { SemOS2AddSingleLineResource( NULL, Y_DEFAULTICON, NULL, $2.string ); }
    | Y_DEFAULTICON resource-options file-name
        { SemOS2AddSingleLineResource( NULL, Y_DEFAULTICON, &($2), $3.string ); }
    ;

resource-options
    : resource-option
        { $$ = SemOS2AddFirstResOption( $1.Option, $1.Value ); }
    | resource-options resource-option
        { $$ = SemOS2AddResOption( $1, $2.Option, $2.Value ); }
    ;

resource-option
    : Y_PRELOAD
        { $$.Option = Y_PRELOAD; }
    | Y_LOADONCALL
        { $$.Option = Y_LOADONCALL; }
    | Y_FIXED
        { $$.Option = Y_FIXED; }
    | Y_MOVEABLE
        { $$.Option = Y_MOVEABLE; }
    | Y_PURE
        { $$.Option = Y_PURE; }
    | Y_IMPURE
        { $$.Option = Y_IMPURE; }
    | Y_DISCARDABLE
        { $$.Option = Y_DISCARDABLE; }
    | Y_SEGALIGN
        { $$.Option = Y_SEGALIGN; }
    | Y_INTEGER
        { $$.Option = Y_INTEGER; $$.Value = $1.val; }
    ;

file-name
    : Y_NAME
    | Y_DOS_FILENAME
    | string-constant
    ;

resource-type
    : Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_FONT
        { $$ = Y_FONT; }
    | Y_ICON
        { $$ = Y_ICON; }
    | Y_POINTER
        { $$ = Y_POINTER; }
    ;

user-defined-resource
    : Y_RESOURCE type-id comma-opt name-id user-defined-data
        {
            SemAddResourceFree( $4, $2,
                    MEMFLAG_DISCARDABLE | MEMFLAG_PURE | MEMFLAG_MOVEABLE, $5 );
        }
    | Y_RESOURCE type-id comma-opt name-id resource-options user-defined-data
        {
            SemOS2CheckResFlags( &($5), 0, MEMFLAG_DISCARDABLE | MEMFLAG_MOVEABLE,
                    MEMFLAG_PURE );
            SemAddResourceFree( $4, $2, $5.flags, $6 );
        }
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

control-data-section
    : Y_CTLDATA raw-data-items
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
            $$.IsString    = TRUE;
            $$.LongItem    = $1.lstring;
            $$.StrLen      = $1.length;
            $$.TmpStr      = TRUE;
            $$.WriteNull   = FALSE;
            $$.Item.String = $1.string;
        }
    | constant-expression
        { $$.IsString = FALSE; $$.Item.Num = $1.Value; $$.LongItem = $1.longVal; }
    ;

raw-numeric-data-items
    : raw-numeric-data-item
        { $$ = SemNewDataElemList( $1 ); }
    | raw-numeric-data-items raw-numeric-data-item
        { $$ = SemAppendDataElem( $1, $2 ); }
    | raw-numeric-data-items Y_COMMA
        { $$ = $1; }
    ;

raw-numeric-data-item
    : constant-expression
        { $$.IsString = FALSE; $$.Item.Num = $1.Value; $$.LongItem = FALSE; }
    ;

rc-data-items
    : rc-data-item
        { $$ = SemNewDataElemList( $1 ); }
    | rc-data-items rc-data-item
        { $$ = SemAppendDataElem( $1, $2 ); }
    | rc-data-items Y_COMMA
        { $$ = $1; }
    ;

rc-data-item
    : string-constant
        {
            $$.IsString    = TRUE;
            $$.LongItem    = $1.lstring;
            $$.StrLen      = $1.length;
            $$.TmpStr      = TRUE;
            $$.WriteNull   = TRUE;
            $$.Item.String = $1.string;
        }
    | constant-expression
        { $$.IsString = FALSE; $$.Item.Num = $1.Value; $$.LongItem = $1.longVal; }
    ;

rc-data-section
    : Y_BEGIN rc-data-items Y_END
        { $$ = $2; }
    | Y_LBRACE rc-data-items Y_RBRACE
        { $$ = $2; }
    ;

rc-data
    : rc-data-section
        { $$ = SemFlushDataElemList( $1, TRUE ); }
    ;

rcdata-resource
    : Y_RCDATA name-id rc-data
        {
            SemAddResourceFree( $2, WResIDFromNum( OS2_RT_RCDATA ),
                    MEMFLAG_PURE | MEMFLAG_MOVEABLE, $3 );
        }
    | Y_RCDATA name-id resource-options rc-data
        {
            SemOS2CheckResFlags( &($3), 0, MEMFLAG_DISCARDABLE | MEMFLAG_MOVEABLE,
                    MEMFLAG_PURE );
            SemAddResourceFree( $2, WResIDFromNum( OS2_RT_RCDATA ), $3.flags, $4 );
        }
    ;

presparam-list
    : presparam-stmt
        { $$ = SemOS2NewPresParamList( $1 ); }
    | presparam-list presparam-stmt
        { $$ = SemOS2AppendPresParam( $1, $2 ); }
    | /* nothing */
        { $$ = NULL; }
    ;

presparam-stmt
    : Y_PRESPARAMS presparam-name comma-opt raw-data-items
        {
            $$.Name     = $2;
            $$.dataList = $4;
            $$.size     = 0;
        }
    ;

presparam-name
    : name-id
        { $$ = WResIDToNameOrOrd( $1 ); RcMemFree( $1 ); }
    ;

string-table-resource
    : Y_STRINGTABLE string-section
        {
            SemOS2MergeStrTable( $2,
                MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                SemOS2DefaultCodepage() );
        }
    | Y_STRINGTABLE resource-options string-section
        {
            SemOS2CheckResFlags( &($2), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemOS2MergeStrTable( $3, $2.flags, $2.codePage );
        }
    ;

message-table-resource
    : Y_MESSAGETABLE string-section
        {
            SemOS2MergeMsgTable( $2,
                MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE );
        }
    | Y_MESSAGETABLE resource-options string-section
        {
            SemOS2CheckResFlags( &($2), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemOS2MergeMsgTable( $3, $2.flags );
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
            $$ = SemOS2NewStringTable();
            SemOS2AddStrToStringTable( $$, $1.ItemID, $1.String );
            RcMemFree( $1.String );
        }
    | string-items string-item
        {
            SemOS2AddStrToStringTable( $1, $2.ItemID, $2.String );
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

id-value
    : constant-expression
        { $$ = $1.Value; }
    ;

help-table-resource
    : Y_HELPTABLE name-id help-table-section
        {
            SemOS2WriteHelpTable( $2,
                MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, $3 );
        }
    ;

help-table-section
    : Y_BEGIN help-items Y_END
        { $$ = $2; }
    | Y_LBRACE help-items Y_RBRACE
        { $$ = $2; }
    ;

help-items
    : help-item
        { $$ = SemOS2NewHelpTable( $1 ); }
    | help-items help-item
        { $$ = SemOS2AddHelpItem( $2, $1 ); }
    | /* nothing */
        { $$ = NULL; }
    ;

help-item
    : Y_HELPITEM id-value comma-opt id-value comma-opt id-value comma-opt
        { $$ = SemOS2MakeHelpItem( $2, $4, $6 ); }
    ;

help-subtable-resource
    : Y_HELPSUBTABLE name-id help-subtable-section
        {
            SemOS2WriteHelpSubTable( $2, 2,
                MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, $3 );
        }
    | Y_HELPSUBTABLE name-id Y_SUBITEMSIZE constant-expression help-subtable-section
        {
            SemOS2WriteHelpSubTable( $2, $4.Value,
                MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, $5 );
        }
    ;

help-subtable-section
    : Y_BEGIN help-subitems Y_END
        { $$ = $2; }
    | Y_LBRACE help-subitems Y_RBRACE
        { $$ = $2; }
    ;

help-subitems
    : help-subitem
        { $$ = SemOS2NewHelpSubTable( $1 ); }
    | help-subitems help-subitem
        { $$ = SemOS2AddHelpSubItem( $2, $1 ); }
    | /* nothing */
        { $$ = NULL; }
    ;

help-subitem
    : Y_HELPSUBITEM raw-numeric-data-items
        { $$ = $2; }
    ;

accel-table-resource
    : Y_ACCELTABLE name-id acc-section
        {
            SemOS2WriteAccelTable( $2, MEMFLAG_PURE | MEMFLAG_MOVEABLE,
                SemOS2DefaultCodepage(), $3 );
        }
    | Y_ACCELTABLE name-id resource-options acc-section
        {
            SemOS2CheckResFlags( &($3), 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            SemOS2WriteAccelTable( $2, $3.flags, $3.codePage, $4 );
        }
    ;

acc-section
    : Y_BEGIN acc-items Y_END
        { $$ = $2; }
    | Y_LBRACE acc-items Y_RBRACE
        { $$ = $2; }
    ;

acc-items
    : acc-item
        { $$ = SemOS2NewAccelTable( $1 ); }
    | acc-items acc-item
        { $$ = SemOS2AddAccelEntry( $2, $1 ); }
    ;

acc-item
    : acc-event comma-opt acc-cmd
        { $$ = SemOS2MakeAccItem( $1, $3, DefaultAccelFlagsOS2 ); }
    | acc-event comma-opt acc-cmd comma-opt acc-item-options
        { $$ = SemOS2MakeAccItem( $1, $3, $5 ); }
    ;

acc-event
    : string-constant
        {
            $$.event = SemOS2StrToAccelEvent( $1.string );
            $$.strevent = TRUE;
            RcMemFree( $1.string );
        }
    | constant-expression
        {
            $$.event = $1.Value;
            $$.strevent = FALSE;
        }
    ;

acc-cmd
    : constant-expression
        { $$ = $1.Value; }
    ;

acc-item-options
    : acc-item-option
    | acc-item-options comma-opt acc-item-option
        {
            if ($3.flags == OS2_ACCEL_CHAR) {
                $$.flags = $1.flags & ~OS2_ACCEL_VIRTUALKEY;
            } else {
                $$.flags = $1.flags | $3.flags;
            }
            $$.typegiven = $1.typegiven || $3.typegiven;
        }
    ;

acc-item-option
    : Y_ALT
        { $$.flags = OS2_ACCEL_ALT;  $$.typegiven = FALSE; }
    | Y_CHAR
        { $$.flags = OS2_ACCEL_CHAR; $$.typegiven = FALSE; }
    | Y_CONTROL
        { $$.flags = OS2_ACCEL_CTRL; $$.typegiven = FALSE; }
    | Y_HELP
        { $$.flags = OS2_ACCEL_HELP; $$.typegiven = FALSE; }
    | Y_LONEKEY
        { $$.flags = OS2_ACCEL_LONEKEY; $$.typegiven = FALSE; }
    | Y_SCANCODE
        { $$.flags = OS2_ACCEL_SCANCODE; $$.typegiven = FALSE; }
    | Y_SHIFT
        { $$.flags = OS2_ACCEL_SHIFT; $$.typegiven = FALSE; }
    | Y_SYSCOMMAND
        { $$.flags = OS2_ACCEL_SYSCOMMAND; $$.typegiven = FALSE; }
    | Y_VIRTUALKEY
        { $$.flags = OS2_ACCEL_VIRTUALKEY; $$.typegiven = TRUE; }
    ;

menu-resource
    : Y_MENU name-id menu-section
        { SemOS2WriteMenu( $2, MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                    $3, Y_MENU, SemOS2DefaultCodepage() ); }
    | Y_MENU name-id resource-options menu-section
        {
            SemOS2CheckResFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemOS2WriteMenu( $2, $3.flags, $4, Y_MENU, $3.codePage );
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
        { $$ = SemOS2NewMenu( $1 ); }
    | menu-items menu-item
        { $$ = SemOS2AddMenuItem( $1, $2 ); }
    | /* nothing */
        { $$ = NULL; }
    ;

menu-id
    : constant-expression
      { $$ = $1.Value; }
    ;

menuitem-style
    : constant-expression
      { $$ = $1.Value; }
    ;

menuitem-attrib
    : constant-expression
      { $$ = $1.Value; }
    ;

menu-item
    : menu-entry-stmt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.item = $1;
            $$.submenu = NULL;
        }
    | submenu-entry-stmt
        { $$ = $1; }
    ;

menu-entry-stmt
    : Y_MENUITEM menu-entry-defn
        { $$ = $2; }
    ;

submenu-entry-stmt
    : Y_SUBMENU menu-text Y_COMMA menu-id submenu-section
        {
            $$.item.ItemStyle = OS2_MIS_SUBMENU | OS2_MIS_TEXT;
            $$.item.ItemAttrs = 0;
            $$.item.ItemCmd   = $4;
            $$.item.ItemText  = $2.string;
            $$.submenu        = $5;
        }
    | Y_SUBMENU menu-text Y_COMMA menu-id Y_COMMA menuitem-style submenu-section
        {
            $$.item.ItemStyle = OS2_MIS_SUBMENU | $6;
            $$.item.ItemAttrs = 0;
            $$.item.ItemCmd   = $4;
            $$.item.ItemText  = $2.string;
            $$.submenu        = $7;
        }
    | Y_SUBMENU menu-text Y_COMMA menu-id Y_COMMA menuitem-style
                Y_COMMA menuitem-attrib submenu-section
        {
            $$.item.ItemStyle = OS2_MIS_SUBMENU | $6;
            $$.item.ItemAttrs = $8;
            $$.item.ItemCmd   = $4;
            $$.item.ItemText  = $2.string;
            $$.submenu        = $9;
        }
     ;

submenu-section
    : Y_BEGIN menu-items Y_END
        { $$ = $2; }
    | Y_LBRACE menu-items Y_RBRACE
        { $$ = $2; }
    ;

menu-entry-defn
    : Y_SEPARATOR
        {
            $$.ItemStyle = OS2_MIS_SEPARATOR;
            $$.ItemAttrs = OS2_MIA_DISABLED;
            $$.ItemCmd   = -1;
            $$.ItemText  = NULL;
        }
    | menu-text Y_COMMA menu-id
        {
            $$.ItemStyle = OS2_MIS_TEXT;
            $$.ItemAttrs = 0;
            $$.ItemCmd   = $3;
            $$.ItemText  = $1.string;
        }

    | menu-text Y_COMMA menu-id Y_COMMA menuitem-style
        {
            $$.ItemStyle = $5;
            $$.ItemAttrs = 0;
            $$.ItemCmd   = $3;
            $$.ItemText  = $1.string;
        }
    | menu-text Y_COMMA menu-id Y_COMMA Y_COMMA menuitem-attrib
        {
            $$.ItemStyle = OS2_MIS_TEXT;
            $$.ItemAttrs = $6;
            $$.ItemCmd   = $3;
            $$.ItemText  = $1.string;
        }
    | menu-text Y_COMMA menu-id Y_COMMA menuitem-style Y_COMMA menuitem-attrib
        {
            $$.ItemStyle = $5;
            $$.ItemAttrs = $7;
            $$.ItemCmd   = $3;
            $$.ItemText  = $1.string;
        }
    ;

menu-text
    : string-constant
    ;

menu-item-options
    : menu-item-option
        { $$ = SemOS2AddFirstMenuOption( $1 ); }
    | menu-item-options comma-opt menu-item-option
        { $$ = SemOS2AddMenuOption( $1, $3 ); }
    ;

menu-item-option
    : Y_BITMAP
        { $$ = Y_BITMAP; }
    | Y_HELP
        { $$ = Y_HELP; }
    ;

dialogtemplate
    : Y_DLGTEMPLATE
        { $$ = Y_DLGTEMPLATE; }
    | Y_WINDOWTEMPLATE
        { $$ = Y_WINDOWTEMPLATE; }
    ;

dlg-template
    : dialogtemplate name-id diag-control-section
        {
            SemOS2WriteDialogTemplate( $2,
               MEMFLAG_PURE | MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
               SemOS2DefaultCodepage(), $3 );
        }
    | dialogtemplate name-id resource-options diag-control-section
        {
            SemOS2CheckResFlags( &($3), 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                            MEMFLAG_PURE );
            SemOS2WriteDialogTemplate( $2, $3.flags, $3.codePage, $4 );
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

diag-options-stmt
    : menu-stmt
    | font-stmt
    ;

menu-stmt
    : Y_MENU name-id
        {
            $$.token = Y_MENU;
            $$.Opt.Name = WResIDToNameOrOrd( $2 );
            RcMemFree( $2 );
        }
    ;

ctl-class-name
    : string-constant
        { $$ = ResStrToNameOrOrd( $1.string ); RcMemFree( $1.string ); }
    | Y_PUSHBUTTON
        { $$ = ResStrToNameOrOrd( "PUSHBUTTON" ); }
    | Y_COMBOBOX
        { $$ = ResStrToNameOrOrd( "COMBOBOX" ); }
    | Y_ENTRYFIELD
        { $$ = ResStrToNameOrOrd( "ENTRYFIELD" ); }
    | Y_LISTBOX
        { $$ = ResStrToNameOrOrd( "LISTBOX" ); }
    | constant-expression
        { $$ = ResNumToNameOrOrd( $1.Value | 0x80 ); }
    /* A little hack - OS/2 standard window classes are defined like this:
       #define WC_BUTTON ((PSZ)0xffff0003L)
       Since PSZ doesn't mean anything to wrc, it won't recognize the
       constant. So we add a special case to get rid of the PSZ. Not very
       clean but does the job.
     */
    | Y_LPAREN Y_LPAREN Y_PSZ Y_RPAREN constant-expression Y_RPAREN
        { $$ = ResNumToNameOrOrd( $5.Value | 0x80 ); }
    ;

font-stmt
    : Y_FONT comma-opt
        {
            $$.token = Y_FONT;
            $$.Opt.Font.FontName = NULL;
            $$.Opt.Font.FontItalic = 0;
            $$.Opt.Font.FontExtra = 1;
        }
    ;

diag-control-section
    : Y_BEGIN diag-control-stmts Y_END
        { $$ = $2; }
    | Y_LBRACE diag-control-stmts Y_RBRACE
        { $$ = $2; }
    ;

diag-data-elements
    : control-data-section
    | /* Nothing */
      { $$ = NULL; }
    ;

diag-control-stmts
    : diag-control-stmt diag-data-elements
        { $$ = SemOS2NewDiagCtrlList( $1, $2, NULL ); }
    | diag-control-stmts diag-control-stmt diag-data-elements
        { $$ = SemOS2AddDiagCtrlList( $1, $2, $3, NULL ); }
    ;

diag-control-stmt
    : autocheckbox-stmt
    | autoradiobutton-stmt
    | checkbox-stmt
    | combobox-stmt
    | container-stmt
    | ctext-stmt
    | defpushbutton-stmt
    | edittext-stmt
    | groupbox-stmt
    | icon-stmt
    | listbox-stmt
    | ltext-stmt
    | mle-stmt
    | notebook-stmt
    | pushbutton-stmt
    | radiobutton-stmt
    | rtext-stmt
    | slider-stmt
    | spinbutton-stmt
    | valueset-stmt
    | control-stmt
    | dialog-stmt
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
    : comma-opt cntl-id comma-opt size-info comma-opt
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style.Mask = 0;
            $$.Style.Value = 0;
            $$.Text = NULL;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt cntl-style
        {
            $$.ID = $2;
            $$.Size = $4;
            $$.Style = $6;
            $$.Text = NULL;
        }
    ;

cntl-style
    : constant-expression
      { $$ = $1; }
    ;

frame-style
    : constant-expression
      { $$ = $1; }
    ;

cntl-id
    : constant-expression
        { $$ = $1.Value; }
    ;

autocheckbox-stmt
    : Y_AUTOCHECKBOX cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_AUTOCHECKBOX, $2, $3 ); }
    ;

autoradiobutton-stmt
    : Y_AUTORADIOBUTTON cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_AUTORADIOBUTTON, $2, $3 ); }
    ;

checkbox-stmt
    : Y_CHECKBOX cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_CHECKBOX, $2, $3 ); }
    ;

combobox-stmt
    : Y_COMBOBOX cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_COMBOBOX, $2, $3 ); }
    ;

container-stmt
    : Y_CONTAINER cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_CONTAINER, $2, $3 ); }
    ;

ctext-stmt
    : Y_CTEXT cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_CTEXT, $2, $3 ); }
    ;

defpushbutton-stmt
    : Y_DEFPUSHBUTTON cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_DEFPUSHBUTTON, $2, $3 ); }
    ;

edittext-stmt
    : Y_EDITTEXT cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_EDITTEXT, $2, $3 ); }
    | Y_ENTRYFIELD cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_EDITTEXT, $2, $3 ); }
    ;

groupbox-stmt
    : Y_GROUPBOX cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_GROUPBOX, $2, $3 ); }
    ;

listbox-stmt
    : Y_LISTBOX cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_LISTBOX, $2, $3 ); }
    ;

ltext-stmt
    : Y_LTEXT cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_LTEXT, $2, $3 ); }
    ;

mle-stmt
    : Y_MLE cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_MLE, $2, $3 ); }
    ;

notebook-stmt
    : Y_NOTEBOOK cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_NOTEBOOK, $2, $3 ); }
    ;

pushbutton-stmt
    : Y_PUSHBUTTON cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_PUSHBUTTON, $2, $3 ); }
    ;

radiobutton-stmt
    : Y_RADIOBUTTON cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_RADIOBUTTON, $2, $3 ); }
    ;

rtext-stmt
    : Y_RTEXT cntl-text-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_RTEXT, $2, $3 ); }
    ;

slider-stmt
    : Y_SLIDER cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_SLIDER, $2, $3 ); }
    ;

spinbutton-stmt
    : Y_SPINBUTTON cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_SPINBUTTON, $2, $3 ); }
    ;

valueset-stmt
    : Y_VALUESET cntl-options presparam-list
        { $$ = SemOS2NewDiagCtrl( Y_VALUESET, $2, $3 ); }
    ;

icon-stmt
    : Y_ICON control-name comma-opt cntl-id comma-opt icon-parms
        { $6.Text = $2; $6.ID = $4; $$ = SemOS2NewDiagCtrl( Y_ICON, $6, NULL ); }
    ;

icon-parms
    : size-x comma-opt size-y comma-opt size-w comma-opt size-h
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = $5;
            $$.Size.height = $7;
            $$.Style.Mask  = 0;         /* no style given */
            $$.Style.Value = 0;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h comma-opt cntl-style
        {
            $$.Size.x = $1;
            $$.Size.y = $3;
            $$.Size.width = $5;
            $$.Size.height = $7;
            $$.Style = $9;
        }
    ;

control-name
    : name-id
        { $$ = WResIDToNameOrOrd( $1 ); RcMemFree( $1 ); }
    ;

/* OS/2 accepts only numbers or strings for control statement text field */
control-stmt
    : Y_CONTROL cntl-text comma-opt cntl-id comma-opt size-info comma-opt
                ctl-class-name presparam-list
        {
            IntMask mask = {0};
            $$ = SemOS2SetControlData( $2, $4, $6, $8, mask, NULL, $9 );
        }
    | Y_CONTROL cntl-text comma-opt cntl-id comma-opt size-info comma-opt
                ctl-class-name comma-opt cntl-style presparam-list
        {
            $$ = SemOS2SetControlData( $2, $4, $6, $8, $10, NULL, $11 );
        }
    ;

cntl-text
    : string-constant
        { $$ = ResStrToNameOrOrd( $1.string ); RcMemFree( $1.string ); }
    | constant-expression
        { $$ = ResNumToNameOrOrd( $1.Value ); }
    | Y_NAME
        {
            $$ = ResNumToNameOrOrd( 0 );
            RcError( ERR_SYMBOL_NOT_DEFINED, $1.string );
            ErrorHasOccured = TRUE;
        }
    ;

dialog-or-frame
    : Y_DIALOG
        { $$ = Y_DIALOG; }
    | Y_FRAME
        { $$ = Y_FRAME; }
    ;

dialog-stmt
    : dialog-or-frame cntl-text-options presparam-list
        {
            IntMask mask = {0};
            $$ = SemOS2SetWindowData( $2, mask, $3, NULL, $1 );
        }
    | dialog-or-frame cntl-text-options Y_COMMA frame-style presparam-list
        {
            $$ = SemOS2SetWindowData( $2, $4, $5, NULL, $1 );
        }
    | dialog-or-frame cntl-text-options presparam-list diag-control-section
        {
            IntMask mask = {0};
            $$ = SemOS2SetWindowData( $2, mask, $3, $4, $1 );
        }
    | dialog-or-frame cntl-text-options Y_COMMA frame-style presparam-list diag-control-section
        {
            $$ = SemOS2SetWindowData( $2, $4, $5, $6, $1 );
        }
    ;

string-constant
    : string-group
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
    | Y_NOT_KEYWORD unary-exp
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
