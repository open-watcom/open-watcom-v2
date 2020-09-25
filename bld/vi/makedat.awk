# Print set counter variable
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    FS = "pick"
    num = 0
    fnum = 0
    lines_count = 0
}

FNR == 1 {
    fnum = fnum + 1
    if( fnum == 3 ) {
        printf( "%d\n", lines_count ) > OUTFILE
        num = -100
    }
    if( fnum == 4 ) {
        num = 0
    }
}

# Process all lines beginning with 'pick'
/^[\t ]*pick/ {
    for( i = 2; i <= NF; ++i ) {
        if( fnum < 3 ) {
            lines_count = lines_count + 1
        } else {
            split( $i, f, "[\t ,]+" )
            printf( "%s %d\n", f[2], num ) > OUTFILE
            num = num + 1   # Increment num
        }
    }
}
