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

my(@p4_messages);
my($OStype);
my($ext);
my($setenv);
my($WATCOM);
my($relsubdir);
my($buildlog);
my($bldbase);
my($bldlast);
my($build_platform);

if ($#ARGV == -1) {
    Common::read_config( "config.txt" );
} elsif ($#ARGV == 0) {
    Common::read_config( $ARGV[0] );
} else {
    print "Usage: dobuild [config_file]\n";
    exit 1;
}

my $home           = $Common::config{"HOME"};
my $OW             = $Common::config{"OW"};

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

my $build_batch_name     = "$home\/build.$ext";
my $docs_batch_name      = "$home\/docsbld.$ext";
my $build_installer_name = "$home\/instbld.$ext";
my $test_batch_name      = "$home\/test.$ext";
my $rotate_batch_name    = "$home\/rotate.$ext";
my $setvars              = "$OW\/setvars.$ext";
my $prev_changeno_name   = "$home\/changeno.txt";
my $prev_changeno        = "0";
my $prev_report_stamp    = "";
my $build_needed         = 1;

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
}

sub set_prev_changeno
{
    open(CHNGNO, ">$prev_changeno_name") || return;
    print CHNGNO $_[0], " ", $_[1];
    close(CHNGNO);
}

sub get_reldir
{
    if ($OStype eq "UNIX") {
        return "$OW\/$relsubdir";
    } else {
        return "$OW\\$relsubdir";
    }
}

sub make_build_batch
{
    my($pass1) = ($WATCOM eq $Common::config{"WATCOM"});

    open(BATCH, ">$build_batch_name") || die "Unable to open $build_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n/\n/;
        if    (/$setenv OWROOT=/i) { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i) { print BATCH "$setenv WATCOM=", $WATCOM, "\n"; }
        elsif (/$setenv OW_DOSBOX=/i) { print BATCH "$setenv OW_DOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        else                      { print BATCH; }
    }
    close(INPUT);
    # Add additional commands to do the build.
    print BATCH "$setenv RELROOT=", get_reldir(), "\n";
    print BATCH "$setenv DOC_BUILD=0\n";
    print BATCH "$setenv DOC_QUIET=1\n";
    # Create fresh builder tools, to prevent lockup build server 
    # if builder tools from previous build are somehow broken
    print BATCH "cd $OW\ncd bld\n";
    if ($^O eq "MSWin32") {
        print BATCH "cd builder\ncd nt386\n";
    } elsif ($^O eq "linux") {
        print BATCH "cd builder\ncd linux386\n";
    } elsif ($^O eq "os2") {
        print BATCH "cd builder\ncd os2386\n";
    }
    print BATCH "wmake -h clean\n";
    print BATCH "wmake -h\n";
    # Remove release directory.
    print BATCH "rm -rf ", get_reldir(), "\n";
    # Clean previous build.
    print BATCH "cd $OW\ncd bld\n";
    print BATCH "builder -i clean\n";
    # Create new builder tools, previous clean removed them.
    print BATCH "cd $OW\ncd bld\n";
    if ($^O eq "MSWin32") {
        print BATCH "cd builder\ncd nt386\n";
    } elsif ($^O eq "linux") {
        print BATCH "cd builder\ncd linux386\n";
    } elsif ($^O eq "os2") {
        print BATCH "cd builder\ncd os2386\n";
    }
    print BATCH "wmake -h\n";
    # Start build process.
    print BATCH "cd $OW\ncd bld\n";
    if ($pass1) {
        print BATCH "builder -i pass1\n";
    } else {
        print BATCH "builder -i pass2\n";
    }
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
        if    (/$setenv OWROOT=/i) { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i) { print BATCH "$setenv WATCOM=", $WATCOM, "\n"; }
        elsif (/$setenv OW_DOSBOX=/i) { print BATCH "$setenv OW_DOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        elsif (/$setenv GHOSTSCRIPTPATH/i) { ; }
        elsif (/$setenv WIN95HC/i) { ; }
        elsif (/$setenv HHC/i)    { ; }
        else                      { print BATCH; }
    }
    close(INPUT);
    # Add additional commands to do the build.
    print BATCH "$setenv RELROOT=", get_reldir(), "\n";
    if ($Common::config{"GHOSTSCRIPTPATH"} ne "") {
        print BATCH "$setenv GHOSTSCRIPTPATH=", $Common::config{"GHOSTSCRIPTPATH"}, "\n";
    }
    if ($Common::config{"WIN95HC"} ne "") {
        print BATCH "$setenv WIN95HC=", $Common::config{"WIN95HC"}, "\n";
    }
    if ($Common::config{"HHC"} ne "") {
        print BATCH "$setenv HHC=", $Common::config{"HHC"}, "\n";
    }
    print BATCH "$setenv DOC_QUIET=1\n";
    # Start build process.
    print BATCH "cd $OW\ncd docs\n";
    print BATCH "builder -i clean\n";
    print BATCH "builder -i pass1\n";
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
        if    (/$setenv OWROOT=/i) { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM=/i) { print BATCH "$setenv WATCOM=", get_reldir(), "\n"; }
        elsif (/$setenv OW_DOSBOX=/i) { print BATCH "$setenv OW_DOSBOX=", $Common::config{"DOSBOX"}, "\n"; }
        else                      { print BATCH; }
    }
    close(INPUT);

    # Add additional commands to do the testing.
    print BATCH "\n";
    if ($^O eq "MSWin32") { 
        print BATCH "if '%OW_DOSBOX%' == '' $setenv EXTRA_ARCH=i86\n\n";
    }
    print BATCH "cd $OW\ncd bld\ncd ctest\n";
    print BATCH "rm *.log\n";
    print BATCH "wmake -h targ_env_386=cw386\n";
    print BATCH "cd $OW\ncd bld\ncd wasmtest\n";
    print BATCH "rm *.log\n";
    print BATCH "wmake -h targ_env_386=cw386\n";
    print BATCH "cd $OW\ncd bld\ncd f77\ncd regress\n";
    print BATCH "rm *.log\n";
    print BATCH "wmake -h targ_env_386=cw386\n";
    print BATCH "cd $OW\ncd bld\ncd plustest\n";
    print BATCH "rm *.log\n";
    print BATCH "wmake -h targ_env_386=cw386\n";
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
        if    (/$setenv OWROOT/i) { print BATCH "$setenv OWROOT=", $OW, "\n"; }
        elsif (/$setenv WATCOM/i) { print BATCH "$setenv WATCOM=", $Common::config{"WATCOM"}, "\n"; }
        else                      { print BATCH; }
    }
    close(INPUT);
    # Add additional commands to do installers.
    print BATCH "$setenv RELROOT=", get_reldir(), "\n";
    if ($OStype eq "UNIX") {
        # set up max open file handle to be enough for uzip
        print BATCH "ulimit -n 4096\n";
    }
    print BATCH "cd $OW\ncd distrib\ncd ow\n";
    print BATCH "builder missing\n";
    print BATCH "builder rel2\n";
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_installer_name;
}

sub process_log
{
    my($ehmix)         = "BLD\\PLUSTEST\\REGRESS\\EHMIX";
    my($os2_result)    = "success";
    my($result)        = "success";
    my($project_name)  = "none";
    my($first_message) = "yes";
    my($arch_test)     = "";
    my(@fields);

    open(LOGFILE, $_[0]) || die "Can't open $_[0]";
    while (<LOGFILE>) {
        s/\r?\n/\n/;
        if (/^[=]+ .* [=]+$/) {        # new project directory
            if ($project_name ne "none") {     # previous not PASSed
                if ($first_message eq "yes") {
                    if (($project_name ne $ehmix) or ($OStype ne "OS2")) {
                        print REPORT "Failed!\n";
##                      print REPORT "Failed1! $project_name  $ehmix $OStype\n";
                        $result = "fail";
                    }
                    if ( ($OStype eq "OS2") and ($project_name eq $ehmix) ) {
                        print REPORT "Failed, but EHMIX errors ignored on OS/2!\n";
                        $os2_result = "fail";
                    }
                    $first_message = "no";
                }
                if ($arch_test ne "") {
                    print REPORT "\t\t$project_name\t$arch_test\n";
                    if (($project_name ne $ehmix) or ($OStype ne "OS2")) {
##                      print REPORT "Failed2! $project_name  $ehmix $OStype\n";
                        $result = "fail";      # in case others failed on os2
                    }
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
      if ($arch_test ne "") {          # no TEST seen ignore project
        if ($first_message eq "yes") {
            print REPORT "Failed!\n";
            $first_message = "no";
        }
        print REPORT "\t\t$project_name\t$arch_test\n";
        $result = "fail";
##      print REPORT "$project_name last one failed\n";# test ++++++++++++
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

sub display_p4_messages
{
    my($message);

    print REPORT "p4 Messages\n";
    print REPORT "-----------\n\n";

    foreach $message (@p4_messages) {
        print REPORT "$message\n";
    }   
    print REPORT "p4 Messages end\n";
}

sub run_tests
{
    my($result) = "success";

    # Run regression tests for the Fortran, C, C++ compilers and WASM.

    make_test_batch();
    print REPORT "REGRESSION TESTS STARTED  : ", get_datetime(), "\n";
    system("$test_batch_name");
    print REPORT "REGRESSION TESTS COMPLETED: ", get_datetime(), "\n\n";

    print REPORT "\tFortran Compiler: ";
    if (process_log("$OW\/bld\/f77\/regress\/result.log") ne "success") { $result = "fail"; }
    print REPORT "\tC Compiler      : ";
    if (process_log("$OW\/bld\/ctest\/result.log") ne "success") { $result = "fail"; }
    print REPORT "\tC++ Compiler    : ";
    if (process_log("$OW\/bld\/plustest\/result.log") ne "success") { $result = "fail"; }
    print REPORT "\tWASM            : ";
    if (process_log("$OW\/bld\/wasmtest\/result.log") ne "success") { $result = "fail"; }
    print REPORT "\n";

    return $result;
}

sub run_build
{
    make_build_batch();
    print REPORT "CLEAN+BUILD STARTED  : ", get_datetime(), "\n";
    if (system($build_batch_name) != 0) {
        print REPORT "clean+build failed!\n";
        return "fail";
    } else {
        print REPORT "CLEAN+BUILD COMPLETED: ", get_datetime(), "\n\n";

        # Analyze build result.

        Common::process_summary($buildlog, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return "fail";
        } else {

            # Run regression tests

            $WATCOM = get_reldir();
            return run_tests();
        }
    }
}

sub run_docs_build
{
    make_docs_batch();
    print REPORT "CLEAN+BUILD STARTED  : ", get_datetime(), "\n";
    if (system($docs_batch_name) != 0) {
        print REPORT "clean+build failed!\n\n";
        return "fail";
    } else {
        print REPORT "CLEAN+BUILD COMPLETED: ", get_datetime(), "\n\n";
        # Analyze build result.
        Common::process_summary($buildlog, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return "fail";
        } else {
            return "success";
        }
    }
}

sub p4_sync
{
    #force all files update to head
    #open(SYNC, "p4 sync -f $OW\/...#head |");
    
    #open(SYNC, "p4 sync $OW\/... |"); does'nt work on OS/2 old client?
    open(SYNC, "p4 sync |");           # this does...
    while (<SYNC>) {
        my @fields = split;
        my $loc = Common::remove_OWloc($fields[-1]);
        if( $loc ne "" ) {
            push(@p4_messages, sprintf("%-8s %s", $fields[2], $loc));
        } else {
            push(@p4_messages, sprintf("%s", $_));
        }
    }
    if (!close(SYNC)) {
        print REPORT "p4 failed!\n";
        close(REPORT);
        exit 1;
    }
    print REPORT "'p4 sync' Successful (messages below).\n";

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

# Do a p4 sync to get the latest changes.
#########################################

if ($Common::config{'OWCVS'} eq "p4") {
    p4_sync();

    get_prev_changeno;
    
    if ($prev_changeno > 0) {
       print REPORT "\tBuilt through change   : $prev_changeno on $prev_report_stamp\n";
    } else {
       $prev_changeno = -1; # no previous changeno / build
    }
    
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
    print REPORT "\n";
    if (!$build_needed) { # nothing changed, don't waste computer time
        close(REPORT);
        exit 0;
    }
}

############################################################
#
#  pass 1  Build and test full Open Watcom
#
############################################################

print REPORT "\n";
print REPORT "Compilers and Tools (pass 1)\n";
print REPORT "============================\n\n";

$WATCOM    = $Common::config{"WATCOM"};
$relsubdir = "pass1";
$buildlog  = "$OW\/bld\/pass1.log";
$bldbase   = "$home\/$Common::config{'BLDBASE1'}";
$bldlast   = "$home\/$Common::config{'BLDLAST1'}";

my $pass1_result = run_build();

############################################################
#
#  pass 2  Build and test Compilers and Tools only
#          it uses OW pass1 version
#
############################################################

print REPORT "\n";
print REPORT "Compilers and Tools (pass 2)\n";
print REPORT "============================\n\n";

$WATCOM    = get_reldir();
$relsubdir = "pass2";
$buildlog  = "$OW\/bld\/pass2.log";
$bldbase   = "$home\/$Common::config{'BLDBASE2'}";
$bldlast   = "$home\/$Common::config{'BLDLAST2'}";

my $pass2_result = run_build();

############################################################
#
#  Build the Documentation
#
############################################################

print REPORT "\n";
print REPORT "Documentation Build\n";
print REPORT "===================\n\n";

$WATCOM    = $Common::config{"WATCOM"};
$relsubdir = "pass1";
$buildlog  = "$OW\/docs\/pass1.log";
$bldbase   = "$home\/$Common::config{'BLDBASED'}";
$bldlast   = "$home\/$Common::config{'BLDLASTD'}";

my $docs_result = run_docs_build();

# Display p4 sync messages for reference.
##########################################

if ($Common::config{'OWCVS'} eq "p4") {
    display_p4_messages();
}

set_prev_changeno( $prev_changeno, $date_stamp );  #remember changeno and date

# Rotate the freshly built system into position on the web site.
################################################################
if (($pass1_result eq "success") &&
    ($pass2_result eq "success") &&
    ($docs_result eq "success")) {

    $relsubdir = "pass1";
    print REPORT "\nINSTALLER BUILD STARTED  : ", get_datetime(), "\n";
    make_installer_batch();
    if (system($build_installer_name) != 0) {
        print REPORT "INSTALLER BUILD FAILED!\n";
    } else {
        system("$rotate_batch_name");
        print REPORT "INSTALLER BUILD COMPLETED: ", get_datetime(), "\n\n";
    }
}
