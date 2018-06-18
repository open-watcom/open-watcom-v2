

// For now, omit error checking
// define OMIT_ERROR_RECOVERY

#ifndef YYPRINT
#define YYPRINT         printf
#endif

#ifndef YYTABTYPE
#define YYTABTYPE       unsigned short
#endif

#ifndef YYCHKTYPE
#define YYCHKTYPE       YYTABTYPE
#endif

#ifndef YYACTTYPE
#define YYACTTYPE       YYTABTYPE
#endif

#ifndef YYPRODTYPE
#define YYPRODTYPE      YYTABTYPE
#endif

#ifndef YYFAR
#define YYFAR
#endif

typedef struct packed_action {
    unsigned short      token       : 12;
    unsigned short      high_action : 4;
    unsigned char       low_action;
} packed_action;

#define YYPACKTYPE      packed_action

#define _check( packed )        (packed).token
#define _action( packed)        ((packed).low_action + ((packed).high_action << 8))



#ifndef YYSTYPE
#define YYSTYPE         int
#endif

#define yyerrok         yyerrflag = 0
#define yyclearin       token = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
#if !defined(OMIT_ERROR_RECOVERY)
#define YYERROR         goto yyerrlab
#endif

extern int              yylex( void );
extern void             yyerror( void );

YYSTYPE yylval = {0};

static void             actions( YYPRODTYPE yyprod, YYSTYPE *yyvp );

static YYACTTYPE find_action( YYACTTYPE base, YYTOKENTYPE yytoken )
{
    packed_action YYFAR *pack;
    packed_action YYFAR *probe;
    YYCHKTYPE           check;

    // yychktab[base] is parent + YYPARENT
    // yyacttab[base] is default reduction
    for( ;; ) {     // Chase up through parent productions
        pack = yyacttab + base;
#if !defined(OMIT_ERROR_RECOVER)
        if( _check( *pack ) < YYPARENT ) {
            yyerror( "Invalid state" );
            return( YYNOACTION );
        }
#endif
        for( probe = pack + 1; ; ++probe ) {
            check = _check( *probe );
            if( check >= YYPARENT )
                break;
            if( check == yytoken ) {
                return( _action( *probe ) );
            }
        }
        // Found next base in table -- lookup is not in list
        // See if there is a parent production for original production
        base = _check( *pack ) - YYPARENT;
        if( base == YYUSED ) {
            return( YYNOACTION );
        }
    }
}

static YYACTTYPE find_default( YYACTTYPE base )
{
    packed_action YYFAR *pack;
    YYACTTYPE           yyaction;

    for( ;; ) {
        pack = yyacttab + base;
        yyaction = _action( *pack );
        if( yyaction >= YYUSED )
            break;                              // Found a reduction
        if( yyaction == YYNOACTION )
            break;                              // Default is error
        // Check parent production for default
        base = _check( *pack ) - YYPARENT;
        if( base == YYUSED ) {
            // No parent
            return( YYNOACTION );
        }
    }
    return( yyaction );
}

int yyparse( void )
{
    unsigned short production;
    YYACTTYPE yyaction;
    YYTOKENTYPE yytoken;
    YYACTTYPE yys[MAXDEPTH], *yysp;
    YYSTYPE yyv[MAXDEPTH], *yyvp;
    unsigned short plen, lhs;
#if !defined(OMIT_ERROR_RECOVERY)
    unsigned short  yyerrflag;

    yyerrflag = 0;
#endif
    yysp = yys;
    yyvp = yyv;
    *yysp = YYSTART;
    yytoken = yylex();
    for( ;; ) {     /* parse loop */
        if( yysp >= &yys[MAXDEPTH - 1] ) {
            yyerror( "parse stack overflow" );
            YYABORT;
        }
        yyaction = find_action( *yysp, yytoken );
        if( yyaction == YYNOACTION ) {
            // No action -- look for default action
            yyaction = find_default( *yysp );
            if( yyaction == YYNOACTION ) {
#if defined(OMIT_ERROR_RECOVERY)
                yyerror( "syntax error" );
                YYABORT;
#else
                // No default action either -- error!
                switch( yyerrflag ) {
                case 0:
                    yyerror( "syntax error" );
                    YYERROR;
yyerrlab:
                case 1:
                case 2:
                    yyerrflag = 3;
                    for( ;; ) {
                        // Look for a shift rule for YYERRTOKEN
                        yyaction = find_action( *yysp, YYERRTOKEN );
                        if( yyaction != YYNOACTION && action < YYUSED ) {
                            break;      // found error rule
                        }
                        if( yysp == yys ) {
                            YYABORT;
                        }
                        --yysp;
                        --yyvp;
                    }
                    *++yysp = yyaction;
                    ++yyvp;
                    continue;           /* Through parse loop again */
                case 3:
                    if( yytoken == YYEOFTOKEN ) {
                        YYABORT;
                    }
                    yytoken = yylex();
                    continue;           /* Through parse loop again */
                }
#endif
            }
        }
        if( yyaction < YYUSED ) {
            // Shift to new state 'action'
            if( yyaction == YYSTOP ) {
                YYACCEPT;
            }
            *++yysp = yyaction;
            *++yyvp = yylval;
#if !defined(OMIT_ERROR_RECOVERY)
            if( yyerrflag )
                --yyerrflag;
#endif
            yytoken = yylex();
        } else {
            // Reduce to production indicated by action
            // Production is
            //  target : token_or_target1 ...
            //
            // yyvp[] are the lexical values ($1, $2, etc.)
            production = yyaction - YYUSED;
            lhs = yyprodtab[production];
            // Mask out length & lhs
            plen = lhs >> YYPRODSIZE;
            lhs &= (1 << YYPRODSIZE) - 1;
            yysp -= plen;
            yyvp -= plen;
#if !defined(OMIT_ERROR_RECOVERY)
            if( yysp < yys ) {
                YYPRINT( "stack underflow\n" );
                YYABORT;
            }
#endif
            yyaction = find_action( *yysp, lhs );
#if !defined(OMIT_ERROR_RECOVERY)
            if( yyaction == YYNOACTION ) {
                YYPRINT( "missing nonterminal\n" );
                YYABORT;
            }
#endif
            *++yysp = yyaction;
            ++yyvp;
            actions( production, yyvp );
        }
    }
}

static void actions( unsigned short production, YYSTYPE * yyvp )
{
    YYSTYPE         yyval;

    switch( production ) {

    default:
        // No action required
        return;
    }
    yyvp[0] = yyval;
}
