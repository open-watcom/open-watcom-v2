%{  /* include files */
#include <stdio.h>
#include <stddef.h>
#include "parsedef.h"
#include "imps.h"
%}

%token T_INTEGER
%token T_SINGLECHAR
%token T_LONGINT
%token T_UNSINT
%token T_UNS_LONGINT
%token T_FLOATNUM
%token T_DOUBLENUM
%token T_ID
%token T_STRING
%token T_LPAR
%token T_RPAR
%token T_LSQR
%token T_RSQR
%token T_DOT
%token T_ARROW
%token T_INCR
%token T_DECR
%token T_COMMA
%token T_AND
%token T_STAR
%token T_PLUS
%token T_MINUS
%token T_NOT
%token T_EXCLAIM
%token T_SIZEOF
%token T_DIV
%token T_MOD
%token T_LSHIFT
%token T_RSHIFT
%token T_LT
%token T_LE
%token T_GT
%token T_GE
%token T_EQ
%token T_NE
%token T_EXOR
%token T_OR
%token T_LOGAND
%token T_LOGOR
%token T_QUEST
%token T_COLON
%token T_ASS
%token T_STAR_ASS
%token T_DIV_ASS
%token T_MOD_ASS
%token T_PLUS_ASS
%token T_MINUS_ASS
%token T_LSHIFT_ASS
%token T_RSHIFT_ASS
%token T_AND_ASS
%token T_EXOR_ASS
%token T_OR_ASS
%token T_SEMI
%token T_STATIC
%token T_EXTERN
%token T_TYPEDEF
%token T_REGISTER
%token T_AUTO
%token T_CHAR
%token T_SHORT
%token T_INT
%token T_LONG
%token T_UNSIGNED
%token T_SIGNED
%token T_FLOAT
%token T_DOUBLE
%token T_VOID
%token T_TYPE_ID
%token T_RBRACE
%token T_STRUCT
%token T_UNION
%token T_LBRACE
%token T_ENUM
%token T_CONST
%token T_VOLATILE
%token T_DOTDOT
%token T_CASE
%token T_DEFAULT
%token T_IF
%token T_ELSE
%token T_SWITCH
%token T_WHILE
%token T_DO
%token T_FOR
%token T_GOTO
%token T_CONTINUE
%token T_BREAK
%token T_RETURN

%%

goal
       : translation_unit

       ;


constant
       : T_INTEGER
               { $$ = TxtInt( $1 ); }
       |  T_SINGLECHAR
               { $$ = TxtChar( $1 ); }
       |  T_LONGINT
               { $$ = TxtLInt( $1 ); }
       |  T_UNSINT
               { $$ = TxtUInt( $1 ); }
       |  T_UNS_LONGINT
               { $$ = TxtULInt( $1 ); }
       |  T_FLOATNUM
               { $$ = TxtFloat( $1 ); }
       |  T_DOUBLENUM
               { $$ = TxtDouble( $1 ); }
       ;


primary_expression
       : T_ID
               { $$ = TxtId( $1 ); }
       |  constant

       |  T_STRING
               { $$ = TxtStr( $1 ); }
       |  T_LPAR expression T_RPAR
               { $$ = $2; }
       ;

postfix_expression
       : primary_expression

       |  postfix_expression T_LSQR expression T_RSQR
               { $$ = TxtSubscript( $1, $3 ); }
       |  postfix_expression T_LPAR T_RPAR
               { $$ = FinishParm( TxtCall( $1 ) ); }
       |  argument_list T_RPAR
               { $$ = FinishParm( $1 ); }
       |  postfix_expression T_DOT any_id
               { $$ = TxtDotting( $1, $3 ); }
       |  postfix_expression T_ARROW any_id
               { $$ = TxtDotting( TxtPointing( $1 ), $3 ); }
       |  postfix_expression T_INCR
               { $$ = TxtSideOps( OP_POST_INC, $1 ); }
       |  postfix_expression T_DECR
               { $$ = TxtSideOps( OP_POST_DEC, $1 ); }
       ;

argument_list
       : postfix_expression T_LPAR assignment_expression
               { $$ = TxtParm( TxtCall( $1 ), $3 ); }
       |  argument_list T_COMMA assignment_expression
               { $$ = TxtParm( $1, $3 ); }
       ;

unary_expression
       : postfix_expression
               { $$ = ChkUndef( $1 ); }
       |  T_INCR unary_expression
               { $$ = TxtSideOps( OP_PRE_INC, $2 ); }
       |  T_DECR unary_expression
               { $$ = TxtSideOps( OP_PRE_DEC, $2 ); }
       |  T_AND cast_expression
               { $$ = TxtFoldName( $2 ); }
       |  T_STAR cast_expression
               { $$ = TxtPointing( $2 ); }
       |  T_PLUS cast_expression
               { $$ = TxtUnaryPlus( $2 ); }
       |  T_MINUS cast_expression
               { $$ = TxtNegate( $2 ); }
       |  T_NOT cast_expression
               { $$ = TxtComplement( $2 ); }
       |  T_EXCLAIM cast_expression
               { $$ = TxtNot( $2 ); }
       |  T_SIZEOF unary_expression
               { $$ = TxtExprSize( $2 ); }
       |  T_SIZEOF T_LPAR type_name T_RPAR
               { $$ = TxtTypeSize( $3 );
                 PopType(); }
       ;


cast_expression
       : unary_expression

       |  T_LPAR type_name T_RPAR cast_expression
               { $$ = TxtCasting( $2, $4 );
                 PopType(); }
       ;


multiplicative_expression
       : cast_expression

       |  multiplicative_expression T_STAR cast_expression
               { $$ = TxtMult( OP_TIMES, $1, $3 ); }
       |  multiplicative_expression T_DIV cast_expression
               { $$ = TxtMult( OP_DIV, $1, $3 ); }
       |  multiplicative_expression T_MOD cast_expression
               { $$ = TxtMult( OP_MOD, $1, $3 ); }
       ;

additive_expression
       : multiplicative_expression

       |  additive_expression T_PLUS multiplicative_expression
               { $$ = TxtAdd( OP_PLUS, $1, $3 ); }
       |  additive_expression T_MINUS multiplicative_expression
               { $$ = TxtAdd( OP_MINUS, $1, $3 ); }

       ;

shift_expression
       : additive_expression

       |  shift_expression T_LSHIFT additive_expression
               { $$ = TxtShift( OP_LSHIFT, $1, $3 ); }
       |  shift_expression T_RSHIFT additive_expression
               { $$ = TxtShift( OP_RSHIFT, $1, $3 ); }
       ;


rel_expr
       : shift_expression

       |  rel_expr T_LT shift_expression
               { $$ = TxtRelation( OP_LT, $1, $3 ); }
       |  rel_expr T_LE shift_expression
               { $$ = TxtRelation( OP_LE, $1, $3 ); }
       |  rel_expr T_GT shift_expression
               { $$ = TxtRelation( OP_GT, $1, $3 ); }
       |  rel_expr T_GE shift_expression
               { $$ = TxtRelation( OP_GE, $1, $3 ); }
       ;


equality_expression
       : rel_expr

       |  equality_expression T_EQ rel_expr
               { $$ = TxtEquality( OP_EQ, $1, $3 ); }
       |  equality_expression T_NE rel_expr
               { $$ = TxtEquality( OP_NE, $1, $3 ); }
       ;


AND_expression
       : equality_expression

       |  AND_expression T_AND equality_expression
               { $$ = TxtBitwise( OP_AND, $1, $3 ); }
	;


exclusive_OR_expression
       : AND_expression

       |  exclusive_OR_expression T_EXOR AND_expression
               { $$ = TxtBitwise( OP_XOR, $1, $3 ); }

       ;

inclusive_OR_expression
       : exclusive_OR_expression

       |  inclusive_OR_expression T_OR exclusive_OR_expression
               { $$ = TxtBitwise( OP_OR, $1, $3 ); }
       ;


logical_AND_expression
       : inclusive_OR_expression

       |  logical_AND_expression T_LOGAND inclusive_OR_expression
               { $$ = TxtLogical( OP_FLOW_AND, $1, $3 ); }
       ;


logical_OR_expression
       : logical_AND_expression

       |  logical_OR_expression T_LOGOR logical_AND_expression
               { $$ = TxtLogical( OP_FLOW_OR, $1, $3 ); }

       ;

conditional_expression
       : logical_OR_expression

   | logical_OR_expression T_QUEST expression T_COLON conditional_expression
               { $$ = TxtCondition( $1, $3, $5 ); }
       ;


assignment_expression
       : conditional_expression

       |  unary_expression T_ASS assignment_expression
               { $$ = TxtAssign( OP_GETS, $1, $3 ); }
       |  unary_expression T_STAR_ASS assignment_expression
               { $$ = TxtAssMult( OP_TIMES_GETS, $1, $3 ); }
       |  unary_expression T_DIV_ASS assignment_expression
               { $$ = TxtAssMult( OP_DIV_GETS, $1, $3 ); }
       |  unary_expression T_MOD_ASS assignment_expression
               { $$ = TxtAssMult( OP_MOD_GETS, $1, $3 ); }
       |  unary_expression T_PLUS_ASS assignment_expression
               { $$ = TxtAssAdd( OP_PLUS_GETS, $1, $3 ); }
       |  unary_expression T_MINUS_ASS assignment_expression
               { $$ = TxtAssAdd( OP_MINUS_GETS, $1, $3 ); }
       |  unary_expression T_LSHIFT_ASS assignment_expression
               { $$ = TxtAssShift( OP_LSHIFT_GETS, $1, $3 ); }
       |  unary_expression T_RSHIFT_ASS assignment_expression
               { $$ = TxtAssShift( OP_RSHIFT_GETS, $1, $3 ); }
       |  unary_expression T_AND_ASS assignment_expression
               { $$ = TxtAssBit( OP_AND_GETS, $1, $3 ); }
       |  unary_expression T_EXOR_ASS assignment_expression
               { $$ = TxtAssBit( OP_XOR_GETS, $1, $3 ); }
       |  unary_expression T_OR_ASS assignment_expression
               { $$ = TxtAssBit( OP_OR_GETS, $1, $3 ); }
       ;


expression
       : assignment_expression

       |  expression T_COMMA assignment_expression
               { $$ = TxtComma( $1, $3 ); }
       ;


constant_expression
       : conditional_expression

       ;

declaration
       : declaration_specifiers T_SEMI
               { VacuousDefn( $1 );
                 PopType(); }
       |  declare_list T_SEMI
               { PopType(); }
       |  default_declare_list T_SEMI
               { PopType(); }
       ;

declare_list
      : declaration_specifiers init_decl

      |   declare_list T_COMMA init_decl

      ;

default_declare_list
       : default_specifiers init_declid

      |   default_declare_list T_COMMA init_declid

      ;

init_decl
       : declarator
               { AttachBaseType( BaseStack->tptr, $1 );
                 Block( $1 );
                 NoInit(); }
       |  beg_init initializer
               { FiniInit(); }
       ;


beg_init
       : declarator T_ASS
               { AttachBaseType( BaseStack->tptr, $1 );
                 Block( $1 );
                 BeginInit(); }
     ;

init_declid
       : declid
               { AttachBaseType( BaseStack->tptr, $1 );
                 Block( $1 );
                 NoInit(); }
       |  beg_initid initializer
               { FiniInit(); }
       ;

beg_initid
       : declid T_ASS
               { AttachBaseType( BaseStack->tptr, $1 );
                 Block( $1 );
                 BeginInit(); }
       ;

default_specifiers
        : sc_tq_specs
               { $$ = EndBase( $1 ); }
        ;

declaration_specifiers
        : basic_specifier
               { $$ = EndBase( $1 ); }
        |  tag_typeid_specifier
               { $$ = EndBase( $1 ); }
        ;

basic_specifier
        : sc_tq_specs basic_type
              { $$ = SetBasic( $1, $2 ); }
       |   basic_type
              { $$ = StartNewType( SC_NULL, NO_MODIFIERS, $1 ); }
       |   basic_specifier basic_type
              { $$ = SetBasic( $1, $2 ); }
       |   basic_specifier storage_class
              { $$ = SetStorageClass( $1, $2 ); }
       |   basic_specifier type_qualifier
              { $$ = SetTypeQualifier( $1, $2 ); }
       ;

tag_typeid_specifier
        : sc_tq_specs type_id
              { $$ = SetTID( $1, $2 ); }
        |  sc_tq_specs tag_type
              { $$ = SetDefType( $1, $2 ); }
        |  type_id
              { $$ = StartTID( $1 ); }
        |  tag_type
              { $$ = StartDefType( $1 ); }
        |  tag_typeid_specifier storage_class
              { $$ = SetStorageClass( $1, $2 ); }
        |  tag_typeid_specifier type_qualifier
              { $$ = SetTypeQualifier( $1, $2 ); }
        ;

type_tqs_specifiers
        : basic_tqs
              { $$ = EndBase( $1); }
        |  tag_typeid_tqs
              { $$ = EndBase( $1 ); }
      ;


basic_tqs
       :  tq_specs basic_type
               { $$ = SetBasic( $1, $2 ); }
       |   basic_type
               { $$ = StartNewType( SC_NULL, NO_MODIFIERS, $1 ); }
       |   basic_tqs basic_type
               { $$ = SetBasic( $1, $2 ); }
       |   basic_tqs type_qualifier
               { $$ = SetTypeQualifier( $1, $2 ); }
       ;

tag_typeid_tqs
       : tq_specs tag_type
               { $$ = SetDefType( $1, $2 ); }
       |  tq_specs type_id
               { $$ = SetTID( $1, $2 ); }
       |  tag_type
               { $$ = StartDefType( $1 ); }
       |  type_id
               { $$ = StartTID( $1 ); }
       |  tag_typeid_tqs type_qualifier
               { $$ = SetTypeQualifier( $1, $2 ); }
       ;

sc_tq_specs
       : storage_class
              { $$ = StartNewType( $1, NO_MODIFIERS, TYPE_UNDEF ); }
       |  type_qualifier
              { $$ = StartNewType(  SC_NULL, $1, TYPE_UNDEF ); }
       |  sc_tq_specs storage_class
              { $$ = SetStorageClass( $1, $2 ); }
       |  sc_tq_specs type_qualifier
               { $$ = SetTypeQualifier( $1, $2 ); }
	       ;

tq_specs
       : type_qualifier
               { $$ = StartNewType( SC_NULL, $1, TYPE_UNDEF ); }
       |   tq_specs type_qualifier
               { $$ = SetTypeQualifier( $1, $2 ); }
       ;


storage_class
       : T_STATIC
               { $$ = SC_STATIC; }
       |  T_EXTERN
               { $$ = SC_IMPORT; }
       |  T_TYPEDEF
               { $$ = SC_TYPEDEF; }
       |  T_REGISTER
               { $$ = SC_REGISTER; }
       |  T_AUTO
               { $$ = SC_AUTO; }
	       ;


basic_type
       : T_CHAR
               { $$ = TYPE_CHAR; }
       |  T_SHORT
               { $$ = TYPE_SHORT; }
       |  T_INT
               { $$ = TYPE_INT; }
       |  T_LONG
               { $$ = TYPE_LONG; }
       |  T_UNSIGNED
               { $$ = TYPE_UNSIGNED; }
       |  T_SIGNED
               { $$ = TYPE_SIGNED; }
       |  T_FLOAT
               { $$ = TYPE_FLOAT; }
       |  T_DOUBLE
               { $$ = TYPE_DOUBLE; }
       |  T_VOID
               { $$ = TYPE_VOID; }
       ;

type_id
        :  T_TYPE_ID
               { $$ = $1; }
        ;
any_id
       : T_ID
               { $$ = $1; }
       |  T_TYPE_ID
               { $$ = $1; }
       ;


tag_type :
           enum_specifier
               { /* = FindDefineType( TYPE_INT );  */
                 $$ = $1; }
       |  struct_or_union any_id
               { $$ = GetStruct( $2, $1 ); }
       |  struct T_RBRACE
               { $$ = StructFini( $1 ); }
       ;

struct_or_union
       : T_STRUCT
               { $$ = TYPE_STRUCT; }
       |  T_UNION
               { $$ = TYPE_UNION; }
       ;



struct
       : struct_or_union T_LBRACE
               { $$ = StructHeader( $1 ); }
       |  struct_or_union any_id T_LBRACE
               { $$ = StructIHeader( $2, $1 ); }
       |  struct_decl T_SEMI

       ;

struct_decl
       : struct type_tqs_specifiers struct_declarator
               { StartStructDecl( $1, $2 );
                 StructDecl( $1, $3 );
                 PopType();
                 $$ = $1; }
       |  struct tq_specs struct_declid
               { StartStructDecl( $1, EndBase( $2 ) );
                 StructDecl( $1, $3 );
                 PopType();
                 $$ = $1; }
       |  struct_decl T_COMMA struct_declarator
               { StructDecl( $1, $3 );
                 $$ = $1; }
       ;

struct_declid
       : declid

       |  declid T_COLON constant_expression
               { $$ = FieldType( $1, ConValue( $3 ) ); }
       |  T_COLON constant_expression
               { $$ = FieldType( NULL, ConValue( $2 ) ); }
       ;


struct_declarator
       : declarator

       |  declarator T_COLON constant_expression
               { $$ = FieldType( $1, ConValue( $3 ) ); }
       |  T_COLON constant_expression
               { $$ = FieldType( NULL, ConValue( $2 ) ); }
       ;

enum_specifier
       : T_ENUM T_LBRACE enumerator_list T_RBRACE
               { $$ = UntaggedTypeEnum( $3 ); }
       |  T_ENUM any_id T_LBRACE enumerator_list T_RBRACE
               { $$ = TaggedTypeEnum( $2, $4 ); }
       |  T_ENUM any_id
               { $$ = GetEnumType( $2 ); }
       ;

enumerator_list
       : enumerator
               { $$ = AddToEnumList( NULL, $1 ); }
       |  enumerator_list T_COMMA enumerator
               { $$ = AddToEnumList( $1, $3 ); }
       ;


enumerator
       : T_ID
               { $$ = EnumId( $1, VALUE_NOT_SPECIFIED, FALSE ); }
       |  T_ID T_ASS constant_expression
               { $$ = EnumId( $1, ConValue( $3 ), TRUE ); }
       ;


type_qualifier
       : T_CONST
               { $$ = CONSTANT_MOD; }
       |  T_VOLATILE
               { $$ = VOLATILE_MOD; }
       ;


declarator
       : direct_declarator

       |  pointer direct_declarator
               { $$ = AttachDecl( $1, $2 ); }
       ;

direct_declarator
       : any_id
               { $$ = Decl( $1 ); }
       |  T_LPAR declarator T_RPAR
               { $$ = $2; }
       |  direct_declarator T_LSQR constant_expression T_RSQR
               { $$ = ArrayType( $1, ConValue( $3 ) ); }
       |  direct_declarator T_LSQR T_RSQR
               { $$ = ArrayType( $1, VALUE_NOT_SPECIFIED ); }
       |  direct_declarator T_LPAR T_RPAR
               { $$ = FuncType( $1, NULL ); }
       |  direct_declarator T_LPAR parameter_type_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       |  direct_declarator T_LPAR identifier_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       ;

declid
       : direct_declid

       |  pointer direct_declid
               { $$ = AttachDecl( $1, $2 ); }
       ;

direct_declid
       : T_ID
               { $$ = Decl( $1 ); }
       |  T_LPAR declid T_RPAR
               { $$ = $2; }
       |  direct_declid T_LSQR constant_expression T_RSQR
               { $$ = ArrayType( $1, ConValue( $3 ) ); }
       |  direct_declid T_LSQR T_RSQR
               { $$ = ArrayType( $1, VALUE_NOT_SPECIFIED ); }
       |  direct_declid T_LPAR T_RPAR
               { $$ = FuncType( $1, NULL ); }
       |  direct_declid T_LPAR parameter_type_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       |  direct_declid T_LPAR identifier_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       ;

decl_parmtid
       : direct_parmtid

       |  pointer direct_parmtid
               { $$ = AttachDecl( $1, $2 ); }
       ;

direct_parmtid
       : T_TYPE_ID
               { $$ = Decl( $1 ); }
       |  T_LPAR pointer direct_parmtid T_RPAR
               { $$ = AttachDecl( $2, $3 ); }
       |  direct_parmtid T_LSQR constant_expression T_RSQR
               { $$ = ArrayType( $1, ConValue( $3 ) ); }
       |  direct_parmtid T_LSQR T_RSQR
               { $$ = ArrayType( $1, VALUE_NOT_SPECIFIED ); }
       |  direct_parmtid T_LPAR T_RPAR
               { $$ = FuncType( $1, NULL ); }
       |  direct_parmtid T_LPAR parameter_type_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       |  direct_parmtid T_LPAR identifier_list T_RPAR
               { $$ = FuncType( $1, $3 ); }
       ;

pointer
       : T_STAR
               { $$ = PointerType( NULL, Decl( NULL ) ); }
       |  T_STAR tq_specs
               { $$ = PointerType( $2, Decl( NULL ) ); }
       |  T_STAR pointer
               { $$ = PointerType( NULL, $2 ); }
       |  T_STAR tq_specs pointer
               { $$ = PointerType( $2, $3 ); }
       ;


parameter_type_list
       : parameter_list
               { $$ = $1; }
       |  parameter_list T_COMMA T_DOTDOT T_DOT
                 { $$ = AddEllipsis( $1 ); }
       ;

parameter_list
       : parameter_declaration
               { $$ = StartParmList( $1 );
                 PopType(); }
       |  parameter_list T_COMMA parameter_declaration
               { $$ = AddParm( $1, $3 );
                 PopType(); }
       ;

parameter_declaration
       : declaration_specifiers declid
               { AttachBaseType( $1, $2 );
                 $$ = $2; }
       |  declaration_specifiers decl_parmtid
               { AttachBaseType( $1, $2 );
                 $$ = $2; }
       |  sc_tq_specs  declid
               { AttachBaseType( EndBase( $1 ), $2 );
                 $$ = $2; }
       |  sc_tq_specs  abstract_declarator
               { AttachBaseType( EndBase( $1 ), $2 );
                 $$ = $2; }
       |  declaration_specifiers
               { $$ = Decl( NULL );
                 AttachBaseType( $1, $$ ); }
       |  declaration_specifiers abstract_declarator
               { AttachBaseType( $1, $2 );
                 $$ = $2; }
       ;


identifier_list
       : T_ID
               { $$ = StartIdList( $1 ); }
       |  identifier_list T_COMMA T_ID
               { $$ = AddId( $1, $3 ); }
       ;


type_name
       : type_tqs_specifiers
               { $$ = BaseStack->tptr; }
       |  type_tqs_specifiers abstract_declarator
               { AttachBaseType( BaseStack->tptr, $2 );
                 $$ = DeclType( $2 ); }
       ;

abstract_declarator
       : pointer
               { $$ = AttachDecl( $1, Decl( NULL ) ); }
       |  direct_abstract_declarator
               { $$ = $1; }
       |  pointer direct_abstract_declarator
               { $$ = AttachDecl( $1, $2 ); }
       ;

direct_abstract_declarator
       : T_LPAR abstract_declarator T_RPAR
               { $$ = $2; }
       |  T_LSQR T_RSQR
               { $$ = ArrayType( Decl( NULL ), VALUE_NOT_SPECIFIED ); }
       |  T_LSQR constant_expression T_RSQR
               { $$ = ArrayType( Decl( NULL ), ConValue( $2 ) ); }
       |  direct_abstract_declarator T_LSQR T_RSQR
               { $$ = ArrayType( $1, VALUE_NOT_SPECIFIED ); }
       |  direct_abstract_declarator T_LSQR constant_expression T_RSQR
               { $$ = ArrayType( $1, ConValue( $3 ) ); }
       |  T_LPAR T_RPAR
               { $$ = FuncType( Decl( NULL ), NULL ); }
       |  T_LPAR parameter_type_list T_RPAR
               { $$ = FuncType( Decl( NULL ), $2 ); }
       |  direct_abstract_declarator T_LPAR T_RPAR
               { $$ = FuncType( $1, NULL ); }
       |  direct_abstract_declarator T_LPAR parameter_type_list T_RPAR
               { $$ = FuncType( $1 , $3 ); }
       ;


initializer
       : assignment_expression
               { InitExpr( $1 ); }
       |  T_LBRACE beg_initializer initializer_list T_RBRACE
               { PopInitEntry(); }
       |  T_LBRACE beg_initializer initializer_list T_COMMA T_RBRACE
               { PopInitEntry(); }
       ;

initializer_list
       : initializer

       |  initializer_list T_COMMA initializer

       ;

beg_initializer
       :
               { PushInitEntry(); }
       ;



statement
       : statement_beg statement1

       ;

statement1
       : labeled_statement

       |  compound_statement

       |  expression_statement

       |  selection_statement

       |  iteration_statement

       |  jump_statement

       ;

statement_beg
       :
               { ELinenum(); }
       ;


labeled_statement
       : label statement

       ;


label
       : T_ID T_COLON
               { LabelStmt( $1 ); }
       |  T_CASE constant_expression T_COLON
               { CaseStmt( $2 ); }
       |  T_DEFAULT T_COLON
               { DefaultStmt(); }
       ;


compound_statement
       : beg_compound T_RBRACE
               { PopCompound(); }
       |  beg_compound statement_list T_RBRACE
               { PopCompound(); }
       |  beg_compound declaration_list T_RBRACE
               { PopCompound(); }
       |  beg_compound declaration_list statement_list T_RBRACE
               { PopCompound(); }
       ;


beg_compound
       : T_LBRACE
               { PushCompound();
                 BlockType = SL_BLOCK; }
       ;

declaration_list
       : declaration

       |  declaration_list declaration

       ;

statement_list
       : statement

       |  statement_list statement

       ;


expression_statement
       : expression T_SEMI
               { RootEval( $1 ); }
       |  T_SEMI

       ;


selection_statement
       : if_beg statement
               { AltBlock( $1 );
                 EndBlock( $1 ); }
       |  if_else_beg statement
               { AltBlock( $1 );
                 EndBlock( $1 ); }
       |  switch_beg statement
               { EndSwitch( $1 ); }
       ;

if_beg
       : T_IF T_LPAR expression T_RPAR
               { $$ = BegBlock( C_COND );
                 JFAlt( $$, $3 ); }
       ;

if_else_beg
       : if_beg matched_stat T_ELSE
               { JEnd( $1 );
                 AltBlock( $1 );
                 $$ = $1; }
       ;

switch_beg
       : T_SWITCH T_LPAR expression T_RPAR
               { $$ = SwitchStmt( $3 ); }
       ;


iteration_statement
       : while_beg statement
               { AltBlock( $1 );
                 JBeg( $1 );
                 EndBlock( $1 ); }
       |  do_beg statement T_WHILE T_LPAR expression T_RPAR T_SEMI
               { AltBlock( $1 );
                 JFEnd( $1, $5 );
                 JBeg( $1 );
                 EndBlock( $1 ); }
       |  for_beg opt_expression T_RPAR statement
               { AltBlock( $1 );
                 RootEval( $2 );
                 JBeg( $1 );
                 EndBlock( $1 ); }
       ;

while_beg
       : T_WHILE T_LPAR expression T_RPAR
               { $$ = BegBlock( C_LOOP );
                 JFEnd( $$, $3 ); }
       ;

do_beg
       : T_DO
               { $$ = BegBlock( C_LOOP ); }
       ;

for_beg
       : T_FOR T_LPAR opt_expression T_SEMI opt_expression T_SEMI
               { $$ = ForStmt( $3, $5 ); }
       ;



jump_statement
       : T_GOTO T_ID T_SEMI
               { GotoStmt( $2 ); }
       |  T_CONTINUE T_SEMI
               { ContinueStmt(); }
       |  T_BREAK T_SEMI
               { BreakStmt(); }
       |  T_RETURN opt_expression T_SEMI
               { ReturnStmt( $2 ); }
       ;


matched_stat
       : statement_beg matched_stat2

       ;

matched_stat2
       : labelled_matched_stat

       |  compound_statement

       |  expression_statement

       |  jump_statement

       |  do_beg statement T_WHILE T_LPAR expression T_RPAR T_SEMI
               { AltBlock( $1 );
                 JFEnd( $1, $5 );
                 JBeg( $1 );
                 EndBlock( $1 ); }
       |  if_else_beg matched_stat
               { AltBlock( $1 );
                 EndBlock( $1 ); }
       |  while_beg matched_stat
               { JBeg( $1 );
                 AltBlock( $1 );
                 EndBlock( $1 ); }
       |  for_beg opt_expression T_RPAR matched_stat
               { AltBlock( $1 );
                 RootEval( $2 );
                 JBeg( $1 );
                 EndBlock( $1 ); }
       |  switch_beg matched_stat
               { EndSwitch( $1 ); }
       ;

labelled_matched_stat
       : label matched_stat

       ;

opt_expression
       :
               { $$ = NULL; }
       |  expression

       ;



translation_unit
       : external_definition

       |  translation_unit external_definition

       ;

external_definition
       : function_definition

       |  declaration

       ;


function_definition
       : function_header function_beg function_body
               { FuncEpilogue( FuncPtr );
                 BlockType = SL_EXTERNAL; }
       ;

function_header
       : declaration_specifiers declarator
               { AttachBaseType( BaseStack->tptr, $2 );
                 FuncPtr = FuncHeader( $2 );
                 PopType(); }
       |  default_specifiers declid
               { AttachBaseType( BaseStack->tptr, $2 );
                 FuncPtr = FuncHeader( $2 );
                 PopType(); }
       |  declid
               { AttachBaseType( FindDefinedType( TYPE_INT ), $1 );
                 StorageClass = SC_NULL;
                 FuncPtr = FuncHeader( $1 ); }
       ;

function_body
       : declaration_list function_prologue compound_statement

       |  function_prologue compound_statement

       ;

function_beg
       :
               { BlockType = SL_FUNC; }
       ;

function_prologue
       :
               { FuncPrologue( FuncPtr );
                 BlockType = SL_BLOCK; }
       ;
%%
/* function section */

extern char SynError;

int Parser() {
    SynError = 0;
    yyparse();
}

static yyerror( char *msg ) {
    SynError = 1;
    Synmsg( msg, NULL );
}
