
c$include winapi.fi


      subroutine make_buttons( hwnd )
      integer*2 hwnd

      include 'drawwin.fi'

      integer*2 offset
      integer*2 max_height
      integer*2 btn_width
      integer*2 btn_height
      integer*2 index
      integer*2 buttons(9)

      buttons( 1 ) = IDM_ROTATE_UP
        buttons( 2 ) = IDM_MOVE_IN
        buttons( 3 ) = IDM_ROTATE_LEFT
        buttons( 4 ) = IDM_ROTATE_RIGHT
        buttons( 5 ) = IDM_MOVE_OUT
        buttons( 6 ) = IDM_ROTATE_DOWN
        buttons( 7 ) = IDM_MORE_BRIGHTNESS
        buttons( 8 ) = IDM_LESS_BRIGHTNESS
        buttons( 9 ) = 0

      Draw_area.top = Draw_area.top + 2
      offset = Draw_area.left + 2
      max_height = 0
      index = 1
      while( buttons( index ) .ne. 0 ) do
            call add_button( hwnd, Draw_area.top, offset,
     &              buttons(index), btn_width, btn_height )
            if( max_height .lt. btn_height ) then
            max_height = btn_height
          endif
          index = index + 1
          offset = offset + ( btn_width - 1 )
      end while
      ! remove buttons from drawing area
      Draw_area.top = Draw_area.top + max_height + 2
      end


c$pragma aux FWinMain parm(value*2,value*2,value*4,value*2)
      integer*2 function FWinMain( inst, prev_inst, cmd_line, show )
      integer*2    inst
      integer*2    prev_inst
      integer      cmd_line
      integer*2    show
      external     init_app
      integer*2    init_app
      external     init_instance
      integer*2    init_instance
      external     IsAuto
      integer*2    IsAuto

      include 'drawwin.fi'
      include 'threed.fi'

      record /MSG/ msg

      hThree_d = 0
c$ifdef undefined
      call memclear( 0, loc( Draw_area ), isizeof( RECT ) )
c$else
      call memclear( loc( Draw_area ), 0, isizeof( RECT ) )
c$endif
      Auto_run = 0

c$ifdef __WINDOWS_386__
      hThreedDLL = 0
      three_d_init_Proc = 0
      three_d_fini_Proc = 0
      three_d_begin_Proc = 0
      three_d_box_Proc = 0
      three_d_draw_Proc = 0
      three_d_end_Proc = 0
      three_d_get_Proc = 0
      three_d_set_Proc = 0
c$endif

      Auto_run = IsAuto( cmd_line )
      if( prev_inst .eq. 0 ) then
            if( init_app( inst ) .eq. 0 ) then
            FWinMain = 0
               return
            endif
      endif

c Perform initializations that apply to a specific instance

      call three_d_init()
      call define_room()
      if( init_instance( inst, show ) .eq. 0 ) then
            call finish_room()
            call three_d_fini()
          FWinMain = 0
            return
      endif

c Acquire and dispatch messages until a WM_QUIT message is received.

      while( GetMessage( msg, NULL, NULL, NULL ) .ne. 0 ) do
            call TranslateMessage( msg )
            call DispatchMessage( msg )
      end while

      call finish_room()
      call three_d_fini()

      FWinMain = msg.wParam
      end


c$ifdef __WINDOWS_386__

      subroutine three_d_init()
c Use the DLL initialization entry point to load the DLL and get the
c address of the entry points

      include 'drawwin.fi'
      include 'threed.fi'

      hThreedDLL = LoadLibrary( 'win_3d.dll'//char(0) )
      if( hThreedDLL .lt. 32 ) then
            hThreedDLL = NULL
            return
      endif

      three_d_init_Proc  = GetProcAddress( hThreedDLL,
     &               'three_d_init'//char(0) )
      three_d_fini_Proc  = GetProcAddress( hThreedDLL,
     &               'three_d_fini'//char(0) )
      three_d_begin_Proc = GetProcAddress( hThreedDLL,
     &               'three_d_begin'//char(0) )
      three_d_box_Proc   = GetProcAddress( hThreedDLL,
     &               'three_d_box'//char(0) )
      three_d_draw_Proc  = GetProcAddress( hThreedDLL,
     &               'three_d_draw'//char(0) )
      three_d_end_Proc   = GetProcAddress( hThreedDLL,
     &               'three_d_end'//char(0) )
      three_d_get_Proc   = GetProcAddress( hThreedDLL,
     &               'three_d_get'//char(0) )
      three_d_set_Proc   = GetProcAddress( hThreedDLL,
     &               'three_d_set'//char(0) )

c Invoke the real DLL entry point
      call _Call16( three_d_init_Proc, char(0) )
      end


      subroutine three_d_fini()
c Use the DLL finish routine to free up the DLL

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif

      call _Call16( three_d_fini_Proc, char(0) )
      call FreeLibrary( hThreedDLL )
      hThreedDLL = NULL
      end


      function three_d_begin( dc, drw_are )
c$pragma aux KEEP_REF = \
c                   parm( reference ) [eax] \
c                   value [eax] \
c                   modify exact []
      integer*2 dc
      integer   drw_are
      integer   KEEP_REF

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          three_d_begin = 0
          return
      endif

      three_d_begin = _Call16( three_d_begin_Proc, 'wp'//char(0),
     &             dc, KEEP_REF( drw_are ) )
      return
      end


      subroutine  three_d_box( handle, x1, x2, y1, y2, z1, z2,
     &             red, green, blue )
      integer*2 handle
        real     x1
      real    x2
      real    y1
      real    y2
      real    z1
      real    z2
        integer*2 red
      integer*2 green
      integer*2 blue

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif
      call _Call16( three_d_box_Proc, 'wddddddwww'//char(0), handle,
     &                x1, x2, y1, y2, z1, z2, red, green, blue )
      end


      subroutine three_d_draw( handle )
      integer*2 handle

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif
      call _Call16( three_d_draw_Proc, 'w'//char(0), handle )
      end


      subroutine three_d_end( handle )
      integer*2 handle

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif
      call _Call16( three_d_end_Proc, 'w'//char(0), handle )
      end


      function three_d_get( handle, attr )
      integer*2 handle
      integer*2 attr

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif
      three_d_get = _Call16(three_d_get_Proc, 'ww'//char(0), handle,
     &            attr )
      end


      subroutine three_d_set( handle, attr, new_val )
      integer*2 handle
      integer*2 attr
      integer*2 new_val

      include 'drawwin.fi'
      include 'threed.fi'

      if( hThreedDLL .eq. NULL ) then
          return
      endif

      call _Call16( three_d_set_Proc, 'www'//char(0), handle, attr,
     &         new_val )
      end
c$endif


      subroutine add_wrap( attr, add, minimum, maximum )
      integer*2 attr
      integer*2 add
      integer*2 minimum
      integer*2 maximum

      include 'drawwin.fi'
      include 'threed.fi'

      integer*2 start

      start = three_d_get( hThree_d, attr ) + add

      while( start .ge. maximum ) do
            start = start - ( maximum - minimum )
      end while

      while( start .lt. minimum ) do
            start = start + maximum - minimum
      end while

      call three_d_set( hThree_d, attr, start )
      end


      subroutine add_range( attr, add, minimum, maximum )
      integer*2 attr
      integer*2 add
      integer*2 minimum
      integer*2 maximum

      include 'drawwin.fi'
      include 'threed.fi'

      integer*2 start

      start = three_d_get( hThree_d, attr ) + add

      if( start .ge. maximum ) then
            call three_d_set( hThree_d, attr, maximum )
      else if( start .le. minimum ) then
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
      integer*2 red
      integer*2 green
      integer*2 blue

      include 'drawwin.fi'
      include 'threed.fi'

      call three_d_box( hThree_d, x1, x2, y1, y2, z1, z2, red, green,
     &           blue )
      end


      subroutine draw_stuff( hwnd )
      integer*2            hwnd

      include 'drawwin.fi'
      include 'threed.fi'

      integer*2            win_dc
      integer*2            wrc
      record /PAINTSTRUCT/ paint
      record /RECT/        intersect

      win_dc = BeginPaint( hwnd, paint )
      wrc = IntersectRect( intersect, paint.rcPaint, Draw_area )

      if( wrc .ne. 0 ) then
            hThree_d = three_d_begin( win_dc, Draw_area )
            call draw_room()
            call three_d_draw( hThree_d )
            call three_d_end( hThree_d )
      endif

      call EndPaint( hwnd, paint )
      end


*$pragma aux (callback) main_proc parm(value*2,value*2,value*2,value*4)
      integer function main_proc( hwnd, msg, wparam, lparam )
      integer*2 hwnd
      integer*2 msg
      integer*2 wparam
      integer*4 lparam

      include 'drawwin.fi'
      include 'threed.fi'

      integer*2 cmd
        integer*4 count
        integer   will_be_one
        integer   old_left
        integer   ptr
      integer*1 map_item(:)

      select( msg )
      case( WM_CREATE )
            call GetClientRect( hwnd, Draw_area )
            call make_buttons( hwnd )

      case( WM_COMMAND )
            cmd = LOWORD( wparam )
            select( LOWORD( cmd ) )
            case( IDM_EXIT )
                call PostQuitMessage( 0 )
                main_proc = 0
                return
            case( IDM_ROTATE_LEFT )
                call add_wrap( THREE_D_HORZ_ANGLE, -15, 0, 360 )
            case( IDM_ROTATE_RIGHT )
                call add_wrap( THREE_D_HORZ_ANGLE, 15, 0, 360 )
            case( IDM_ROTATE_UP )
                call add_range( THREE_D_VERT_ANGLE, 5, -90, 90 )
            case( IDM_ROTATE_DOWN )
                call add_range( THREE_D_VERT_ANGLE, -5, -90, 90 )
            case( IDM_MOVE_IN )
                call add_range(THREE_D_ZOOM, 10, 5, 200 )
            case( IDM_MOVE_OUT )
                call add_range( THREE_D_ZOOM, -10, 5, 200 )
            case( IDM_MORE_PERSPECTIVE )
                call add_range( THREE_D_PERSPECTIVE, 10, 5, 200 )
            case( IDM_LESS_PERSPECTIVE )
                call add_range( THREE_D_PERSPECTIVE, -10, 5, 200 )
            case( IDM_MORE_CONTRAST )
                call add_range( THREE_D_CONTRAST, 10, 0, 100 )
            case( IDM_LESS_CONTRAST )
                call add_range( THREE_D_CONTRAST, -10, 0, 100 )
            case( IDM_MORE_BRIGHTNESS )
                call add_range( THREE_D_BRIGHTNESS, 10, 0, 100 )
            case( IDM_LESS_BRIGHTNESS )
                call add_range( THREE_D_BRIGHTNESS, -10, 0, 100 )
            case( IDM_LIGHT_HORZ_LEFT )
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 1 )
            case( IDM_LIGHT_HORZ_MIDDLE )
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 2 )
            case( IDM_LIGHT_HORZ_RIGHT )
                call three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 3 )
            case( IDM_LIGHT_VERT_TOP )
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 1 )
            case( IDM_LIGHT_VERT_MIDDLE )
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 2 )
            case( IDM_LIGHT_VERT_BOTTOM )
                call three_d_set( hThree_d, THREE_D_LIGHT_VERT, 3 )
            case( IDM_LIGHT_DEPTH_FRONT )
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 1 )
            case( IDM_LIGHT_DEPTH_MIDDLE )
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 2 )
            case( IDM_LIGHT_DEPTH_BACK )
                call three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 3 )
            end select
            call InvalidateRect( hwnd, Draw_area,
     &             cmd .ge. IDM_FIRST_REQUIRING_CLEAR )
      case( WM_DESTROY )
            call PostQuitMessage( 0 )
          main_proc = 0
            return
      case( WM_PAINT )
            call draw_stuff( hwnd )
            if( Auto_run ) then
                if( Auto_run .eq. 5 ) then
                    call PostQuitMessage( 0 )
                else
                    will_be_one = 1
             count = 0
             while( count .lt. 1000000 ) do
                        will_be_one = will_be_one .xor. 1
            count = count + 1
             end while
                    Auto_run = Auto_run + will_be_one
                    call PostMessage(hwnd,WM_COMMAND,IDM_ROTATE_LEFT,0)
         endif
            endif
          main_proc = 0
            return
        case( WM_SIZE )
            if( wparam .ne. SIZEICONIC ) then
                old_left = Draw_area.left
                call GetClientRect( hwnd, Draw_area )
                Draw_area.left = old_left
                call InvalidateRect( hwnd, NULL, 1 )
         main_proc = 0
                return
            endif
        case( WM_MEASUREITEM )
c$ifdef __WINDOWS_386__
            ptr = MapAliasToFlat(lparam)
c$else
            ptr = lparam
c$endif
            allocate( map_item(1), location=ptr )
            call measure_button( hwnd, wparam, map_item )
          main_proc = 1
            return

        case( WM_DRAWITEM )
c$ifdef __WINDOWS_386__
            ptr = MapAliasToFlat(lparam)
c$else
            ptr = lparam
c$endif
            allocate( map_item(16), location=ptr )

            call draw_button( wparam, map_item )
          main_proc = 1
            return
        end select

        main_proc = DefWindowProc( hwnd, msg, wparam, lparam )
      end

      function init_instance( inst, show )
      integer*2 inst
      integer*2 show

      include 'drawwin.fi'
      include 'threed.fi'

      integer*2 hwnd

      hwnd = CreateWindow( 'DrawDemo'//char(0),
     &      'Draw a Picture'//char(0), WS_OVERLAPPEDWINDOW,
     &      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
     &      CW_USEDEFAULT, NULL, NULL, inst, NULL )

      ! If window could not be created, return "failure"

      if( hwnd .eq. NULL ) then
          init_instance = 0
            return
      endif

      ! Make the window visible
      ! update its client area
      ! and return "success"

      call ShowWindow( hwnd, show )
      call UpdateWindow( hwnd )
      init_instance = 1
      end


      function init_app( inst )
      integer*2         inst
      external          main_proc

      include 'drawwin.fi'
      include 'threed.fi'

      record /WNDCLASS/ wc

      ! Fill in window class structure with parameters that
      ! describe the main window.

      wc.style = NULL
      wc.lpfnWndProc = MakeProcInstance( main_proc, inst )
      wc.cbClsExtra = 0
      wc.cbWndExtra = 0
      wc.hInstance = inst
      wc.hIcon = LoadIcon( NULL, 'DrawIcon'//char(0) )
      wc.hCursor = LoadCursor( NULL, IDC_ARROW )
      wc.hbrBackground = GetStockObject( WHITE_BRUSH )
      wc.lpszMenuName = loc( 'DrawMenu'//char(0) )
      wc.lpszClassName = loc( 'DrawDemo'//char(0) )

      ! Register the window class and return success/failure code

      init_app = RegisterClass( wc )
      end


      subroutine memclear( locale, ch, len )
      integer*4 locale
      integer*1 ch
      integer   len
      integer    cnt
      integer*1 arr(:)

c$ifdef undefined
c$pragma array arr far
c$endif
      allocate( arr( len ), location=locale )
      cnt = len
      while( cnt .gt. 0 ) do
          arr( cnt ) = ch
          cnt = cnt - 1
      end while
      end

      integer function IsAuto( cmd_line )
      integer cmd_line

      cmd_line = cmd_line
      IsAuto = 0
      end
