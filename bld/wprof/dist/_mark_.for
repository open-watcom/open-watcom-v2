*      _mark_.for       WATCOM Execution Sampler mark subprogram

*$pragma aux __MARK = zCC parm (string) [dx ax] modify exact []

        SUBROUTINE _MARK_( MARK )
        CHARACTER*(*) MARK
        CHARACTER*(81) NEWMARK

        NEWMARK = MARK(1:MIN(LEN(MARK),80)) // CHAR(0)
        CALL __MARK( NEWMARK )
        END
