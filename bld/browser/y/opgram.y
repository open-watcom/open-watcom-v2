%{  /* include files */
    #include <wstd.h>
    #include "opscancm.h"
    #include "optmgr.h"
    #include "wbrwin.h"
    #include "assure.h"
    #include "opgram.h"

#pragma warning 17 5	// shut off the unreferenced goto warning
#pragma warning 389 5	// shut off the integral truncated warning

%}

%token T_MethFilt
%token T_MemFilt
%token T_BothFilt
%token T_File
%token T_Disabled
%token T_TreeView
%token T_Edges
%token T_EnumStyle
%token T_SearchType
%token T_IgnoreCase
%token T_Magic
%token T_MagicChars
%token T_MergeFile

%token T_All
%token T_Visible
%token T_Local
%token T_Public
%token T_Protected
%token T_Private
%token T_Static
%token T_NonStatic
%token T_Virtual
%token T_NonVirtual
%token T_Vertical
%token T_Horizontal
%token T_SquareEdges
%token T_StraightEdges
%token T_Hex
%token T_Decimal
%token T_Octal
%token T_StartsWith
%token T_Contains
%token T_On
%token T_Off
%token T_None
%token T_Some

%token '~'
%token '!'
%token ','
%token '@'
%token '='
%token '{'
%token '}'

%token T_String
%token T_MagicString
%token T_FileSync

%%

init	: { init(); } goal
	;

goal
	: /* empty */
	| direct_list 
	;

direct_list
	: startdir direct
	| startdir direct direct_list
	| startdir direct T_FileSync direct_list
	;

direct
	: filter_directive startdir
	  {
// NYI	      WBRWinBase::optManager()->setFiltFlags( whichFilt, FiltFlags );
	  }
	| include_directive startdir
	| tree_directive startdir
	| edge_directive startdir
	| enum_directive startdir
	| searchtype_directive startdir
	| ignorecase_directive startdir
	| magic_directive startdir
	| magicchars_directive startdir
	;

directive_token
	: T_MemFilt
	| T_MethFilt
	| T_BothFilt
	| T_EnumStyle
	| T_Disabled
	| T_SearchType
	| T_IgnoreCase
	| T_Magic
	| T_MagicChars
	| '@'
	;
	
filter_directive
	: T_MemFilt startmemfilt allfilt_opts
	| T_MethFilt startmethfilt allfilt_opts
	| T_BothFilt startbothfilt allfilt_opts
	;

allfilt_opts
	: startopt allfilt_option
	| startopt allfilt_option ',' allfilt_opts
	| startopt ',' allfilt_opts
	  {
	      if( yyerror( "Warning - found empty filter option " ) ) {
		YYABORT;
	      }
	  }
	| startopt directive_token
	  {
	      if( yyerror( "Error - missing filter option " ) ) {
		  YYABORT;
	      }
	      stutterOn();
	  }
	;

allfilt_option
	: pos_option
	  { 
// NYI	    if( setFilt( (FilterFlags) $1, 1 ) ) YYABORT; 
	  }
	| '~' neggable_option
	  { 
// NYI	    if( setFilt( (FilterFlags) $1, 0 ) ) YYABORT; 
	  }
	| '~' non_neggable_option
	  {
	      if( yyerror( "Error - '~' can not be combined with"
			   " all, visible or local " ) ) {
		  YYABORT;
	      }
	      recoverTo( LF_DirAndOpt );
	  }
	;

pos_option
	: non_neggable_option
	  { $$ = $1; }
	| neggable_option
	  { $$ = $1; }
	;

non_neggable_option
	: T_All
	  { 
// NYI	    $$ = FILT_ALL_INHERITED; 
	  }
	| T_Visible
	  { 
// NYI	    $$ = FILT_INHERITED; 
	  }
	| T_Local
	  { 
// NYI	    $$ = FILT_NO_INHERITED; 
	  }
	;
	
neggable_option
	: T_Public
/* NYI	  { $$ = FILT_PUBLIC; } */
	| T_Protected
/* NYI	  { $$ = FILT_PROTECTED; } */
	| T_Private
/* NYI	  { $$ = FILT_PRIVATE; } */
	| T_Static
/* NYI	  { $$ = FILT_STATIC; } */
	| T_NonStatic
/* NYI	  { $$ = FILT_NONSTATIC; } */
	| T_Virtual
/* NYI	  { $$ = FILT_VIRTUAL; } */
	| T_NonVirtual
/* NYI	  { $$ = FILT_NONVIRTUAL; } */
	;

startmemfilt
	: {} /* { startFilt( Filt_Members ); } */
	;

startmethfilt
      	: {} /* { startFilt( Filt_Methods ); } */
	;

startbothfilt
      	: {} /* { startFilt( Filt_Both ); } */
	;


include_directive 
	: '@' startstring T_String startdir
	  {
	    if( includeFile( getString( $3 ) ) ) {
		WString err( "Unable to open include file " );
		err.concat( getString( $3 ) );
		yyerror( err );
	    } else {
//		activeProject->tellMeIncluded( getString( $3 ) );
	    }
	  }
	;

tree_directive
	: T_TreeView startopt T_Vertical
/* NYI    { WBRWinBase::optManager()->setTreeDirection( TreeVertical ); } */
	| T_TreeView startopt T_Horizontal
/* NYI    { WBRWinBase::optManager()->setTreeDirection( TreeHorizontal ); } */
	;

edge_directive
	: T_Edges startopt T_StraightEdges
/* NYI    { WBRWinBase::optManager()->setSmartEdges( FALSE ); } */
	| T_Edges startopt T_SquareEdges
/* NYI    { WBRWinBase::optManager()->setSmartEdges( TRUE ); } */
	;

enum_directive
	: T_EnumStyle startopt T_Hex
	  {
/* NYI      WBRWinBase::optManager()->setEnumStyle( EV_HexMixedCase ); */
	  }
	| T_EnumStyle startopt T_Octal
	  {
/* NYI      WBRWinBase::optManager()->setEnumStyle( EV_Octal ); */
	  }
	| T_EnumStyle startopt T_Decimal
	  {
/* NYI      WBRWinBase::optManager()->setEnumStyle( EV_decimal ); */
	  }
	;

searchtype_directive
	: T_SearchType startopt T_StartsWith
	  {
/* NYI      WBRWinBase::optManager()->setSearchAnchor( TRUE ); */
	  }
	| T_SearchType startopt T_Contains
	  {
/* NYI      WBRWinBase::optManager()->setSearchAnchor( FALSE ); */
	  }
	;

ignorecase_directive
	: T_IgnoreCase startopt T_On
	  {
/* NYI      WBRWinBase::optManager()->setIgnoreCase( TRUE ); */
	  }
	| T_IgnoreCase startopt T_Off
	  {
/* NYI      WBRWinBase::optManager()->setIgnoreCase( FALSE ); */
	  }
	;

magic_directive
	: T_Magic startopt T_All
	  {
/* NYI      WBRWinBase::optManager()->setMagic( Magic_All ); */

	  }
	| T_Magic startopt T_None
	  {
/* NYI      WBRWinBase::optManager()->setMagic( Magic_None ); */
	  }
	| T_Magic startopt T_Some 
	  {
/* NYI      WBRWinBase::optManager()->setMagic( Magic_Some ); */
	  }
	;

magicchars_directive
	: T_MagicChars startmagicstring T_MagicString
	  {
/* NYI      WBRWinBase::optManager()->setMagicString( getString( $3 )); */
	  }
	;

/* tell the scanner what we're looking for */

startmagicstring
	:
	{ setLookFor( LF_MagicString ); }
	;
	
startstring
	:
	{ setLookFor( LF_String ); }
	;
	
startopt
	:
	{ setLookFor( LF_DirAndOpt ); }
	;

startdir
	:
	{ setLookFor( LF_Directive ); }
	;

%%

