c$define INCL_GRE
c$include pmddim.fap

c$ifdef INCL_GRE_ARCS

        integer function GreGetArcParameters( a, b )
        integer Gre32Entry4, a, b
        GreGetArcParameters = Gre32Entry4( a, loc( b ), 0, '00004000'x )
        end

        integer function GreSetArcParameters( a, b )
        integer Gre32Entry4, a, b
        GreSetArcParameters = Gre32Entry4( a, loc( b ), 0, '00004001'x )
        end

        integer function GreArc( a, b )
        integer Gre32Entry4, a, b
        GreArc = Gre32Entry4( a, loc( b ), 0, '00004002'x )
        end

        integer function GrePartialArc( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GrePartialArc = Gre32Entry7( a, loc( b ), c, d, e, 0,
     +                               '00004003'x )
        end

        integer function GreFullArcInterior( a, b )
        integer Gre32Entry4, a, b
        GreFullArcInterior = Gre32Entry4( a, b, 0, '00004004'x )
        end

        integer function GreFullArcBoundary( a, b )
        integer Gre32Entry4, a, b
        GreFullArcBoundary = Gre32Entry4( a, b, 0, '00004005'x )
        end

        integer function GreFullArcBoth( a, b )
        integer Gre32Entry4, a, b
        GreFullArcBoth = Gre32Entry4( a, b, 0, '00004006'x )
        end

        integer function GreBoxInterior( a, b )
        integer Gre32Entry4, a, b
        GreBoxInterior = Gre32Entry4( a, loc( b ), 0, '00004007'x )
        end

        integer function GreBoxBoundary( a, b )
        integer Gre32Entry4, a, b
        GreBoxBoundary = Gre32Entry4( a, loc( b ), 0, '00004008'x )
        end

        integer function GreBoxBoth( a, b )
        integer Gre32Entry4, a, b
        GreBoxBoth = Gre32Entry4( a, loc( b ), 0, '00004009'x )
        end

        integer function GrePolyFillet( a, b, c )
        integer Gre32Entry5, a, b, c
        GrePolyFillet = Gre32Entry5( a, loc( b ), c, 0, '0000400A'x )
        end

        integer function GrePolyFilletSharp( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GrePolyFilletSharp = Gre32Entry6( a, loc( b ), c, loc( d ), 0,
     +                                    '0000400B'x )
        end

        integer function GrePolySpline( a, b, c )
        integer Gre32Entry5, a, b, c
        GrePolySpline = Gre32Entry5( a, loc( b ), c, 0, '0000400C'x )
        end

        integer function GreDrawConicsInPath( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDrawConicsInPath = Gre32Entry6( a, loc( b ), loc( c ), d, 0,
     +                                     '0000400D'x )
        end

        integer function GreCookWholePath( a, b )
        integer Gre32Entry4, a, b
        GreCookWholePath = Gre32Entry4( a, loc( b ), 0, '0000400E'x )
        end

        integer function GreCookPathCurves( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCookPathCurves = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                                   '0000400F'x )
        end

        integer function GreRenderPath( a, b, c, d, e, f, g, h )
        integer Gre32Entry10, a, b, c, d, e, f, g, h
        GreRenderPath = Gre32Entry10( a, loc( b ), loc( c ), d, e, f,
     +                                loc( g ), h, 0, '00004011'x )
        end

c$endif

c$ifdef INCL_GRE_LINES

        integer function GreDisjointLines( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDisjointLines = Gre32Entry5( a, loc( b ), c, 0, '00004016'x )
        end

        integer function GreGetCurrentPosition( a, b )
        integer Gre32Entry4, a, b
        GreGetCurrentPosition = Gre32Entry4( a, loc( b ), 0,
     +                                       '00004017'x )
        end

        integer function GreSetCurrentPosition( a, b )
        integer Gre32Entry4, a, b
        GreSetCurrentPosition = Gre32Entry4( a, loc( b ), 0,
     +                                       '00004018'x )
        end

        integer function GrePolyLine( a, b, c )
        integer Gre32Entry5, a, b, c
        GrePolyLine = Gre32Entry5( a, loc( b ), c, 0, '00004019'x )
        end

        integer function GreDrawLinesInPath( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDrawLinesInPath = Gre32Entry6( a, loc( b ), loc( c ), d, 0,
     +                                    '0000401A'x )
        end

        integer function GrePolyShortLine( a, b )
        integer Gre32Entry4, a, b
        GrePolyShortLine = Gre32Entry4( a, loc( b ), 0, '0000401B'x )
        end

c$endif

c$ifdef INCL_GRE_SCANS

        integer function GrePolyScanline( a, b )
        integer Gre32Entry4, a, b
        GrePolyScanline = Gre32Entry4( a, loc( b ), 0, '0000401C'x )
        end

c$endif

c$ifdef INCL_GRE_BITMAPS

        integer function GreDrawBits( a, b, c, d, e, f, g )
        integer Gre32Entry9, a, b, c, d, e, f, g
        GreDrawBits = Gre32Entry9( a, loc( b ), loc( c ), d, loc( e ),
     +                             f, g, 0, '00004022'x )
        end

        integer function GreDeviceCreateBitmap( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreDeviceCreateBitmap = Gre32Entry7( a, loc( b ), c, loc( d ),
     +                                       loc( e ), 0, '00004023'x )
        end

        integer function GreDeviceDeleteBitmap( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDeviceDeleteBitmap = Gre32Entry6( a, b, loc( c ), d, 0,
     +                                       '00004024'x )
        end

        integer function GreDeviceSelectBitmap( a, b )
        integer Gre32Entry4, a, b
        GreDeviceSelectBitmap = Gre32Entry4( a, b, 0, '00004025'x )
        end

        integer function GreBitblt( a, b, c, d, e, f, g )
        integer Gre32Entry9, a, b, c, d, e, f, g
        GreBitblt = Gre32Entry9( a, b, c, loc( d ), e, f, loc( g ), 0,
     +                           '00004026'x )
        end

        integer function GreGetPel( a, b )
        integer Gre32Entry4, a, b
        GreGetPel = Gre32Entry4( a, loc( b ), 0, '00004027'x )
        end

        integer function GreSetPel( a, b )
        integer Gre32Entry4, a, b
        GreSetPel = Gre32Entry4( a, loc( b ), 0, '00004028'x )
        end

        integer function GreImageData( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreImageData = Gre32Entry6( a, loc( b ), c, d, 0, '00004029'x )
        end

        integer function GreSaveScreenBits( a, b )
        integer Gre32Entry4, a, b
        GreSaveScreenBits = Gre32Entry4( a, loc( b ), 0, '0000402C'x )
        end

        integer function GreRestoreScreenBits( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreRestoreScreenBits = Gre32Entry6( a, b, loc( c ), d, 0,
     +                                      '0000402D'x )
        end

        integer function GreDrawBorder( a, b, c, d, e, f, g )
        integer Gre32Entry9, a, b, c, d, e, f, g
        GreDrawBorder = Gre32Entry9( a, loc( b ), c, d, e, f, g, 0,
     +                               '0000402E'x )
        end

        integer function GreDeviceSetCursor( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceSetCursor = Gre32Entry5( a, loc( b ), c, 0,
     +                                    '0000402F'x )
        end

        integer function GreGetBitmapBits( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreGetBitmapBits = Gre32Entry8( a, b, c, d, loc( e ), loc( f ),
     +                                  0, '00004030'x )
        end

        integer function GreSetBitmapBits( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreSetBitmapBits = Gre32Entry8( a, b, c, d, loc( e ), loc( f ),
     +                                  0, '00004031'x )
        end

c$endif

c$ifdef INCL_WINPOINTERS

        integer function GreSetColorCursor( a, b )
        integer Gre32Entry4, a, b
        GreSetColorCursor = Gre32Entry4( a, loc( b ), 0, '00004032'x )
        end

c$endif

c$ifdef INCL_GRE_STRINGS

        integer function GreCharString( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCharString = Gre32Entry5( a, b, loc( c ), 0, '00004035'x )
        end

        integer function GreCharStringPos(a,b,c,d,e,f,g,h)
        integer Gre32Entry10, a, b, c, d, e, f, g, h
        GreCharStringPos = Gre32Entry10( a, loc( b ), c, d, e, loc( f ),
     +                                   loc( g ), loc( h ), 0,
     +                                   '00004036'x )
        end

        integer function GreQueryTextBox( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryTextBox = Gre32Entry7( a, b, loc( c ), d, loc( e ), 0,
     +                                 '00004037'x )
        end

        integer function GreQueryCharPositions( a, b, c, d, e, f, g )
        integer Gre32Entry9, a, b, c, d, e, f, g
        GreQueryCharPositions = Gre32Entry9( a, loc( b ), c, d,
     +                                       loc( e ), loc( f ),
     +                                       loc( g ), 0, '00004038'x )
        end

        integer function GreQueryWidthTable( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryWidthTable = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                    '00004039'x )
        end

c$endif

c$ifdef INCL_GRE_MARKERS

        integer function GrePolyMarker( a, b, c )
        integer Gre32Entry5, a, b, c
        GrePolyMarker = Gre32Entry5( a, loc( b ), c, 0, '0000403A'x )
        end

c$endif

c$ifdef INCL_GRE_STRINGS

        integer function GreCharRect( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCharRect = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                             '0000403B'x )
        end

        integer function GreCharStr( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCharStr = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                            '0000403C'x )
        end

        integer function GreScrollRect( a, b, c )
        integer Gre32Entry5, a, b, c
        GreScrollRect = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                               '0000403D'x )
        end

        integer function GreUpdateCursor( a, b )
        integer Gre32Entry4, a, b
        GreUpdateCursor = Gre32Entry4( a, loc( b ), 0, '0000403E'x )
        end

c$endif

c$ifdef INCL_GRE_PATHS

        integer function GreBeginArea( a, b )
        integer Gre32Entry4, a, b
        GreBeginArea = Gre32Entry4( a, b, 0, '00004046'x )
        end

        integer function GreEndArea( a, b )
        integer Gre32Entry4, a, b
        GreEndArea = Gre32Entry4( a, b, 0, '00004047'x )
        end

        integer function GreBeginPath( a, b )
        integer Gre32Entry4, a, b
        GreBeginPath = Gre32Entry4( a, loc( b ), 0, '00004048'x )
        end

        integer function GreEndPath( a, b )
        integer Gre32Entry4, a, b
        GreEndPath = Gre32Entry4( a, b, 0, '00004049'x )
        end

        integer function GreCloseFigure( a )
        integer Gre32Entry3, a
        GreCloseFigure = Gre32Entry3( a, 0, '0000404A'x )
        end

        integer function GreFillPath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreFillPath = Gre32Entry5( a, loc( b ), c, 0, '0000404B'x )
        end

        integer function GreOutlinePath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreOutlinePath = Gre32Entry5( a, loc( b ), c, 0, '0000404C'x )
        end

        integer function GreModifyPath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreModifyPath = Gre32Entry5( a, loc( b ), c, 0, '0000404D'x )
        end

        integer function GreStrokePath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreStrokePath = Gre32Entry5( a, loc( b ), c, 0, '0000404E'x )
        end

        integer function GreSelectClipPath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSelectClipPath = Gre32Entry5( a, loc( b ), c, 0,
     +                                   '0000404F'x )
        end

        integer function GreSavePath( a, b )
        integer Gre32Entry4, a, b
        GreSavePath = Gre32Entry4( a, b, 0, '00004050'x )
        end

        integer function GreRestorePath( a, b )
        integer Gre32Entry4, a, b
        GreRestorePath = Gre32Entry4( a, b, 0, '00004051'x )
        end

        integer function GreClip1DPath( a, b, c )
        integer Gre32Entry5, a, b, c
        GreClip1DPath = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                               '00004052'x )
        end

        integer function GreDrawRawPath( a, b )
        integer Gre32Entry4, a, b
        GreDrawRawPath = Gre32Entry4( a, loc( b ), 0, '00004053'x )
        end

        integer function GreDrawCookedPath( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDrawCookedPath = Gre32Entry6( a, loc( b ), loc( c ), d, 0,
     +                                   '00004054'x )
        end

        integer function GreAreaSetAttributes( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreAreaSetAttributes = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                      '00004055'x )
        end

c$endif

c$ifdef INCL_GRE_REGIONS

        integer function GreGetRegionBox( a, b, c )
        integer Gre32Entry5, a, b, c
        GreGetRegionBox = Gre32Entry5( a, b, loc( c ), 0, '0000405D'x )
        end

        integer function GreGetRegionRects( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreGetRegionRects = Gre32Entry7( a, b, loc( c ), loc( d ),
     +                                   loc( e ), 0, '0000405E'x )
        end

        integer function GreOffsetRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreOffsetRegion = Gre32Entry5( a, b, loc( c ), 0, '0000405F'x )
        end

        integer function GrePtInRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GrePtInRegion = Gre32Entry5( a, b, loc( c ), 0, '00004060'x )
        end

        integer function GreRectInRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreRectInRegion = Gre32Entry5( a, b, loc( c ), 0, '00004061'x )
        end

        integer function GreCreateRectRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCreateRectRegion = Gre32Entry5( a, loc( b ), c, 0,
     +                                     '00004062'x )
        end

        integer function GreDestroyRegion( a, b )
        integer Gre32Entry4, a, b
        GreDestroyRegion = Gre32Entry4( a, b, 0, '00004063'x )
        end

        integer function GreSetRectRegion( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreSetRectRegion = Gre32Entry6( a, b, loc( c ), d, 0,
     +                                  '00004064'x )
        end

        integer function GreCombineRegion( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreCombineRegion = Gre32Entry7( a, b, c, d, e, 0, '00004065'x )
        end

        integer function GreCombineRectRegion( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreCombineRectRegion = Gre32Entry7( a, b, loc( c ), d, e, 0,
     +                                      '00004066'x )
        end

        integer function GreCombineShortLineRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCombineShortLineRegion = Gre32Entry5( a, b, loc( c ), 0,
     +                                           '00004067'x )
        end

        integer function GreEqualRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreEqualRegion = Gre32Entry5( a, b, c, 0, '00004068'x )
        end

        integer function GrePaintRegion( a, b )
        integer Gre32Entry4, a, b
        GrePaintRegion = Gre32Entry4( a, b, 0, '00004069'x )
        end

        integer function GreSetRegionOwner( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetRegionOwner = Gre32Entry5( a, b, c, 0, '0000406A'x )
        end

        integer function GreFrameRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreFrameRegion = Gre32Entry5( a, b, loc( c ), 0, '0000406B'x )
        end

c$endif

c$ifdef INCL_GRE_CLIP

        integer function GreGetClipBox( a, b )
        integer Gre32Entry4, a, b
        GreGetClipBox = Gre32Entry4( a, loc( b ), 0, '0000406E'x )
        end

        integer function GreGetClipRects( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreGetClipRects = Gre32Entry6( a, loc( b ), loc( c ), loc( d ),
     +                                 0, '0000406F'x )
        end

        integer function GreOffsetClipRegion( a, b )
        integer Gre32Entry4, a, b
        GreOffsetClipRegion = Gre32Entry4( a, loc( b ), 0, '00004070'x )
        end

        integer function GrePtVisible( a, b )
        integer Gre32Entry4, a, b
        GrePtVisible = Gre32Entry4( a, loc( b ), 0, '00004071'x )
        end

        integer function GreRectVisible( a, b )
        integer Gre32Entry4, a, b
        GreRectVisible = Gre32Entry4( a, loc( b ), 0, '00004072'x )
        end

        integer function GreQueryClipRegion( a )
        integer Gre32Entry3, a
        GreQueryClipRegion = Gre32Entry3( a, 0, '00004073'x )
        end

        integer function GreSelectClipRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSelectClipRegion = Gre32Entry5( a, b, loc( c ), 0,
     +                                     '00004074'x )
        end

        integer function GreIntersectClipRectangle( a, b )
        integer Gre32Entry4, a, b
        GreIntersectClipRectangle = Gre32Entry4( a, loc( b ), 0,
     +                                           '00004075'x )
        end

        integer function GreExcludeClipRectangle( a, b )
        integer Gre32Entry4, a, b
        GreExcludeClipRectangle = Gre32Entry4( a, loc( b ), 0,
     +                                         '00004076'x )
        end

        integer function GreSetXformRect( a, b )
        integer Gre32Entry4, a, b
        GreSetXformRect = Gre32Entry4( a, loc( b ), 0, '00004077'x )
        end

        integer function GreSaveRegion( a, b )
        integer Gre32Entry4, a, b
        GreSaveRegion = Gre32Entry4( a, b, 0, '0000407B'x )
        end

        integer function GreRestoreRegion( a, b )
        integer Gre32Entry4, a, b
        GreRestoreRegion = Gre32Entry4( a, b, 0, '0000407C'x )
        end

        integer function GreClipPathCurves( a, b, c )
        integer Gre32Entry5, a, b, c
        GreClipPathCurves = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                                   '0000407D'x )
        end

        integer function GreSelectPathRegion( a, b )
        integer Gre32Entry4, a, b
        GreSelectPathRegion = Gre32Entry4( a, b, 0, '0000407E'x )
        end

        integer function GreRegionSelectBitmap( a, b )
        integer Gre32Entry4, a, b
        GreRegionSelectBitmap = Gre32Entry4( a, b, 0, '0000407F'x )
        end

        integer function GreCopyClipRegion( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreCopyClipRegion = Gre32Entry6( a, b, loc( c ), d, 0,
     +                                   '00004080'x )
        end

        integer function GreSetupDC( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreSetupDC = Gre32Entry8( a, b, c, d, loc( e ), f, 0,
     +                            '00004081'x )
        end

c$endif

c$ifdef INCL_GRE_XFORMS

        integer function GreGetPageUnits( a, b )
        integer Gre32Entry4, a, b
        GreGetPageUnits = Gre32Entry4( a, loc( b ), 0, '00004083'x )
        end

        integer function GreSetPageUnits( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreSetPageUnits = Gre32Entry6( a, b, c, d, 0, '00004084'x )
        end

        integer function GreGetModelXform( a, b )
        integer Gre32Entry4, a, b
        GreGetModelXform = Gre32Entry4( a, loc( b ), 0, '00004085'x )
        end

        integer function GreSetModelXform( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetModelXform = Gre32Entry5( a, loc( b ), c, 0, '00004086'x )
        end

        integer function GreGetWindowViewportXform( a, b )
        integer Gre32Entry4, a, b
        GreGetWindowViewportXform = Gre32Entry4( a, loc( b ), 0,
     +                                           '00004087'x )
        end

        integer function GreSetWindowViewportXform( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetWindowViewportXform = Gre32Entry5( a, loc( b ), c, 0,
     +                                           '00004088'x )
        end

        integer function GreGetGlobalViewingXform( a, b )
        integer Gre32Entry4, a, b
        GreGetGlobalViewingXform = Gre32Entry4( a, loc( b ), 0,
     +                                          '00004089'x )
        end

        integer function GreSetGlobalViewingXform( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetGlobalViewingXform = Gre32Entry5( a, loc( b ), c, 0,
     +                                          '0000408A'x )
        end

        integer function GreSaveXformData( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSaveXformData = Gre32Entry5( a, b, loc( c ), 0, '0000408B'x )
        end

        integer function GreRestoreXformData( a, b, c )
        integer Gre32Entry5, a, b, c
        GreRestoreXformData = Gre32Entry5( a, b, loc( c ), 0,
     +                                     '0000408C'x )
        end

        integer function GreGetPageViewport( a, b )
        integer Gre32Entry4, a, b
        GreGetPageViewport = Gre32Entry4( a, loc( b ), 0, '0000408D'x )
        end

        integer function GreSetPageViewport( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetPageViewport = Gre32Entry5( a, loc( b ), c, 0,
     +                                    '0000408E'x )
        end

        integer function GreGetGraphicsField( a, b )
        integer Gre32Entry4, a, b
        GreGetGraphicsField = Gre32Entry4( a, loc( b ), 0, '00004091'x )
        end

        integer function GreSetGraphicsField( a, b )
        integer Gre32Entry4, a, b
        GreSetGraphicsField = Gre32Entry4( a, loc( b ), 0, '00004092'x )
        end

        integer function GreGetViewingLimits( a, b )
        integer Gre32Entry4, a, b
        GreGetViewingLimits = Gre32Entry4( a, loc( b ), 0, '00004093'x )
        end

        integer function GreSetViewingLimits( a, b )
        integer Gre32Entry4, a, b
        GreSetViewingLimits = Gre32Entry4( a, loc( b ), 0, '00004094'x )
        end

        integer function GreQueryViewportSize( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryViewportSize = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                      '00004095'x )
        end

        integer function GreConvert( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreConvert = Gre32Entry7( a, b, c, loc( d ), e, 0, '00004096'x )
        end

        integer function GreConvertPath( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreConvertPath = Gre32Entry8( a, loc( b ), c, d, loc( e ), f, 0,
     +                                '00004097'x )
        end

        integer function GreSaveXform( a, b )
        integer Gre32Entry4, a, b
        GreSaveXform = Gre32Entry4( a, b, 0, '00004098'x )
        end

        integer function GreRestoreXform( a, b )
        integer Gre32Entry4, a, b
        GreRestoreXform = Gre32Entry4( a, b, 0, '00004099'x )
        end

        integer function GreMultiplyXforms( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreMultiplyXforms = Gre32Entry6( a, loc( b ), loc( c ), d, 0,
     +                                   '0000409A'x )
        end

        integer function GreConvertWithMatrix( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreConvertWithMatrix = Gre32Entry6( a, loc( b ), c, loc( d ),
     +                                      0, '0000409B'x )
        end

c$endif

c$ifdef INCL_GRE_DEVMISC1

        integer function GreDeviceGetAttributes( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDeviceGetAttributes = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                        '0000409D'x )
        end

        integer function GreDeviceSetAVIOFont2( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDeviceSetAVIOFont2 = Gre32Entry6( a, loc( b ), loc( c ), d,
     +                                       0, '0000409E'x )
        end

        integer function GreGetPairKerningTable( a, b, c )
        integer Gre32Entry5, a, b, c
        GreGetPairKerningTable = Gre32Entry5( a, b, loc( c ), 0,
     +                                        '000040A0'x )
        end

        integer function GreDeviceSetAVIOFont( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceSetAVIOFont = Gre32Entry5( a, loc( b ), c, 0,
     +                                      '000040A1'x )
        end

        integer function GreDeviceSetAttributes( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreDeviceSetAttributes = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                        '000040A2'x )
        end

        integer function GreDeviceSetGlobalAttribute( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreDeviceSetGlobalAttribute = Gre32Entry6( a, b, c, d, 0,
     +                                             '000040A3'x )
        end

c$endif

c$ifdef INCL_GRE_DEVMISC2

        integer function GreNotifyClipChange( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreNotifyClipChange = Gre32Entry6( a, loc( b ), c, d, 0,
     +                                     '000040A4'x )
        end

        integer function GreNotifyTransformChange( a, b, c )
        integer Gre32Entry5, a, b, c
        GreNotifyTransformChange = Gre32Entry5( a, b, loc( c ), 0,
     +                                          '000040A5'x )
        end

        integer function GreRealizeFont( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreRealizeFont = Gre32Entry6( a, b, loc( c ), loc( d ), 0,
     +                                '000040A6'x )
        end

        integer function GreErasePS( a )
        integer Gre32Entry3, a
        GreErasePS = Gre32Entry3( a, 0, '000040A7'x )
        end

        integer function GreSetStyleRatio( a, b )
        integer Gre32Entry4, a, b
        GreSetStyleRatio = Gre32Entry4( a, loc( b ), 0, '000040A8'x )
        end

        integer function GreDeviceQueryFontAttributes( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceQueryFontAttributes = Gre32Entry5( a, b, loc( c ), 0,
     +                                              '000040A9'x )
        end

        integer function GreDeviceQueryFonts( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreDeviceQueryFonts = Gre32Entry8( a, b, c, loc( d ), e,
     +                                     loc( f ), 0, '000040AA'x )
        end

        integer function GreDeviceInvalidateVisRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceInvalidateVisRegion = Gre32Entry5( a, b, loc( c ), 0,
     +                                              '000040AB'x )
        end

c$endif

c$ifdef INCL_GRE_PICK

        integer function GreGetPickWindow( a, b )
        integer Gre32Entry4, a, b
        GreGetPickWindow = Gre32Entry4( a, loc( b ), 0, '000040AC'x )
        end

        integer function GreSetPickWindow( a, b )
        integer Gre32Entry4, a, b
        GreSetPickWindow = Gre32Entry4( a, loc( b ), 0, '000040AD'x )
        end

c$endif

c$ifdef INCL_GRE_DEVMISC3

        integer function GreResetBounds( a, b )
        integer Gre32Entry4, a, b
        GreResetBounds = Gre32Entry4( a, b, 0, '000040AE'x )
        end

        integer function GreGetBoundsData( a, b, c )
        integer Gre32Entry5, a, b, c
        GreGetBoundsData = Gre32Entry5( a, b, loc( c ), 0, '000040AF'x )
        end

        integer function GreAccumulateBounds( a, b )
        integer Gre32Entry4, a, b
        GreAccumulateBounds = Gre32Entry4( a, loc( b ), 0, '000040B0'x )
        end

        integer function GreGetCodePage( a )
        integer Gre32Entry3, a
        GreGetCodePage = Gre32Entry3( a, 0, '000040B3'x )
        end

        integer function GreSetCodePage( a, b )
        integer Gre32Entry4, a, b
        GreSetCodePage = Gre32Entry4( a, b, 0, '000040B4'x )
        end

        integer function GreLockDevice( a )
        integer Gre32Entry3, a
        GreLockDevice = Gre32Entry3( a, 0, '000040B5'x )
        end

        integer function GreUnlockDevice( a )
        integer Gre32Entry3, a
        GreUnlockDevice = Gre32Entry3( a, 0, '000040B6'x )
        end

        integer function GreDeath( a )
        integer Gre32Entry3, a
        GreDeath = Gre32Entry3( a, 0, '000040B7'x )
        end

        integer function GreResurrection( a, b, c )
        integer Gre32Entry5, a, b, c
        GreResurrection = Gre32Entry5( a, b, loc( c ), 0, '000040B8'x )
        end

        integer function GreGetDCOrigin( a, b )
        integer Gre32Entry4, a, b
        GreGetDCOrigin = Gre32Entry4( a, loc( b ), 0, '000040BA'x )
        end

        integer function GreDeviceSetDCOrigin( a, b )
        integer Gre32Entry4, a, b
        GreDeviceSetDCOrigin = Gre32Entry4( a, loc( b ), 0,
     +                                      '000040BB'x )
        end

        integer function GreGetLineOrigin( a, b )
        integer Gre32Entry4, a, b
        GreGetLineOrigin = Gre32Entry4( a, loc( b ), 0, '000040BC'x )
        end

        integer function GreSetLineOrigin( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetLineOrigin = Gre32Entry5( a, loc( b ), c, 0, '000040BD'x )
        end

c$endif

c$ifdef INCL_GRE_DEVMISC2

        integer function GreGetStyleRatio( a, b )
        integer Gre32Entry4, a, b
        GreGetStyleRatio = Gre32Entry4( a, loc( b ), 0, '000040BE'x )
        end

c$endif

c$ifdef INCL_GRE_COLORTABLE

        integer function GreQueryColorData( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryColorData = Gre32Entry5( a, b, loc( c ), 0,
     +                                   '000040C3'x )
        end

        integer function GreQueryLogColorTable( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryLogColorTable = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                       '000040C4'x )
        end

        integer function GreCreateLogColorTable( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreCreateLogColorTable = Gre32Entry8( a, b, c, d, e, loc( f ),
     +                                        0, '000040C5'x )
        end

        integer function GreRealizeColorTable( a )
        integer Gre32Entry3, a
        GreRealizeColorTable = Gre32Entry3( a, 0, '000040C6'x )
        end

        integer function GreUnrealizeColorTable( a )
        integer Gre32Entry3, a
        GreUnrealizeColorTable = Gre32Entry3( a, 0, '000040C7'x )
        end

        integer function GreQueryRealColors( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryRealColors = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                    '000040C8'x )
        end

        integer function GreQueryNearestColor( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryNearestColor = Gre32Entry5( a, b, c, 0, '000040C9'x )
        end

        integer function GreQueryColorIndex( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryColorIndex = Gre32Entry5( a, b, c, 0, '000040CA'x )
        end

        integer function GreQueryRGBColor( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryRGBColor = Gre32Entry5( a, b, c, 0, '000040CB'x )
        end

c$endif

c$ifdef INCL_GRE_DEVICE

        integer function GreQueryDeviceBitmaps( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryDeviceBitmaps = Gre32Entry5( a, loc( b ), c, 0,
     +                                       '000040D0'x )
        end

        integer function GreQueryDeviceCaps( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryDeviceCaps = Gre32Entry6( a, b, loc( c ), d, 0,
     +                                    '000040D1'x )
        end

        integer function GreEscape( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreEscape = Gre32Entry8( a, b, c, loc( d ), loc( e ), loc( f ),
     +                           0, '000040D2'x )
        end

        integer function GreQueryHardcopyCaps( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryHardcopyCaps = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                      '000040D3'x )
        end

        integer function GreQueryDevResource2( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryDevResource2 = Gre32Entry5( a, b, c, 0, '000040D4'x )
        end

c$endif

c$ifdef INCL_GRE_PALETTE

        integer function GreDeviceCreatePalette( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceCreatePalette = Gre32Entry5( a, loc( b ), c, 0,
     +                                        '000040D5'x )
        end

        integer function GreDeviceDeletePalette( a, b )
        integer Gre32Entry4, a, b
        GreDeviceDeletePalette = Gre32Entry4( a, b, 0, '000040D6'x )
        end

        integer function GreDeviceSetPaletteEntries( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreDeviceSetPaletteEntries = Gre32Entry8( a, b, c, d, e,
     +                                            loc( f ), 0,
     +                                            '000040D7'x )
        end

        integer function GreDeviceAnimatePalette( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreDeviceAnimatePalette = Gre32Entry8( a, b, c, d, e, loc( f ),
     +                                         0, '000040D8'x )
        end

        integer function GreDeviceResizePalette( a, b, c )
        integer Gre32Entry5, a, b, c
        GreDeviceResizePalette = Gre32Entry5( a, b, c, 0, '000040D9'x )
        end

        integer function GreRealizePalette( a, b, c )
        integer Gre32Entry5, a, b, c
        GreRealizePalette = Gre32Entry5( a, b, loc( c ), 0,
     +                                   '000040DA'x )
        end

        integer function GreQueryHWPaletteInfo( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryHWPaletteInfo = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                       '000040DB'x )
        end

        integer function GreUpdateColors( a )
        integer Gre32Entry3, a
        GreUpdateColors = Gre32Entry3( a, 0, '000040DC'x )
        end

        integer function GreQueryPaletteRealization( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreQueryPaletteRealization = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                            '000040DD'x )
        end

        integer function GreGetVisRects( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreGetVisRects = Gre32Entry6( a, loc( b ), loc( c ), loc( d ),
     +                                0, '000040DE'x )
        end

        integer function GreCreatePalette( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreCreatePalette = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                  '00000240'x )
        end

        integer function GreDeletePalette( a )
        integer Gre32Entry3, a
        GreDeletePalette = Gre32Entry3( a, 0, '00000241'x )
        end

        integer function GreSelectPalette( a, b )
        integer Gre32Entry4, a, b
        GreSelectPalette = Gre32Entry4( a, b, 0, '00004242'x )
        end

        integer function GreSetPaletteEntries( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreSetPaletteEntries = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                      '00000243'x )
        end

        integer function GreAnimatePalette( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreAnimatePalette = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                   '00000244'x )
        end

        integer function GreQueryPalette( a )
        integer Gre32Entry3, a
        GreQueryPalette = Gre32Entry3( a, 0, '00004245'x )
        end

        integer function GreQueryPaletteInfo( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryPaletteInfo = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                     '00000246'x )
        end

        integer function GreSetPaletteOwner( a, b )
        integer Gre32Entry4, a, b
        GreSetPaletteOwner = Gre32Entry4( a, b, 0, '00000247'x )
        end

        integer function GreResizePalette( a, b )
        integer Gre32Entry4, a, b
        GreResizePalette = Gre32Entry4( a, b, 0, '00000248'x )
        end

        integer function GreQueryDefaultPaletteInfo( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryDefaultPaletteInfo = Gre32Entry7( a, b, c, d, loc( e ),
     +                                            0, '00004249'x )
        end

c$endif

c$ifdef INCL_GRE_DCS

        integer function GreOpenDC( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreOpenDC = Gre32Entry7( a, b, c, d, loc( e ), 0, '00000200'x )
        end

        integer function GreCloseDC( a )
        integer Gre32Entry3, a
        GreCloseDC = Gre32Entry3( a, 0, '00004201'x )
        end

        integer function GreResetDC( a, b )
        integer Gre32Entry4, a, b
        GreResetDC = Gre32Entry4( a, b, 0, '00004202'x )
        end

        integer function GreGetHandle( a, b )
        integer Gre32Entry4, a, b
        GreGetHandle = Gre32Entry4( a, b, 0, '00004203'x )
        end

        integer function GreSetHandle( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetHandle = Gre32Entry5( a, b, c, 0, '00004204'x )
        end

        integer function GreGetProcessControl( a )
        integer Gre32Entry3, a
        GreGetProcessControl = Gre32Entry3( a, 0, '00004205'x )
        end

        integer function GreSetProcessControl( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetProcessControl = Gre32Entry5( a, b, c, 0, '00004206'x )
        end

        integer function GreSaveDC( a )
        integer Gre32Entry3, a
        GreSaveDC = Gre32Entry3( a, 0, '00004207'x )
        end

        integer function GreRestoreDC( a, b )
        integer Gre32Entry4, a, b
        GreRestoreDC = Gre32Entry4( a, b, 0, '00004208'x )
        end

        integer function GreQueryDeviceNames( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreQueryDeviceNames = Gre32Entry8( a, loc( b ), loc( c ),
     +                                     loc( d ), loc( e ), loc( f ),
     +                                     0, '00000209'x )
        end

        integer function GreQueryEngineVersion()
        integer Gre32Entry2
        GreQueryEngineVersion = Gre32Entry2( 0, '0000020A'x )
        end

        integer function GreSetDCOwner( a, b )
        integer Gre32Entry4, a, b
        GreSetDCOwner = Gre32Entry4( a, b, 0, '0000420B'x )
        end

c$endif

c$ifdef INCL_GRE_DEVSUPPORT

        integer function GreInvalidateVisRegion( a, b, c )
        integer Gre32Entry5, a, b, c
        GreInvalidateVisRegion = Gre32Entry5( a, b, loc( c ), 0,
     +                                        '0000420F'x )
        end

        integer function GreCreateBitmap( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreCreateBitmap = Gre32Entry7( a, b, c, loc( d ), loc( e ), 0,
     +                                 '00004210'x )
        end

        integer function GreDeleteBitmap( a )
        integer Gre32Entry3, a
        GreDeleteBitmap = Gre32Entry3( a, 0, '00000211'x )
        end

        integer function GreSelectBitmap( a, b )
        integer Gre32Entry4, a, b
        GreSelectBitmap = Gre32Entry4( a, b, 0, '00004212'x )
        end

        integer function GreGetBitmapParameters( a, b )
        integer Gre32Entry4, a, b
        GreGetBitmapParameters = Gre32Entry4( a, loc( b ), 0,
     +                                        '00000213'x )
        end

        integer function GreGetBitmapDimension( a, b )
        integer Gre32Entry4, a, b
        GreGetBitmapDimension = Gre32Entry4( a, loc( b ), 0,
     +                                       '00000214'x )
        end

        integer function GreSetBitmapDimension( a, b )
        integer Gre32Entry4, a, b
        GreSetBitmapDimension = Gre32Entry4( a, loc( b ), 0,
     +                                       '00000215'x )
        end

        integer function GreSetCursor( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetCursor = Gre32Entry5( a, loc( b ), c, 0, '00004216'x )
        end

        integer function GreGetAttributes( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreGetAttributes = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                  '00004217'x )
        end

        integer function GreSetAttributes( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreSetAttributes = Gre32Entry7( a, b, c, d, loc( e ), 0,
     +                                  '00004218'x )
        end

        integer function GreSetGlobalAttribute( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreSetGlobalAttribute = Gre32Entry6( a, b, c, d, 0,
     +                                       '00004219'x )
        end

        integer function GreSetBitmapOwner( a, b )
        integer Gre32Entry4, a, b
        GreSetBitmapOwner = Gre32Entry4( a, b, 0, '0000021A'x )
        end

        integer function GreGetDefaultAttributes( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreGetDefaultAttributes = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                         '0000421B'x )
        end

        integer function GreSetDefaultAttributes( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreSetDefaultAttributes = Gre32Entry6( a, b, c, loc( d ), 0,
     +                                         '0000421C'x )
        end

        integer function GreGetDefaultArcParameters( a, b )
        integer Gre32Entry4, a, b
        GreGetDefaultArcParameters = Gre32Entry4( a, loc( b ), 0,
     +                                            '0000421D'x )
        end

        integer function GreSetDefaultArcParameters( a, b )
        integer Gre32Entry4, a, b
        GreSetDefaultArcParameters = Gre32Entry4( a, loc( b ), 0,
     +                                            '0000421E'x )
        end

        integer function GreGetDefaultViewingLimits( a, b )
        integer Gre32Entry4, a, b
        GreGetDefaultViewingLimits = Gre32Entry4( a, loc( b ), 0,
     +                                            '0000421F'x )
        end

        integer function GreSetDefaultViewingLimits( a, b )
        integer Gre32Entry4, a, b
        GreSetDefaultViewingLimits = Gre32Entry4( a, loc( b ), 0,
     +                                            '00004220'x )
        end

        integer function GreInitializeAttributes( a, b )
        integer Gre32Entry4, a, b
        GreInitializeAttributes = Gre32Entry4( a, b, 0, '00004221'x )
        end

c$endif

c$ifdef INCL_GRE_SETID

        integer function GreDeleteSetId( a, b )
        integer Gre32Entry4, a, b
        GreDeleteSetId = Gre32Entry4( a, b, 0, '00004222'x )
        end

        integer function GreQueryNumberSetIds( a, b )
        integer Gre32Entry4, a, b
        GreQueryNumberSetIds = Gre32Entry4( a, b, 0, '00004223'x )
        end

        integer function GreQuerySetIds( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreQuerySetIds = Gre32Entry8( a, b, loc( c ), loc( d ),
     +                                loc( e ), f, 0, '00004224'x )
        end

c$endif

c$ifdef INCL_GRE_LCID

        integer function GreQueryBitmapHandle( a, b )
        integer Gre32Entry4, a, b
        GreQueryBitmapHandle = Gre32Entry4( a, b, 0, '00004225'x )
        end

        integer function GreSetBitmapID( a, b, c )
        integer Gre32Entry5, a, b, c
        GreSetBitmapID = Gre32Entry5( a, b, c, 0, '00004226'x )
        end

        integer function GreCopyDCLoadData( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCopyDCLoadData = Gre32Entry5( a, b, c, 0, '00004227'x )
        end

c$endif

c$ifdef INCL_GRE_FONTS

        integer function GreQueryLogicalFont( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryLogicalFont = Gre32Entry7( a, b, loc( c ), loc( d ), e,
     +                                     0, '0000422C'x )
        end

        integer function GreCreateLogicalFont( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreCreateLogicalFont = Gre32Entry6( a, b, loc( c ), loc( d ), 0,
     +                                      '0000422D'x )
        end

        integer function GreLoadFont( a )
        integer Gre32Entry3, a
        GreLoadFont = Gre32Entry3( a, 0, '0000022E'x )
        end

        integer function GreUnloadFont( a )
        integer Gre32Entry3, a
        GreUnloadFont = Gre32Entry3( a, 0, '0000022F'x )
        end

        integer function GreQueryFonts( a, b, c, d, e, f )
        integer Gre32Entry8, a, b, c, d, e, f
        GreQueryFonts = Gre32Entry8( a, b, c, loc( d ), e, loc( f ), 0,
     +                               '00004230'x )
        end

        integer function GreQueryFontAttributes( a, b, c )
        integer Gre32Entry5, a, b, c
        GreQueryFontAttributes = Gre32Entry5( a, b, loc( c ), 0,
     +                                        '00004231'x )
        end

        integer function GreInstallIFI( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GreInstallIFI = Gre32Entry6( a, b, c, d, 0, '00000232'x )
        end

        integer function GreLoadPublicFont( a )
        integer Gre32Entry3, a
        GreLoadPublicFont = Gre32Entry3( a, 0, '00000233'x )
        end

        integer function GreUnLoadPublicFont( a )
        integer Gre32Entry3, a
        GreUnLoadPublicFont = Gre32Entry3( a, 0, '00000234'x )
        end

        integer function GreQueryCodePageVector( a )
        integer Gre32Entry3, a
        GreQueryCodePageVector = Gre32Entry3( a, 0, '00000235'x )
        end

        integer function GreQueryFontFileDescriptions( a, b, c )
        GreQueryFontFileDescriptions = Gre32Entry5( a, b, loc( c ), 0,
     +                                              '00000236'x )
        end

c$endif

c$ifdef INCL_GRE_JOURNALING

        integer function GreCreateJournalFile( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCreateJournalFile = Gre32Entry5( a, b, c, 0, '0000023A'x )
        end

        integer function GreDeleteJournalFile( a )
        integer Gre32Entry3, a
        GreDeleteJournalFile = Gre32Entry3( a, 0, '0000023B'x )
        end

        integer function GreStartJournalFile( a, b )
        integer Gre32Entry4, a, b
        GreStartJournalFile = Gre32Entry4( a, b, 0, '0000023C'x )
        end

        integer function GreStopJournalFile( a, b )
        integer Gre32Entry4, a, b
        GreStopJournalFile = Gre32Entry4( a, b, 0, '0000023D'x )
        end

        integer function GrePlayJournalFile( a, b )
        integer Gre32Entry4, a, b
        GrePlayJournalFile = Gre32Entry4( a, b, 0, '0000023E'x )
        end

        integer function GreOpenJournalFile( a, b, c )
        integer Gre32Entry5, a, b, c
        GreOpenJournalFile = Gre32Entry5( a, b, c, 0, '0000023F'x )
        end

c$endif

c$ifdef INCL_GRE_HFONT

        integer function GreCreateFontHandle( a, b, c )
        integer Gre32Entry5, a, b, c
        GreCreateFontHandle = Gre32Entry5( a, loc( b ), loc( c ), 0,
     +                                     '00000250'x )
        end

        integer function GreQueryFontHandle( a, b, c, d, e )
        integer Gre32Entry7, a, b, c, d, e
        GreQueryFontHandle = Gre32Entry7( a, loc( b ), loc( c ),
     +                                    loc( d ), e, 0, '00000251'x )
        end

        integer function GreSelectFontHandle( a, b )
        integer Gre32Entry4, a, b
        GreSelectFontHandle = Gre32Entry4( a, b, 0, '00004252'x )
        end

        integer function GreDeleteFontHandle( a )
        integer Gre32Entry3, a
        GreDeleteFontHandle = Gre32Entry3( a, 0, '00000253'x )
        end

        integer function GreSetFontHandleOwner( a, b )
        integer Gre32Entry4, a, b
        GreSetFontHandleOwner = Gre32Entry4( a, b, 0, '00000254'x )
        end

c$endif

c$ifdef INCL_GRE_POLYGON

        integer function GreDrawRLE( a, b )
        integer Gre32Entry4, a, b
        GreDrawRLE = Gre32Entry4( a, loc( b ), 0, '00004255'x )
        end

        integer function GrePolySet( a, b, c, d )
        integer Gre32Entry6, a, b, c, d
        GrePolySet = Gre32Entry6( a, b, c, loc( d ), 0, '00004256'x )
        end

c$endif
