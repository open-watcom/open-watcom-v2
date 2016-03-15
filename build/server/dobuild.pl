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
use File::Path qw( make_path );

$\ = "\n";

my(@CVS_messages);
my($OStype);
my($ext);
my($setenv);
my($build_platform);

if ($#ARGV == -1) {
    Common::read_config( 'config.txt' );
} elsif ($#ARGV == 0) {
    Common::read_config( $ARGV[0] );
} else {
    print 'Usage: dobuild [config_file]';
    exit 1;
}

my $home      = $Common::config{'HOME'};
my $OW        = $Common::config{'OW'};
my $TOOLS     = $Common::config{'TOOLS'};
my $relroot   = $Common::config{'RELROOT'};

if ($^O eq 'MSWin32') {
    $OStype = 'WIN32';
    $ext    = 'bat';
    $setenv = 'set';
    if ($Config{archname} =~ /x64/) {
        $build_platform = 'win32-x64';
    } else {
        $build_platform = 'win32-x86';
    }
} elsif ($^O eq 'linux') {
    $OStype = 'UNIX';
    $ext    = 'sh';
    $setenv = 'export';
    if ($Config{archname} =~ /x86_64/) {
        $build_platform = 'linux-x64';
    } else {
        $build_platform = 'linux-x86';
    }
} elsif ($^O eq 'os2') {
    $OStype = 'OS2';
    $ext    = 'cmd';
    $setenv = 'set';
    $build_platform = 'os2-x86';
} else {
    print "Unsupported or unknown platform '$^O' !";
    print 'Review dobuild.pl file and fix it for new platform!';
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
my $prev_changeno         = '0';
my $prev_report_stamp     = '';
my $build_needed          = 1;
my $boot_result           = 'fail';
my $pass_result           = 'fail';
my $docs_result           = 'fail';
my $CVS_result            = 'fail';

sub get_prev_changeno
{
    my @fields;
    open(CHNGNO, "$prev_changeno_name") || return;
    while (<CHNGNO>) {
        s/\r?\n//;
        @fields = split;
        $prev_changeno = $fields[0];
        $prev_report_stamp  = $fields[1];
    }
    close(CHNGNO);
    if (!defined($prev_report_stamp)) {
        $prev_report_stamp = '';
    }
}

sub set_prev_changeno
{
    open(CHNGNO, ">$prev_changeno_name") || return;
    print CHNGNO $_[0], ' ', $_[1];
    close(CHNGNO);
}

sub batch_output_make_change_objdir
{
    if ($OStype eq 'UNIX') {
        print BATCH 'if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi';
        print BATCH 'cd $OWOBJDIR';
    } else {
        print BATCH 'if not exist %OWOBJDIR% mkdir %OWOBJDIR%';
        print BATCH 'cd %OWOBJDIR%';
    }
}

sub batch_output_set_test_env
{
    print BATCH "$setenv WATCOM=", $relroot;
    if ($^O eq 'MSWin32') {
        print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\nt";
        if ($Config{archname} =~ /x64/) {
            print BATCH "$setenv PATH=%WATCOM%\\binnt64;%WATCOM%\\binnt;%WATCOM%\\binw;%PATH%";
        } else {
            print BATCH "$setenv PATH=%WATCOM%\\binnt;%WATCOM%\\binw;%PATH%";
        }
    } elsif ($^O eq 'os2') {
        print BATCH "$setenv INCLUDE=%WATCOM%\\h;%WATCOM%\\h\\os2";
        print BATCH "$setenv PATH=%WATCOM%\\binp;%WATCOM%\\binw;%PATH%";
        print BATCH "$setenv BEGINLIBPATH=%WATCOM%\\binp\\dll;%BEGINLIBPATH%";
    } elsif ($^O eq 'linux') {
        print BATCH "$setenv INCLUDE=\$WATCOM/lh";
        if ($Config{archname} =~ /x86_64/) {
            print BATCH "$setenv PATH=\$WATCOM/binl64:\$WATCOM/binl:\$PATH";
        } else {
            print BATCH "$setenv PATH=\$WATCOM/binl:\$PATH";
        }
    }
}

sub batch_output_reset_env
{
    if ($OStype eq 'UNIX') {
        print BATCH ". \$OWROOT/cmnvars.$ext";
    } else {
        print BATCH "call %OWROOT%\\cmnvars.$ext";
    }
}

sub batch_output_build_wmake_builder_rm
{
    # Create fresh builder tools to prevent lockup build server 
    # if builder tools from previous build are somehow broken
    #
    # Create new wmake tool, previous clean removed it.
    print BATCH "cd $OW"; print BATCH 'cd bld'; print BATCH 'cd wmake';
    batch_output_make_change_objdir();
    if ($OStype eq 'UNIX') {
        print BATCH 'rm -f $OWBINDIR/wmake';
        if ($TOOLS eq 'WATCOM') {
            print BATCH 'wmake -h -f ../wmake clean';
            print BATCH 'wmake -h -f ../wmake';
        } else {
            print BATCH 'make -f ../posmake clean';
            print BATCH 'make -f ../posmake';
        }
    } else {
        print BATCH 'if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe';
        if ($TOOLS eq 'WATCOM') {
            print BATCH 'wmake -h -f ..\wmake clean';
            print BATCH 'wmake -h -f ..\wmake';
        } else {
            print BATCH 'nmake -nologo -f ..\nmake clean';
            print BATCH 'nmake -nologo -f ..\nmake';
        }
    }
    # Create new builder tool, previous clean removed it.
    print BATCH "cd $OW"; print BATCH 'cd bld'; print BATCH 'cd builder';
    batch_output_make_change_objdir();
    if ($OStype eq 'UNIX') {
        print BATCH 'rm -f $OWBINDIR/builder';
        print BATCH '$OWBINDIR/wmake -h -f ../binmake bootstrap=1 clean';
        print BATCH '$OWBINDIR/wmake -h -f ../binmake bootstrap=1 builder.exe';
    } else {
        print BATCH 'if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe';
        print BATCH '%OWBINDIR%\wmake -h -f ..\binmake bootstrap=1 clean';
        print BATCH '%OWBINDIR%\wmake -h -f ..\binmake bootstrap=1 builder.exe rm.exe';
    }
}

sub make_boot_batch
{
    open(BATCH, ">$build_boot_batch_name") || die "Unable to open $build_boot_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n//;
        if    (/$setenv OWROOT=/i)  { print BATCH "$setenv OWROOT=", $OW; }
        elsif (/$setenv OWTOOLS=/i)  { print BATCH "$setenv OWTOOLS=", $TOOLS; }
        else                        { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot;
    # Add additional commands to do the build.
    print BATCH "$setenv OWDOCBUILD=0";
    print BATCH "$setenv OWDOCQUIET=1";
    # Create fresh builder tools, to prevent lockup build server
    # if builder tools from previous build are somehow broken
    batch_output_build_wmake_builder_rm();
    print BATCH "cd $OW"; print BATCH 'cd bld';
    print BATCH 'builder -i bootclean';
    batch_output_build_wmake_builder_rm();
    print BATCH "cd $OW"; print BATCH 'cd bld';
    print BATCH 'builder boot';
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_boot_batch_name;
}

sub make_build_batch
{
    open(BATCH, ">$build_batch_name") || die "Unable to open $build_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n//;
        if    (/$setenv OWROOT=/i)   { print BATCH "$setenv OWROOT=", $OW; }
        elsif (/$setenv OWTOOLS=/i)  { print BATCH "$setenv OWTOOLS=", $TOOLS; }
        elsif (/$setenv OWDOSBOX=/i) { ; }
        else                         { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot;
    # Add additional commands to do the build.
    if (($Common::config{'DOSBOX'} || '') ne '') {
        print BATCH "$setenv OWDOSBOX=", $Common::config{'DOSBOX'};
    }
    print BATCH "$setenv OWDOCBUILD=0";
    print BATCH "$setenv OWDOCQUIET=1";
    # start building by bootstrap tools.
    # Remove release directory tree.
    print BATCH 'rm -rf ', $relroot;
    # Clean previous build.
    print BATCH "cd $OW"; print BATCH 'cd bld';
    print BATCH 'builder -i clean';
    # Start build process.
    print BATCH "cd $OW"; print BATCH 'cd bld';
    print BATCH 'builder -i pass';
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_batch_name;
}

sub make_docs_batch
{
    open(BATCH, ">$docs_batch_name") || die "Unable to open $docs_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n//;
        if    (/$setenv OWROOT=/i)            { print BATCH "$setenv OWROOT=", $OW; }
        elsif (/$setenv OWTOOLS=/i)            { print BATCH "$setenv OWTOOLS=", $TOOLS; }
        elsif (/$setenv OWDOSBOX=/i)          { ; }
        elsif (/$setenv OWGHOSTSCRIPTPATH=/i) { ; }
        elsif (/$setenv OWWIN95HC=/i)         { ; }
        elsif (/$setenv OWHHC=/i)             { ; }
        else                                  { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot;
    # Add additional commands to do the build.
    if (($Common::config{'DOSBOX'} || '') ne '') {
        print BATCH "$setenv OWDOSBOX=", $Common::config{'DOSBOX'};
    }
    if (($Common::config{'GHOSTSCRIPTPATH'} || '') ne '') {
        print BATCH "$setenv OWGHOSTSCRIPTPATH=", $Common::config{'GHOSTSCRIPTPATH'};
    }
    if (($Common::config{'WIN95HC'} || '') ne '') {
        print BATCH "$setenv OWWIN95HC=", $Common::config{'WIN95HC'};
    }
    if (($Common::config{'HHC'} || '') ne '') {
        print BATCH "$setenv OWHHC=", $Common::config{'HHC'};
    }
    print BATCH "$setenv OWDOCQUIET=1";
    # Start build process.
    print BATCH "cd $OW"; print BATCH 'cd docs';
    print BATCH 'builder -i docsclean';
    print BATCH 'builder -i docs';
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $docs_batch_name;
}

sub make_test_batch
{
    open(BATCH, ">$test_batch_name") || die "Unable to open $test_batch_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n//;
        if    (/$setenv OWROOT=/i)   { print BATCH "$setenv OWROOT=", $OW; }
        elsif (/$setenv OWTOOLS=/i)  { print BATCH "$setenv OWTOOLS=", $TOOLS; }
        elsif (/$setenv OWDOSBOX=/i) { ; }
        else                         { print BATCH; }
    }
    close(INPUT);
    batch_output_set_test_env();
    # Add additional commands to do the testing.
    if (($Common::config{'DOSBOX'} || '') ne '') {
        print BATCH "$setenv OWDOSBOX=", $Common::config{'DOSBOX'};
    }
    print BATCH '';
    if ($^O eq 'MSWin32') { 
        if ($Config{archname} =~ /x64/) {
#            print BATCH "if not '%OWDOSBOX%' == '' $setenv EXTRA_ARCH=i86";
        } else {
            print BATCH "$setenv EXTRA_ARCH=i86";
        }
    }
    print BATCH "cd $OW"; print BATCH 'cd bld';
    print BATCH 'rm -f ctest/*.log';
    print BATCH 'rm -f wasmtest/*.log';
    print BATCH 'rm -f f77test/*.log';
    print BATCH 'rm -f plustest/*.log';
    print BATCH 'rm -f clibtest/*.log';
    print BATCH 'builder -i test';
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $test_batch_name;
}

sub make_installer_batch
{
    open(BATCH, ">$build_installer_name") || die "Unable to open $build_installer_name file.";
    open(INPUT, "$setvars") || die "Unable to open $setvars file.";
    while (<INPUT>) {
        s/\r?\n//;
        if    (/$setenv OWROOT=/i)  { print BATCH "$setenv OWROOT=", $OW; }
        elsif (/$setenv OWTOOLS=/i)  { print BATCH "$setenv OWTOOLS=", $TOOLS; }
        else                        { print BATCH; }
    }
    close(INPUT);
    print BATCH "$setenv OWRELROOT=", $relroot;
    # Add additional commands to do installers.
    print BATCH "cd $OW"; print BATCH 'cd distrib'; print BATCH 'cd ow';
    print BATCH 'builder missing';
    print BATCH 'builder build';
    close(BATCH);
    # On Windows it has no efect
    chmod 0777, $build_installer_name;
}

sub process_log
{
    my($title)         = $_[0];
    my($os2_result)    = 'success';
    my($result)        = 'success';
    my($project_name)  = 'none';
    my($first_message) = 'yes';
    my($arch_test)     = '';
    my(@fields);

    open(LOGFILE, $_[1]) || die "Can't open $_[1]";
    while (<LOGFILE>) {
        s/\r?\n//;
        if (/^[=]+ .* [=]+$/) {     # new project start
            if ($project_name ne 'none') {
                if ($first_message eq 'yes') {
                    print REPORT "$title Failed!";
                    $result = 'fail';
                    $first_message = 'no';
                }
                if ($arch_test ne '') {
                    print REPORT "\t\t$project_name\t$arch_test";
                    $result = 'fail';
                }
            }                   
            @fields = split;
            $project_name = Common::remove_OWloc($fields[2]);
            $arch_test = '';
        } elsif (/^TEST/) {
            @fields = split;
            $arch_test = $fields[1];
        } elsif (/^PASS/) {
            $project_name = 'none';
        }
    }
    close(LOGFILE);

    # Handle the case where the failed test is the last one.
    if ($project_name ne 'none') {
      if ($arch_test ne '') {
        if ($first_message eq 'yes') {
            print REPORT "$title Failed!";
            $first_message = 'no';
        }
        print REPORT "\t\t$project_name\t$arch_test";
        $result = 'fail';
      } 
    }

    # This is what we want to see.
    if ($result eq 'success') {
        print REPORT "$title Succeeded.";
    }
    return $result;
}

sub get_shortdate
{
    my(@now) = gmtime time;
    return sprintf '%04d-%02d', $now[5] + 1900, $now[4] + 1;
}

sub get_date
{
    my(@now) = gmtime time;
    return sprintf '%04d-%02d-%02d', $now[5] + 1900, $now[4] + 1, $now[3];
}

sub get_datetime
{
    my(@now) = gmtime time;
    return sprintf '%04d-%02d-%02d, %02d:%02d UTC',
        $now[5] + 1900, $now[4] + 1, $now[3], $now[2], $now[1];
}

sub display_CVS_messages
{
    my($message);
    my($cvs_cmd) = $_[0];

    print REPORT $cvs_cmd, ' Messages';
    print REPORT '-----------';
    print REPORT '';

    foreach $message (@CVS_messages) {
        print REPORT "$message";
    }   
    print REPORT $cvs_cmd, ' Messages end';
}

sub run_tests
{
    my($aresult) = 'fail';
    my($cresult) = 'fail';
    my($fresult) = 'fail';
    my($presult) = 'fail';

    # Run regression tests for the Fortran, C, C++ compilers and WASM.

    print REPORT 'REGRESSION TESTS STARTED   : ', get_datetime();
    system("$test_batch_name");
    print REPORT 'REGRESSION TESTS COMPLETED : ', get_datetime();
    print REPORT '';

    $fresult = process_log("\tFortran Compiler :", "$OW\/bld\/f77test\/result.log");
    $cresult = process_log("\tC Compiler       :", "$OW\/bld\/ctest\/result.log");
    $presult = process_log("\tC++ Compiler     :", "$OW\/bld\/plustest\/result.log");
    $aresult = process_log("\tWASM             :", "$OW\/bld\/wasmtest\/result.log");
    $presult = process_log("\tC run-time libr. :", "$OW\/bld\/clibtest\/result.log");
    print REPORT '';

    if ($aresult eq 'success' && $cresult eq 'success' && $fresult eq 'success' && $presult eq 'success') {
        return 'success';
    } else {
        return 'fail';
    }
}

sub run_boot_build
{
    print REPORT 'CLEAN+BOOTSTRAP STARTED    : ', get_datetime();
    if (system($build_boot_batch_name) == 0) {
        print REPORT 'CLEAN+BOOTSTRAP COMPLETED  : ', get_datetime();
        print REPORT '';
        return 'success';
    } else {
        print REPORT 'clean+bootstrap failed!';
        print REPORT '';
        return 'fail';
    }
}

sub run_build
{
    my $logfile = "$OW\/bld\/pass.log";
    my $bldbase = "$home\/$Common::config{'BLDBASE'}";
    my $bldlast = "$home\/$Common::config{'BLDLAST'}";

    print REPORT 'CLEAN+BUILD STARTED        : ', get_datetime();
    if (system($build_batch_name) == 0) {
        print REPORT 'CLEAN+BUILD COMPLETED      : ', get_datetime();
        print REPORT '';

        # Analyze build result.

        Common::process_summary($logfile, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return 'fail';
        } else {

            # Run regression tests

            return run_tests();
        }
    } else {
        print REPORT 'clean+build failed!';
        print REPORT '';
        return 'fail';
    }
}

sub run_docs_build
{
    my $logfile = "$OW\/docs\/doc.log";
    my $bldbase = "$home\/$Common::config{'BLDBASED'}";
    my $bldlast = "$home\/$Common::config{'BLDLASTD'}";

    print REPORT 'CLEAN+BUILD STARTED        : ', get_datetime();
    if (system($docs_batch_name) != 0) {
        print REPORT 'clean+build failed!';
        print REPORT '';
        return 'fail';
    } else {
        print REPORT 'CLEAN+BUILD COMPLETED      : ', get_datetime();
        print REPORT '';
        # Analyze build result.
        Common::process_summary($logfile, $bldlast);
        # If 'compare' fails, end now. Don't test if there was a build failure.
        if (Common::process_compare($bldbase, $bldlast, \*REPORT)) {
            return 'fail';
        } else {
            return 'success';
        }
    }
}

sub CVS_sync
{
    my($cvs_cmd) = $_[0];

    if ($cvs_cmd eq 'git') {
#        system("git --git-dir=$OW/.git --work-tree=$OW checkout master");
        open(SYNC, "git --git-dir=$OW/.git --work-tree=$OW pull --no-rebase --ff-only |");
        while (<SYNC>) {
            push(@CVS_messages, sprintf('%s', $_));
        }
        if (!close(SYNC)) {
            print REPORT 'Git failed!';
            return 'fail';
        }
    } elsif ($cvs_cmd eq 'p4') {
        open(SYNC, 'p4 sync |');           # this does...
        while (<SYNC>) {
            my @fields = split;
            my $loc = Common::remove_OWloc($fields[-1]);
            if( $loc ne '' ) {
                push(@CVS_messages, sprintf('%-8s %s', $fields[2], $loc));
            } else {
                push(@CVS_messages, sprintf('%s', $_));
            }
        }
        if (!close(SYNC)) {
            print REPORT 'p4 failed!';
            return 'fail';
        }
    }
    return 'success';
}

sub CVS_check_sync
{
    my($cvs_cmd) = $_[0];

    if (CVS_sync($cvs_cmd) eq 'fail') {
        display_CVS_messages($cvs_cmd);
        return 'fail';
    }
    get_prev_changeno;
    
    if ($prev_report_stamp ne '') {
        print REPORT "\tBuilt through change   : $prev_changeno on $prev_report_stamp";
    } else {
        $prev_changeno = '';
    }
    if ($cvs_cmd eq 'git') {
        open(LEVEL, "git --git-dir=$OW/.git rev-parse HEAD|");
        while (<LEVEL>) {
            if (/^(.*)/) {
                if ($prev_changeno eq $1) {
                    $build_needed = 0;
                    print REPORT "\tNo source code changes, build not needed";
                } else {
                    $prev_changeno = $1;
                    print REPORT "\tBuilding through change: $1";
                }
            }
        }
        close(LEVEL);
    } elsif ($cvs_cmd eq 'p4') {
        open(LEVEL, 'p4 counters|');
        while (<LEVEL>) {
            if (/^change = (.*)/) {
                if ($prev_changeno eq $1) {
                    $build_needed = 0;
                    print REPORT "\tNo source code changes, build not needed";
                } else {
                    $prev_changeno = $1;
                    print REPORT "\tBuilding through change: $1";
                }
            }
        }
        close(LEVEL);
    }
    print REPORT '';
    if (!$build_needed) { # nothing changed, don't waste computer time
        return 'nochange';
    }
    return 'success';
}

#######################
#      Main Script
#######################

# This test should be enhanced to deal properly with subfolders, etc.
if ($home eq $OW) {
    print "Error! The build system home folder can not be under $OW";
    exit 1;
}

my $shortdate_stamp = get_shortdate();
my $date_stamp = get_date();
my $report_directory = "$Common::config{'REPORTS'}\/$shortdate_stamp";
make_path($report_directory);
my $report_name = "$report_directory\/$date_stamp-report-$build_platform.txt";
my $bak_name    = "$report_directory\/$date_stamp-report-$build_platform.txt.bak";
if (stat($report_name)) {
    rename $report_name, $bak_name;
}
open(REPORT, ">$report_name") || die "Unable to open $report_name file.";
print REPORT 'Open Watcom Build Report (build on ', $build_platform, ')';
print REPORT '================================================';
print REPORT '';

# create all batch files
########################

make_boot_batch();
make_build_batch();
make_test_batch();
make_docs_batch();
make_installer_batch();

# Do a CVS sync to get the latest changes.
##########################################

if (($Common::config{'OWCVS'} || '') ne '') {
    $CVS_result = CVS_check_sync($Common::config{'OWCVS'});
} else {
    $CVS_result = 'success';
}
if ($CVS_result eq 'fail') {
    close(REPORT);
    exit 2;
}
if ($CVS_result eq 'nochange') {
    close(REPORT);
    exit 0;
}

############################################################
#
#  pass 1  Build and test full Open Watcom
#
############################################################

print REPORT '';
print REPORT 'Compilers and Tools';
print REPORT '===================';
print REPORT '';

$boot_result = run_boot_build();

if ($boot_result eq 'success') {

    $pass_result = run_build();

############################################################
#
#  Build the Documentation
#
############################################################

    print REPORT '';
    print REPORT 'Documentation Build';
    print REPORT '===================';
    print REPORT '';

    if (defined($ENV{'OWDOCSKIP'}) && ($ENV{'OWDOCSKIP'} eq '1')) {
        $docs_result = 'success';
        print REPORT 'Build skipped.';
        print REPORT '';
    } else {
        $docs_result = run_docs_build();
    }
}

# Run installers build and post batch file
################################################################
if (($boot_result eq 'success') &&
    ($pass_result eq 'success') &&
    ($docs_result eq 'success')) {
    my $installers_result = 'fail';

    print REPORT '';
    print REPORT 'Installers build';
    print REPORT '================';
    print REPORT '';
    print REPORT 'INSTALLER BUILD STARTED    : ', get_datetime();
    if (system($build_installer_name) != 0) {
        print REPORT 'INSTALLER BUILD FAILED!';
    } else {
        print REPORT 'INSTALLER BUILD COMPLETED  : ', get_datetime();
        $installers_result = 'success';
    }
    print REPORT '';
    print REPORT 'Post tasks';
    print REPORT '================';
    print REPORT '';
    if ($installers_result eq 'fail') {
        print REPORT 'POST RUN skipped.';
    } elsif (system($post_batch_name) != 0) {
        print REPORT 'POST RUN FAILED!';
    } else {
        print REPORT 'POST RUN COMPLETED         : ', get_datetime();
    }
    print REPORT '';
    print REPORT '';
    if (($Common::config{'OWCVS'} || '') ne '') {
        set_prev_changeno( $prev_changeno, $date_stamp );  #remember changeno and date
    }
}

# Output CVS sync messages for reference.
##########################################

if (($Common::config{'OWCVS'} || '') ne '') {
    display_CVS_messages($Common::config{'OWCVS'});
}

close(REPORT);
