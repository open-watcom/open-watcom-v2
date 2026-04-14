; Test: NAME directive without operand must produce E066,
; not crash.  The label+directive heuristic rewrites
; "call name" into label "call" + directive NAME with
; no operand, which used to dereference a NULL pointer.
;
call name
END
