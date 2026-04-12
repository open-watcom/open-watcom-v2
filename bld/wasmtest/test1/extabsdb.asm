; Test: EXTRN symbol:ABS in data-emission context (DB).
;
.8086
_TEXT SEGMENT BYTE PUBLIC 'CODE'
    EXTRN MY_CONST:ABS
    DB MY_CONST
_TEXT ENDS
END
