c$define INCL_DOSDEVICES
c$include os2.fap

C       open serial port and set transfer characteristics
C
C       iport - port number to open: 1, 2, 3, 4
C       ibaud - baud rate:      1200, 2400, 4800, 9600, etc
C       ibits - #data bits:     5, 6, 7, 8
C       istop - #stop bits:     0 (1 stop bit)
C                               1 (1.5 stop bits)
C                               2 (2 stop bits)
C       ipar  - parity:         0 (none)
C                               1 (odd)
C                               2 (even)
C                               3 (mark)
C                               4 (space)
C
        subroutine opencom( iport, ibaud, ibits, istop, ipar )
c$include bse.fi
        character*10 comport
        integer*2 syshandle, h
        integer*2 ibaudtmp
        integer*1 serial(3)

        integer         ASYNC
        parameter( ASYNC = 1 )

        integer         SETBAUDRATE, SETLINECTRL
        parameter( SETBAUDRATE = 65, SETLINECTRL = 66 )

        write( comport, '(3hCOMI1)' ) iport
        open( unit=1, file=comport, access='sequential',
     &        form='unformatted', recordtype='fixed' )
        write( 1 ) ! force allocation of handle
        h = syshandle( 1 )
        ibaudtmp = ibaud
C
C       Really should check the error return on this call
C
        call DosDevIOCtl( h, ASYNC, SETBAUDRATE,
     &      LOC( ibaudtmp ), 2, 2, 0, 0, 0 )
        serial(1) = ibits
        serial(2) = ipar
        serial(3) = istop
C
C       Really should check the error return on this call
C
        call DosDevIOCtl( h, ASYNC, SETLINECTRL,
     &      LOC( serial ), 3, 3, 0, 0, 0 )
        return
        end

C       transfer data to the serial port
C
        subroutine writecom( data )
        character*(*) data

        write( 1 ) data
        return
        end

C       close the serial port
C
        subroutine closecom
        close( 1 )
        return
        end

C
C       Test mainline. Write a string to a port at various baud rates
C       and line characteristics
C
C       loop
C           read( *, * ) iport, ibaud, ibits, ipar, istop
C           at end, quit
C           call opencom( iport, ibaud, ibits, istop, ipar )
C           call writecom( 'hi there' // CHAR( 13 ) // CHAR( 10 ) )
C           call closecom
C       endloop
C       stop
C       end
