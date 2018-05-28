

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



#ifndef YYSTYPE
#define YYSTYPE         int
#endif

YYSTYPE yyval, yylval;

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
#define YYERROR         goto yyerrlab

static YYACTTYPE find_action( YYACTTYPE yyk, YYTOKENTYPE yytoken )
{
    int     yyi;

    while( (yyi = yyk + yytoken) < 0 || yyi >= YYUSED || yychktab[yyi] != yytoken ) {
        if( (yyi = yyk + YYPARTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPARTOKEN ) {
            return( YYNOACTION );
        }
        yyk = yyacttab[yyi];
    }
    return( yyacttab[yyi] );
}

int yyparse( void )
{
    short yypnum;
    short yyi, yylhs;
    YYACTTYPE yyaction;
    YYTOKENTYPE yytoken;
    YYACTTYPE yys[MAXDEPTH], *yysp;
    YYSTYPE yyv[MAXDEPTH], *yyvp;
    short yyerrflag;

    yyerrflag = 0;
    yysp = yys;
    yyvp = yyv;
    *yysp = YYSTART;
    yytoken = yylex();
    for( ;; ) {
yynewact:
        if( yysp >= &yys[MAXDEPTH - 1] ) {
            yyerror( "parse stack overflow" );
            YYABORT;
        }
        yyaction = find_action( *yysp, yytoken );
        if( yyaction == YYNOACTION ) {
            yyaction = find_action( *yysp, YYDEFTOKEN );
            if( yyaction == YYNOACTION ) {
                switch( yyerrflag ) {
                case 0:
                    yyerror( "syntax error" );
                    YYERROR;
yyerrlab:
                case 1:
                case 2:
                    yyerrflag = 3;
                    while( yysp >= yys ) {
                        yyaction = find_action( *yysp, YYERRTOKEN );
                        if( yyaction != YYNOACTION && yyaction < YYUSED ) {
                            *++yysp = yyaction;
                            ++yyvp;
                            goto yynewact;
                        }
                        --yysp;
                        --yyvp;
                    }
                    YYABORT;
                case 3:
                    if( yytoken == YYEOFTOKEN )
                        YYABORT;
                    yytoken = yylex();
                    goto yynewact;
                }
            }
        }
        if( yyaction < YYUSED ) {
            if( yyaction == YYSTOP ) {
                YYACCEPT;
            }
            *++yysp = yyaction;
            *++yyvp = yylval;
            if( yyerrflag )
                --yyerrflag;
            yytoken = yylex();
        } else {
            yypnum = yyaction - YYUSED;
            yyi = yyplentab[yypnum];
            yysp -= yyi;
            yyvp -= yyi;
            yylhs = yyplhstab[yypnum];
            if( yysp < yys ) {
                printf( "stack underflow\n" );
                YYABORT;
            }
            yyaction = find_action( *yysp, yylhs );
            if( yyaction == YYNOACTION ) {
                printf( "missing nonterminal\n" );
                YYABORT;
            }
            *++yysp = yyaction;
            ++yyvp;
            switch( yypnum ) {

            default:
                yyval = yyvp[0];
            }
            *yyvp = yyval;
        }
    }
}
