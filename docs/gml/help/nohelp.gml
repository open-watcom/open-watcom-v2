:set symbol='help' value=''.
:set symbol='ehelp' value=''.
:set symbol='hbrace' value=''.
:set symbol='ehbrace' value=''.
:set symbol='b' value=' '
:set symbol='hs' value='\b\b '
:set symbol='colon' value=':'
:set symbol='appsp' value=''.

.dm tail begin
.dm tail end

.dm etail begin
.dm etail end

.dm tline begin
.dm tline end


.dm helphd begin
.dm helphd end

.dm fig begin
:fig id='&*id.' frame=&*frame..
.dm fig end

.dm figref begin
:figref refid='&*refid'.
.dm figref end

.dm hinclude begin
:include file='&file'.
.dm hinclude end

.dm comref begin
&refid
.dm comref end

.dm snap begin
:fig id='&*1.' place=inline frame=none.
:graphic file='&*1..ps' depth= '2.63i'
:figcap.&*2.
:cmt.:figcap.'&*1'->&*2.
:efig
.dm snap end

:set symbol='blank' value='\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ '
:set symbol='appblank' value='&blank.&blank.'

:cmt..dm HID0 begin
:cmt.    :H0 id='&*id.'.&*head.
:cmt..dm HID0 end
:cmt..dm HID1 begin
:cmt.    :H1 id='&*id.'.&*head.
:cmt..dm HID1 end
:cmt..dm HID2 begin
:cmt.    :H2 id='&*id.'.&*head.
:cmt..dm HID2 end
:cmt..dm HID3 begin
:cmt.    :H3 id='&*id.'.&*head.
:cmt..dm HID3 end
:cmt..dm HID4 begin
:cmt.    :H4 id='&*id.'.&*head.
:cmt..dm HID4 end
:cmt..dm HID5 begin
:cmt.    :H5 id='&*id.'.&*head.
:cmt..dm HID5 end
:cmt.
:cmt..dm HH0 begin
:cmt.    :H0 &*head.
:cmt..dm HH0 end
:cmt..dm HH1 begin
:cmt.    :H1 &*head.
:cmt..dm HH1 end
:cmt..dm HH2 begin
:cmt.    :H2 &*head.
:cmt..dm HH2 end
:cmt..dm HH3 begin
:cmt.    :H3 &*head.
:cmt..dm HH3 end
:cmt..dm HH4 begin
:cmt.    :H4 &*head.
:cmt..dm HH4 end
:cmt..dm HH5 begin
:cmt.    :H5 &*head.
:cmt..dm HH5 end
:cmt..dm AH1 begin
:cmt.    :H1 &*head.
:cmt..dm AH1 end
:cmt..dm AH2 begin
:cmt.    :H2 &*head.
:cmt..dm AH2 end
:cmt..dm AH3 begin
:cmt.    :H3 &*head.
:cmt..dm AH3 end

:cmt. ..gt HH0 add HH0 att nocont
:cmt. ..ga * ID length 7
:cmt. ..ga * STITLE any
:cmt. 
:cmt. ..gt HH1 add HH1 att nocont
:cmt. ..ga * ID length 7
:cmt. ..ga * STITLE any
:cmt. 
:cmt. ..gt HH2 add HH2 att nocont
:cmt. ..ga * ID length 7
:cmt. 
:cmt. ..gt HH3 add HH3 att nocont
:cmt. ..ga * ID length 7
:cmt. 
:cmt. ..gt HH4 add HH4 att nocont
:cmt. ..ga * ID length 7
:cmt. 
:cmt. ..gt HH5 add HH5 att nocont
:cmt. ..ga * ID length 7
:cmt. 
:cmt. ..gt HDREF add @hdref att nocont
:cmt. ..ga * ID length 7

