c$ifdef __386__
c$define INCL_SUB
c$include os2.fap
c$else
*$pragma aux os2 "^" parm routine reverse [] \
*                    modify [ax bx cx dx es]
*$pragma aux (os2) VioSetCurPos parm (value)
c$endif

      implicit none
      integer NARROW, WIDE
      parameter (NARROW = 3)
      parameter (WIDE   = 4)
      integer*2 year, month, day
      character WideTitle*27 /'Sun Mon Tue Wed Thu Fri Sat'/
      character NarrowTitle*20 /'Su Mo Tu We Th Fr Sa' /

      call ClearScreen()

* get today's date

      call getdat( year, month, day )

* draw calendar for this month

      call Calendar( month, year, 10, 26, WIDE, WideTitle )

* draw calendar for last month

      month = month - 1
      if( month .lt. 1 )then
          month = 12
          year = year - 1
      endif
      call Calendar( month, year, 5, 3, NARROW, NarrowTitle )

* draw calendar for next month

      month = month + 2
      if( month .gt. 12 )then
          month = month - 12
          year = year + 1
      endif
      call Calendar( month, year, 5, 56, NARROW, NarrowTitle )

      call PosCursor( 20, 1 )
      end

      subroutine Calendar( month, year, row, col, width, title )
      implicit none
      integer*2 month, year
      integer row, col, width, nrow
      character*(*) title

      integer lentrim
      integer start, days, box_width, i

      character*9 str
      character*9 MonthName( 1:12 )
     &    / 'January', 'February', 'March', 'April',
     &      'May', 'June', 'July', 'August',
     &      'September', 'October', 'November', 'December' /
      integer Jump( 1:12 ) / 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 /
      integer FEBRUARY
      parameter (FEBRUARY = 2)
      integer MonthDays( 1:12 )
     &    / 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /

      box_width = 7 * width - 1
      call Box( row, col, box_width, 8 )
      str = MonthName( month )
      call PosCursor( row - 1,
     &    col + 1 + ( box_width - lentrim( str ) - 5 ) / 2 )
      print '(A,1X,I4)', str(1:lentrim( str )), year
      call PosCursor( row + 1, col + 1 )
      print *, title

      start = (year - 1900) + (year - 1900) / 4 + Jump( month )
      if( ( mod( year, 4 ) .eq. 0 ) .and. ( month .le. FEBRUARY ) )then
          start = start - 1
      endif
      start = mod( start, 7 ) + 1
      if( ( mod( year, 4 ) .eq. 0 ) .and. ( month .eq. FEBRUARY ) )then
          days = 29
      else
          days = MonthDays( month )
      endif
      nrow = row + 3
      do i = 1, days
          call PosCursor( nrow, col + width * start - 2 )
          print '(I2)', i
          if( start .eq. 7 )then
              nrow = nrow + 1
              start = 1
          else
              start = start + 1
          endif
      enddo
      end


      subroutine Box( row, col, width, height )
      implicit none
      integer row, col, width, height, i

      call Line( row, col, width, 'Ú', 'Ä', '¿' )
      call Line( row + 1, col, width, '³', ' ', '³' )
      call Line( row + 2, col, width, 'Ã', 'Ä', '´' )
      do i = 3, height
          call Line( row + i, col, width, '³', ' ', '³' )
      enddo
      call Line( row + height + 1, col, width, 'À', 'Ä', 'Ù' )
      end


      subroutine Line( row, col, width, left, centre, right )
      implicit none
      integer row, col, width, i
      character left, centre, right
      character buffer( 0:79 )

      buffer( 0 ) = left
      do i = 1, width
          buffer( i ) = centre
      enddo
      buffer( width + 1 ) = right
      call PosCursor( row, col )
      print *, (buffer( i ), i = 0, width + 1)
      end

      subroutine PosCursor( row, col )
      implicit none
      integer row, col
      integer*2 _row, _col, _hvio

c$ifdef __386__
      include 'os2.fi'
c$endif

      _row = row
      _col = col
      _hvio = 0
      call VioSetCurPos( _row, _col, _hvio )
      end

      subroutine ClearScreen()
      implicit none

      include 'fsublib.fi'

      call fsystem( 'cls' )
      end
