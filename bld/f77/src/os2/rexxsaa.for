c$include rexxsaa.fap

        logical function RXNULLSTRING( r )
        include 'rxstring.fi'
        record /rxstring/ r
        RXNULLSTRING = r.strptr .eq. 0
        end

        logical function RXZEROLENSTRING( r )
        include 'rxstring.fi'
        record /rxstring/ r
        RXZEROLENSTRING = r.strptr .ne. 0 .and. r.strlength .eq. 0
        end

        logical function RXVALIDSTRING( r )
        include 'rxstring.fi'
        record /rxstring/ r
        RXVALIDSTRING = r.strptr .ne. 0 .and. r.strlength .ne. 0
        end

        integer function RXSTRLEN( r )
        include 'rxstring.fi'
        record /rxstring/ r
        if( r.strptr .eq. 0 )then
            RXSTRLEN = 0
        else
            RXSTRLEN = r.strlength
        end if
        end

        integer function RXSTRPTR( r )
        include 'rxstring.fi'
        record /rxstring/ r
        RXSTRPTR = r.strptr
        end

        subroutine MAKERXSTRING( r, p, l )
        include 'rxstring.fi'
        record /rxstring/ r
        integer p, l
        r.strptr = p
        r.strlength = l
        end
