/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


extern int              yylex();
extern void             yyerror();

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
    unsigned short      token : 12;
    unsigned short      high_action : 4;
    unsigned char       low_action;
} packed_action;

#define YYPACKTYPE      packed_action

#define _check( packed )        (packed).token
#define _action( packed)        ((packed).low_action + ((packed).high_action << 8))



#ifndef YYSTYPE
#define YYSTYPE         int
#endif

#ifndef __TURBOC__
YYSTYPE yylval = {0};
#else
YYSTYPE yylval;
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

static void             actions( unsigned short production, YYSTYPE * yyvp );

static YYACTTYPE find_action( unsigned base, YYCHKTYPE lookup )
{
    packed_action YYFAR *       pack;
    packed_action YYFAR *       probe;
    YYCHKTYPE                   check;

    // yychktab[ base ] is parent + YYPARENT
    // yyacttab[ base ] is default reduction
    for(;;) {   // Chase up through parent productions
        pack = yyacttab + base;
#if !defined(OMIT_ERROR_RECOVER)
        if( _check( *pack ) < YYPARENT ) {
            yyerror( "Invalid state" );
            return( YYNOACTION );
        }
#endif
        for( probe = pack + 1; ; ++ probe ) {
            check = _check( *probe );
            if( check >= YYPARENT ) break;
            if( check == lookup ) {
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

static YYACTTYPE find_default( unsigned base )
{
    packed_action YYFAR *       pack;
    YYACTTYPE                   action;

    for(;;) {
        pack = yyacttab + base;
        action = _action( *pack );
        if( action >= YYUSED ) break;           // Found a reduction
        if( action == YYNOACTION ) break;       // Default is error
        // Check parent production for default
        base = _check( *pack ) - YYPARENT;
        if( base == YYUSED ) {
            // No parent
            return( YYNOACTION );
        }
    }
    return( action );
}

yyparse()
{
    unsigned short production;
    unsigned short action;
    unsigned short token;
    unsigned short yys[MAXDEPTH], *yysp;
    YYSTYPE yyv[MAXDEPTH], *yyvp;
    unsigned short plen, lhs;
#if !defined(OMIT_ERROR_RECOVERY)
    unsigned short yyerrflag;

    yyerrflag = 0;
#endif
    yysp = yys;
    yyvp = yyv;
    *yysp = YYSTART;
    token = yylex();
    for(;;) {   /* parse loop */
        if( yysp >= &yys[MAXDEPTH-1] ){
            yyerror( "parse stack overflow" );
            YYABORT;
        }
        action = find_action( *yysp, token );
        if( action == YYNOACTION ){
            // No action -- look for default action
            action = find_default( *yysp );
            if( action == YYNOACTION ) {
#if defined(OMIT_ERROR_RECOVERY)
                yyerror( "syntax error" );
                YYABORT;
#else
                // No default action either -- error!
                switch( yyerrflag ){
                  case 0:
                    yyerror( "syntax error" );
                    yyerrlab:
                case 1:
                case 2:
                    yyerrflag = 3;
                    for(;;) {
                        // Look for a shift rule for YYERRTOKEN
                        action = find_action( *yysp, YYERRTOKEN );
                        if( action != YYNOACTION && action < YYUSED ) {
                            break;      // found error rule
                        }
                        if( yysp == yys ) {
                            YYABORT;
                        }
                        --yysp;
                        --yyvp;
                    }
                    *++yysp = action;
                    ++yyvp;
                    continue;           /* Through parse loop again */
                case 3:
                    if( token == 0 ) {  /* EOF token */
                        YYABORT;
                    }
                    token = yylex();
                    continue;           /* Through parse loop again */
                }
#endif
            }
        }
        if( action < YYUSED ) {
            // Shift to new state 'action'
            if( action == YYSTOP ) {
                YYACCEPT;
            } else {
                *++yysp = action;
                *++yyvp = yylval;
#if !defined(OMIT_ERROR_RECOVERY)
                if( yyerrflag )
                    --yyerrflag;
#endif
                token = yylex();
            }
        } else {
            // Reduce to production indicated by action
            // Production is
            //  target : token_or_target1 ...
            //
            // yyvp[] are the lexical values ($1, $2, etc.)
            production = action - YYUSED;
            lhs = yyprodtab[ production ];
            // Mask out length & lhs
            plen = lhs >> YYPRODSIZE;
            lhs &= (1 << YYPRODSIZE) - 1;
            yysp -= plen;
            yyvp -= plen;
#if !defined(OMIT_ERROR_RECOVERY)
            if( yysp < yys ){
                YYPRINT( "stack underflow\n" );
                YYABORT;
            }
#endif
            action = find_action( *yysp, lhs );
#if !defined(OMIT_ERROR_RECOVERY)
            if( action == YYNOACTION ) {
                YYPRINT( "missing nonterminal\n" );
                YYABORT;
            }
#endif
            *++yysp = action;
            ++yyvp;
            actions( production, yyvp );
        }
    }
}

static void actions( unsigned short production, YYSTYPE * yyvp )
{
    YYSTYPE             yyval;

    switch( production ) {

    default:
        // No action required
        return;
    }
    yyvp[0] = yyval;
}
