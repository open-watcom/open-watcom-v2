@echo off
spell -f duh %1.gml > x
sort < x > y
uniq < y > %1.spl
erase y
echo "%1.gml" >> result
type %1.spl >> result
erase x
