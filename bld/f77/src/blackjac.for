**********************************************************************
*    Blackjack Card Game                                             *
*    - ilustrates many features of FORTRAN 77 and                    *
*      extensions supported by WATFOR-77                             *
*    - example of an interactive application                         *
*    - the program does not support "insurance" or pair splits       *
**********************************************************************

*$NOEXTENSIONS
      INTEGER DECKSZ
      PARAMETER (DECKSZ=4*52)
      INTEGER SEED
      COMMON SEED
      COMMON /CARDS/DECK
      CHARACTER*3 DECK( DECKSZ )
      CHARACTER*3 HAND(8), DEALER(8)
      INTEGER THAND, TDEAL, CCARD, NCHOICE
      REAL CASH
      LOGICAL DONE
      INTEGER*2 IHRS, IMINS, ISECS, IHDTHS
*     Supporting Functions
      CHARACTER*3 DEAL
      INTEGER TOTAL
      REAL GETBET
*
      CASH = 100.00
      CALL GETTIM( IHRS, IMINS, ISECS, IHDTHS )
      SEED = 477 + IMINS - ISECS + IHDTHS
      CCARD = DECKSZ
      WHILE( CASH .GT. 0.0 )DO
          BET = GETBET( CASH )
          IF( BET .EQ. 0 ) QUIT
          CASH = CASH - BET
          HAND(1) = DEAL( CCARD )
          DEALER(1) = DEAL( CCARD )
          HAND(2) = DEAL( CCARD )
          DEALER(2) = DEAL( CCARD )
          NHAND = NDEAL = 2
          DONE = .FALSE.
          NCHOICE = 0
          LOOP
              THAND = TOTAL(HAND,NHAND)
100           CALL STATUS(HAND, NHAND, DEALER, NDEAL, DONE)
              IF( DONE )QUIT
              IF( THAND .EQ. 21 ) QUIT
              IF( NCHOICE .EQ. 3 ) QUIT
              IF( NHAND .EQ. 8 ) QUIT
              MAXC = 2
              IF( CASH .GE. BET )THEN
                  MAXC = 3
              ENDIF
              LOOP
                  WRITE( *, FMT=101 ) (I,I=1,MAXC)
101               FORMAT(/,'Press',I2,' for ''Stay'';',
     1                             I2,' for ''Hit'';',
     2                             I2,' for ''Double Down''' )
                  READ( *, *, END=100, ERR=100 ) NCHOICE
              UNTIL( NCHOICE .GE. 1  .AND.  NCHOICE .LE. MAXC )
              IF( NCHOICE .EQ. 1 ) QUIT
              IF( NCHOICE .EQ. 3 )THEN
                      CASH = CASH - BET
                      BET = BET * 2
              ENDIF
              NHAND = NHAND + 1
              HAND(NHAND) = DEAL( CCARD )
          ENDLOOP
          IF( NHAND .EQ. 2  .AND.  THAND .EQ. 21 )THEN
              PRINT *, 'Congratulations! Pays 1 and 1/2 times.'
              CASH = CASH + BET + BET * 1.5
              DONE = .TRUE.
          ENDIF
          IF( .NOT. DONE )THEN
              LOOP
                  IF( TOTAL(DEALER,NDEAL) .GE. 17 ) QUIT
                  NDEAL = NDEAL + 1
                  DEALER(NDEAL) = DEAL( CCARD )
                  IF( NDEAL .EQ. 8 ) QUIT
              ENDLOOP
              TDEAL = TOTAL(DEALER,NDEAL)
              PRINT 300, THAND, TDEAL,
     1                   (HAND(I),I=1,NHAND),  ('   ',I=NHAND+1,8),
     2                   (DEALER(I),I=1,NDEAL),('   ',I=NDEAL+1,8)
              IF( TDEAL .GT. 21 )THEN
                  PRINT *, 'Dealer Busted!'
                  CASH = CASH + BET + BET
              ELSEIF( TDEAL .GT. THAND )THEN
                  PRINT *, 'Dealer Wins.'
              ELSEIF( TDEAL .EQ. THAND )THEN
                  PRINT *, 'Draw... no winner'
                  CASH = CASH + BET
              ELSE
                  PRINT *, 'You win...'
                  CASH = CASH + BET + BET
              ENDIF
          ENDIF
          CASH = INT( CASH * 100 ) / 100.0
      ENDWHILE
      CALL RESULTS( CASH, 100.0 )
      STOP
300   FORMAT(
     1  /,  ' Your Cards           Total: ',I2,
     2 4X,  ' Dealer Cards         Total: ',I2,
     3  /,2('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ '),
     4  /,2('³' ,8(A3,1X),                   '³ '),
     5  /,2('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ '))
      END

      FUNCTION GETBET( CASH )
      REAL CASH
      REAL BET
      CHARACTER*20 RESPNS
      LOOP
          BET = 0.0
100       WRITE( *, FMT=101 ) CASH
101       FORMAT( /,'You have $',F8.2,
     1            /,'Enter your bet or QUIT: ',$)
          READ( 5, FMT='(A)', END=100, ERR=100, IOSTAT=IOS )RESPNS
          IF( RESPNS(1:1) .EQ. 'Q'  .OR.  RESPNS(1:1) .EQ. 'q' ) QUIT
          READ( RESPNS, *, END=100, ERR=100 ) BET
          IF( BET .GT. 0  .AND.  BET .LE. CASH ) QUIT
          IF( BET .GT. CASH )THEN
              PRINT *, 'You can''t cover that!'
          ELSE
              PRINT *, 'Give me a break ...'
              IF( BET. EQ. 0 )THEN
                  PRINT *, 'Make a bet or move on!'
              ELSE
                  PRINT *, 'You can''t bet negative!'
              ENDIF
          ENDIF
      ENDLOOP
      GETBET = BET
      END

      SUBROUTINE RESULTS( CASH, START )
      REAL CASH, START
      IF( CASH .LE. 0.0 )THEN
          PRINT *, 'You''re broke!'
          PRINT *, 'Sorry, but you''ll have to visit your banker.'
          PRINT *, 'Better luck next time.'
      ELSE
          PRINT 200, CASH
200       FORMAT('You are leaving with $',F8.2,' in your pocket.')
          PROFIT = CASH - START
          IF( PROFIT .GT. 0 )THEN
              PRINT 201, PROFIT
201           FORMAT('You actually gained $',F8.2,'!')
              PRINT *, 'You won''t get away with that again.'
          ELSE
              PRINT 205, -PROFIT
205           FORMAT('You lost $',F6.2,'.')
              PRINT *, 'Win some, lose some more.'
              PRINT *, 'That''s the way to play the game!'
          ENDIF
      ENDIF
      END

      INTEGER FUNCTION TOTAL( CARDS, NUMCARDS )
      CHARACTER*3 CARDS(8)
      INTEGER NUMCARDS
      TOTAL = 0
      ZERO = ICHAR('0')
      DO I = 1, NUMCARDS
          IF( CARDS(I)(2:2) .EQ. 'J'
     1   .OR. CARDS(I)(2:2) .EQ. 'Q'
     2   .OR. CARDS(I)(2:2) .EQ. 'K' )THEN
              TOTAL = TOTAL + 10
          ELSEIF( CARDS(I)(2:2) .EQ. 'A' )THEN
              IF( TOTAL .LT. 11 )THEN
                  TOTAL = TOTAL + 11
              ELSE
                  TOTAL = TOTAL + 1
              ENDIF
          ELSEIF( CARDS(I)(1:2) .EQ. '10' )THEN
              TOTAL = TOTAL + 10
          ELSE
              TOTAL = TOTAL + ICHAR( CARDS(I)(2:2) ) - ZERO
          ENDIF
      ENDDO
      DO I = 1, 2
          IF( (CARDS(I)(2:2) .EQ. 'A') .AND. (TOTAL .GT. 21) )THEN
              TOTAL = TOTAL - 10
          ENDIF
      ENDDO
      END

      SUBROUTINE STATUS( HAND, NHAND, DEALER, NDEAL, DONE )
      CHARACTER*3 HAND(8), DEALER(8)
      INTEGER NHAND, NDEAL
      LOGICAL DONE
      INTEGER TOTAL, THAND
      THAND = TOTAL(HAND, NHAND)
      PRINT 300, THAND, (HAND(I),I=1,NHAND),
     1                  ('   ',I=NHAND+1,8),
     2                  (DEALER(I),I=2,NDEAL)
300   FORMAT(
     1  /, ' Your Cards           Total: ',I2,'     Dealer Cards '
     2 ,/, 'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ÚÄÄÄÄÄÄÄÄÄÄÄÄÄ¿'
     3 ,/, '³' ,8(A3,1X),                   '³ ³ ?? ',A3,'      ³'
     4 ,/, 'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÙ')
      IF( THAND .GT. 21 )THEN
          PRINT *, 'You''re ***BUSTED***!'
          DONE = .TRUE.
      ENDIF
      END

      CHARACTER FUNCTION DEAL*3( CARDNO )
**
** Deal -- deal a card from the deck
**         reshuffling and cutting if necessary
**
      INTEGER DECKSZ
      PARAMETER (DECKSZ=4*52)
      CHARACTER*3 DECK( DECKSZ )
      COMMON /CARDS/DECK
      INTEGER CARDNO
      IF( CARDNO .GE. (DECKSZ*.90) )THEN
          CALL SHUFFLE
          CALL CUT
          CARDNO = 2
      ENDIF
      DEAL = DECK( CARDNO )
      CARDNO = CARDNO + 1
      END

      SUBROUTINE SHUFFLE
**
** Shuffle -- shuffle a number of 52-card decks
**
      INTEGER DECKSZ
      PARAMETER (DECKSZ=4*52)
      CHARACTER*3 DECK( DECKSZ )
      INTEGER SEED
      COMMON SEED
      COMMON /CARDS/DECK
      CHARACTER*3 TEMP
      PRINT *, 'Shuffling cards...'
      DO I = 1, DECKSZ
          J = URAND( SEED ) * DECKSZ + 1
          K = URAND( SEED ) * DECKSZ + 1
          IF( J .NE. K )THEN
              TEMP = DECK( J )
              DECK( J ) = DECK( K )
              DECK( K ) = TEMP
          ENDIF
      ENDDO
      END

      SUBROUTINE CUT
**
** Cut - cut a card deck
**
      INTEGER DECKSZ
      PARAMETER (DECKSZ=4*52)
      CHARACTER*3 DECK( DECKSZ )
      INTEGER SEED
      COMMON SEED
      COMMON /CARDS/DECK
      CHARACTER*3 TEMP
100   WRITE( *, FMT=101 ) DECKSZ - 10
101   FORMAT( 'Cutting the deck...',/,
     1'Enter a number between 10 and ',I3,': ')
      READ( *, FMT=102, END=100, ERR=100 ) ICUT
102   FORMAT( I5 )
      WHILE( ICUT .LT. 10    .OR.    ICUT .GT. DECKSZ - 10 )DO
          ICUT = URAND( SEED ) * DECKSZ - 10
      ENDWHILE
      DO I = ICUT, DECKSZ
          TEMP = DECK( I - ICUT + 1 )
          DECK( I - ICUT + 1 ) = DECK( I )
          DECK( I ) = TEMP
      ENDDO
      END

      BLOCK DATA
      INTEGER DECKSZ
      PARAMETER (DECKSZ=4*52)
      COMMON /CARDS/DECK
      CHARACTER*3 DECK( DECKSZ )
      DATA (DECK(I),I=52*0+1,52*1)/
     1' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9','10',' J',
     2' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9',
     3'10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',
     4' 8',' 9','10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',
     5' 6',' 7',' 8',' 9','10',' J',' Q',' K'/
      DATA (DECK(I),I=52*1+1,52*2)/
     1' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9','10',' J',
     2' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9',
     3'10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',
     4' 8',' 9','10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',
     5' 6',' 7',' 8',' 9','10',' J',' Q',' K'/
      DATA (DECK(I),I=52*2+1,52*3)/
     1' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9','10',' J',
     2' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9',
     3'10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',
     4' 8',' 9','10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',
     5' 6',' 7',' 8',' 9','10',' J',' Q',' K'/
      DATA (DECK(I),I=52*3+1,52*4)/
     1' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9','10',' J',
     2' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',' 8',' 9',
     3'10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',' 6',' 7',
     4' 8',' 9','10',' J',' Q',' K',' A',' 2',' 3',' 4',' 5',
     5' 6',' 7',' 8',' 9','10',' J',' Q',' K'/
      END
