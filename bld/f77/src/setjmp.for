        include 'setjmp.fi'
        record /jmp_buf/ jmp_buf
        common jmp_buf
        integer rc
        rc = setjmp( jmp_buf )
        if( rc .eq. 0 )then
            call sub1()
        else
            print *, 'return code', rc
        endif
        end

        subroutine sub1()
        print *, 'calling sub2'
        call sub2()
        print *, 'returned from sub2'
        end

        subroutine sub2()
        include 'setjmp.fi'
        record /jmp_buf/ jmp_buf
        common jmp_buf
        call longjmp( jmp_buf, 73 )
        end
