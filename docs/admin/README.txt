
README.txt
==========

This folder contains notes and administrative scripts that might be
useful to the project maintainer.

making-release.txt
   Notes on the procedure for creating a release.

writable-files.js
   A script that finds all writable files. This is useful because
   'builder clean' does not really clean everything. If you want to
   create a pristine copy of the source you should delete rel2, do
   'builder clean' and then run this script to find all the remaining
   writable files and delete them manually. I create a batch file out of
   the output of this script.

empty-dirs.js
   But wait... you aren't done yet. You also need to run empty-dirs.js
   to find all the folders that are now empty (after removing all the
   stuff above there are usually a few). You need to remove those
   folders as well.

   But wait... you aren't done yet. You need to run empty-dirs.js
   multiple times since the deletion of a folder might cause its parent
   folder to become empty. Run empty-dirs.js repeatedly (removing the
   folders it finds each time) until there are no more empty folders.

makeOW.bat
   This is the batch file I use to build Open Watcom.
