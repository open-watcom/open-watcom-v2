#!/usr/bin/perl -w
############################################################################
#
#                            Open Watcom Project
#
#  Copyright (c) 2004-2006 The Open Watcom Contributors. All Rights Reserved.
#
#  ========================================================================
#
#    This file contains Original Code and/or Modifications of Original
#    Code as defined in and that are subject to the Sybase Open Watcom
#    Public License version 1.0 (the 'License'). You may not use this file
#    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
#    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
#    provided with the Original Code and Modifications, and is also
#    available at www.sybase.com/developer/opensource.
#
#    The Original Code and all software distributed under the License are
#    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
#    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
#    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
#    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
#    NON-INFRINGEMENT. Please see the License for the specific language
#    governing rights and limitations under the License.
#
#  ========================================================================
#
#  Description: This file is the main program for the build server.
#
###########################################################################
use strict;

use Common;
use Config;

my(@CVS_messages);
my($OStype);
my($ext);
my($setenv);
my($build_platform);

if ($#ARGV == -1) {
    Common::read_config( "config.txt" );
} elsif ($#ARGV == 0) {
    Common::read_config( $ARGV[0] );
} else {
    print "Usage: dobuild [config_file]\n";
    exit 1;
}

my $home      = $Common::config{"HOME"};
my $OW        = $Common::config{"OW"};
my $WATCOM    = $Common::config{"WATCOM"};
my $relroot   = $Common::config{"RELROOT"};

if ($^O eq "MSWin32") {
    $OStype = "WIN32";
    $ext    = "bat";
    $setenv = "set";
    if ($Config{archname} =~ /64/) {
        $build_platform = "win32-x64";
    } else {
        $build_platform = "win32-x86";
    }
} elsif ($^O eq "linux") {
    $OStype = "UNIX";
    $ext    = "sh";
    $setenv = "export";
    $build_platform = "linux-x86";
} elsif ($^O eq "os2") {
    $OStype = "OS2";
    $ext    = "cmd";
    $setenv = "set";
    $build_platform = "os2-x86";
} else {
    print "Unsupported or unknown platform '$^O' !\n";
    print "Review dobuild.pl file and fix it for new platform!\n";
    exit 1;
}

my $build_boot_batch_name = "$home\/boot.$ext";
my $build_batch_name      = "$home\/build.$ext";
my $docs_batch_name       = "$home\/docsbld.$ext";
my $build_installer_name  = "$home\/instbld.$ext";
my $test_batch_name       = "$home\/test.$ext";
my $post_batch_name       = "$home\/post.$ext";
my $setvars               = "$OW\/setvars.$ext";
my $prev_changeno_name    = "$home\/changeno.txt";
my $prev_changeno         = "0";
my $prev_report_stamp     = "";
my $build_needed          = 1;
my $boot_result           = "fail";
my $pass_result           = "fail";
my $docs_result           = "fail";
my $CVS_result            = "fail";

sub get_prev_changeno
{
    my @fields;
    open(CHNGNO, "$prev_changeno_name") || return;
    while (<CHNGNO>) {
        s/\r?\n/\n/;
        @fields = split;
        $prev_changeno = $fields[0];
        $prev_report_stamp  = $fields[1];
    }
    close(CHNGNO);
    if (!defined($prev_report_stamp)) {
        $prev_report_stamp = "";
    }
}

sub set_prev_changeno
{
    open(CHNGNO, ">$prev_changeno_name") || return;
    print CHNGNO $_[0], " ", $_[1];
    close(CHNGNO);
}

sub batch_output_make_change_objdir
{
    if ($OStype eq "UNIX") {
        print BATCH "if [ ! -d \$OWOBJDIR ]; then mkdir \$OWOBJDIR; fi\n";
        print BATCH "cd \$OWOBJDIR\n";
    } else {
        print BATCH "if not exist %OWOBJDIR% mkdir %OWOBJDIR%\n";
        print BATCH "cd %OWOBJDIR%\n";
    }
}

sub batch_output_set_test_env
{
    print BATCH "$setenv WATCOM=", $relroot, "\n";
    if ($^O eq "MSWin32") {
        print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\nt\n";
        if ($Config{archname} =~ /64/) {
            print BATCH "$setenv PATH=%WATCOM%\\binnt64;%WATCOM%\\binnt;%WATCOM%\\binw;%PATH%\n";
        } else {
            print BATCH "$setenv PATH=%WATCOM%\\binnt;%WATCOM%\\binw;%PATH%\n";
        }
    } elsif ($^O eq "os2") {
        print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\os2\n";
        print BATCH "$setenv PATH=%WATCOM%\\binp;%WATCOM%\\binw;%PATH%\n";
        print BATCH "$setenv BEGINLIBPATH=%WATCOM%\\binp\\dll;%BEGINLIBPATH%\n";
    } elsif ($^O eq "linux") {
        print BATCH "$setenv INCLUDE=\$WATCOM/lh\n";
        print BATCH "$setenv PATH=\$WATCOM/binl:\$PATH\n";
    }
}

sub batch_output_set_boot_env
{
    if ($WATCOM ne "") {
	print BATCH "$setenv WATCOM=", $WATCOM, "\n";
	if ($^O eq "MSWin32") {
	    print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\nt\n";
	    print BATCH "$setenv PATH=%OWBINDIR%;%OWROOT%\\build;%WATCOM%\\binnt;%WATCOM%\\binw;%OWDEFPATH%\n";
	} elsif ($^O eq "os2") {
	    print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\os2\n";
	    print BATCH "$setenv PATH=%OWBINDIR%;%OWROOT%\\build;%WATCOM%\\binp;%WATCOM%\\binw;%OWDEFPATH%\n";
	    print BATCH "$setenv BEGINLIBPATH=%WATCOM%\\binp\\dll;%OWDEFBEGINLIBPATH%\n";
	} elsif ($^O eq "linux") {
	    print BATCH "$setenv INCLUDE=\$WATCOM/lh\n";
	    print BATCH "$setenv PATH=\$OWBINDIR:\$OWROOT/build:\$WATCOM/binl:\$OWDEFPATH\n";
	}
    }
}

sub batch_output_reset_env
{
    if ($OStype eq "UNIX") {
        print BATCH ". \$OWROOT/cmnvars.$ext\n";
    } else {
        print BATCH "call %OWROOT%\\cmnvars.$ext\n";
    }
}

sub batch_output_build_wmake_builder_rm
{
    # Create fresh builder tools to prevent lockup build server 
    # if builder tools from previous build are somehow broken
    #
    # Create new wmake tool, previous clean removed it.
    print BATCH "cd $OW\ncd bld\ncd wmake\n";
    batch_output_make_change_objdir();
    if ($OStype eq "UNIX") {
        print BATCH "rm -f \$OWBINDIR/wmake\n";
        if ($WATCOM eq "") {
            print BATCH "make -f ../posmake clean\n";
            print BATCH "make -f ../posmake\n";
        } else {
            print BATCH "wmake -h -f ../wmake clean\n";
            print BATCH "wmake -h -f ../wmake\n";
        }
    } else {
        print BATCH "if exist %OWBINDIR%\\wmake.exe del %OWBINDIR%\\wmake.exe\n";
        if ($WATCOM eq "") {
            print BATCH "nmake -nologo -f ..\\nmake clean\n";
            print BATCH "nmake -nologo -f ..\\nmake\n";
        } else {
            print BATCH "wmake -h -f ..\\wmake clean\n";
            print BATCH "wmake -h -f ..\\wmake\n";
        }
    }
    # Create new builder tool, previous clean removed it.
    print BATCH "cd $OW\ncd bld\ncd builder\n";
    batch_output_make_change_objdir();
    if ($OStype eq "UNIX") {
        print BATCH "rm -f \$OWBINDIR/builder\n";
        print BATCH "\$OWBINDIR/wmake -h -f ../binmake bootstrap=1 clean\n";
        print BATCH "\$OWBINDIR/wmake -h -f ../binmake bootstrap=1 builder.exe\n";
    } else {
        print BATCH "if exist %OWBINDIR%\\builder.exe del %OWBINDIR%\\builder.exe\n";
        print BATCH "%OWBINDIR%\\wmake -h -f ..\\binmake bootstrap=1 clean\n";
        print BATCH "%OWBINDIR%\\wmake -h -f ..\\binmake bootstrap=1 builder.exe rm.exe\n";
    }
}

sub make_boot_batch
{
    open(BATCH, ">$build_boot_batch_name") || die "Unable to open $build_boot_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i)  { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i)  { ; }
        elsif (/$setenv INCLUDE=/i) { ; }
        elsif (/$setenv PATH=/i)    { ; }
        else                        { print BATCH; }
    }
    close(INPUT);
    batch_output_set_boot_env();
    print BATCH "$setenv OWRELROOT=", $relroot, "\n";
    # Add additional commands to do the build.
    print BATCH "$setenv OWDOCBUILD=0\n";
    print BATCH "$setenv OWDOCQUIET=1\n";
    # Create fresh builder tools, to prevent lockup build server
    # if builder tools from previous build are somehow broken
    batch_output_build_wmake_builder_rm();
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "builder -i bootclean\n";
    batch_output_build_wmake_builder_rm();
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "builder boot\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_boot_batch_name;
}

sub make_build_batch
{
    open(BATCH, ">$build_batch_name") || die "Unable to open $build_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i)   { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i)   { ; }
        elsif (/$setenv INCLUDE=/i)  { ; }
        elsif (/$setenv PATH=/i)     { ; }
        elsif (/$setenv OWDOSBOX=/i) { print BATCH "$setenv OWDOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        else                         { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot, "\n";
    # Add additional commands to do the build.
    print BATCH "$setenv OWDOCBUILD=0\n";
    print BATCH "$setenv OWDOCQUIET=1\n";
    # start building by bootstrap tools.
    # Remove release directory tree.
    print BATCH "rm -rf ", $relroot, "\n";
    # Clean previous build.
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "builder -i clean\n";
    # Start build process.
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "builder -i pass\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_batch_name;
}

sub make_docs_batch
{
    open(BATCH, ">$docs_batch_name") || die "Unable to open $docs_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i)            { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i)            { ; }
        elsif (/$setenv INCLUDE=/i)           { ; }
        elsif (/$setenv PATH=/i)              { ; }
        elsif (/$setenv OWDOSBOX=/i)          { print BATCH "$setenv OWDOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        elsif (/$setenv OWGHOSTSCRIPTPATH=/i) { ; }
        elsif (/$setenv OWWIN95HC=/i)         { ; }
        elsif (/$setenv OWHHC=/i)             { ; }
        else                                  { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot, "\n";
    # Add additional commands to do the build.
    if ($Common::config{"GHOSTSCRIPTPATH"} ne "") {
        print BATCH "$setenv OWGHOSTSCRIPTPATH=", $Common::config{"GHOSTSCRIPTPATH"}, "\n";
    }
    if ($Common::config{"WIN95HC"} ne "") {
        print BATCH "$setenv OWWIN95HC=", $Common::config{"WIN95HC"}, "\n";
    }
    if ($Common::config{"HHC"} ne "") {
        print BATCH "$setenv OWHHC=", $Common::config{"HHC"}, "\n";
    }
    print BATCH "$setenv OWDOCQUIET=1\n";
    # Start build process.
    print BATCH "cd $OW\ncd docs\n";
    print BATCH "builder -i docsclean\n";
    print BATCH "builder -i docs\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $docs_batch_name;
}

sub make_test_batch
{
    open(BATCH, ">$test_batch_name") || die "Unable to open $test_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i)   { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i)   { ; }
        elsif (/$setenv INCLUDE=/i)  { ; }
        elsif (/$setenv PATH=/i)     { ; }
        elsif (/$setenv OWDOSBOX=/i) { print BATCH "$setenv OWDOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        else                         { print BATCH; }
    }
    close(INPUT);
    batch_output_set_test_env();
    # Add additional commands to do the testing.
    print BATCH "\n";
    if ($^O eq "MSWin32") { 
        if ($Config{archname} =~ /64/) {
#            print BATCH "if not '%OWDOSBOX%' == '' $setenv EXTRA_ARCH=i86\n\n";
        } else {
            print BATCH "$setenv EXTRA_ARCH=i86\n\n";
        }
    }
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "rm -f ctest/*.log\n";
    print BATCH "rm -f wasmtest/*.log\n";
    print BATCH "rm -f f77test/*.log\n";
    print BATCH "rm -f plustest/*.log\n";
    print BATCH "builder -i test\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $test_batch_name;
}

sub make_installer_batch
{
    open(BATCH, ">$build_installer_name") || die "Unable to open $build_installer_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i)  { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i)  { ; }
        elsif (/$setenv INCLUDE=/i) { ; }
        elsif (/$setenv PATH=/i)    { ; }
        else                        { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot, "\n";
    # Add additional commands to do installers.
    print BATCH "cd $OW\ncd distrib\ncd ow\n";
    print BATCH "builder missing\n";
    print BATCH "builder build\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_installer_name;
}

sub process_log
{
    my($os2_result)    = "success";
    my($result)        = "success";
    my($project_name)  = "none";
    my($first_message) = "yes";
    my($arch_test)     = "";
    my(@fields);

    open(LOGFILE, $_[0]) || die "Can't open $_[0]";
    while (<LOGFILE>) {
        s/\r?\n/\n/;
        if (/^[=]+ .* [=]+$/) {     # new project start
            if ($project_name ne "none") {
                if ($first_message eq "yes") {
                    print REPORT "Failed!\n";
                    $result = "fail";
                    $first_message = "no";
                }
                if ($arch_test ne "") {
                    print REPORT "\t\t$project_name\t$arch_test\n";
                    $result = "fail";
                }
            }                   
            @fields = split;
            $project_name = Common::remove_OWloc($fields[2]);
            $arch_test = "";
        } elsif (/^TEST/) {
            @fields = split;
            $arch_test = $fields[1];
        } elsif (/^PASS/) {
            $project_name = "none";
        }
    }
    close(LOGFILE);

    # Handle the case where the failed test is the last one.
    if ($project_name ne "none") {
      if ($arch_test ne "") {
        if ($first_message eq "yes") {
            print REPORT "Failed!\n";
            $first_message = "no";
        }
        print REPORT "\t\t$project_name\t$arch_test\n";
        $result = "fail";
      } 
    }

    # This is what we want to see.
    if ($result eq "success") {
        print REPORT "Succeeded.\n";
    }
    return $result;
}

sub get_shortdate
{
    my(@now) = gmtime time;
    return sprintf "%04d-%02d", $now[5] + 1900, $now[4] + 1;
}

sub get_date
{
    my(@now) = gmtime time;
    return sprintf "%04d-%02d-%02d", $now[5] + 1900, $now[4] + 1, $now[3];
}

sub get_datetime
{
    my(@now) = gmtime time;
    return sprintf "%04d-%02d-%02d, %02d:%02d UTC",
        $now[5] + 1900, $now[4] + 1, $now[3], $now[2], $now[1];
}

sub display_CVS_messages
{
    my($message);
    my($cvs_cmd) = $_[0];

    print REPORT $cvs_cmd, " Messages\n";
    print REPORT "-----------\n\n";

    foreach $message (@CVS_messages) {
        print REPORT "$message\n";
    }   
    print REPORT $cvs_cmd, " Messages end\n";
}

sub run_tests
{
    my($aresult) = "fail";
    my($cresult) = "fail";
    my($fresult) = "fail";
    my($presult) = "fail";

    # Run regression tests for the Fortran, C, C++ compilers and WASM.

    print REPORT "REGRESSION TESTS STARTED   : ", get_datetime(), "\n";
    system("$test_batch_name");
    print REPORT "REGRESSION TESTS COMPLETED : ", get_datetime(), "\n\n";

    print REPORT "\tFortran Compiler: ";
    $fresult = process_log("$OW\/bld\/f77test\/result.log");
    print REPORT "\tC Compiler      : ";
    $cresult = process_log("$OW\/bld\/ctest\/result.log");
    print REPORT "\tC++ Compiler    : ";
    $presult = process_log("$OW\/bld\/plustest\/result.log");
    print REPORT "\tWASM            : ";
    $aresult = process_log("$OW\/bld\/wasmtest\/result.log");
    print REPORT "\n";

    if ($aresult eq "success" && $cresult eq "success" && $fresult eq "success" && $presult eq "success") {
        return "success";
    } else {
        return "fail";
    }
}

sub run_boot_build
{
    print REPORT "CLEAN+BOOTSTRAP STARTED    : ", get_datetime(), "\n";
    if (system($build_boot_batch_name) == 0) {
        print REPORT "CLEAN+BOOTSTRAP COMPLETED  : ", get_datetime(), "\n\n";
        return "success";
    } else {
        print REPORT "clean+bootstrap failed!\n\n";
        return "fail";
    }
}

sub run_build
{
    my $logfile = "$OW\/bld\/pass.log";
    my $bldbase = "$home\/$Common::config{'BLDBASE'}";
    my $bldlast = "$home\/$Common::config{'BLDLAST'}";

    print REPORT "CLEAN+BUILD STARTED        : ", get_datetime(), "\n";
    if (system($build_batch_name) == 0) {
        print REPORT "CLEAN+BUILD COMPLETED      : ", get_datetime(), "\n\n";

        # Analyze build result.

        Common::process_summary($logfile, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return "fail";
        } else {

            # Run regression tests

            return run_tests();
        }
    } else {
        print REPORT "clean+build failed!\n\n";
        return "fail";
    }
}

sub run_docs_build
{
    my $logfile = "$OW\/docs\/doc.log";
    my $bldbase = "$home\/$Common::config{'BLDBASED'}";
    my $bldlast = "$home\/$Common::config{'BLDLASTD'}";

    print REPORT "CLEAN+BUILD STARTED        : ", get_datetime(), "\n";
    if (system($docs_batch_name) != 0) {
        print REPORT "clean+build failed!\n\n";
        return "fail";
    } else {
        print REPORT "CLEAN+BUILD COMPLETED      : ", get_datetime(), "\n\n";
        # Analyze build result.
        Common::process_summary($logfile, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return "fail";
        } else {
            return "success";
        }
    }
}

sub CVS_sync
{
    my($cvs_cmd) = $_[0];

    if ($cvs_cmd eq "git") {
#        system("git --git-dir=$OW/.git --work-tree=$OW checkout master");
        open(SYNC, "git --git-dir=$OW/.git --work-tree=$OW pull --no-rebase --ff-only |");
        while (<SYNC>) {
            push(@CVS_messages, sprintf("%s", $_));
        }
        if (!close(SYNC)) {
            print REPORT "Git failed!\n";
            return "fail";
        }
    } elsif ($cvs_cmd eq "p4") {
        open(SYNC, "p4 sync |");           # this does...
        while (<SYNC>) {
            my @fields = split;
            my $loc = Common::remove_OWloc($fields[-1]);
            if( $loc ne "" ) {
                push(@CVS_messages, sprintf("%-8s %s", $fields[2], $loc));
            } else {
                push(@CVS_messages, sprintf("%s", $_));
            }
        }
        if (!close(SYNC)) {
            print REPORT "p4 failed!\n";
            return "fail";
        }
    }
    return "success";
}

sub CVS_check_sync
{
    my($cvs_cmd) = $_[0];

    if (CVS_sync($cvs_cmd) eq "fail") {
        display_CVS_messages($cvs_cmd);
        return "fail";
    }
    get_prev_changeno;
    
    if ($prev_report_stamp ne "") {
        print REPORT "\tBuilt through change   : $prev_changeno on $prev_report_stamp\n";
    } else {
        $prev_changeno = "";
    }
    if ($cvs_cmd eq "git") {
        open(LEVEL, "git --git-dir=$OW/.git rev-parse HEAD|");
        while (<LEVEL>) {
            if (/^(.*)/) {
                if ($prev_changeno eq $1) {
                    $build_needed = 0;
                    print REPORT "\tNo source code changes, build not needed\n";
                } else {
                    $prev_changeno = $1;
                    print REPORT "\tBuilding through change: $1\n";
                }
            }
        }
        close(LEVEL);
    } elsif ($cvs_cmd eq "p4") {
        open(LEVEL, "p4 counters|");
        while (<LEVEL>) {
            if (/^change = (.*)/) {
                if ($prev_changeno eq $1) {
                    $build_needed = 0;
                    print REPORT "\tNo source code changes, build not needed\n";
                } else {
                    $prev_changeno = $1;
                    print REPORT "\tBuilding through change: $1\n";
                }
            }
        }
        close(LEVEL);
    }
    print REPORT "\n";
    if (!$build_needed) { # nothing changed, don't waste computer time
        return "nochange";
    }
    return "success";
}

#######################
#      Main Script
#######################

# This test should be enhanced to deal properly with subfolders, etc.
if ($home eq $OW) {
    print "Error! The build system home folder can not be under $OW\n";
    exit 1;
}

my $shortdate_stamp = get_shortdate();
my $date_stamp = get_date();
my $report_directory = "$Common::config{'REPORTS'}\/$shortdate_stamp";
if (!stat($report_directory)) {
    mkdir($report_directory);
}
my $report_name = "$report_directory\/$date_stamp-report-$build_platform.txt";
my $bak_name    = "$report_directory\/$date_stamp-report-$build_platform.txt.bak";
if (stat($report_name)) {
    rename $report_name, $bak_name;
}
open(REPORT, ">$report_name") || die "Unable to open $report_name file.";
print REPORT "Open Watcom Build Report (build on ", $build_platform, ")\n";
print REPORT "================================================\n\n";

# create all batch files
########################

make_boot_batch();
make_build_batch();
make_test_batch();
make_docs_batch();
make_installer_batch();

# Do a CVS sync to get the latest changes.
##########################################

if ($Common::config{'OWCVS'} ne "") {
    $CVS_result = CVS_check_sync($Common::config{'OWCVS'});
} else {
    $CVS_result = "success";
}
if ($CVS_result eq "fail") {
    close(REPORT);
    exit 2;
}
if ($CVS_result eq "nochange") {
    close(REPORT);
    exit 0;
}

############################################################
#
#  pass 1  Build and test full Open Watcom
#
############################################################

print REPORT "\n";
print REPORT "Compilers and Tools\n";
print REPORT "===================\n\n";

$boot_result = run_boot_build();

if ($boot_result eq "success") {

    $pass_result = run_build();

############################################################
#
#  Build the Documentation
#
############################################################

    print REPORT "\n";
    print REPORT "Documentation Build\n";
    print REPORT "===================\n\n";

    if (defined($ENV{"OWDOCSKIP"}) && ($ENV{"OWDOCSKIP"} eq "1")) {
        $docs_result = "success";
        print REPORT "Build skipped.\n\n";
    } else {
        $docs_result = run_docs_build();
    }
}

# Run installers build and post batch file
################################################################
if (($boot_result eq "success") &&
    ($pass_result eq "success") &&
    ($docs_result eq "success")) {
    my $installers_result = "fail";

    print REPORT "\n";
    print REPORT "Installers build\n";
    print REPORT "================\n";
    print REPORT "\n";
    print REPORT "INSTALLER BUILD STARTED    : ", get_datetime(), "\n";
    if (system($build_installer_name) != 0) {
        print REPORT "INSTALLER BUILD FAILED!\n";
    } else {
        print REPORT "INSTALLER BUILD COMPLETED  : ", get_datetime(), "\n";
        $installers_result = "success";
    }
    print REPORT "\n";
    print REPORT "Post tasks\n";
    print REPORT "================\n";
    print REPORT "\n";
    if ($installers_result eq "fail") {
        print REPORT "POST RUN skipped.\n";
    } elsif (system($post_batch_name) != 0) {
	print REPORT "POST RUN FAILED!\n";
    } else {
	print REPORT "POST RUN COMPLETED  : ", get_datetime(), "\n";
    }
    print REPORT "\n\n";
    if ($Common::config{'OWCVS'} ne "") {
        set_prev_changeno( $prev_changeno, $date_stamp );  #remember changeno and date
    }
}

# Output CVS sync messages for reference.
##########################################

if ($Common::config{'OWCVS'} ne "") {
    display_CVS_messages($Common::config{'OWCVS'});
}

close(REPORT);
