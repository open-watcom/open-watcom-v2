::
:: yacc MENU resource types and grammar
::
:segment TYPES
%type <menuptr>         menu-section
%type <menuptr>         menu-items
%type <menufull>        menu-item
%type <menufull>        menu-popup-stmt
%type <menuitem>        menu-popup-defn
%type <menuitem>        menu-entry-stmt
%type <menuitem>        menu-entry-defn
%type <residnum>        menu-result
:elsesegment
menu-resource
    : name-id Y_MENU resource-options-mdp opt-resource-info-stmts menu-section
        { SemWINWriteMenu( $1, $3, $5, false ); }
    ;

menu-section
    : Y_BEGIN menu-items Y_END
        { $$ = $2; }
    | Y_LBRACE menu-items Y_RBRACE
        { $$ = $2; }
    ;

menu-items
    : menu-item
        { $$ = SemWINAddMenuItem( NULL, $1 ); }
    | menu-items menu-item
        { $$ = SemWINAddMenuItem( $1, $2 ); }
    ;

menu-item
    : menu-entry-stmt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.iswin32 = CmdLineParms.iswin32;
            $$.item = $1;
            $$.item.ExType = 0;
            $$.item.ExState = 0;
            $$.item.IsOldWin = ( CmdLineParms.winver < 30 );
            $$.submenu = NULL;
        }
    | menu-popup-stmt
        { $$ = $1; }
    ;

menu-popup-stmt
    : Y_POPUP menu-text menu-popup-defn menu-section comma-opt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.iswin32 = CmdLineParms.iswin32;
            $$.item = $3;
            $$.item.ItemText = $2.string;
            $$.item.u.ItemFlags |= MENU_POPUP;
            $$.item.ItemId = 0;
            $$.item.ExType = 0;
            $$.item.ExState = 0;
            $$.item.ExHelpId = 0;
            $$.item.IsOldWin = ( CmdLineParms.winver < 30 );
            $$.submenu = $4;
        }
    ;

menu-popup-defn
    : /* nothing */
        { $$.u.ItemFlags = 0; }
    | comma-opt menu-item-options
        { $$.u.ItemFlags = $2; }
    ;

menu-entry-stmt
    : Y_MENUITEM Y_SEPARATOR comma-opt
        {
            $$.ItemText = NULL;
            $$.ItemId = 0;
            $$.u.ItemFlags = MENU_SEPARATOR;
        }
    | Y_MENUITEM menu-text comma-opt menu-result menu-entry-defn
        {
            $$ = $5;
            $$.ItemText = $2.string;
            $$.ItemId = $4;
        }
    ;

menu-entry-defn
    : /* nothing */
        { $$.u.ItemFlags = 0; }
    | comma-opt menu-item-options
        { $$.u.ItemFlags = $2; }
    ;

menu-text
    : string-constant
    ;

menu-result
    : constant-expression
        { $$ = $1.Value; }
    ;
:endsegment
