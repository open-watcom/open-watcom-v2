:INIT
   place=start
   :value
      %image("*START INIT VALUE block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue
   :fontvalue
      %image("*START INIT FONTVALUE block for ")
      %image(%font_outname1())
      %image("*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
      %image("Line Height: ") %image(%decimal(%line_height())) 
      %image(" Line Space: ") %image(%decimal(%line_space())) 
      %recordbreak()
      %image("Font Height: ") %image(%decimal(%font_height())) 
      %image(" Font Space: ") %image(%decimal(%font_space())) 
      %recordbreak()
      %image("Default Width: ") %image(%decimal(%default_width())) 
      %recordbreak()
   :efontvalue
:eINIT

:INIT
   place=document
   :value
      %recordbreak()
      %image("*DOCUMENT INIT VALUE block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue
   :fontvalue
      %image("*DOCUMENT INIT FONTVALUE block for ")
      %image(%font_outname1())
      %image("*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :efontvalue
:eINIT

:FINISH
   place=document
   :value
      %recordbreak()
      %image("*DOCUMENT FINISH VALUE block*")
      %recordbreak()
      %image("Font Number: ") %image(%decimal(%font_number()))
      %image(" Instance: ") %image(%decimal(%getnumsymbol("fontcount")))
      %recordbreak()
      %image("Pages: ") %image(%decimal(%pages())) 
      %image(" Location: ") %image(%decimal(%x_address())) %image(",") %image(%decimal(%y_address())) 
      %recordbreak()
   :evalue
:eFINISH

