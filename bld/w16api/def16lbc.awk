BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
}
# get uppercased module name
/^LIBRARY/ { ModuleName = toupper( $2 ); next }

# skip uninteresting lines
/^ *(EXPORTS|;)/ { next }

# process symbols with ordinals using "symbol  @ord" format
/^[ \t]*[A-Za-z0-9_]+[ \t]+@[0-9]+/ {
  sub( /@/, "", $2 ) # kill of the at sign in ordinal
  printf( "++%s.%s.%s\n", $1, ModuleName, $2 ) > OUTFILE
}
