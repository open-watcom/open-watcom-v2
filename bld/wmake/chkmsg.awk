#
# Print a list of unreferenced message symbols.
#

BEGIN {
    load_messages( MSGFILE1 )
    load_messages( MSGFILE2 )

    #
    # Scan all source files.
    #
    while( (getline file < SRCFILES) > 0 ) {
        while( (getline line < file) > 0 ) {
            #
            # Split source text into C identifiers.
            #
            gsub( /[^A-Za-z0-9_]+/, " ", line )

            n = split( line, token, / +/ )
            for( i = 1; i <= n; ++i ) {
                if( token[i] in defined )
                    referenced[token[i]] = 1
            }
        }
        close( file )
    }
    close( SRCFILES )

    #
    # Print defined but unreferenced messages.
    #
    for( msg in defined ) {
        if( !(msg in referenced) )
            print msg
    }
}


#
# Add all message symbols defined by pick().
#
function load_messages( file,    line, msg )
{
    while( (getline line < file) > 0 ) {
        if( match( line, /^pick\([ \t]*[A-Z0-9_]+,/ ) ) {
            msg = substr( line, RSTART, RLENGTH )
            sub( /^pick\([ \t]*/, "", msg )
            sub( /,$/, "", msg )
            defined[msg] = 1
        }
    }
    close( file )
}
