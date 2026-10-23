::
:: LANGUAGE statement
::
:segment TYPES
%type <langinfo>        language-stmt
%type <diagopts>        dialog-lang-stmt
%type <diagopts>        dialog-version-stmt
%type <diagopts>        dialog-characteristics-stmt
:elsesegment
language-resource
    : language-stmt
        { SemWINSetGlobalLanguage( &$1 ); }
    ;

dialog-lang-stmt
    : language-stmt
        { $$.token = Y_LANGUAGE; $$.Opt.lang = $1; }
    ;

dialog-version-stmt
    : version-stmt
        { $$.token = Y_VERSION; }
    ;

dialog-characteristics-stmt
    : characteristics-stmt
        { $$.token = Y_CHARACTERISTICS; }
    ;

resource-info-stmts
    : resource-info-stmt
    | resource-info-stmts resource-info-stmt
    ;

resource-info-stmt
    : language-stmt
        { SemWINSetResourceLanguage( &$1, true ); }
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

:endsegment
