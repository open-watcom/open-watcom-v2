
c$define INCL_DOSFILEMGR
c$include os2.fap

        subroutine      LoadPatternFile
c
c       Load the pattern file named in "Buffer" into our pattern menu.
c
        include         'life.fi'
        include         'os2.fi'

        integer                 hdl
        record /FILEFINDBUF3/   fbuff
        integer                 count
        integer*2               rc

        hdl = 1
        count = 1
        rc = DosFindFirst( loc( Buffer ), hdl, 0, fbuff,
     +                     isizeof( fbuff ), count, FIL_STANDARD )
        if( rc .ne. 0 .or. count .ne. 1 ) then
            call Error( 'File not found'c )
            return
        end if
        if( ReadAPatternFile( fbuff.achName,
     +                        fbuff.cbFile, NumberPatterns+1 ) ) then
            NumberPatterns = NumberPatterns + 1
            call CreatePatternMenu()
        end if

        end
