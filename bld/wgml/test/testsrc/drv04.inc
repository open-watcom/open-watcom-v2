:ABSOLUTEADDRESS
   :value.
      %recordbreak()
      %image("*ABSOLUTEADDRESS block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue.
:eABSOLUTEADDRESS.

:HLINE
   thickness = 8
   :value.
      %recordbreak()
      %image("*HLINE block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue.
:eHLINE.

:VLINE
   thickness = 10
   :value.
      %recordbreak()
      %image("*VLINE block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue.
:eVLINE.

:DBOX
   thickness = 8
   :value.
      %recordbreak()
      %image("*DBOX block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue.
:eDBOX.

