::
:: yacc DIALOGEX resource types and grammar
::
:segment TYPES
%type <diagopts>        font-stmt-ex
%type <resbyte>         fontitalic
%type <ressizenum>      fontweight
%type <resbyte>         fontcharset
%type <diagopts>        dialogex-options-stmt
%type <diaghead>        dialogex-options-section
%type <diagctrllist>    dialogex-control-section
%type <diagctrllist>    dialogex-control-stmts
%type <diagctrl>        dialogex-control-stmt
%type <diagctrlopts>    cntl-text-options-ex
%type <diagctrlopts>    cntl-options-ex
%type <diagctrl>        ltext-stmt-ex
%type <diagctrl>        rtext-stmt-ex
%type <diagctrl>        ctext-stmt-ex
%type <diagctrl>        autocheckbox-stmt-ex
%type <diagctrl>        autoradiobutton-stmt-ex
%type <diagctrl>        auto3state-stmt-ex
%type <diagctrl>        checkbox-stmt-ex
%type <diagctrl>        pushbutton-stmt-ex
%type <diagctrl>        listbox-stmt-ex
%type <diagctrl>        groupbox-stmt-ex
%type <diagctrl>        defpushbutton-stmt-ex
%type <diagctrl>        radiobutton-stmt-ex
%type <diagctrl>        edittext-stmt-ex
%type <diagctrl>        combobox-stmt-ex
%type <diagctrl>        icon-stmt-ex
%type <diagctrl>        scrollbar-stmt-ex
%type <diagctrl>        control-stmt-ex
%type <diagctrl>        state3-stmt-ex
%type <diagctrlopts>    icon-parms-ex
:elsesegment
dialogex-resource
    : name-id Y_DIALOG_EX resource-options-mdp comma-opt size-info
            helpId-opt
            dialogex-options-section dialogex-control-section
        { SemWINWriteDialogBox( $1, $3, $5, $7, $8, $6, true ); }
    ;

dialogex-options-section
    : /* nothing */
        { $$ = NULL; }
    | dialogex-options-section dialogex-options-stmt
        { $$ = SemWINDialogOptions( $1, &($2) ); }
    ;

dialogex-options-stmt
    : style-stmt
    | exstyle-stmt
    | exstyle-equal-stmt
    | caption-stmt
    | menu-stmt
    | class-stmt
    | font-stmt-ex
    | dialog-lang-stmt
    | dialog-version-stmt
    | dialog-characteristics-stmt
    ;

font-stmt-ex
    : Y_FONT point-size comma-opt typeface
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = 0;
            $$.Opt.Font.FontItalic = 0;
            $$.Opt.Font.FontCharset = DEFAULT_CHARSET;
            $$.Opt.Font.FontWeightDefined = false;
            $$.Opt.Font.FontItalicDefined = false;
            $$.Opt.Font.FontCharsetDefined = false;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = 0;
            $$.Opt.Font.FontCharset = DEFAULT_CHARSET;
            $$.Opt.Font.FontWeightDefined = true;
            $$.Opt.Font.FontItalicDefined = false;
            $$.Opt.Font.FontCharsetDefined = false;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight comma-opt fontitalic
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = $8;
            $$.Opt.Font.FontCharset = DEFAULT_CHARSET;
            $$.Opt.Font.FontWeightDefined = true;
            $$.Opt.Font.FontItalicDefined = true;
            $$.Opt.Font.FontCharsetDefined = false;
        }
    | Y_FONT point-size comma-opt typeface comma-opt fontweight comma-opt fontitalic comma-opt fontcharset
        {
            $$.token = Y_FONT;
            $$.Opt.Font.PointSize = $2;
            $$.Opt.Font.FontName = $4.string;
            $$.Opt.Font.FontWeight = $6;
            $$.Opt.Font.FontItalic = $8;
            $$.Opt.Font.FontCharset = $10;
            $$.Opt.Font.FontWeightDefined = true;
            $$.Opt.Font.FontItalicDefined = true;
            $$.Opt.Font.FontCharsetDefined = true;
        }
    ;

fontcharset
    : constant-expression
        { $$ = $1.Value & 0xFF; }
    ;

fontweight
    : constant-expression
        { $$ = $1.Value; }
    ;

fontitalic
    : constant-expression
        { $$ = ( $1.Value != 0 ); }
    ;

dialogex-control-section
    : Y_BEGIN dialogex-control-stmts Y_END
        { $$ = $2; }
    | Y_LBRACE dialogex-control-stmts Y_RBRACE
        { $$ = $2; }
    | Y_BEGIN Y_END
        { $$ = SemWINEmptyDialogCtrlList(); }
    | Y_LBRACE Y_RBRACE
        { $$ = SemWINEmptyDialogCtrlList(); }
    ;

dialogex-control-stmts
    : dialogex-control-stmt dialog-data-elements
        { $$ = SemWINAddDialogCtrlList( NULL, $1, $2 ); }
    | dialogex-control-stmts dialogex-control-stmt dialog-data-elements
        { $$ = SemWINAddDialogCtrlList( $1, $2, $3 ); }
    ;

dialogex-control-stmt
    : ltext-stmt-ex
    | rtext-stmt-ex
    | ctext-stmt-ex
    | autocheckbox-stmt-ex
    | autoradiobutton-stmt-ex
    | auto3state-stmt-ex
    | checkbox-stmt-ex
    | pushbutton-stmt-ex
    | listbox-stmt-ex
    | groupbox-stmt-ex
    | defpushbutton-stmt-ex
    | radiobutton-stmt-ex
    | edittext-stmt-ex
    | combobox-stmt-ex
    | icon-stmt-ex
    | scrollbar-stmt-ex
    | state3-stmt-ex
    | control-stmt-ex
    ;

cntl-text-options-ex
    : string-constant cntl-options-ex
        {
            $2.Text = ResStrToNameOrOrdinal( $1.string );
            MemFree( $1.string );
            $$ = $2;
        }
    ;

cntl-options-ex
    : comma-opt cntl-id comma-opt size-info
        {
            $$.ID = $2;
            $$.SizeInfo = $4;
            $$.Style.Mask = 0;
            $$.Text = NULL;
            $$.ExtendedStyle = 0;
            $$.HelpId = 0;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style
        {
            $$.ID = $2;
            $$.SizeInfo = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = 0;
            $$.HelpId = 0;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style comma-opt exstyle
        {
            $$.ID = $2;
            $$.SizeInfo = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = $8.Value;
            $$.HelpId = 0;
        }
    | comma-opt cntl-id comma-opt size-info comma-opt style comma-opt
           exstyle comma-opt helpId
        {
            $$.ID = $2;
            $$.SizeInfo = $4;
            $$.Style = $6;
            $$.Text = NULL;
            $$.ExtendedStyle = $8.Value;
            $$.HelpId = $10;
        }
    ;

ltext-stmt-ex
    : Y_LTEXT cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_LTEXT, $2 ); }
    ;

rtext-stmt-ex
    : Y_RTEXT cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_RTEXT, $2 ); }
    ;

ctext-stmt-ex
    : Y_CTEXT cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_CTEXT, $2 ); }
    ;

autocheckbox-stmt-ex
    : Y_AUTOCHECKBOX cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_AUTOCHECKBOX, $2 ); }
    ;

autoradiobutton-stmt-ex
    : Y_AUTORADIOBUTTON cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_AUTORADIOBUTTON, $2 ); }
    ;

auto3state-stmt-ex
    : Y_AUTO3STATE cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_AUTO3STATE, $2 ); }
    ;

checkbox-stmt-ex
    : Y_CHECKBOX cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_CHECKBOX, $2 ); }
    ;

pushbutton-stmt-ex
    : Y_PUSHBUTTON cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_PUSHBUTTON, $2 ); }
    ;

listbox-stmt-ex
    : Y_LISTBOX cntl-options-ex
        { $$ = SemWINNewDialogCtrl( Y_LISTBOX, $2 ); }
    ;

groupbox-stmt-ex
    : Y_GROUPBOX cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_GROUPBOX, $2 ); }
    ;

defpushbutton-stmt-ex
    : Y_DEFPUSHBUTTON cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_DEFPUSHBUTTON, $2 ); }
    ;

radiobutton-stmt-ex
    : Y_RADIOBUTTON cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_RADIOBUTTON, $2 ); }
    ;

edittext-stmt-ex
    : Y_EDITTEXT cntl-options-ex
        { $$ = SemWINNewDialogCtrl( Y_EDITTEXT, $2 ); }
    ;

combobox-stmt-ex
    : Y_COMBOBOX cntl-options-ex
        { $$ = SemWINNewDialogCtrl( Y_COMBOBOX, $2 ); }
    ;

icon-stmt-ex
    : Y_ICON icon-name comma-opt cntl-id comma-opt icon-parms-ex
        { $6.Text = $2; $6.ID = $4; $$ = SemWINNewDialogCtrl( Y_ICON, $6 ); }
    ;

state3-stmt-ex
    : Y_STATE3 cntl-text-options-ex
        { $$ = SemWINNewDialogCtrl( Y_STATE3, $2 ); }
    ;

icon-parms-ex
    : size-x comma-opt size-y
        {
            $$.SizeInfo.x = $1;
            $$.SizeInfo.y = $3;
            $$.SizeInfo.width = 0;          /* ignore width, height, style */
            $$.SizeInfo.height = 0;
            $$.Style.Mask = 0;
            $$.ExtendedStyle = 0;
        }
    | size-x comma-opt size-y comma-opt style
        {
            $$.SizeInfo.x = $1;
            $$.SizeInfo.y = $3;
            $$.SizeInfo.width = 0;          /* ignore width, height */
            $$.SizeInfo.height = 0;
            $$.Style = $5;
            $$.ExtendedStyle = 0;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h
        {
            $$.SizeInfo.x = $1;
            $$.SizeInfo.y = $3;
            $$.SizeInfo.width = $5;         /* ignore style */
            $$.SizeInfo.height = $7;
            $$.Style.Mask = 0;
            $$.ExtendedStyle = 0;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h comma-opt style
        {
            $$.SizeInfo.x = $1;
            $$.SizeInfo.y = $3;
            $$.SizeInfo.width = $5;
            $$.SizeInfo.height = $7;
            $$.Style = $9;
            $$.ExtendedStyle = 0;
        }
    | size-x comma-opt size-y comma-opt size-w comma-opt size-h comma-opt style
             comma-opt exstyle
        {
            $$.SizeInfo.x = $1;
            $$.SizeInfo.y = $3;
            $$.SizeInfo.width = $5;
            $$.SizeInfo.height = $7;
            $$.Style = $9;
            $$.ExtendedStyle = $11.Value;
        }
    ;

scrollbar-stmt-ex
    : Y_SCROLLBAR cntl-options-ex
        { $$ = SemWINNewDialogCtrl( Y_SCROLLBAR, $2 ); }
    ;

control-stmt-ex
    : Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info
        { $$ = SemWINSetControlData( $8, $4, $10, $2, $6, 0, 0 ); }

    | Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info comma-opt exstyle helpId-opt
        { $$ = SemWINSetControlData( $8, $4, $10, $2, $6, $12.Value, $13 ); }
    ;

:endsegment
