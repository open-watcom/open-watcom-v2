::
:: yacc TOOLBAR MFC private resource type 0xF1
:: from reserved range 0xF0–0xFF
::
:segment TYPES
%type <toolbar>         toolbar-block
%type <toolbar>         toolbar-items
%type <residnum>        toolbar-item
:elsesegment
toolbar-resource
    : name-id Y_TOOLBAR resource-options-mp constant-expression comma-opt constant-expression toolbar-block
        { SemWINWriteToolBar( $1, $7, $4.Value, $6.Value, $3 ); }
    ;

toolbar-block
    : Y_BEGIN toolbar-items Y_END
        { $$ = $2; }
    | Y_LBRACE toolbar-items Y_RBRACE
        { $$ = $2; }
    ;

toolbar-items
    : toolbar-item
        { $$ = SemWINAddToolBarItem( NULL, $1 ); }
    | toolbar-items toolbar-item
        { $$ = SemWINAddToolBarItem( $1, $2 ); }
    ;

toolbar-item
    : Y_BUTTON constant-expression
        { $$ = $2.Value; }
    | Y_SEPARATOR
        { $$ = 0; }
    ;
:endsegment
