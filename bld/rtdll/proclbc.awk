# Munge a wlib function import script. If BR is set, apply special
# treatment to functions which need '_br' appended. If OS is os2,
# also strip the module name extension.

BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    if( LSTFILE == "" ) {
        printf( "LSTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 2
    }
    if( OS == "" ) {
        printf( "OS variable must be set!\n" ) > "/dev/stderr"
        exit 3
    }
    # Read a list of functions that need _br treatment
    while( ( getline < LSTFILE ) > 0 ) {
        br_funcs[$1] = 1        # Associative array!
        br_funcs["_" $1] = 1    # Derive underscored version
        ++num_fns
    }
    if( num_fns < 2 ) {
        printf( "Failed to read input from file '%s'\n", LSTFILE ) > "/dev/stderr"
        exit 3
    }
    FS = "'"    # Split input lines on apostrophes
}

# Process the input lines
$1 ~ /\+\+/ {
    if( OS == "os2" ) {
        split( $4, mod, /\./ )
        $4 = mod[1]
    }
    if( BR && $2 in br_funcs )
        printf( "++'%s_br'.'%s'..'%s'\n", $2, $4, $2 ) > OUTFILE
    else
        printf( "++'%s'.'%s'\n", $2, $4 ) > OUTFILE
}
