; Test absolute segment alignment. Since only the segment portion is
; specified, the segments are obviously paragraph aligned.
; Also note that in OMF, an absolute segment is specified by setting
; alignment to 0.

BDA     segment at 40h

var1    db      ?

align   8

var2    dw      ?

align   16

var3    db      ?

BDA     ends

; BUG:
; The following shouldn't be allowed, but currently is. The segment
; ends up not being absolute
;
;BAD     segment at 40h word
;BAD     ends

        end
