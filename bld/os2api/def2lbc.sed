# Delete empty lines
/^$/d
# Extract the library name
/^LIBRARY *\([A-Za-z0-9_]*\)$/s//\1/
# Put the first line (library name) into hold space
1h
# Delete the first line
1d
# Remove the 'EXPORTS' line
/^EXPORTS.*$/d
# Append the hold space to each line, preceded by a newline
G
# Delete all whitespace
/ /s///g
# Munge remaining lines into the format expected by wlib
/^\([A-Za-z0-9_]*\)@\([0-9]*\)\n\(.*\)$/s//++'\1'.'\3'.'\2'/
