@echo off
rem this batch file is part of the interface to a Revision Control System (RCS)
rem call RCS command to force out a copy of the file
rem the first (and only) parameter ( %1 ) is the file name
rem you can change this to use your own RCS by replacing the command below

forceout %1
