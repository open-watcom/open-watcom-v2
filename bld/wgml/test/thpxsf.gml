.* test for a : hpx and : SF tags in varying combinations
:gdoc
:body
:sf font=2. SF 2:eSF.
:sf font=3. SF 3 :SF font=1. sf1 within sf3:esf.:eSF.
normal text
:HP3. hp3 :SF font=2. additional SF 2:hp1. additional hp1 :SF font=0. add SF 0 :eSF.
                        SF0 end      :eHP1. HP1 ended :eSF. SF end :ehp3
hp3 end
.br
:hp0. HP0
:hp1. HP1
:hp1. HP1 duplicate
:hp0. HP0 again
:eHP0. HP0 end
:hp2. HP2
:hp3. HP3
:ehp3.
:sf font=1. SF1 active.
:sf font=2. SF2 active.
.br
:esf.
:esf.
:ehp2.
:ehp1.
:ehp1.
:ehp0.
.im thpxsf2.inc
.br text.
:egdoc
