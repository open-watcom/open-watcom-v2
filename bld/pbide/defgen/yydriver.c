#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defgen.h"
#include "semantic.h"
#include "scan.h"
#include "ytab.h"

#ifndef YYTABTYPE
#define YYTABTYPE       short
#endif

#ifndef YYCHKTYPE
#define YYCHKTYPE       YYTABTYPE
#endif

#ifndef YYACTTYPE
#define YYACTTYPE       YYTABTYPE
#endif

#ifndef YYPLENTYPE
#define YYPLENTYPE      YYTABTYPE
#endif

#ifndef YYPLHSTYPE
#define YYPLHSTYPE      YYTABTYPE
#endif

#ifndef YYFAR
#define YYFAR
#endif



typedef union {
        int             Token;
        int             Counter;
        char            *String;
        ParamInfo       *Parm;
        TypeInfo        *Type;
        ModifierInfo    *Modifiers;
        FunctionInfo    *FuncInfo;
} YYSTYPE;

YYSTYPE *yyvp, yyval, yylval;

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
#define YYERROR         goto yyerrlab

static void yyerror( char *str ) {
    ReportError( str );
}

static int yylex() {
    int         curtoken;
    ScanValue   value;

    curtoken = Scan( &value );
    yylval.String = NULL;
    if( Config.debug ) {
        switch( curtoken ) {
            case T_NUM:
                printf( "T_NUM\n" );
                break;
            case T_NAME:
                printf( "T_NAME\n" );
                break;
            case T_CHAR:
                printf( "T_CHAR\n" );
                break;
            case T_SHORT:
                printf( "T_SHORT\n" );
                break;
            case T_INT:
                printf( "T_INT\n" );
                break;
            case T_LONG:
                printf( "T_LONG\n" );
                break;
            case T_SIGNED:
                printf( "T_SIGNED\n" );
                break;
            case T_UNSIGNED:
                printf( "T_UNSIGNED\n" );
                break;
            case T_FLOAT:
                printf( "T_FLOAT\n" );
                break;
            case T_DOUBLE:
                printf( "T_DOUBLE\n" );
                break;
            case T_VOID:
                printf( "T_VOID\n" );
                break;
            case T_CONST:
                printf( "T_CONST\n" );
                break;
            case T_STRUCT:
                printf( "T_STRUCT\n" );
                break;
            case T_UNION:
                printf( "T_UNION\n" );
                break;
            case T_NEAR:
                printf( "T_NEAR\n" );
                break;
            case T_FAR:
                printf( "T_FAR\n" );
                break;
            case T_HUGE:
                printf( "T_HUGE\n" );
                break;
            case T_CDECL:
                printf( "T_CDECL\n" );
                break;
            case T_PASCAL:
                printf( "T_PASCAL\n" );
                break;
            case T_FORTRAN:
                printf( "T_FORTRAN\n" );
                break;
            case T_SYSCALL:
                printf( "T_SYSCALL\n" );
                break;
            case T_EXPORT:
                printf( "T_EXPORT\n" );
                break;
            case T_EXTERN:
                printf( "T_EXTERN\n" );
                break;
            case T_LPAREN:
                printf( "T_LPAREN\n" );
                break;
            case T_RPAREN:
                printf( "T_RPAREN\n" );
                break;
            case T_LSQ_BRACKET:
                printf( "T_LSQ_BRACKET\n" );
                break;
            case T_RSQ_BRACKET:
                printf( "T_RSQ_BRACKET\n" );
                break;
            case T_SEMICOLON:
                printf( "T_SEMICOLON\n" );
                break;
            case T_STAR:
                printf( "T_STAR\n" );
                break;
            case T_COMMA:
                printf( "T_COMMA\n" );
                break;
            case T_POUND:
                printf( "T_POUND\n" );
                break;
            case T_SLASH:
                printf( "T_SLASH\n" );
                break;
            case T_LINE:
                printf( "T_LINE\n" );
                break;
            case T_STRING:
                printf( "T_STRING\n" );
                break;
            case T_FNAME:
                printf( "T_FNAME\n" );
                break;
            case T_PERIOD:
                printf( "T_PERIOD\n" );
                break;
        }
    }
    switch( curtoken ) {
    case T_NAME:
    case T_NEAR:
    case T_FAR:
    case T_HUGE:
    case T_CDECL:
    case T_PASCAL:
    case T_FORTRAN:
    case T_SYSCALL:
    case T_EXPORT:
    case T_CHAR:
    case T_CONST:
    case T_FLOAT:
    case T_DOUBLE:
    case T_INT:
    case T_LONG:
    case T_SHORT:
    case T_SIGNED:
    case T_STRUCT:
    case T_UNION:
    case T_UNSIGNED:
    case T_VOID:
    case T_EXTERN:
    case T_LINE:
    case T_FNAME:
    case T_NUM:
    case T_STRING:
        yylval.String = value.str;
//      printf( "%s\n", value.str );
        break;
    default:
        break;
    }
    return( curtoken );
}

int yyparse( void )
{
  short yypnum;
  short yyi, yyk, yylhs, yyaction;
  short yytoken;
  short yys[MAXDEPTH], *yysp;
  YYSTYPE yyv[MAXDEPTH], *yyvp;
  short yyerrflag;

  yyerrflag = 0;
  yysp = yys;
  yyvp = yyv;
  *yysp = YYSTART;
  yytoken = yylex();
  for(;;){
yynewact:
      yyk = *yysp;
      while( (yyi = yyk + yytoken) < 0 || yyi >= YYUSED || yychktab[yyi] != yytoken )
          if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
              goto yycheck1;
          else
              yyk = yyacttab[yyi];
      yyaction = yyacttab[yyi];
      if( yyaction == YYNOACTION ){
yycheck1:
          yyk = *yysp;
          while( (yyi = yyk + YYDTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYDTOKEN )
              if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
                  goto yycheck2;
              else
                  yyk = yyacttab[yyi];
          yyaction = yyacttab[yyi];
          if( yyaction == YYNOACTION ){
yycheck2:
              switch( yyerrflag ){
                case 0:
                  yyerror( "syntax error" );
//                yyerrlab:
                case 1:
                case 2:
                  yyerrflag = 3;
                  while( yysp >= yys ){
                      yyk = *yysp;
                      while( (yyi = yyk + YYETOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYETOKEN )
                          if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
                              goto continu;
                          else
                              yyk = yyacttab[yyi];
                      yyaction = yyacttab[yyi];
                      if( yyaction < YYUSED ){
                          *++yysp = yyaction;
                          ++yyvp;
                          goto yynewact;
                      };
                      continu:;
                      --yysp;
                      --yyvp;
                  };
                  YYABORT;
                case 3:
                  if( yytoken == 0 ) /* EOF token */
                      YYABORT;
                  yytoken = yylex();
                  goto yynewact;
              };
          };
      };
      if( yyaction < YYUSED ){
          if( yyaction == YYSTOP ){
              YYACCEPT;
          } else {
              *++yysp = yyaction;
              *++yyvp = yylval;
              if( yyerrflag )
                  --yyerrflag;
              yytoken = yylex();
          };
      } else {
          yypnum = yyaction - YYUSED;
          yyi = yyplentab[yypnum];
          yysp -= yyi;
          yyvp -= yyi;
          yylhs = yyplhstab[yypnum];
          if( yysp < yys ){
              printf( "stack underflow\n" );
              YYABORT;
          };
          yyk = *yysp;
          while( (yyi = yyk + yylhs) < 0 || yyi >= YYUSED || yychktab[yyi] != yylhs ){
              if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN ){
                  printf( "missing nonterminal\n" );
                  YYABORT;
              };
              yyk = yyacttab[yyi];
          };
          *++yysp = yyacttab[yyi];
          ++yyvp;
          switch( yypnum ){

            default:
              yyval = yyvp[0];
          };
          *yyvp = yyval;
      };
  };
}
