
This folder contains "translation" headers for use by PC-Lint. Since PC-Lint does not understand
Open Watcom's method for locating header files, it is necessary to actually provide a file for
each header with a long name. That file just #includes the short named header actually in the
Open Watcom distribution.

PC-Lint may have a way of defining header aliases. If it does that method should be used instead
of this rather hacked solution.

This folder also contains local copies of the various PC-Lint configuration files. These are the
files that are actually used by PC-Lint if the LIN.BAT batch file in the parent folder is used
to invoke PC-Lint.
