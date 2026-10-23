::
:: yacc DIALOG resource types and grammar
::
:segment TYPES
%type <diagopts>        font-stmt
%type <diagopts>        dialog-options-stmt
%type <diaghead>        dialog-options-section
%type <diagctrllist>    dialog-control-section
%type <diagctrllist>    dialog-control-stmts
%type <diagctrl>        dialog-control-stmt
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
%type <diagctrlopts>    icon-parms
:elsesegment
dialog-resource
    : name-id Y_DIALOG resource-options-mdp comma-opt size-info
            dialog-options-section dialog-control-section
        { SemWINWriteDialogBox( $1, $3, $5, $6, $7, 0, false ); }
    ;

dialog-options-section
    : /* nothing */
        { $$ = NULL; }
    | dialog-options-section dialog-options-stmt
        { $$ = SemWINDialogOptions( $1, &($2) ); }
    ;

dialog-options-stmt
    : style-stmt
    | exstyle-stmt
    | exstyle-equal-stmt
    | caption-stmt
    | menu-stmt
    | class-stmt
    | font-stmt
:segment WIN32
    | dialog-lang-stmt
    | dialog-version-stmt
    | dialog-characteristics-stmt
:endsegment
    ;

font-stmt
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
    ;

dialog-control-section
    : Y_BEGIN dialog-control-stmts Y_END
        { $$ = $2; }
    | Y_LBRACE dialog-control-stmts Y_RBRACE
        { $$ = $2; }
    | Y_BEGIN Y_END
        { $$ = SemWINEmptyDialogCtrlList(); }
    | Y_LBRACE Y_RBRACE
        { $$ = SemWINEmptyDialogCtrlList(); }
    ;

dialog-control-stmts
    : dialog-control-stmt dialog-data-elements
        { $$ = SemWINAddDialogCtrlList( NULL, $1, $2 ); }
    | dialog-control-stmts dialog-control-stmt dialog-data-elements
        { $$ = SemWINAddDialogCtrlList( $1, $2, $3 ); }
    ;

dialog-control-stmt
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
            $2.Text = ResStrToNameOrOrdinal( $1.string );
            MemFree( $1.string );
            $$ = $2;
        }
    ;

cntl-options
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
    ;

ltext-stmt
    : Y_LTEXT cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_LTEXT, $2 ); }
    ;

rtext-stmt
    : Y_RTEXT cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_RTEXT, $2 ); }
    ;

ctext-stmt
    : Y_CTEXT cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_CTEXT, $2 ); }
    ;

autocheckbox-stmt
    : Y_AUTOCHECKBOX cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_AUTOCHECKBOX, $2 ); }
    ;

autoradiobutton-stmt
    : Y_AUTORADIOBUTTON cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_AUTORADIOBUTTON, $2 ); }
    ;

auto3state-stmt
    : Y_AUTO3STATE cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_AUTO3STATE, $2 ); }
    ;

checkbox-stmt
    : Y_CHECKBOX cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_CHECKBOX, $2 ); }
    ;

pushbutton-stmt
    : Y_PUSHBUTTON cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_PUSHBUTTON, $2 ); }
    ;

listbox-stmt
    : Y_LISTBOX cntl-options
        { $$ = SemWINNewDialogCtrl( Y_LISTBOX, $2 ); }
    ;

groupbox-stmt
    : Y_GROUPBOX cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_GROUPBOX, $2 ); }
    ;

defpushbutton-stmt
    : Y_DEFPUSHBUTTON cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_DEFPUSHBUTTON, $2 ); }
    ;

radiobutton-stmt
    : Y_RADIOBUTTON cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_RADIOBUTTON, $2 ); }
    ;

edittext-stmt
    : Y_EDITTEXT cntl-options
        { $$ = SemWINNewDialogCtrl( Y_EDITTEXT, $2 ); }
    ;

combobox-stmt
    : Y_COMBOBOX cntl-options
        { $$ = SemWINNewDialogCtrl( Y_COMBOBOX, $2 ); }
    ;

icon-stmt
    : Y_ICON icon-name comma-opt cntl-id comma-opt icon-parms
        { $6.Text = $2; $6.ID = $4; $$ = SemWINNewDialogCtrl( Y_ICON, $6 ); }
    ;

state3-stmt
    : Y_STATE3 cntl-text-options
        { $$ = SemWINNewDialogCtrl( Y_STATE3, $2 ); }
    ;

icon-parms
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

scrollbar-stmt
    : Y_SCROLLBAR cntl-options
        { $$ = SemWINNewDialogCtrl( Y_SCROLLBAR, $2 ); }
    ;

control-stmt
    : Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info
        { $$ = SemWINSetControlData( $8, $4, $10, $2, $6, 0, 0 ); }
    | Y_CONTROL cntl-text comma-opt cntl-id comma-opt ctl-class-name comma-opt
                    style comma-opt size-info comma-opt exstyle
        { $$ = SemWINSetControlData( $8, $4, $10, $2, $6, $12.Value, 0 ); }
    ;

:endsegment
