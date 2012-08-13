m1 macro data1
	db data1
        exitm
        a = 3
endm

.model small
.code
.data

	irpc xx1,<abcdefghijkl>
		m1 '&xx1'
		irpc xx2,<1234567890>
			m1 xx2
			exitm
                        a = 2
		endm
		exitm
                a = 1
	endm

end
