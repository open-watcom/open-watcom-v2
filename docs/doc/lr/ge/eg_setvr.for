        include 'graphapi.fi'
        include 'graph.fi'

        integer rows
        character*80 buff

        rows = _setvideomoderows( _TEXTC80, _MAXTEXTROWS )
        if( rows .ne. 0 )then
            write( buff,
     +             '(''Number of rows is '', i2, a1 )' )
     +             rows, char(0)
            call _outtext( buff )
            pause
            call _setvideomode( _DEFAULTMODE )
        endif
        end
