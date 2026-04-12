; Test: EXTRN symbol:ABS must be usable in byte-sized contexts.
; WASM incorrectly rejected this with E050 because it treated ABS
; externals as WORD-sized offsets instead of absolute values.
;
.8086
_TEXT SEGMENT BYTE PUBLIC 'CODE'
    EXTRN MY_CONST:ABS
my_var DB 0
    cmp my_var, MY_CONST
_TEXT ENDS
END
