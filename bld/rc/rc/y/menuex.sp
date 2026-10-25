::
:: yacc MENUEX resource types and grammar
::
:segment TYPES
%type <menuptr>         menuex-section
%type <menuptr>         menuex-items
%type <menufull>        menuex-item
%type <menufull>        menuex-popup-stmt
%type <menuitem>        menuex-popup-defn
%type <menuitem>        menuex-entry-stmt
%type <menuitem>        menuex-entry-defn
%type <residnum>        menuId
%type <integral>        menuType
%type <integral>        menuState
:elsesegment
menuex-resource
    : name-id Y_MENU_EX resource-options-mdp menuex-section
        { SemWINWriteMenu( $1, $3, $4, true ); }
    ;

menuex-section
    : Y_BEGIN menuex-items Y_END
        { $$ = $2; }
    | Y_LBRACE menuex-items Y_RBRACE
        { $$ = $2; }
    ;

menuex-items
    : menuex-item
        { $$ = SemWINAddMenuItem( NULL, $1 ); }
    | menuex-items menuex-item
        { $$ = SemWINAddMenuItem( $1, $2 ); }
    ;

menuex-item
    : menuex-entry-stmt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.item = $1;
            $$.item.u.ExFlags = 0;
            $$.item.IsOldWin = false;
            $$.submenu = NULL;
        }
    | menuex-popup-stmt
        { $$ = $1; }
    ;

menuex-popup-stmt
    : Y_POPUP menu-text menuex-popup-defn menuex-section comma-opt
        {
            $$.next = NULL;
            $$.prev = NULL;
            $$.item = $3;
            $$.item.ItemText = $2.string;
            $$.item.u.ExFlags = MENUEX_POPUP;
            $$.item.IsOldWin = false;
            $$.submenu = $4;
        }
    ;

menuex-popup-defn
    : /* nothing */
        {
            $$.ItemId = 0;
            $$.ExType = 0;
            $$.ExState = 0;
            $$.ExHelpId = 0;
        }
    | comma-opt menuId
        {
            $$.ItemId = $2;
            $$.ExType = 0;
            $$.ExState = 0;
            $$.ExHelpId = 0;
        }
    | comma-opt menuId comma-opt menuType
        {
            $$.ItemId = $2;
            $$.ExType = $4;
            $$.ExState = 0;
            $$.ExHelpId = 0;
        }
    | comma-opt menuId comma-opt menuType comma-opt menuState
        {
            $$.ItemId = $2;
            $$.ExType = $4;
            $$.ExState = $6;
            $$.ExHelpId = 0;
        }
    | comma-opt menuId comma-opt menuType comma-opt menuState comma-opt helpId
        {
            $$.ItemId = $2;
            $$.ExType = $4;
            $$.ExState = $6;
            $$.ExHelpId = $8;
        }
    ;

menuex-entry-stmt
    : Y_MENUITEM Y_SEPARATOR comma-opt
        {
            $$.ItemText = NULL;
            $$.ItemId = 0;
            $$.ExType = MENUEX_TYPE_SEPARATOR;
            $$.ExState = 0;
        }
    | Y_MENUITEM menu-text menuex-entry-defn
        {
            $$ = $3;
            $$.ItemText = $2.string;
        }
    ;

menuex-entry-defn
    : /* nothing */
        {
            $$.ItemId = 0;
            $$.ExType = 0;
            $$.ExState = 0;
        }
    | comma-opt menuId
        {
            $$.ItemId = $2;
            $$.ExType = 0;
            $$.ExState = 0;
        }
    | comma-opt menuId comma-opt menuType
        {
            $$.ItemId = $2;
            $$.ExType = $4;
            $$.ExState = 0;
        }
    | comma-opt menuId comma-opt menuType comma-opt menuState
        {
            $$.ItemId = $2;
            $$.ExType = $4;
            $$.ExState = $6;
        }
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
:endsegment
