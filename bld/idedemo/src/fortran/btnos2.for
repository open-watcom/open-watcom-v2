c$define INCL_PM
c$include os2.fap


      subroutine button_size( hwnd, id, pwidth, pheight )
      integer                   hwnd
      integer                   id
      integer                   pwidth
      integer                   pheight

      include 'drawos2.fi'

      integer                   bmp
      record /BITMAPINFOHEADER/ bitmap
      integer                   hps

      hwnd = hwnd
      hps = WinGetPS( HWND_DESKTOP )
      bmp = GpiLoadBitmap( hps, 0, id, 0, 0 )
      call WinReleasePS( hps )
      if( bmp .eq. 0 ) then
          return
      endif
      bitmap.cbFix = isizeof( BITMAPINFOHEADER )
      call GpiQueryBitmapParameters( bmp, bitmap )
      pwidth = bitmap.cx + 5
      pheight = bitmap.cy + 5
      end


      subroutine add_button(parent, top, left, id, pwidth, pheight)
      integer parent
      integer top
      integer left
      integer id
      integer pwidth
      integer pheight

      integer        hbutton

      include 'drawos2.fi'

      ! Note that the resource ID is the same as the control ID
      call button_size( parent, id, pwidth, pheight )

      hbutton = WinCreateWindow( parent, WC_BUTTON, char(0),
     &               WS_VISIBLE .or. BS_USERBUTTON, left,
     &               top - pheight, pwidth, pheight,
     &            parent, HWND_TOP, id, NULL, NULL )
      end


      function measure_button( parent, mp1, mp2 )
      integer parent
      integer mp1
      integer mp2

      include 'drawos2.fi'

      integer width
      integer height
      integer button_id

      button_id = SHORT1FROMMP( mp1 )
      mp2 = mp2
      call button_size( parent, button_id, width, height )
      measure_button = MPFROM2SHORT( width, height )
      end


      subroutine horizontal( hps, rect, row )

      include 'drawos2.fi'

      integer         hps
      record /RECTL/  rect
      integer         row

      record /POINTL/ pt

      pt.x = rect.xLeft + 1
      pt.y = row
      call GpiSetCurrentPosition( hps, pt )
      pt.x = rect.xRight - 2
      pt.y = row
      call GpiLine( hps, pt )
      end


      subroutine vertical( hps, rect, column )

      include 'drawos2.fi'

      integer         hps
      record /RECTL/  rect
      integer         column

        record /POINTL/ pt

      pt.x = column
      pt.y = rect.yBottom + 1
      call GpiSetCurrentPosition( hps, pt )
      pt.y = column
      pt.y = rect.yTop - 2
      call GpiLine( hps, pt )
      end


      subroutine draw_button( mp1, mp2 )
      integer mp1
      integer mp2

      include 'drawos2.fi'

      integer                   bmp
      integer                   oldbmp
      record /LINEBUNDLE/       blackpen
      record /LINEBUNDLE/       shadowpen
      record /LINEBUNDLE/       brightpen
      record /LINEBUNDLE/       facepen
      record /LINEBUNDLE/       oldpen
      record /BITMAPINFOHEADER/ bitmap
      integer                   memdc
      integer                   tmp_ps
      integer                   hdc
      integer                   hab
      integer                   shift
      record /SIZEL/            sizl
      data sizl /0, 0/
      record /USERBUTTON/       b2(:)
      record /RECTL/            rect
      record /POINTL/           pts(3)
      integer                   button_id
      record /DEVOPENSTRUC/     dop
      data dop /0, 0, 0, 0, 0, 0, 0, 0, 0 /

      dop.pszDriverName = LOC( 'DISPLAY'//char(0) )

      allocate( b2(1), location= mp2 )
      call WinQueryWindowRect( b2(1).hwnd, rect )
      button_id = SHORT1FROMMP( mp1 )

      hab = WinQueryAnchorBlock( b2(1).hwnd )
      tmp_ps = WinGetPS( HWND_DESKTOP )
      bmp = GpiLoadBitmap( tmp_ps, 0, button_id, 0, 0 )
      call WinReleasePS( tmp_ps )
      if( bmp .eq. 0 ) then
          return
      endif

      bitmap.cbFix = isizeof( BITMAPINFOHEADER )
      call GpiQueryBitmapParameters( bmp, bitmap )

      hdc = DevOpenDC( hab, OD_MEMORY, '*'//char(0), 5, dop,
     &          NULLHANDLE )
      memdc = GpiCreatePS( hab, hdc, sizl, PU_PELS .or. GPIA_ASSOC)

      oldbmp = GpiSetBitmap( memdc, bmp )

      if( LOUSHORT( b2(1).fsState ) .eq. BDS_HILITED ) then
            shift = 4
      else
            shift = 2
      endif

      pts(1).x = rect.xLeft + shift
      pts(1).y = rect.yBottom + 5 - shift
      pts(2).x = rect.xLeft + shift + bitmap.cx
      pts(2).y = rect.yBottom + 5 - shift + bitmap.cy
      pts(3).x = 0
      pts(3).y = 0
      call GpiBitBlt(b2(1).hps,memdc,3,pts,ROP_SRCCOPY,BBO_IGNORE)

      call GpiSetBitmap( memdc, oldbmp )
      call GpiDestroyPS( memdc )
      call DevCloseDC( hdc )
      call GpiDeleteBitmap( bmp )

      ! Draw four sides of the button except one pixel in each corner
      blackpen.lColor = CLR_BLACK
      blackpen.usType = LINETYPE_SOLID
      blackpen.usMixMode = FM_OVERPAINT
      brightpen.lColor = CLR_WHITE
      brightpen.usType = LINETYPE_SOLID
      brightpen.usMixMode = FM_OVERPAINT
      shadowpen.lColor = SYSCLR_BUTTONDARK
      shadowpen.usType = LINETYPE_SOLID
      shadowpen.usMixMode = FM_OVERPAINT
      facepen.lColor = SYSCLR_BUTTONMIDDLE
      facepen.usType = LINETYPE_SOLID
      facepen.usMixMode = FM_OVERPAINT

      call GpiQueryAttrs( b2(1).hps, PRIM_LINE,
     &        LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &       oldpen )
      call GpiSetAttrs( b2(1).hps, PRIM_LINE,
     &        LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &        0, blackpen )

      call horizontal( b2(1).hps, rect, rect.yBottom )
      call horizontal( b2(1).hps, rect, rect.yTop - 1 )
      call vertical( b2(1).hps, rect, rect.xLeft )
      call vertical( b2(1).hps, rect, rect.xRight - 1 )
      ! Now the shading

      call GpiSetAttrs( b2(1).hps, PRIM_LINE,
     &       LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &       0, shadowpen )

      if( LOUSHORT( b2(1).fsState ) .eq. BDS_HILITED ) then
            call horizontal( b2(1).hps, rect, rect.yTop - 2 )
            call vertical( b2(1).hps, rect, rect.xLeft + 1 )

            call GpiSetAttrs( b2(1).hps, PRIM_LINE,
     &         LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &         0, facepen )

            call horizontal( b2(1).hps, rect, rect.yTop - 3 )
            call vertical( b2(1).hps, rect, rect.xLeft + 2 )
            call horizontal( b2(1).hps, rect, rect.yTop - 4 )
            call vertical( b2(1).hps, rect, rect.xLeft + 3 )
      else
            call horizontal( b2(1).hps, rect, rect.yBottom + 1 )
            call horizontal( b2(1).hps, rect, rect.yBottom + 2 )
            call vertical( b2(1).hps, rect, rect.xRight - 2 )
            call vertical( b2(1).hps, rect, rect.xRight - 3 )

            call GpiSetAttrs( b2(1).hps, PRIM_LINE,
     &         LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &         0, brightpen )

            call horizontal( b2(1).hps, rect, rect.yTop - 2 )
            call vertical( b2(1).hps, rect, rect.xLeft + 1 )
      endif

      call GpiSetAttrs( b2(1).hps, PRIM_LINE,
     &        LBB_COLOR .or. LBB_WIDTH .or. LBB_TYPE .or. LBB_MIX_MODE,
     &        0, oldpen )

      b2(1).fsStateOld = 0
      b2(1).fsState = 0
      end
