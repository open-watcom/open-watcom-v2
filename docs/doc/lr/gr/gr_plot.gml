The drawing &routines cause pixels to be set with a pixel value.
By default, the value to be set is obtained by replacing the
original pixel value with the supplied pixel value.
Alternatively, the replaced value may be computed as a
function of the original and the supplied pixel values.
.pp
The plotting action can have one of the following values:
.begterm 15
.uterm GPSET
replace the original screen pixel value with the supplied pixel value
.uterm GAND
replace the original screen pixel value with the
.us bitwise and
of the original pixel value and the supplied pixel value
.uterm GOR
replace the original screen pixel value with the
.us bitwise or
of the original pixel value and the supplied pixel value
.uterm GXOR
replace the original screen pixel value with the
.us bitwise exclusive-or
of the original pixel value and the supplied pixel value.
Performing this operation twice will restore the original screen
contents, providing an efficient method to produce animated effects.
.endterm
