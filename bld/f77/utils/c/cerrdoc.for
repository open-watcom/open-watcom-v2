        integer ifile, ipf_file, GetResId
        character*132 record, res$id*5
        ifile = 1
        ipf_file = 2
        open( unit=ifile, file='cerror.gml', form='formatted',
     &    action='read' )
        open( unit=ipf_file, file='wccerrs.ipf', form='formatted' )
        read( ifile, fmt='(A)' ) record
        write( ipf_file, 100 ) ':userdoc.'
        loop : file
            loop
                if( record(1:5) .eq. '.note' ) quit
                read( ifile, fmt='(A)' ) record
                atend, quit : file
            endloop
            write( res$id, '(i5.5)' ) 20000 + GetResId( record(8:) )
            write( ipf_file, 100 ) ! resource id must be greater that 20000
     &        ':h1 res='//res$id//'.'//record(7:lentrim(record))
            read( ifile, fmt='(A)' ) record
            call special_chars( record(5:) )
            write( ipf_file, 100 )
     &        ':hp2.'//record(5:lentrim(record))//':ehp2.'
            write( ipf_file, 100 ) ':p.'
            read( ifile, fmt='(A)' ) record ! skip ".np"
            loop ! process error message description
                read( ifile, fmt='(A)' ) record
                atend, quit : file
                if( record(1:5) .eq. '.note' ) quit
                if( record(1:8) .eq. '.begnote' ) quit
                if( record(1:8) .eq. '.endnote' ) quit
                if( record(1:5) .eq. '.keep' ) quit
                call ipf_massage( record )
                write( ipf_file, 100 ) record(1:lentrim(record))
            endloop
        endloop
        write( ipf_file, 100 ) ':euserdoc.'
        close( ifile )
        close( ipf_file )
100     format( A )
        end

        integer function GetResId( record )
        character*(*) record
        GetResId = 0
        do i = 1, len( record )
            if( record(i:i) .lt. '0' .or. record(i:i) .gt. '9' )quit
            GetResId = GetResId * 10 + ichar( record(i:i) ) - ichar('0')
        enddo
        end

        subroutine special_chars( record )
        character*(*) record
        integer pos, i
        character*132 first, last
        i = 1
        loop
            pos = index( record, ':' )
            if( pos .ne. 0 )then
                first = record(:pos-1)
                last = record(pos+1:)
                record = first(:lentrim(first))// '&colon.'//
     &            last(:lentrim(last))
                i = pos + 1
            endif
            pos = index( record(i:), '&' )
            if( pos .eq. 0 ) quit
            first = record(:pos-1)
            last = record(pos+1:)
            record = first(:lentrim(first))// '&amp.'//
     &        last(:lentrim(last))
            i = pos + 1
        endloop
        end

        subroutine ipf_massage( record )
        character*(*) record
        character*132 tmp
        call special_chars( record )
        if( index( record, '.exam begin' ) .ne. 0 )then
            record = ':xmp.'
        else if( index( record, '.exam end' ) .ne. 0 )then
            record = ':exmp.'
        else if( index( record, '.kw' ) .ne. 0 )then
            tmp = record(5:lentrim(record))
            record = ':hp2. '// tmp(:lentrim(tmp)) // ' :ehp2.'
        else if( index( record, '.pc' ) .ne. 0 .or.
     &           index( record, '.np' ) .ne. 0 )then
            record = ':p.'
        else if( index( record, '.id' ) .ne. 0 .or.
     &           index( record, '.sy' ) .ne. 0 .or.
     &           index( record, '.us' ) .ne. 0 )then
            tmp = record(5:lentrim(record))
            record = ':hp1. '// tmp(:lentrim(tmp)) // ' :ehp1.'
        endif
        end
