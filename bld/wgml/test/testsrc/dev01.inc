:PAUSE
   place = START
   :value.
      %image("*** START PAUSE block.")
      %image(" Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
   :evalue
:EPAUSE.

:PAUSE
   place = DOCUMENT
   :value.
      %image( "*** DOCUMENT PAUSE block." )
      %image(" Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
   :evalue.
:ePAUSE.

:PAUSE
   place = DOCUMENT_PAGE
   :value.
      %image( "*** DOCUMENT_PAGE PAUSE block." )
      %image(" Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
   :evalue.
:ePAUSE.

:PAUSE
   place = DEVICE_PAGE
   :value.
      %image( "*** DEVICE_PAGE PAUSE block. " )
      %image(" Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
   :evalue.
:ePAUSE.

