        integer i, ifile, ofile, ipf_file, msg$idx, res_id
        integer fgetcmd, cmdlen
        character*132 record, idx*2, res$id*5
        character*80 header
        character*128 cmdline
        logical eof, include, noheader, cp_error, watfor
        watfor = .false.
        cmdlen = fgetcmd( cmdline )
        if( cmdlen .ne. 0 )then
            if( cmdline(cmdlen:cmdlen) .eq. 'w' )then
                watfor = .true.
            end if
        end if
        ifile = 1
        ofile = 2
        ipf_file = 3
        open( unit=ifile, file='error.msg', form='formatted',
     &    action='read' )
        open( unit=ofile, file='ferror.gml', form='formatted' )
        if( watfor )then
            open( unit=ipf_file, file='wf77errs.ipf', form='formatted' )
        else
            open( unit=ipf_file, file='wfcerrs.ipf', form='formatted' )
        endif
        eof = .false.
        read( ifile, fmt='(A)' ) record
        write( ipf_file, 100 ) ':userdoc.'
        res_id = 20000  ! resource id must be greater that 20000
        loop : file
            if( record(1:2) .eq. 'MS' ) quit
            header = record(4:)
            noheader = .true.
            msg$idx = 1
            read( ifile, fmt='(A)' ) record
            loop : group
                if( record(3:3) .eq. ' ' ) quit : group
                write( idx, '(i2.2)' ) msg$idx
                write( res$id, '(i5.5)' ) res_id + msg$idx
                i = index( record, '[' )
                if( watfor )then
                    include = record(i+2:i+2) .ne. 'o'
                    cp_error = .true.
                else
                    include = record(i+2:i+2) .ne. 'w'
                    cp_error = ( record(i+4:i+4) .eq. 'c' ) .or.
     &                         ( record(i+4:i+4) .eq. ' ' )
                endif
                if( include )then
                    if( noheader )then
                        write( ofile, 100 )
     &                      '.errhead '//header(1:lentrim(header))
                        noheader = .false.
                    endif
                    if( cp_error )then
                        write( ipf_file, 100 )
     &                    ':h1 res='//res$id//'.'//record(1:2)//'-'//idx
                    endif
                    i = i + index( record(i:), ']' )
                    call massage( record(i+1:) )
                    write( ofile, 100 )
     &                '.errnote '//record(1:2)//'-'//idx//' '//
     &                record(i+1:lentrim(record))
                    call ipf_massage( record(i+1:) )
                    write( ofile, 100 ) '.pc'
                    if( cp_error )then
                        write( ipf_file, 100 )
     &                    ':hp2.'//record(i+1:lentrim(record))//':ehp2.'
                        write( ipf_file, 100 ) ':p.'
                    endif
                endif
                loop    ! skip messages in other languages
                    read( ifile, fmt='(A)' ) record
                until( record(1:1) .eq. ' ' )
                backspace( ifile )
                loop : expl
                    read( ifile, fmt='(A)' ) record
                    at end do
                        eof = .true.
                        quit : expl
                    end at end
                    if( record(1:1) .ne. ' ' ) quit : expl
                    if( include )then
                        write( ofile, 100 ) record(2:lentrim(record))
                        if( cp_error )then
                            call ipf_massage( record(2:) )
                            write( ipf_file, 100 )
     &                        record(2:lentrim(record))
                        endif
                    endif
                endloop
                msg$idx = msg$idx + 1
            until( eof )
            res_id = res_id + 256
            if( .not. noheader )then
                write( ofile, 100 ) '.errtail'
            endif
        until( eof )
        write( ipf_file, 100 ) ':euserdoc.'
        close( ifile )
        close( ofile )
        close( ipf_file )
100     format( A )
        end

        subroutine massage( record )
        character*(*) record
        integer pos
        loop
            pos = index( record, '%u' )
            if( pos .eq. 0 ) pos = index( record, '%i' )
            if( pos .eq. 0 ) quit
            record(pos+1:pos+1) = 'd'
        endloop
        end

        subroutine ipf_massage( record )
        character*(*) record
        integer pos
        character*132 first, last
        loop
            pos = index( record, ':' )
            if( pos .eq. 0 ) quit
            first = record(:pos-1)
            last = record(pos+1:)
            record = first(:lentrim(first))// '&colon.'//
     &        last(:lentrim(last))
        endloop
        if( index( record, '.millust begin' ) .ne. 0 )then
            record = ':xmp.'
        else if( index( record, '.millust end' ) .ne. 0 )then
            record = ':exmp.'
        else if( index( record, '.autonote' ) .ne. 0 )then
            record = ':ol.'
        else if( index( record, '.note' ) .ne. 0 )then
            record = ':li.'
        else if( index( record, '.endnote' ) .ne. 0 )then
            record = ':eol.'
        else if( index( record, '.pc' ) .ne. 0 )then
            record = ':p.'
        else if( index( record, '.sy' ) .ne. 0 )then
            first = record(5:lentrim(record))
            record = ':hp2.' // first(:lentrim(first)) // ':ehp2'
        endif
        end
