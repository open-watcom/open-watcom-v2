.386
        name    DOSSTART

_TEXT           segment byte public use32 'CODE'
        db      0fh, 0a2h
        db      0fh, 030h
        db      0fh, 031h
        db      0fh, 032h
        db      0fh, 022h, 0e0h
        db      0fh, 020h, 0e0h
        db      0fh, 0c7h, 008h
_TEXT   ends

        end
