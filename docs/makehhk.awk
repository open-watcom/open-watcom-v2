BEGIN {
    print "<html><body><ul>"
}

function transsym( str ) {
    targ = str
    gsub( / /, "_", targ )
    gsub( /-/, "M", targ )
    gsub( /\//, "D", targ )
    gsub( /\+/, "P", targ )
    gsub( /\[/, "U", targ )
    gsub( /\]/, "V", targ )
    gsub( /</, "X", targ )
    gsub( />/, "Y", targ )
    gsub( /\=/, "E", targ )
    gsub( /{/, "_", targ )
    gsub( /}/, "_", targ )
    gsub( /,/, "_", targ )
    gsub( /\#/, "_", targ )
    gsub( /\?/, "_", targ )
    gsub( /\./, "_", targ )
    gsub( /\|/, "_", targ )
    gsub( /\"/, "_", targ )
    gsub( /'/, "_", targ )
    gsub( /:/, "_", targ )
    gsub( /\(/, "L", targ )
    gsub( /\)/, "R", targ )
    gsub( /\*/, "T", targ )
    gsub( /\\/, "B", targ )
    gsub( /%/, "_", targ )
    gsub( /~/, "_", targ )
    gsub( /@/, "A", targ )
    gsub( /\$/, "_", targ )
    gsub( /&/, "_", targ )
    gsub( /!/, "_", targ )
    gsub( /\r/, "", targ )
    return targ
}

function fname() {
    if( length( sectid ) ) {
        fn = sectid
    } else {
        fn = prefix file
    }
    return fn
}

/\.helppref / {
    prefix = $0
    gsub( /\.helppref /, "", prefix )
    if( length( prefix ) )
       prefix = transsym( prefix ) "_"
}

/\.ixchap / {
    sectid = ""
    file = $0
    gsub( /\.ixchap /, "", file )
    file = transsym( file )
}

/\.ixsect / {
    sectid = ""
    file = $0
    gsub( /\.ixsect /, "", file )
    file = transsym( file )
}

/\.ixsectid / {
    sectid = $0
    gsub( /\.ixsectid /, "", sectid )
    gsub( /\r/, "", sectid )
}

/\.ixline '(.*)' '(.*)'/ {
    name1 = $0
    gsub( /\.ixline '/, "", name1 )
    gsub( /' '(.*)'[\r]?/, "", name1 )
    name2 = $0
    gsub( /\.ixline '(.*)' '/, "", name2 )
    gsub( /'[\r]?/, "", name2 )
    print "<li><object type=\"text/sitemap\">"
    print "<param name=\"Name\" value=\"" name1 "\">"
    print "<param name=\"See also\" value=\"" name1 "\">"
    print "</object>"
    print "<ul>"
    print "<li><object type=\"text/sitemap\">"
    print "<param name=\"Name\" value=\"" name2 "\">"
    print "<param name=\"Local\" value=\"" fname() ".htm\">"
    print "</object>"
    print "</ul>"
    next
}

/\.ixline '(.*)'/ {
    name = $0
    gsub( /\.ixline /, "", name )
    gsub( /'[\r]?/, "", name )
    print "<li><object type=\"text/sitemap\">"
    print "<param name=\"Name\" value=\"" name "\">"
    print "<param name=\"Local\" value=\"" fname() ".htm\">"
    print "</object>"
}

END {
    print "</ul></body></html>"
}
