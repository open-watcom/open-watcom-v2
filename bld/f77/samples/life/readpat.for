
c$define INCL_DOSFILEMGR
c$include os2.fap

        function ReadPatterns()
c
c       Read in all the pattern files from disk.
c       (*.LIF in the same directory LIFE.EXE)
c
        include                 'life.fi'
        include                 'os2.fi'

        integer                 i
        integer                 hdl
        record /FILEFINDBUF3/   dirinfo
        integer                 count
        integer                 rc
        integer*1               Pixie /1/


        MenuBitMap = NULL
        PatternMenu = NULL
        Cursor = 1
        Patterns(1) = loc( Pixie )
        PatternDimX(1) = 1
        PatternDimY(1) = 1
        NumberPatterns = 1
        hdl = 1
        count = 1
        rc = DosFindFirst( '*.LIF'c, hdl, 0, dirinfo,
     +                     isizeof( dirinfo ), count, FIL_STANDARD )
        if( rc .ne. 0 ) then
            ReadPatterns = .TRUE.
            return
        end if
        i = 2
        while( count .eq. 1 ) do
            if( .not. ReadAPatternFile( dirinfo.achName,
     +                                  dirinfo.cbFile, i ) ) then
                call Error( 'Error reading pattern file'c )
                ReadPatterns = .TRUE.
                return
            end if
            call DosFindNext( hdl, dirinfo, isizeof( dirinfo ), count )
            i = i + 1
        end while
        NumberPatterns = i-1
        ReadPatterns = .TRUE.
        return

        end
