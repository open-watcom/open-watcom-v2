@echo off
rem this batch file is part of the interface to a Revision Control System (RCS)
rem call the rcs command to unlock (scrap) this file, 
rem and go back to the old version
rem the first (and only) parameter ( %1 ) is the file name
rem you can change this to use your own RCS by replacing the command below

pscrap %1
