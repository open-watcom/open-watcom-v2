.* test for .co .fo .ju  control word scanning
.ty
.ty         default settings: concat &$co, justify &$ju
.ty
.ju off
.ty after '.ju OFF'     call: concat &$co, justify &$ju
.ju
.ty after '.ju '        call&gml. concat &$co, justify &$ju
.fo on
.ty after '.fo ON'      call&gml concat &$co, justify &$ju
.fo off
.ty after '.fo OFF'     call&gml concat &$co, justify &$ju
.ju center
.ty after '.ju CENTER'  call&gml concat &$co, justify &$ju
.ju half
.ty after '.ju HALF'    call&gml concat &$co, justify &$ju
.co on
.ty after '.co ON'      call&gml concat &$co, justify &$ju
.fo off
.ty after '.fo OFF'     call&gml concat &$co, justify &$ju
.ju inside
.ty after '.ju INSIDE'  call&gml concat &$co, justify &$ju
.ju outside
.ty after '.ju OUTSIDE' call&gml concat &$co, justify &$ju
.fo centre
.ty after '.fo CENTRE'  call&gml concat &$co, justify &$ju
.ju left
.ty after '.ju LEFT'    call&gml concat &$co, justify &$ju
.ju right
.ty after '.ju RIGHT'   call&gml concat &$co, justify &$ju
