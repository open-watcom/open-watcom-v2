c$include winapi.fi


      subroutine button_size( hwnd, id, pwidth, pheight )
      integer*2       hwnd
      integer*2       id
      integer*2       pwidth
      integer*2       pheight

      include 'drawwin.fi'

      integer*2       inst
      integer*2       bmp
      record /BITMAP/ bitmap

      inst = GetWindowWord( hwnd, GWW_HINSTANCE )
      bmp = LoadBitmap( inst, MAKEINTRESOURCE( id ) )
      if( bmp .eq. 0 ) then
          return
      endif
      call GetObject( bmp, isizeof(BITMAP), bitmap )
      pwidth = bitmap.bmWidth + 5
      pheight = bitmap.bmHeight + 5
      call DeleteObject( bmp )
      end


      subroutine add_button(parent, top, left, id, pwidth, pheight)
      integer*2 parent
      integer*2 top
      integer*2 left
      integer*2 id
      integer*2 pwidth
      integer*2 pheight

      include 'drawwin.fi'

      integer*2 hbutton
      integer*2 inst

      ! Note that the resource ID is the same as the control ID
      inst = GetWindowWord( parent, GWW_HINSTANCE )
      call button_size( parent, id, pwidth, pheight )

      hbutton = CreateWindow( 'BUTTON'//char(0), NULL,
     &         WS_CHILD .or. WS_VISIBLE .or. BS_OWNERDRAW,
     &         left, top, pwidth, pheight, parent, id, inst,0)
      end


      subroutine measure_button( parent, button_id, measure )
      integer*2 parent
      integer*2 button_id
      record /MEASUREITEMSTRUCT/ measure

      include 'drawwin.fi'

      integer*2 width
      integer*2 height

      call button_size( parent, button_id, width, height )
      measure.itemWidth = width
      measure.itemHeight = height
      end


      subroutine horizontal( draw, row )
      record /DRAWITEMSTRUCT/ draw
      integer*2 row

      include 'drawwin.fi'

      call MoveTo( draw.hDC, draw.rcItem.left + 1, row )
      call LineTo( draw.hDC, draw.rcItem.right - 1, row )
      end


      subroutine vertical( draw, column )
      record /DRAWITEMSTRUCT/ draw
      integer*2      column

      include 'drawwin.fi'

      call MoveTo( draw.hDC, column, draw.rcItem.top + 1 )
      call LineTo( draw.hDC, column, draw.rcItem.bottom - 1 )
      end


      subroutine draw_button( button_id, draw )
      integer*2      button_id
      record /DRAWITEMSTRUCT/ draw

      include 'drawwin.fi'

      integer*2      bmp
      integer*2   oldbmp
      integer*2   blackpen
      integer*2   shadowpen
      integer*2   brightpen
      integer*2   facepen
      integer*2   oldpen
      record /BITMAP/ bitmap
      integer*2   memdc
      integer*2   inst
      integer*2   shift

      inst = GetWindowWord( draw.hwndItem, GWW_HINSTANCE )
      bmp = LoadBitmap( inst, MAKEINTRESOURCE( button_id ) )
      if( bmp .eq. 0 ) then
          return
      endif
      call GetObject( bmp, isizeof(BITMAP), bitmap )
      memdc = CreateCompatibleDC( draw.hDC )
      oldbmp = SelectObject( memdc, bmp )
      if( ( draw.itemState .and. ODS_SELECTED ) .ne. 0 ) then
            shift = 4
      else
            shift = 2
      endif
      call BitBlt( draw.hDC, draw.rcItem.left + shift,
     &           draw.rcItem.top + shift, bitmap.bmWidth,
     &           bitmap.bmHeight, memdc, 0, 0, SRCCOPY )
      call SelectObject( memdc, oldbmp )
      call DeleteDC( memdc )
      call DeleteObject( bmp )

      ! Draw four sides of the button except one pixel in corners
      blackpen = CreatePen( PS_SOLID, 0, RGB(0,0,0) )
      brightpen = CreatePen( PS_SOLID, 0, RGB(255,255,255) )
      shadowpen = CreatePen( PS_SOLID, 0,
     &            GetSysColor( COLOR_BTNSHADOW ) )
      facepen = CreatePen( PS_SOLID, 0,
     &            GetSysColor( COLOR_BTNFACE ) )
      oldpen = SelectObject( draw.hDC, blackpen )
      call horizontal( draw, draw.rcItem.top )
      call horizontal( draw, draw.rcItem.bottom - 1 )
      call vertical( draw, draw.rcItem.left )
      call vertical( draw, draw.rcItem.right - 1 )

      ! Now the shading
      call SelectObject( draw.hDC, shadowpen )
      if( ( draw.itemState .and. ODS_SELECTED ) .ne. 0 ) then
            call horizontal( draw, draw.rcItem.top + 1 )
            call vertical( draw, draw.rcItem.left + 1 )
            call SelectObject( draw.hDC, facepen )
            call horizontal( draw, draw.rcItem.top + 2 )
            call vertical( draw, draw.rcItem.left + 2 )
            call horizontal( draw, draw.rcItem.top + 3 )
            call vertical( draw, draw.rcItem.left + 3 )
      else
            call horizontal( draw, draw.rcItem.bottom - 2 )
            call horizontal( draw, draw.rcItem.bottom - 3 )
            call vertical( draw, draw.rcItem.right - 2 )
            call vertical( draw, draw.rcItem.right - 3 )
            call SelectObject( draw.hDC, brightpen )
            call horizontal( draw, draw.rcItem.top + 1 )
            call vertical( draw, draw.rcItem.left + 1 )
      endif
      call SelectObject( draw.hDC, oldpen )
      call DeleteObject( blackpen )
      call DeleteObject( brightpen )
      call DeleteObject( shadowpen )
      call DeleteObject( facepen )
      end
