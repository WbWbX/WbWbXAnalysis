#!/usr/bin/perl -w

use strict;
use warnings;
use Getopt::Std;
use File::Spec;
use File::Path;
use POSIX;

my %arg;
getopts('d:c:r:sj:m:f:R:q', \%arg);

unless ( $arg{d} && $arg{c} && -f $arg{c}) {
    print <<'USAGE';
Syntax:
 $ runallGC.pl [-g] -d directoryName -c configFile.py [-f files.txt]

Run runallGC.pl to run over all data samples given in files.txt using the
configuration file configFile.py - and use GridControl to submit jobs

A directory on the sonar user space named "NAF_workingDir" will
be created to contain working directories and output.

-q      run in queue (default 48h) does not work right now

-r regexp
If a regexp is given only jobs matching regexp will be submitted (case sensitive).

-R regexp
If a regexp is given only jobs NOT matching the regexp will be submitted (case sensitive).

-s                    really submit jobs using GridControl
-j NJobs              submit NJobs for each dataset. 
//-m maxEventsPerJob    Overwrites default value from files.txt

-o output            output directory when output is downloaded from dcache

-f files.txt          (default value: files.txt)
files.txt must contain lines of
cff_file root_output number_of_jobs
# comments with # and empty lines are allowed
Variables can be added via ${VAR} and will be replaced with environment variable $VAR

USAGE
    exit;
}

my $source = $arg{f} ? $arg{f} : 'files.txt';
my $queue = $arg{q} ? $arg{q} : '48';
open my $IN, '<', $source or die "Cannot open input file: $!\n";

my @run;
my @check;
my @kill;



# prepare workingdir

my $globalWorkingdir="/scratch/hh/dust/naf/cms/user/$ENV{USER}/NAF_workingDir";

unless(-e $globalWorkingdir){
    print "creating global NAF working dir at $globalWorkingdir...\n";
    mkdir($globalWorkingdir);
}


my $workDirWithTime="$arg{d}_" . strftime("%FT%H-%M-%S",localtime);
my $path = "${globalWorkingdir}/$workDirWithTime";
mkpath($path);


my $fullCMSSWcfpath=File::Spec->rel2abs($arg{c});
#$fullCMSSWcfpath=$arg{c};
my $cfgfilename = ( split m{/}, $arg{c} )[-1];
my $nafJobSplitter="$ENV{CMSSW_BASE}/src/TopAnalysis/TopUtils/scripts/nafJobSplitter2.pl";

my $startstring="#!/bin/sh\nif [[ \"\${CMSSW_BASE}\" != $ENV{CMSSW_BASE} ]] ;\nthen\necho Wrong CMSSW env set. should be $ENV{CMSSW_BASE}\nexit\nfi\nexport PYTHONPATH=/scratch/hh/dust/naf/cms/user/\$USER/.nafJobSplitter/python:\$PYTHONPATH\n";

push @run, $startstring;
push @check, $startstring;
push @kill, $startstring;

push @run, "cd $path\n";
push @check, "cd $path\n";

while(my $line = <$IN>) {
    chomp $line;
    next if $line =~ /^\s*#/; #skip comments
    next if exists $arg{'R'} && $line =~ /$arg{'R'}/;
    next unless $line =~ /\w/; #skip empty lines
    next unless (!exists $arg{'r'} || $line =~ /$arg{'r'}/);
    
    my ($numJobs, $dataset, $outputFile, $options, $jsonFile) =
        map { s!\${(\w+)}!$ENV{$1}!g; $_ }
        split / {2,}/, $line;

    $options =~ s/\s+$//;
    $dataset =~ s/\s+$//;

    next unless $outputFile;
    my $allOptions;
    my $json = "";
    if($jsonFile){
	$jsonFile =~ s/\s+$//;
	$json = File::Spec->rel2abs($jsonFile);
	$allOptions="\"outputFile=$outputFile json=$json $options\"";
    }
    else{
	$allOptions="\"outputFile=$outputFile $options\"";
    }
    

#run script, killall script should be sufficient
    
    push @run, "echo submitting $outputFile with $cfgfilename for $dataset\n";
    push @run, "$nafJobSplitter -c $allOptions -d $outputFile -D $dataset -o $path -W 0 $numJobs $cfgfilename\n";

    push @check, "$nafJobSplitter check naf_$outputFile\n";

}
push @run, "cd -\n";
push @check, "cd -\n";

#let submit script write a file after each submitted task(N jobs) and let checkJobs check if file exists (and only then perform a check

open my $OUT, '>', "$path/run.sh" or die $!;
print $OUT $_ for @run;
close $OUT;
chmod 0755, "$path/run.sh";
print "run $path/run.sh to submit jobs\n";

open my $OUTA, '>', "$path/check.sh" or die $!;
print $OUTA $_ for @check;
close $OUTA;
chmod 0755, "$path/check.sh";
print "run $path/check.sh to check jobs\n";

system("cp ${fullCMSSWcfpath} $path");
system("ln -s $ENV{CMSSW_BASE} $path/CMSSW_BASE");
