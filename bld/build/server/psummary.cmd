rem OS/2 command file to ease the calling of the Perl summary.pl
rem pathes to be adjusted
rem this file creates new base protocol files

set PERLLIB_PREFIX=e:/perl5/lib;e:/OWBUILD
set beginlibpath=e:\perl5\bin
e:\perl5\bin\perl.exe summary.pl e:\ow\bld\pass1.log bldbase.txt
e:\perl5\bin\perl.exe summary.pl e:\ow\bld\pass2.log bldbase2.txt
e:\perl5\bin\perl.exe summary.pl e:\ow\docs\pass1.log bldbased.txt
