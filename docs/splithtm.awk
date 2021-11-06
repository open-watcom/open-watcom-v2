BEGIN {
    file = ""
}

/<h. id=\"(.*)\"> (.*) <\/h.>/ {
    if( length( file ) > 0 ) {
        print "</body>" >> file
        print "</html>" >> file
        close( file )
    }
    file = $0
    gsub( /<h. id=\"/, "", file )
    gsub( /\"> (.*) <\/h.>/, "", file )
    file = file ".htm"
    if( length( dir ) > 0 ) {
        file = dir "/" file
    }
    title = $0
    gsub( /<h. id=\"(.*)\"> /, "", title )
    gsub( / <\/h.>/, "", title )
    print "<html>" > file
    print "<head><title>" title "</title></head>" >> file
    print "<body>" >> file
    print "<h1>" title "</h1>" >> file
    next
}

length( file ) > 0 {
    line = $0
    while( match( line, /<a href=\"\#([^>]*)\">/ ) ) {
        temp = substr( line, 0, RSTART + 8 )
        temp = temp substr( line, RSTART + 10, RLENGTH - 12 ) ".htm"
        temp = temp substr( line, RSTART + RLENGTH - 2 )
        line = temp
    }
    print line >> file
}
