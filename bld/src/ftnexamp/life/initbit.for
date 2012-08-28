
        subroutine InitBitMap()
c
c       Process a new bit map for cells, pointed to by CellBitMap
c
        include                         'gui.fi'

        record /BITMAPINFOHEADER/       GuiBitInfo
        record /DEVOPENSTRUC/           Dop/NULL,NULL,NULL,NULL,
     +                                      NULL,NULL,NULL,NULL,NULL/
        record /SIZEL/                  sizl
        integer                         ps

        if( CellBitMap .ne. NULL ) call GpiDeleteBitmap( CellBitMap )
        ps = WinGetPS( WinHandle )
        CellBitMap = GpiLoadBitmap( ps, NULL, BitSize, 0, 0 )
        call WinReleasePS( ps )

        Dop.pszDriverName = loc( 'DISPLAY'c )
        MemoryDC = DevOpenDC( AnchorBlock, OD_MEMORY, '*'c, 3, Dop, NULL )
        sizl.cx = 1
        sizl.cy = 1
        MemoryPS = GpiCreatePS( AnchorBlock, MemoryDC, sizl
     +                 , PU_PELS .or. GPIA_ASSOC .or. GPIT_MICRO )
        call GpiSetBitmap( MemoryPS, CellBitMap )
        call GpiQueryBitmapParameters( CellBitMap, GuiBitInfo )
        BitInfo.cx = GuiBitInfo.cx
        BitInfo.cy = GuiBitInfo.cy

        end
