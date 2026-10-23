::
:: MFC private resource types
:: from reserved range 0xF0–0xFF
::
:: DLGINIT  0xF0
:: TOOLBAR  0xF1
::
:segment TYPES
%type <resloc>          dlginit-defined-data
%type <toolbar>         toolbar-block
%type <toolbar>         toolbar-items
%type <residnum>        toolbar-item
:elsesegment
dlginit-resource
    : name-id Y_DLGINIT resource-options-mdp dlginit-defined-data
        { SemAddResourceAndFree( $1, WResIDFromNum( 0xF0 ), $3, $4 ); }
    ;

dlginit-defined-data
    : file-name
        { $$ = SemCopyRawFile( $1.string ); MemFree( $1.string ); }
    | raw-data-section
        { $$ = SemFlushDataElemList( $1, true ); }
    ;

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
