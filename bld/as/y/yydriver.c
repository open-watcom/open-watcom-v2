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



YYSTYPE *yyvp, yyval, yylval;

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
#define YYERROR         goto yyerrlab

#ifndef NDEBUG
static void dump_rule( unsigned rule )
{
    unsigned i;
    const YYTOKENTYPE YYFAR *tok;
    const char YYFAR *p;

    #ifdef _STANDALONE_
    if( !_IsOption( DUMP_PARSE_TREE ) ) return;
    #else
    #ifndef MYDEBUG
    #pragma disable_message( 201 );
    return;
    #endif // !MYDEBUG
    #endif
    for( p = yytoknames[ yyplhstab[ rule ] ]; *p; ++p ) {
        putchar( *p );
    }
    putchar( ' ' );
    putchar( '<' );
    putchar( '-' );
    tok = &yyrhstoks[ yyrulebase[ rule ] ];
    for( i = yyplentab[ rule ]; i != 0; --i ) {
        putchar( ' ' );
        for( p = yytoknames[ *tok ]; *p; ++p ) {
            putchar( *p );
        }
        ++tok;
    }
    putchar( '\n' );
    #pragma enable_message( 201 );
}
#endif

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
                  yyerrlab:
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
#ifndef NDEBUG
          dump_rule( yypnum );
#endif
          switch( yypnum ){

            default:
              yyval = yyvp[0];
          };
          *yyvp = yyval;
      };
  };
}
