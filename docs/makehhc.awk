BEGIN {
    print "<html><body>"
    print "<object type=\"text/site properties\">"
    print "<param name=\"ImageType\" value=\"Folder\">"
    print "</object>"
    print "<ul>"
    level = 1
}

/<h. id=\"(.*)\"> (.*) <\/h.>/ {
    newlevel = $0
    gsub( /<h/, "", newlevel )
    gsub( / id=\"(.*)\" (.*) <\/h.>/, "", newlevel )
    file = $0
    gsub( /<h. id=\"/, "", file )
    gsub( /\"> (.*) <\/h.>/, "", file )
    if( file == "Index_of_Topics" ) {
        next
    }
    title = $0
    gsub( /<h. id=\"(.*)\"> /, "", title )
    gsub( / <\/h.>/, "", title )
    gsub( /\&nbsp\;/, "", title )
    newlevel = newlevel + 0
    if( newlevel < level ) {
        while( newlevel < level ) {
            print "</ul>"
            level--
        }
    } else if( newlevel > level ) {
        print "<ul>"
        level = newlevel
    }
    print "<li><object type=\"text/sitemap\">"
    print "<param name=\"Name\" value=\"" title "\">"
    print "<param name=\"Local\" value=\"" file ".htm\">"
    print "</object>"
}

END {
    while( level > 0 ) {
        print "</ul>"
        level--
    }
    print "</body></html>"
}
