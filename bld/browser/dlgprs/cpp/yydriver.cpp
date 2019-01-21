// NOTE -- yydriver.h is not explicitly included here.  It must be included
//         in the yacc file, and the yacc file must define YYPARSER suitably.

#include "scanner.h"

#define YYCHKTYPE       int_16
#define YYACTTYPE       int_16
#define YYPLENTYPE      int_16
#define YYPLHSTYPE      int_16



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
#define YYERROR         goto yyerrlab

extern const TokenStruct  xtokens[];
extern const int          xtokcnt;

YYPARSER::YYPARSER( const char * fileName )
//-----------------------------------------
{
    _scanner = new Scanner( fileName, T_String, T_Number, T_Ident, xtokens, xtokcnt );
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

static YYACTTYPE xfind( YYACTTYPE yyk, YYTOKENTYPE yytoken )
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

int YYPARSER::yyparse()
//---------------------
{
    int yypnum;
    int yyi, yylhs;
    YYACTTYPE yyaction;
    YYTOKENTYPE yytoken;
    YYACTTYPE yys[MAXDEPTH];
    YYACTTYPE *yysp;
    YYSTYPE yyv[MAXDEPTH];
    YYSTYPE *yyvp;
    int yyerrflag;

    yyval = 0;
    yylval = 0;
    yyerrflag = 0;
    yyaction = 0;
    yysp = yys;
    yyvp = yyv;
    *yysp = YYSTART;
    yytoken = yylex();
    for( ;; ) {
yynewact:
        yyaction = xfind( *yysp, yytoken );
        if( yyaction == YYNOACTION ) {
            yyaction = xfind( *yysp, YYDEFTOKEN );
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
                        yyaction = xfind( *yysp, YYERRTOKEN );
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
                yyerror( "stack underflow\n" );
                YYABORT;
            }
            yyaction = xfind( *yysp, yylhs );
            if( yyaction == YYNOACTION ) {
                yyerror( "missing nonterminal\n" );
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
