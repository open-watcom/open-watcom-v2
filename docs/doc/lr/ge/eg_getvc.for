        include 'graphapi.fi'
        include 'graph.fi'

        integer mode
        record /videoconfig/ vc
        character*80 buff

        call _getvideoconfig( vc )
        select( vc.adapter )
        case( _VGA, _SVGA )
            mode = _VRES16COLOR
        case( _MCGA )
            mode = _MRES256COLOR
        case( _EGA )
            if( vc.monitor .eq. _MONO )then
                mode = _ERESNOCOLOR
            else
                mode = _ERESCOLOR
            endif
        case( _CGA )
            mode = _MRES4COLOR
        case( _HERCULES )
            mode = _HERCMONO
        case default
            stop 'No graphics adapter'
        endselect
        if( _setvideomode( mode ) .ne. 0 )then
            call _getvideoconfig( vc )
            write( buff,
     +             '( i3, '' x '', i3, '' x '', i3, a1 )' )
     +             vc.numxpixels, vc.numypixels,
     +             vc.numcolors, char(0)
            call _outtext( buff )
            pause
            call _setvideomode( _DEFAULTMODE )
        endif
        end
