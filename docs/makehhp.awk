BEGIN {
    print "[OPTIONS]"
    print "Binary TOC=Yes"
    print "Compatibility=1.1 or later"
    print "Contents file=" hbook ".hhc"
    print "Create CHI file=Yes"
    print "Default Window=Main"
    print "Full-text search=Yes"
    print "Index file=" hbook ".hhk"
    print "Language=0x409 English (United States)"
    print "Compiled File=" hbook ".chm"
    
    # Future processing...
    fcount = 0
    last_file=""
}

/<TITLE> (.*) <\/TITLE>/ {
    title = $0
    gsub( /<TITLE> /, "", title )
    gsub( / <\/TITLE>/, "", title )
    print "Title=" title
}

/<H. ID=\"(.*)\">/ {
    file = $0
    gsub( /<H. ID=\"/, "", file )
    gsub( /\"> (.*) <\/H.>/, "", file )

    # "Index_of_Topics" should be included since it is
    # defined in the .h file, but we'll place it last
    # since it shouldn't be our first page
    if (file == "Index_of_Topics") {
        last_file = file
    } 
    else {
    
        # The first file should also be our default topic
        if (fcount == 0) {
            print "Default Topic=" file ".htm"
            print ""
            
            # The [WINDOWS] section isn't strictly necessary
            # so we'll leave it out (since chmcmd handles it
            # poorly) and let Windows make decisions about the
            # window.
            #print "[WINDOWS]"
            #print "Main=\"" title "\",\"" hbook ".hhc\",\"" hbook ".hhk\",,,,,,,0x2420,,0x60300e,,,,,,,,0"
            #print ""
            
            print "[FILES]"
        }
        
        fcount = fcount + 1
        print file ".htm"
    }
}

END {

    # If we encountered a "Index_of_Topics" entry,
    # drop its reference here.
    if (last_file > 0) print last_file ".htm"
    print ""
    
    print "[MAP]"
    print "#include " hbook ".h"
    print ""
    
    # Directly write aliases into the project rather
    # than an include since it seems to work a bit
    # better.
    print "[ALIAS]"
    while ((getline line < (hbook "/" hbook ".hha")) > 0)
        print line
    print ""
    
}
