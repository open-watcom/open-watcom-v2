#ifndef NDEBUG
    #define YYDEBUG
#endif

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



static YYSTYPE yyval;

YYSTYPE yylval;

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(true)
#define YYACCEPT        return(false)
#define YYERROR         goto yyerrlab

#ifdef YYDEBUG
static void dump_rule( unsigned rule )
{
    unsigned i;
    const YYTOKENTYPE YYFAR *yytoken;
    const char YYFAR *p;

  #if defined( _STANDALONE_ ) && defined( AS_DEBUG_DUMP )
    if( !_IsOption( DUMP_PARSE_TREE ) )
        return;
  #endif
    for( p = yytoknames[ yyplhstab[ rule ] ]; *p; ++p ) {
        putchar( *p );
    }
    putchar( ' ' );
    putchar( '<' );
    putchar( '-' );
    yytoken = &yyrhstoks[ yyrulebase[ rule ] ];
    for( i = yyplentab[rule]; i != 0; --i ) {
        putchar( ' ' );
        for( p = yytoknames[*yytoken]; *p; ++p ) {
            putchar( *p );
        }
        ++yytoken;
    }
    putchar( '\n' );
}
#endif

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

bool yyparse( void )
{
    short yypnum;
    short yyi;
    short yylhs;
    YYACTTYPE yyaction;
    YYTOKENTYPE yytoken;
    YYACTTYPE yys[MAXDEPTH], *yysp;
    YYSTYPE yyv[MAXDEPTH], *yyvp;
    short yyerrflag;

    yyerrflag = 0;
    yyaction = 0;
    yysp = yys;
    yyvp = yyv;
    *yysp = YYSTART;
    yytoken = yylex();
    for( ;; ) {
yynewact:
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
#ifdef YYDEBUG
            dump_rule( yypnum );
#endif
            switch( yypnum ) {

            default:
                yyval = yyvp[0];
            }
            *yyvp = yyval;
        }
    }
}
