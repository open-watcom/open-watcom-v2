*      The Knight's Tour
*        by Jack Schueler

*$noextensions
*$nocheck
      CHARACTER REPLY*20, LINE(2)*41
      INTEGER I, J, D, SP, OFFSET, TI, TJ, BOARD
      INTEGER ISTACK, JSTACK, DSTACK, DI, DJ
      DIMENSION ISTACK(0:64), JSTACK(0:64), DSTACK(0:64)
      DIMENSION DI(0:11), DJ(0:11), BOARD(0:11, 0:11)
      DATA DI/2, 1, -1, -2, -2, -1, 1, 2, 2, 1, -1, -2/
      DATA DJ/-1, -2, -2, -1, 1, 2, 2, 1, -1, -2, -2, -1/
      PRINT 100
100   FORMAT(
     1'зддддбддддбддддбддддбддддбддддбддддбдддд©',/,
     2'Ё K  Ё N  Ё I  Ё G  Ё H  Ё T  Ё ''  Ё S  Ё',/,
     3'цддддеддддеддддеддддеддддеддддеддддедддд╢',/,
     4'Ё    Ё    Ё    Ё   Ё    Ё    Ё    Ё    Ё',/,
     5'цддддеддддеддддеддддеддддеддддеддддедддд╢',/,
     6'Ё    Ё    Ё T  Ё O  Ё U  Ё R  Ё    Ё    Ё',/,
     7'цимммоммммоммммоммммоммммоммммоммммоммммоммммммммммммммммм╩',/,
     8'Ё╨This program attempts to find a tour of the chess board ╨',/,
     9'ц╤using the knight''s move to cover all squares once and   ╨',/,
     A'Ё╨only once.  The algorithm uses a heuristic.  Given a    ╨',/,
     B'ц╤sufficient amount of time (sometimes many years) it will╨',/,
     C'Ё╨find a solution.  Fortunately solutions to many starting╨',/,
     D'ц╤positions are found fairly quickly (e.g., 4,3).         ╨',/,
     E'Ёхмммяммммяммммяммммяммммяммммяммммяммммяммммммммммммммммм╪',/,
     F'цддддеддддеддддеддддеддддеддддеддддедддд╢',/,
     G'Ё    Ё W  Ё A  Ё T  Ё C  Ё O  Ё M  Ё    Ё',/,
     H'юддддаддддаддддаддддаддддаддддаддддадддды' )

*--== What do tours such as those starting at (5,5), (6,6) and (7,7)
*          prove about the knight's tour? ==--

      LOOP
*--== Initialize edge positions to "invalid" ==--
          DO I = 0, 11
              DO J = 0, 11
                  BOARD( I, J ) = -1
              END DO
          END DO
*--== Initialize board positions to "not taken" ==--
          DO I = 2, 9
              DO J = 2, 9
                  BOARD( I, J ) = 0
              END DO
          END DO
*--== Get starting position ==--
          PRINT *,' '
          PRINT *,'Enter the starting row,column for the tour to begin.'
          PRINT *,'(1,1) is the top left corner.  Enter two integers'
          PRINT *,'separated by a comma or type ''QUIT''.'
          WRITE( *, '(A,$)' ) 'Row,Column: '
          READ( *, '(A)', END=99, ERR=99 ) REPLY
          READ( REPLY, *, END=99, ERR=99 ) I, J
          IF( I .GT. 8 .OR.  J .GT. 8 ) GO TO 99
*--== Find a solution ==--
          I = I+1
          J = J+1
          D = -1
          SP = 1
          LOOP :L1
              IF (I .LE. J) THEN
                  OFFSET = 4
                ELSE
                  OFFSET = 0
              END IF
              GUESS :G1
                  LOOP :L2
                      D = D + 1
                      IF( D .EQ. 8 ) QUIT :G1
                      TI = I + DI( D + OFFSET )
                      TJ = J + DJ( D + OFFSET )
                  UNTIL( BOARD( TI, TJ ) .EQ. 0 )
                  ISTACK( SP ) = I
                  JSTACK( SP ) = J
                  DSTACK( SP ) = D
                  BOARD( I, J ) = SP
                  I = TI
                  J = TJ
                  SP = SP + 1
                  D = -1
              ADMIT
                  SP = SP - 1
                  BOARD( I, J ) = 0
                  I = ISTACK( SP )
                  J = JSTACK( SP )
                  D = DSTACK( SP )
              END GUESS
          UNTIL( SP .EQ. 64 )
          BOARD( I, J ) = 64
          J = J - 1
*--== Print final board ==--
          PRINT *,' '
          PRINT *,'Solution of Knight''s Tour by WATFOR-77'
          PRINT *,'          1=Start    ()=Finish'
          PRINT *,'зддддбддддбддддбддддбддддбддддбддддбдддд©'
          DO K = 2, 8
              WRITE(LINE, 101) (BOARD( K, L ),L=2,9)
              IF( I .EQ. K )LINE(1)(J*5-2:J*5)='()'
              WRITE(*, '(A41)') LINE
          ENDDO
          WRITE(LINE, 102)(BOARD( 9, L ),L=2,9)
          IF( I .EQ. 9 )LINE(1)(J*5-2:J*5)='()'
          WRITE(*, '(A41)') LINE
      ENDLOOP
101       FORMAT(   'Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё ',I2,
     1    ' Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё',/,
     2    'цддддеддддеддддеддддеддддеддддеддддедддд╢')
102       FORMAT(   'Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё ',I2,
     1    ' Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё ',I2,' Ё',/,
     2    'юддддаддддаддддаддддаддддаддддаддддадддды')
99    END
