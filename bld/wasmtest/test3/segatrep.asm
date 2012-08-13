	DOSSEG
mkseg   macro   mname
&mname  segment at (dummy_segs * 8)
dummy_segs =    dummy_segs + 1
&mname  ends
        endm

dummy_segs =    1
        mkseg   gdtseg
        mkseg   idtseg
        mkseg   rtcodeseg
        mkseg   rtdataseg
        mkseg   pspseg
        mkseg   envseg
        mkseg   mem0seg
        mkseg   biosdatseg
        mkseg   vidAseg
        mkseg   vidB0seg
        mkseg   vidB8seg
        if 1
        rept    16
        mkseg   dummy%dummy_segs
        if      dummy_segs ge 16
        exitm
        endif
        endm
        mkseg   dummy%dummy_segs
        endif
	end
