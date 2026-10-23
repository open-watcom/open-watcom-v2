::
:: yacc DLGINIT MFC private resource type 0xF0
:: from reserved range 0xF0–0xFF
::
:segment TYPES
%type <resloc>          dlginit-defined-data
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
:endsegment
