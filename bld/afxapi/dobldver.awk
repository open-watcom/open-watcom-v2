BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
}
{
    line = $0;
    gsub( /\$\$BLD_VER\$\$/, bld_ver, line );
    print line > OUTFILE;
}
