
This directory and its subdirectories contain the regression tests for
the C++ compiler and its corresponding runtime library. The tests can be
run in a largely automated manner. In the regress directory the command
script 'onetest.cmd' runs all tests once with "typical" compiler options
selected. The command script 'testrun.cmd' runs the tests multiple times
using, for example, different compiler options in each run. Note that
fully processing testrun.cmd takes a considerable amount of time---even
on a fast machine.

The subdirectories of regress contain the individual test suites. Each
test suite contains a 'onetest.cmd' and a 'testrun.cmd' command script
that exercises that test suite only. Running one test suite is naturally
much quicker than running the entire collection of regression tests in
one session.

The output of the command scripts are log files in the bld\plustest
directory. Each time a test is run a line is entered in the log file for
that test containing a time stamp. If the test is successful the word
"PASS" is entered on the next line. If the test is unsuccessful nothing
is written to the log besides the time stamp.

Of particular interest are the suites regress\positive and
regress\diagnose. The positive tests exercise the compiler's handling of
correct C++ language constructs. The diagnose tests, on the other hand,
explore the compiler's ability to detect incorrect programs. The other
test suites under regress exercise the compiler and its libraries in
other ways.

Note that these are *regression* tests and not, for example, standards
conformance tests. At any given time they are all supposed to work. If
any of the tests fail the matter should be investigated and corrected;
it means that something that used to work no longer works. Some tests
have material commented out or extra "bug" files that are not currently
part of the test. Such code doesn't work but should. Once whatever bug
it demonstrates is fixed that code should become an active part of its
test suite. In general if a new feature is added to the compiler or if a
bug is fixed, an appropriate test should be added here so exercise that
feature or fix. In this way if the new feature or fix later breaks the
error will be detected.

{ To do: Describe how to add tests }

One contributor's experience in adding a test: lessons learned.

Context: adding the NOMINMAX test to the OWSTL test suite.

Steps taken (idealized):
1. Copy the file 'template.cpp' and rename it to 'nominmax.cpp'.
2. Modify the some_test() function as-needed to perform the test.
Note: a macro, FAIL, is defined in the file 'sanity.cpp' for use when 
failure is detected. This macro is not mentioned in 'template.cpp'.
3. Insert the name of your file into the list of tests in 'makefile'.
4. Run 'onetest.cmd'. This will produce an error line, even if your test
passes.
5. The error line of interest contains the name of your file surrounded 
by hyphens. Copy the hyphens/filename/hyphens (only, no spaces before 
or after) to the file 'test.chk'. Place it in the same order that you
inserted the file's name in the list of tests in 'makefile'.
6. Run 'onetest.cmd'. If your test passes and no other test fails, this
should produce a PASS result.
7. Test your test: ensure it will detect a failure as well as a 
success.

That last point provides one reason to create the test before fixing 
the problem: you can ensure your test detects the problem if you run it 
before the problem has been fixed.

The file 'nominmax.cpp' was developed before the FAIL macro was discovered.
It offers an alternative approach, and explains the advantages of that 
approach to error reporting. This approach, however, is not without its own
problems: it will not scale well with the number of test functions.

Context: constructing a minimal test environment on a secondary system 
(running OS/2) where the Perforce files are on a main system (running
Windows XP).

When I asked what the minimal set of files was which would allow the 
OWSTL tests to run on the secondary system, I was told that no such list 
existed. These steps do not produce a minimal set of files, but do produce
a minimal set of directories. Note that I use "ow" to refer to the root of 
the Perforce files or the files copied from the main system and that I use 
"openwatcom" to refer to the root of the Open Watcom installation on the 
secondary system. The steps are:

1. Create a version of 'setvars.cmd' (the suffix may be different, 
depending on the secondary system's OS) for the secondary system.
2. The directory ow\bat needs to be copied over (the file 'MAKEINIT', 
at least, is required).
3. The directory ow\bld\build\mif directory needs to be copied over 
(several, if not all, of the files are needed).
4. The build tools for the secondary system's OS need to be created: run 
wmake in the corresponding ow\bld\builder subdirectory (in this example, 
ow\bld\builder\os2386).
5. The *.exe files ('cdsay.exe', at least, is required) should be copied 
to the corresponding ow\bld\build subdirectory (in this example, 
ow\bld\build\binp) on the secondary system.
6. The path to the test must then be created. Taking PLUSTEST as an example, 
this is ow\bld\plustest\regress.
7. To support the single test directory OWSTL, the file 'environ.mif' is 
needed in ow\bld\plustest\regress. If more than one test directory is used, 
then the files 'onetest.cmd', 'testrun.cmd' and 'makefile' would probably 
be needed as well. The files 'onetest.cmd' and 'testrun.cmd' may need to be 
modified unless all of the test directories are used, since they go into 
each test directory in turn and run the 'onetest.cmd' or 'testrun.cmd' found
there.
8. The test directories need to be copied over as subdirectories of (in 
this example) ow\bld\plustest\regress.

If the secondary system is using a specific version of OW (such as OW 1.4), 
you may need to copy some of the current header files to openwatcom\h as 
well. OWSTL, for example, needed the files corresponding to the headers 
<algorithm>, <bitset>, <list>, <type_tra>, and <utility> to be copied over.

