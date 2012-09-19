c$define INCL_DOSPROCESS
c$define INCL_PM
c$include os2.fap


      subroutine make_buttons( hwnd )
      integer hwnd
      integer offset
      integer max_height
      integer btn_width
      integer btn_height
      integer index
      integer buttons( 9 )

      include 'drawos2.fi'

      buttons( 1 ) = IDM_ROTATE_UP
        buttons( 2 ) = IDM_MOVE_IN
        buttons( 3 ) = IDM_ROTATE_LEFT
        buttons( 4 ) = IDM_ROTATE_RIGHT
        buttons( 5 ) = IDM_MOVE_OUT
        buttons( 6 ) = IDM_ROTATE_DOWN
        buttons( 7 ) = IDM_MORE_BRIGHTNESS
        buttons( 8 ) = IDM_LESS_BRIGHTNESS
        buttons( 9 ) = 0

      Draw_area.yTop = Draw_area.yTop - 2
      offset = Draw_area.xLeft + 2
      max_height = 0
      index = 1
      while( buttons( index ) .ne. 0 ) do
          call add_button( hwnd, Draw_area.yTop, offset,
     &              buttons( index ), btn_width, btn_height )
             if( max_height .lt. btn_height ) then
              max_height = btn_height
          endif
          index = index + 1
          offset = offset + btn_width - 1
      end while
      ! remove buttons from drawing area
      Draw_area.yTop = Draw_area.yTop - ( max_height + 2 )
      Button_height = max_height + 4
      end


      include 'drawos2.fi'
      include 'threed.fi'

      integer       init_app
      integer        init_instance
      record /QMSG/ qmsg

      Main_hwnd = 0
      Main_hab = 0
      Main_hmq = 0

c$ifdef DRAW_ALL_AT_ONCE
      Hdc = 0
      Mem_dc = 0
      Draw_bitmap = 0
      Old_bitmap = 0
c$endif

c$ifdef undefined
      call memclear( 0, loc( Draw_area ), isizeof( RECTL ) )
c$else
      call memclear( loc( Draw_area ), 0, isizeof( RECTL ) )
c$endif
      hThree_d = 0
      Button_height = 0
      Auto_run = 0

      Main_hab = WinInitialize( 0 )
      if( Main_hab .eq. 0 ) then
            return
      endif

      Main_hmq = WinCreateMsgQueue( Main_hab, 0 )
      if( Main_hmq .eq. 0 ) then
            return
      endif

      if( init_app( Main_hab ) .eq. 0 ) then
            return
      endif

      ! Perform initializations that apply to a specific instance

      call three_d_init()
      call define_room()
      if ( init_instance( SWP_SHOW .or. SWP_ACTIVATE ) .eq. 0 ) then
            call finish_room()
            call three_d_fini()
            return
        endif

        ! Acquire and dispatch messages until WM_QUIT is received.

        while(WinGetMsg(Main_hab, qmsg, NULL, NULL, NULL) .ne. 0) do
            call WinDispatchMsg( Main_hab, qmsg )
      endwhile
      call finish_room()
      call three_d_fini()

      if( Main_hmq .ne. 0 ) then
          call WinDestroyMsgQueue( Main_hmq )
      endif

      if( Main_hab .ne. 0 ) then
             call WinTerminate( Main_hab )
      endif

      call DosExit( 1, 0)
      end


      subroutine add_wrap( attr, add, minimum, maximum )
      integer attr
      integer add
      integer minimum
      integer maximum

      integer start

        include 'drawos2.fi'
      include 'threed.fi'

        start = three_d_get( hThree_d, attr )
      start = start + add

      while( start .ge. maximum ) do
            start = start - ( maximum - minimum )
      end while

      while( start .lt. minimum ) do
            start = start + maximum - minimum
      end while

      call three_d_set( hThree_d, attr, start )
      end


      subroutine add_range( attr, add, minimum, maximum )
      integer attr
      integer add
      integer minimum
      integer maximum

      integer start

        include 'drawos2.fi'
      include 'threed.fi'

      start = three_d_get( hThree_d, attr )
      start = start + add

      if( start .ge. maximum ) then
            call three_d_set( hThree_d, attr, maximum )
      else if( start .lt. minimum ) then
            call three_d_set( hThree_d, attr, minimum )
      else
            call three_d_set( hThree_d, attr, start )
      endif
      end


      subroutine draw_box(x1, x2, y1, y2, z1, z2, red, green, blue)
      real    x1
      real    x2
      real    y1
      real    y2
      real    z1
      real    z2
      integer red
      integer green
      integer blue

        include 'drawos2.fi'
      include 'threed.fi'

      call three_d_box(hThree_d,x1,x2,y1,y2,z1,z2,red,green,blue)
      end


      subroutine memclear( locale, ch, len )
      integer*4 locale
      integer*1 ch
      integer   len
      integer    cnt
      integer*1 arr(:)

      allocate( arr( len ), location=locale )
      cnt = len
      while( cnt .gt. 0 ) do
          arr( cnt ) = ch
          cnt = cnt - 1
      end while
      end


      subroutine new_display()
c$ifdef DRAW_ALL_AT_ONCE

      include 'drawos2.fi'

      GpiSetBitmap( Mem_dc, Old_bitmap )
      GpiDeleteBitmap( Draw_bitmap )
      Draw_bitmap = NULLHANDLE
      GpiDestroyPS( Mem_dc )
      DevCloseDC( Hdc )
c$endif
      end


      subroutine draw_stuff( hwnd )
      integer hwnd

        include 'drawos2.fi'
      include 'threed.fi'

      integer        win_dc
      record /RECTL/ paint
      record /RECTL/ intersect

c$ifdef DRAW_ALL_AT_ONCE
      integer                    old_top
      integer                    width
      integer           height
      record /SIZEL/             sizl
      data sizl /0, 0/
      record /BITMAPINFOHEADER2/ bmih
      integer                    formats(24)
      record /POINTL/            pts(3)
      integer                    old_cursor
      integer                    hour_glass_cur
      record /RECTL/             interior
      record /DEVOPENSTRUC/      dop
      data dop /0, 0, 0, 0, 0, 0, 0, 0, 0/

      dop.pszDriveName = LOC( 'DISPLAY'//char(0) )
c$endif

      win_dc = WinBeginPaint( hwnd, 0, paint )
      call GpiCreateLogColorTable(win_dc, 0, LCOLF_RGB, 0, 0, NULL)
c$ifdef DRAW_ALL_AT_ONCE
      old_top = paint.yBottom
      paint.yBottom = Draw_area.yTop
c$endif
      call WinFillRect( win_dc, paint, SYSCLR_WINDOW )
c$ifdef DRAW_ALL_AT_ONCE
      paint.yBottom = old_top
c$endif

      if(WinIntersectRect(Main_hab,intersect,paint,Draw_area)) then
c$ifdef DRAW_ALL_AT_ONCE
            width = Draw_area.xRight - Draw_area.xLeft
            height = Draw_area.yTop - Draw_area.yBottom
            interior.xLeft = 0
            interior.yBottom = 0
            interior.xRight = width
            interior.yTop = height
            if( Draw_bitmap .eq. NULLHANDLE ) then
               Hdc = DevOpenDC(Main_hab, OD_MEMORY, '*'//char(0), 5,
     &            dop, NULLHANDLE )
               Mem_dc = GpiCreatePS( Main_hab, Hdc, sizl,
     &                  ( PU_PELS .or. GPIA_ASSOC ) )
         call memclear(loc(bmih), 0, isizeof(BITMAPINFOHEADER2))
               call GpiQueryDeviceBitmapFormats(Mem_dc, 24, formats)
               bmih.cbFix = isizeof( BITMAPINFOHEADER2 )
               bmih.cx = width
               bmih.cy = height
               bmih.cPlanes = formats( 0 )
               bmih.cBitCount = formats( 1 )
               Draw_bitmap = GpiCreateBitmap( Mem_dc, bmih, 0, 0, 0 )
               Old_bitmap = GpiSetBitmap( Mem_dc, Draw_bitmap )
               GpiCreateLogColorTable( Mem_dc, 0, LCOLF_RGB, 0, 0, 0)
               WinFillRect( Mem_dc, interior, SYSCLR_WINDOW )

               hour_glass_cur = WinQuerySysPointer( HWND_DESKTOP,
     &                       SPTR_WAIT, 0 )
               old_cursor = WinQueryPointer( HWND_DESKTOP )
               call WinSetPointer( HWND_DESKTOP, hour_glass_cur )

               hThree_d = three_d_begin( Mem_dc, &interior )
               call draw_room()
               call three_d_draw( hThree_d )
               call three_d_end( hThree_d )

               call WinSetPointer( HWND_DESKTOP, old_cursor )
            endif

            pts(0).x = Draw_area.xLeft
            pts(0).y = Draw_area.yBottom
            pts(1).x = Draw_area.xLeft + width
            pts(1).y = Draw_area.yBottom + height
            pts(2).x = 0
            pts(2).y = 0
            call GpiBitBlt(win_dc,Mem_dc,3,pts,ROP_SRCCOPY,BBO_IGNORE)
c$else
            hThree_d = three_d_begin( win_dc, Draw_area )
            call draw_room()
            call three_d_draw( hThree_d )
            call three_d_end( hThree_d )
c$endif
      endif
      call WinEndPaint( win_dc )
      end


c$pragma aux (__syscall) main_proc parm( value, value, value, value )
      integer function main_proc( hwnd, msg, mp1, mp2 )
      integer hwnd
      integer msg
      integer mp1
      integer mp2

        include 'drawos2.fi'
      include 'threed.fi'

      logical  ltmp
        integer count
        integer will_be_one
      record /CREATESTRUCT/  pcreate(:)

      if( msg .eq. WM_CREATE ) then
            allocate( pcreate( 1 ), location = mp2 )
            Draw_area.xRight = pcreate(1).x + pcreate(1).cx
            Draw_area.yTop = pcreate(1).y + pcreate(1).cy
            Draw_area.yBottom = pcreate(1).y
            Draw_area.xLeft = pcreate(1).x
            call WinQueryWindowRect( hwnd, Draw_area )

      else if( (msg .eq. WM_COMMAND) .or.
     &          ( (msg .eq. WM_CONTROL) .and.
     &       (SHORT2FROMMP( mp1 ) .ne. BN_PAINT) ) ) then

            select( SHORT1FROMMP( mp1 ) )
            case( IDM_EXIT )
                call new_display()
                call WinPostMsg( hwnd, WM_QUIT, 0, 0 )
                main_proc = 0
                return
            case( IDM_ROTATE_LEFT )
                call new_display()
                call add_wrap( THREE_D_HORZ_ANGLE, -15, 0, 360 )
            case( IDM_ROTATE_RIGHT )
                call new_display()
                call add_wrap( THREE_D_HORZ_ANGLE, 15, 0, 360 )
            case( IDM_ROTATE_UP )
                call new_display()
                call add_range( THREE_D_VERT_ANGLE, 5, -90, 90 )
            case( IDM_ROTATE_DOWN )
                call new_display()
                call add_range( THREE_D_VERT_ANGLE, -5, -90, 90 )
            case( IDM_MOVE_IN )
                call new_display()
                call add_range(THREE_D_ZOOM, 10, 5, 200 )
            case( IDM_MOVE_OUT )
                call new_display()
                call add_range( THREE_D_ZOOM, -10, 5, 200 )
            case( IDM_MORE_PERSPECTIVE )
                call new_display()
                call add_range( THREE_D_PERSPECTIVE, 10, 5, 200 )
            case( IDM_LESS_PERSPECTIVE )
                call new_display()
                call add_range( THREE_D_PERSPECTIVE, -10, 5, 200 )
            case( IDM_MORE_CONTRAST )
                call new_display()
                call add_range( THREE_D_CONTRAST, 10, 0, 100 )
            case( IDM_LESS_CONTRAST )
                call new_display()
                call add_range( THREE_D_CONTRAST, -10, 0, 100 )
            case( IDM_MORE_BRIGHTNESS )
                call new_display()
                call add_range( THREE_D_BRIGHTNESS, 10, 0, 100 )
            case( IDM_LESS_BRIGHTNESS )
                call new_display()
                call add_range( THREE_D_BRIGHTNESS, -10, 0, 100 )
            case( IDM_LIGHT_HORZ_LEFT )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 1 )
            case( IDM_LIGHT_HORZ_MIDDLE )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 2 )
            case( IDM_LIGHT_HORZ_RIGHT )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 3 )
            case( IDM_LIGHT_VERT_TOP )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 1 )
            case( IDM_LIGHT_VERT_MIDDLE )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 2 )
            case( IDM_LIGHT_VERT_BOTTOM )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 3 )
            case( IDM_LIGHT_DEPTH_FRONT )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 1 )
            case( IDM_LIGHT_DEPTH_MIDDLE )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 2 )
            case( IDM_LIGHT_DEPTH_BACK )
                call new_display()
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 3 )
          end select
            ltmp = SHORT1FROMMP(mp1) .ge. IDM_FIRST_REQUIRING_CLEAR
            call WinInvalidateRect( hwnd, Draw_area, ltmp )

      else if( msg .eq. WM_DESTROY ) then
            call WinPostMsg( hwnd, WM_QUIT, 0, 0 )
            main_proc = FALSE
          return

      else if( msg .eq. WM_PAINT ) then
            call draw_stuff( hwnd )
            if( Auto_run .ne. 0 ) then
                if( Auto_run .eq. 5 ) then
                    call WinPostMsg( hwnd, WM_QUIT, 0, 0 )
                else
                    will_be_one = 1
             count = 0
             while( count .lt. 1000000 ) do
             will_be_one = will_be_one .xor. 1
             count = count + 1
             end while
                    Auto_run = Auto_run + will_be_one
                    call WinPostMsg( hwnd, WM_COMMAND,
     &                          MPFROMSHORT( IDM_ROTATE_LEFT ),
     &                 0 )
         endif
            endif
          main_proc = FALSE
            return

      else if( msg .eq. WM_SIZE ) then
            call WinQueryWindowRect( hwnd, Draw_area )
            Draw_area.yTop = Draw_area.yTop - Button_height
            call WinInvalidateRect( hwnd, 0, 1 )
          main_proc = FALSE
            return

      else if( msg .eq. WM_MEASUREITEM ) then
            main_proc = measure_button( hwnd, mp1, mp2 )
            return

        else if( (msg .eq. WM_CONTROL) .and.
     &         (SHORT2FROMMP(mp1) .eq. BN_PAINT) ) then
            call draw_button( mp1, mp2 )
            main_proc = TRUE
            return
      endif
        main_proc = WinDefWindowProc( hwnd, msg, mp1, mp2 )
      end


      integer function init_instance( show )
      integer show

        include 'drawos2.fi'

      integer frame_hwnd
      integer flags

      flags = FCF_SIZEBORDER .or. FCF_TITLEBAR .or. FCF_SYSMENU .or.
     &      FCF_MENU .or. FCF_MINMAX .or. FCF_SHELLPOSITION

      frame_hwnd = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
     &                flags, 'DrawDemo'//char(0),
     &                'Draw a Picture OS/2 Version'//char(0),
     &                WS_VISIBLE, 0, 200, Main_hwnd)

      ! If window could not be created, return 'failure'

      if( ( Main_hwnd .eq. 0 ) .or. ( frame_hwnd .eq. 0 ) ) then
          init_instance = FALSE
            return
      endif

      ! Make the window visible
      ! update its client area
      ! and return 'success'

      call WinSetWindowPos( frame_hwnd, HWND_TOP, 50, 50, 200, 200,
     &               show )

      call make_buttons( Main_hwnd)
      call WinShowWindow( frame_hwnd, TRUE )
      call WinUpdateWindow( frame_hwnd )

      init_instance = TRUE
      end


      integer function init_app( hab )
      integer hab

        include 'drawos2.fi'
      external main_proc

      ! Register the window class and return success/failure code.
      init_app=WinRegisterClass(hab, 'DrawDemo'//char(0), main_proc,
     &              NULL, 0 )
      end
