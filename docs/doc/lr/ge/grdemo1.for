	include 'graphapi.fi'

	program grdemo1

	include 'graph.fi'

	integer TextColor, TextColor2, BorderColor
	record /videoconfig/ VC
	common TextColor, TextColor2, BorderColor, VC

	integer width, y

	integer initscreen

	if( initscreen() .eq. 0 )then
	    print *, 'No graphics adapter present'
	    stop
	endif

	! sweep text in from top & bottom of screen, gradually
	! increasing character size as the center of the screen
	! is approached.

	call _setcolor( TextColor )
	width = 0
	y = 5
	do while( y .lt. VC.numypixels / 2 - 10 )
	    call drawtext( width, y )
	    width = width + 1
	    y = y + 5
	enddo

	! draw text over final positions using a different color index

	call _setcolor( TextColor2 )
	call drawtext( width - 1, y - 5 )

	! draw a border around the screen

	call _setcolor( BorderColor )
	call _rectangle( _GBORDER, 0, 0, VC.numxpixels - 1,
     +			     VC.numypixels - 1 )

	if( VC.adapter .gt. _MCGA )then
	    call fadecolors()
	endif

	! wait for keyboard input and then reset the screen

	call _settextposition( VC.numtextrows, VC.numtextcols - 16 )
	call _outtext( 'Press any key...'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end


	subroutine drawtext( width, y )

	include 'graph.fi'

	! This routine displays the text strings.

	integer TextColor, TextColor2, BorderColor
	record /videoconfig/ VC
	common TextColor, TextColor2, BorderColor, VC

	integer width, y, xc

	xc = VC.numxpixels / 2
	call _setcharsize( width, width * 3 / 4 )
	call _settextalign( _CENTER, _BOTTOM )
	call _grtext( xc, y, 'WATCOM FORTRAN 77'c )
	call _setcharsize( width, width )
	call _settextalign( _CENTER, _TOP )
	call _grtext( xc, VC.numypixels - y, 'GRAPHICS'c )
	end


	integer function initscreen()

	include 'graph.fi'

	! This routine selects the best video mode
	! for a given adapter.

	integer TextColor, TextColor2, BorderColor
	record /videoconfig/ VC
	common TextColor, TextColor2, BorderColor, VC

	call _getvideoconfig( VC )
	select( VC.adapter )
	case( _VGA, _SVGA )
	    mode = _VRES16COLOR
	case( _MCGA )
	    mode = _MRES256COLOR
	case( _EGA )
	    if( VC.monitor .eq. _MONO )then
		mode = _ERESNOCOLOR
	    else
		mode = _ERESCOLOR
	    endif
	case( _CGA )
	    mode = _MRES4COLOR
	case( _HERCULES )
	    mode = _HERCMONO
	case default
	    initscreen = 0	  ! report insufficient hardware
	    return
	endselect

	if( _setvideomode( mode ) .eq. 0 )then
	    initscreen = 0
	    return
	endif
	call _getvideoconfig( VC )
	if( VC.numcolors .lt. 4 )then
	    TextColor = 1
	    TextColor2 = 1
	    BorderColor = 1
	else
	    TextColor = 1
	    TextColor2 = 3
	    BorderColor = 2
	endif
	if( VC.adapter .ge. _MCGA )then
	    call NewColors()
	endif
	initscreen = 1
	end


	subroutine newcolors()

	include 'graph.fi'

	! This routine sets the default colors for the program.

	integer TextColor, TextColor2, BorderColor
	record /videoconfig/ VC
	common TextColor, TextColor2, BorderColor, VC

	call _remappalette( TextColor, '003f0000'x )  ! light blue
	call _remappalette( TextColor2, '003f0000'x ) ! light blue
	call _remappalette( BorderColor, _BLACK )     ! black
	end


	subroutine fadecolors()

	include 'graph.fi'

	! This routine gradually fades the background text,
	! brightening the foreground text and the border
	! at the same time.

	integer TextColor, TextColor2, BorderColor
	record /videoconfig/ VC
	common TextColor, TextColor2, BorderColor, VC

	integer i, red, blue, green

	do i=1, 63
	    red = i
	    green = ishl( i, 8 )
	    blue = ishl( 63 - i, 16 )
	    call _remappalette( TextColor, blue )
	    call _remappalette( TextColor2, blue + green )
	    call _remappalette( BorderColor, red )
	enddo
	end
