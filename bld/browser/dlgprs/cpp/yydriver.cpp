// NOTE -- yydriver.h is not explicitly included here.  It must be included
//         in the yacc file, and the yacc file must define YYPARSER suitably.

#include "scanner.h"

#define yychktab    YYPARSER::yychktab
#define yyacttab    YYPARSER::yyactab
#define yyplentab   YYPARSER::yyplentab
#define yyplhstab   YYPARSER::yyplhstab



#ifndef YYSTYPE
#define YYSTYPE         int
#endif

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
//Never used
//#define YYERROR         goto yyerrlab

YYPARSER::YYPARSER( const char * fileName )
//-----------------------------------------
{
    _scanner = new Scanner( fileName );
}

YYPARSER::~YYPARSER()
//-------------------
{
    delete _scanner;
}

void YYPARSER::yyerror( const char * msg )
//----------------------------------------
{
    _scanner->error( msg );
}

int YYPARSER::yyabort()
//---------------------
{
    return 0;
}

int YYPARSER::yylex()
//-------------------
{
    return _scanner->getToken( yylval );
}

int YYPARSER::yyparse()
//---------------------
{
  int yypnum;
  int yyi, yyk, yylhs, yyaction;
  int yytoken;
  YYSTYPE yys[MAXDEPTH];
  YYSTYPE *yysp;
  YYSTYPE yyv[MAXDEPTH];
  YYSTYPE *yyvp;
  int yyerrflag;

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
                  //Never used
                  //yyerrlab:
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
              yyerror( "stack underflow\n" );
              YYABORT;
          };
          yyk = *yysp;
          while( (yyi = yyk + yylhs) < 0 || yyi >= YYUSED || yychktab[yyi] != yylhs ){
              if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN ){
                  yyerror( "missing nonterminal\n" );
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
