%{  /* include files */
    #include <stdio.h>  // printf debugging
    #include <assert.h>

    #include "dialog.h"
    #include "styles.h"

#ifdef __WATCOMC__    
    #pragma warning 17 5        // shut off the unreferenced goto warning
#endif

    #define YYPARSER DialogParser

    extern Dialog *     CurrDialog;
%}

%token T_String
%token T_Number
%token T_Ident

%token T_DialogInclude
%token T_RCData
%token T_Preload
%token T_Moveable
%token T_Discardable
%token T_Fixed
%token T_Impure

%token T_Dialog
%token T_Caption
%token T_Style
%token T_Font

%token T_Begin
%token T_End

%token T_PushButton
%token T_DefPushButton
%token T_CheckBox
%token T_RadioButton
%token T_GroupBox
%token T_LText
%token T_CText
%token T_RText
%token T_Icon
%token T_EditText
%token T_ScrollBar
%token T_ListBox
%token T_ComboBox
%token T_Control

%token ','
%token '{'
%token '}'
%token '|'

%%

goal
        : header dialog_list 
        ;

header
        : T_DialogInclude T_RCData memflag_list block_begin string_list block_end
        ;

memflag_list
        : memflag
        | memflag memflag_list
        ;

memflag
        : T_Preload
        | T_Moveable
        | T_Discardable
        ;


dialog_list
        : dialog
        | dialog dialog_list
        ;

string_list
        : T_String
        | T_String string_list
        ;

dialog
        : dialog_line dialog_modifier_list dialog_block
          { 
            /* if this dialogs were stored in a list, they'd be placed here */
          }
        ;

dialog_line
          /* can't use <rectangle> to match the rect since CurrDialog isn't set yet */
        : T_Ident T_Dialog T_Number ',' T_Number ',' T_Number ',' T_Number
          { 
            assert( CurrDialog == NULL );

            if( CurrDialog != NULL ) {
                delete CurrDialog;
            }
            CurrDialog = new Dialog( _scanner->getIdent( $1 ), Rect( $3, $5, $7, $9 ) );
          }
        | T_Ident T_Dialog dialog_header_list T_Number ',' T_Number ',' T_Number ',' T_Number
          { 
            assert( CurrDialog == NULL );
            CurrDialog = new Dialog( _scanner->getIdent( $1 ), Rect( $4, $6, $8, $10 ) );
          }
        ;

dialog_header_list
        : dialog_header
        | dialog_header dialog_header_list
        ;

dialog_header
        : T_Fixed
        | T_Impure
        ;

dialog_modifier_list
        : dialog_modifier
        | dialog_modifier dialog_modifier_list
        ;

dialog_modifier
        : style_line
        | caption_line
        | font_line
        ;

style_line
        : T_Style style_list
          { CurrDialog->setStyle( $2 ); }
        ;

style_list
        : style
          { $$ = $1; }
        | style '|' style_list
          { $$ = $1 | $3; }
        ;
style
        : T_Ident
          { $$ = getWindowsStyle( _scanner->getIdent( $1 ) ); } 
        | T_Number
          { $$ = $1; }
        ;

caption_line
        : T_Caption T_String
          { CurrDialog->setCaption( _scanner->getString( $2 ) ); }
        ;

font_line
        : T_Font T_Number ',' T_String
          { CurrDialog->setFont( $2, _scanner->getString( $4 ) ); }
        ;

dialog_block 
        : block_begin statement_list block_end
        ;

statement_list
        : statement
        | statement statement_list
        ;

statement
        : control_statement
          /* other types of statements, like pushbutton, defpushbutton, etc */
          /* should be added.  just pass in a static string with the class name, */
          /* and or in the appropriate default style */
        ;

control_statement
        : T_Control T_String ',' T_Ident ',' T_String ',' style_list ',' rectangle
          { 
            CurrDialog->addControl( _scanner->getString( $2 ), _scanner->getIdent( $4 ), _scanner->getString( $6 ), $8, $10 );
          }
        | T_Control T_Ident ',' T_Ident ',' T_String ',' style_list ',' rectangle
          { 
            CurrDialog->addControl( _scanner->getIdent( $2 ), _scanner->getIdent( $4 ), _scanner->getString( $6 ), $8, $10 );
          }
        ;

block_begin
        : T_Begin
        | '{'
        ;

block_end
        : T_End
        | '}'
        ;

rectangle
        : T_Number ',' T_Number ',' T_Number ',' T_Number
          { 
            $$ = (YYSTYPE)CurrDialog->addRect( Rect( $1, $3, $5, $7 ) );
          }
        ;

%%

