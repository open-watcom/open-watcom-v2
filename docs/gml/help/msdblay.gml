:layout
:cmt. Local modifications to the layout
:cmt. Make :h4 large headings
:H4     
        display_heading=no
        page_position=centre
        number_form = none
        post_skip='0.2i'
        pre_top_skip=0
        font=11
        page_eject = yes

:elayout

.dm h4 begin
.pa
.if '&*id' eq '' .do begin
:h4.&*
.do end
.el .do begin
:h4 id='&*id'.&*
.do end
:set symbol='baneven'      value='&*'.
:set symbol='banodd'      value='&*'.
:set symbol='$draft' value=yes
.dm h4 end

.gt h4 add h4 attribute nocont
.ga * id any
