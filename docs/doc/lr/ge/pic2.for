	include 'graphapi.fi'
	include 'graph.fi'

	integer x1, y1, x2, y2, x3, y3, x4, y4
	parameter (x1=120)
	parameter (y1=90)
	parameter (x2=520)
	parameter (y2=390)
	parameter (x3=140)
	parameter (y3=20)
	parameter (x4=190)
	parameter (y4=460)

	call _setvideomode( _VRES16COLOR )
	call _pie( _GBORDER, x1, y1, x2, y2, x3, y3, x4, y4 )
	call _setlinestyle( '8080'x )
	call _rectangle( _GBORDER, x1, y1, x2, y2 )
	call _moveto( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 )
	call _lineto( x3, y3 )
	call _moveto( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 )
	call _lineto( x4, y4 )
	call _settextposition( 6, 8 )
	call _outtext( '(x1,y1)'c )
	call _settextposition( 25, 67 )
	call _outtext( '(x2,y2)'c )
	call _settextposition( 2, 10 )
	call _outtext( '(x3,y3)'c )
	call _settextposition( 29, 17 )
	call _outtext( '(x4,y4)'c )
*	call _settextposition( 16, 43 )
*	call _outtext( '((x1+x2)/2,(y1+y2)/2)'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
