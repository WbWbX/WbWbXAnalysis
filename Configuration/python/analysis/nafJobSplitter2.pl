#!/usr/bin/perl

use strict;
use warnings;
use POSIX;
use File::Basename;
use File::Path;
use File::Spec;
use Data::Dumper;
use Term::ANSIColor qw(colored);
use Getopt::Std;

#my $datasetPythonPath = "$ENV{CMSSW_BASE}/src/NafJobsplitter/Configuration/python";
my $datasetPythonPath = "/nfs/dust/cms/user/$ENV{USER}/.nafJobSplitter/python";
use constant DAS_DOWNLOAD_URL => 'https://cmsweb.cern.ch/das/makepy?dataset=%s&instance=cms_dbs_prod_global';
#change for only valid files at some point
use constant C_OK => 'green bold';
use constant C_FILE => 'bold';
use constant C_ERROR => 'red bold';
use constant C_RESUBMIT => 'magenta';

########################
### PROGRAM BEGINS
########################

my %args;
getopts('SsbW:kJjp:P:q:o:m:t:c:O:d:nQ:M:D:', \%args);
$args{'Q'} ||= ""; # to suppress unitialised warning when not set

if ($args{'p'}) {
    peekIntoJob($args{'p'});
} elsif ($args{'P'}) {
    logIntoHost($args{'P'});
} else {
    my ($numberOfJobs, $config) = @ARGV;
    syntax() unless $config;
    die "Do not use '-' in filenames!\n" if $config =~ /-/;
    if ($numberOfJobs eq 'check') {
        check(@ARGV[1..$#ARGV]);
        exit;
    }
    submitNewJob($numberOfJobs, $config);
}

################################################################################################
## END MAIN
################################################################################################

sub syntax {
    print <<END_USAGE_INFO;
nafJobSplitter.pl
 - A very simple script to split jobs and submit them to the NAF

******************************************************************
* Before submitting                                              *
******************************************************************

Make sure that the name of the variable containing your cms.Process()
is "process". (process = cms.Process("whateverNameYouLike") )

Also make sure that you output your histograms using process.TFileService.

Please cd to the directory containing the config file.


******************************************************************
* Submitting to the NAF batch farm                               *
******************************************************************

The nafJobSplitter is easy to use: instead of running "cmsRun MyAna.py",
execute "nafJobSplitter.pl [parameters] NumberOfJobs MyAna.py"

This will create a directory "naf_MyAna" with all files needed to submit
many jobs. Then it will submit all jobs to the NAF batch system.

The jobs will be split on a per file basis, i.e. if you run over 10 files, you cannot
use more than 10 jobs. If you run over 3 files using 2 jobs, then one job will run
over 2 files and one job will run over 1 file (ignoring file sizes).

Available Parameters
  -q: choose queue, h_rt in hours
        default: -q 24
        to modify the default, use the environt variable NJS_QUEUE, e.g. "export NJS_QUEUE=3"
  -M: maximum memory, default is 3700 (unit is MB), this is the hard limit. Soft limit is 300M less.
        to modify the default, use the environt variable NJS_MEM, e.g. "export NJS_MEM=7000"
  -o: output directory
        default: `pwd`
      Note: the output will always be stored in the current directory. If you specify
      the ouput directory, NJS will create a symlink to it. E.g. it might be useful
      to specify -o /scratch/hh/current/cms/user/\$USER/njs
      Use NJS_OUTPUT environment variable to set a default
  -Q: add options directly to the qsub command, for example -Q "-l site=zn" forces
      jobs to run on Zeuthen hosts (default is -l site=hh)
      the options are not saved, you have to give them again when using check
  -D: run over a specific dataset, e.g. /DoubleElectron/Run2012A-13Jul2012-v1/AOD
      The NJS will automatically download the corresponding python configuration file
      from DAS and store it in the $datasetPythonPath directory.
  -d: directory or symlink suffix of dir/link where files are stored
      e.g. njs -d xxx file.py will create naf_file_xxx/
  -c: additional command line arguments to cmsRun (put after the .py file),
      use for VarParsing
  -m: maximum number of events per job
        default: -1 (i.e. no limit)
  -J: Don't automatically join output files and sum up TrigReports
       If you forget to pass this parameter, touch 'naf_DIR/autojoin'
       to enable automatic joining. Remove the autojoin file to
       disable auto joining.
  -k: keep all source root files after hadd'ing them
       touch/remove the file 'autoremove' to enable/disable the
       feature at some later point in time
  -W: waiting time in secs before submission (default: 2 secs)
  -O: alternative output file name = do not look for a TFileService,
      rather copy output files with a given name (for production jobs)
      WARNING: you need to save your file to the local temp dir, i.e.
      ....fileName=cms.untracked.string(os.getenv('TMPDIR', '.') + '/file.root')

******************************************************************
* What to do after submitting - if jobs crash / to monitor jobs  *
******************************************************************

nafJobSplitter.pl [parameters] check naf_DIRECTORY [naf_DIRECTORY2 ...]

The check command will automatically resubmit crashed jobs and/or
put jobs in Eqw state back to qw.

Available Parameters
  -n: do not resubmit any job
  -j: join output root files, sum the TrigReports if all jobs are done
       You only need this if you have used -J to submit the jobs
  -K: remove root files after joining. Only needed if -k was passed
       while submitting jobs
  -t mins: perform the check every mins minutes
  -s show the read speed summary
     Timing-tstoragefile-read-totalMegabytes and
     Timing-tstoragefile-read-totalMsecs
  -S show extended read speed summary (for each job)
  -b show speed summary in bytes / bytes per second
  -Q direct setting of qsub options, see also submitting section

To peek into running jobs, i.e. to show the current stdout:
  nafJobSplitter.pl -p jobid
where jobid is "jobid.arraynumber", e.g. "4491742.7".

To log into the host a job is running on to do some runtime
debugging (e.g. sending USR1 to the job and look at the current
root-file with the possibility to resume the job):
  nafJobSplitter.pl -P jobid
where jobid is "jobid.arraynumber", e.g. "4491742.7".


******************************************************************
* Resuming jobs                                                  *
******************************************************************

All jobs will be notified 5 Minutes before the end of the specified
job length. This is done via the signal XCPU or USR1. To support
resuming, you need to catch these signals in cmsRun and emit the
signal INT instead so that cmsRun will end the job. To achieve this,
put 'process.load("TopAnalysis.TopUtils.SignalCatcher_cfi")' at the
very end of your config file. Further, your python config file must
support a "skipEvents" parameter, so that "cmsRun config.py skipEvents=100"
would skip 100 events.

Local jobs: to run jobs locally, for example job 6, do:
\$ SGE_TASK_ID=6 naf_directory/j_whatever.sh & ; disown
Note that the "check" function currently does not know about local jobs!
Use this if a certain job is always removed from the batch system.

END_USAGE_INFO
    exit 1;
}

# pass dataset name as parameter
# returns import command for python file (full line, but excluding \n)
sub getDatasetPythonFile {
    my $dataset = shift;
    (my $localFileName = $dataset) =~ s!\W!_!g;
    $localFileName =~ s/^_//;
    mkpath $datasetPythonPath;
    my $localFileNameFull = "$datasetPythonPath/$localFileName.py";
    if (-e $localFileNameFull) {
        print "Dataset configuration file already downloaded.\n";
    } else {
        my $url = sprintf(DAS_DOWNLOAD_URL, $dataset);
        print "Downloading configuration file from DAS:\ngetting $url...\n";
        #no Net::SSLeay available on the NAF
        #my $result = getstore($url, $localFileNameFull);
        #if (is_success($result)) {
        my $result = system("wget", "--no-check-certificate", '-O', $localFileNameFull, $url);
        if ($result == 0) {
            print "Success, file stored as $localFileNameFull\n"
        } else {
            unlink $localFileNameFull;
            die "Could not download dataset file: $?\n";
        }
    }
    #make sure the file can be found even without running scram (dirty hack)
    $ENV{PYTHONPATH} = "$datasetPythonPath:$ENV{PYTHONPATH}";
    return qq{process.load("$localFileName")};
#     return qq{process.load("TopAnalysis.Configuration.$localFileName")};
}

sub submitNewJob {
    my ($numberOfJobs, $config) = @_;
    die "Error: $config is not a CMS configuration file!\n" unless -f $config && -s $config;
    $config =~ s/\.py$//;
    my $shellScript = "j_${config}.sh";

    my $dir = $config;
    if ($args{'d'}) {
        #$dir .= '_' . $args{'d'};
        $dir = $args{d};
        die "Directory $dir must not contain '-'!\n" if $dir =~ /-/;
    } else {
        if (my $varparsing = $args{'c'}) {
            $varparsing =~ s/\W/_/g;
            $dir .= '_' . $varparsing;
        }
    }
    
    my $ownDataset = $args{'D'} ? getDatasetPythonFile($args{'D'}) : '';
    
    createNJSDirectory("naf_$dir");

    my $cfgPy = getConfigTemplate($ownDataset);
    my $cfgSh = getBatchsystemTemplate();

    for ($cfgPy, $cfgSh) {
        s/CONFIGFILE/$config/g;
        s/NUMBER_OF_JOBS/$numberOfJobs/g;
        s/DIRECTORY/$dir/g;
    }

    open my $JOB, '>', "naf_$dir/$config.py" or die $!;
    print $JOB $cfgPy;
    close $JOB or die $!;

    open my $BATCH, '>', "naf_$dir/$shellScript" or die $!;
    print $BATCH $cfgSh;
    chmod((stat $BATCH)[2] | 0700, $BATCH); #make executable
    close $BATCH or die $!;

    unless ($args{'J'}) {
        open my $FH, '>', "naf_$dir/autojoin" or die $!;
    }
    unless ($args{'k'}) {
        open my $FH, '>', "naf_$dir/autoremove" or die $!;
    }
    #mkdir "naf_$dir/$_" or die $! for 1..$numberOfJobs);

    $_ = defined $args{'W'} ? $args{'W'} : 2;
    die "Invalid waiting time!\n" unless /^\d+$/ && $_>=0 && $_<1000;
    ++$_;
    while (--$_) {
        print "Submitting in $_ seconds, press Ctrl-C to cancel\n";
        sleep 1;
    }
    {open my $FH, '>', "naf_$dir/exe" or die $!; print $FH $shellScript;}
    die "Cannot submit job!\n" unless submitJob("naf_$dir", 1, $numberOfJobs, $shellScript);
}

sub submitJob {
    my ($dirWithRelPath, $from, $to, $script) = @_;
    my $current = $ENV{PWD};
    my ($dir, $dirBase) = fileparse(File::Spec->canonpath("$current/$dirWithRelPath"));
    if (chdir($dirBase)) {
        $script ||= do { open my $FH, '<', "$dir/exe"; <$FH> };
        my $line = `qsub -t $from-$to:1 $args{'Q'} $dir/$script`;
        if ($line =~ /Your job-array (\d+)(?:\.\d+-\d+:1) \(".+"\) has been submitted/) {
            my $success;
            my $ids = ''; $ids .= "$_\t$1\n" for $from..$to;
            do {
                $success = open my $FH, '>>', "$dir/jobids.txt";
                $success &&= print $FH $ids;
                if (!$success) {
                    print "$!\nCan't write to $dir/jobids.txt, trying again in 5sec\n";
                    sleep 5;
                }
            } while (!$success);
            chdir $current or die "Cannot chdir back to original directory!\n";
            print $from == $to ? "Job $from submitted with job-id $1\n" : "Jobs $from to $to submitted with job-id $1\n";
            return $1;
        } else {
            die "Cannot submit job!\n$line";
        }
    } else {
        print colored("Could not chdir to $dirBase!\n", C_ERROR);
        return;
    }
}

sub check {
    my @dirs = @_;
    my $alldone;
    my %doneDirs;
    do {
        $alldone = 1;
        my $qstat = QStat->new();
        for my $dir (@dirs) {
            next if $doneDirs{$dir};
            { local $|=1; print "Looking for jobs in $dir"; }
            my $thisDone = checkJob($dir, $qstat);
            $doneDirs{$dir} = 1 if $thisDone;
            $alldone = $thisDone && $alldone;
        }
        if (!$alldone && defined $args{'t'}) {
            print "Waiting for next check at " . localtime(time + $args{'t'}*60) . ", cancel with Ctrl-C...\n";
            sleep $args{'t'}*60-10;
            print "Only 10 seconds left, don't cancel me once the check starts!\n";
            sleep 10;
        }
    } while (!$alldone && defined $args{'t'});
}

sub checkJob {
    my ($dir, $qstat) = @_;
    #$dir contains njs_whatever, assuming you have chdir'ed into the correct directory
    #$qstat contains an instance of a qstat class
    my $isComplete = 0;
    my %jobs = getJobs($qstat, "$dir/jobids.txt"); #get arrayid => job-object

#     opendir(my $dirHandle, $dir) or die "can't opendir $dir: $!";
#     my %existingFiles;
#     @existingFiles{@{[readdir $dirHandle]}} = ();
#     closedir $dirHandle;

    my ($NRunning, $NResubmitted, $NWaiting, $NDoneJobs, $NError) = (0) x 5;

    while (my ($arrId, $job) = each %jobs) {
        if ($job) {
            my $state = $job->state();
            if ($state =~ /E/) {
                print colored("\njob has error state:\n", C_RESUBMIT);
                print grep /error reason/, $job->statusInfo();
                if (!$args{'n'}) {
                    print colored("clearing error state...\n", C_RESUBMIT);
                    $job->clearError() == 256 or die "Cannot clear error state!\n";
                    ++$NResubmitted;
                }
            } elsif ($state =~ /r/) {
                ++$NRunning;
                print "\nRunning job: " . $job->fullId() if $args{'n'};
            } else {
                ++$NWaiting;
                print "\nQueued job: " . $job->fullId() if $args{'n'};
            }
        } else {
            #job is not there
            if (-e "$dir/out$arrId.txt") {
#             if (exists $existingFiles{"out$arrId.txt"}) {
                ++$NDoneJobs;
            } elsif (-e "$dir/err$arrId.txt") {
#             } elsif (exists $existingFiles{"err$arrId.txt"}) {
                ++$NError;
                print colored("\ncmsRun didn't return success, see $dir/err$arrId.txt", C_ERROR);
            } else {
                if ($args{'n'}) {
                    print colored("\n -> job $arrId seems to have died", C_RESUBMIT);
                } else {
                    print colored("\n -> job $arrId seems to have died, resubmitting...", C_RESUBMIT);
                    submitJob($dir, $arrId, $arrId);
                    ++$NResubmitted;
                }
            }
        }
    }
    print "\n" if $NResubmitted || $NError;
    if ($NDoneJobs == keys %jobs) {
        print " --> ", colored("100% done!", 'on_green bold'), "  --  $NDoneJobs jobs";
        $isComplete = 1;
    } else {
        printf " -->  %d%%  --  %d jobs", 100*$NDoneJobs / keys %jobs, scalar keys %jobs;
        my @N = (colored(', %d queueing', 'dark cyan') => $NWaiting,
                 colored(', %d running', 'bold') => $NRunning,
                 colored(', %d resubmitted', C_RESUBMIT) => $NResubmitted,
                 colored(', %d error', C_ERROR) => $NError,
                 colored(', %d done', C_OK) => $NDoneJobs);
        while (@N) {
            my $str = shift @N; my $val = shift @N;
            printf $str, $val if $val;
        }
    }
    my @details;
    @details = showFJRsummary($dir) if $args{'s'} || $args{'S'};
    print ".\n";
    showFJRdetails(@details) if $args{'S'};

    if ($NDoneJobs == keys %jobs) {
        open my $JOINED, '<', "$dir/joined.txt" or die "Cannot open joined.txt: $!\n";
        my $joined = <$JOINED>;
        if (!-e "$dir/$joined") {
            my $config = $dir; $config =~ s/naf_//;
            if ($args{'j'}) {
                print "Joining output files...\n";
                system('hadd', "$dir/$joined", glob("$dir/$config-*.root")) == 0 or die "hadd failed: $?";
                my $str = fileparse($joined, '.root') . '.txt';
                system("sumTriggerReports2.pl $dir/out*.txt > $dir/$str");
                print colored("Joined output file is: ", C_OK),
                    colored("$dir/$joined\n", C_FILE),
                    colored("Joined TrigReport is ", C_OK),
                    colored("$dir/$str\n", C_FILE);
            } else {
                print " - Hint: pass the -j option to join files\n";
            }
        } else {
            #print colored(" - results have already been joined\n", C_OK);
        }
    }
    return $isComplete;
}

sub bytesToHuman {
    my $bytes = shift;
    return ($bytes, '') if $args{'b'};
    my @PREFIX = ('', qw(k M G T P E Z Y));
    my $index = 0;
    while ($bytes >= 1000) {
        $bytes /= 1000;
        ++$index;
    }
    return ($bytes, $PREFIX[$index]);
}

sub readFJRFileWithRE {
    my ($fileName, $sum) = @_;
    open my $fh, '<', $fileName or die "Cannot open $fileName: $!\n";
    my $file = do { local $/; <$fh> };
    my %result;
    my @files = $file =~ m!^.*/(.*\.root)</LFN>$!mg;
    for (keys %$sum) {
        $result{$_} = $1 if $file =~ m!^\s*<Metric Name="$_" Value="(.*?)"/>$!m;
    }
    return (\@files, %result);
}

sub showFJRsummary {
    my $dir = shift;
    my %sum;
    #Timing-dcap-read-totalMegabytes Timing-dcap-read-totalMsecs
    @sum{qw(Timing-tstoragefile-read-totalMegabytes Timing-tstoragefile-read-totalMsecs)} = (0) x 10;
    my @files = glob("$dir/jobreport*.xml");
    my @details;
    for (@files) {
        my ($filesRef, %perf) = readFJRFileWithRE($_, \%sum);
        for (keys %sum) {
            no warnings 'uninitialized'; #can be that the fjr is incomplete!!
            $sum{$_} += $perf{$_};
        }
        push @details, [$filesRef,
                        $perf{'Timing-tstoragefile-read-totalMegabytes'}*1e6,
                        $perf{'Timing-tstoragefile-read-totalMegabytes'}*1e6 / ($perf{'Timing-tstoragefile-read-totalMsecs'}/1e3)
                        ] if $perf{'Timing-tstoragefile-read-totalMsecs'};
    }
    if (@files) {
        printf ", read %.2f %sB at %.2f %sB/s per job",
            bytesToHuman($sum{'Timing-tstoragefile-read-totalMegabytes'}*1e6),
            bytesToHuman($sum{'Timing-tstoragefile-read-totalMegabytes'}*1e6 / ($sum{'Timing-tstoragefile-read-totalMsecs'}/1e3));
    }

    return @details;
}

sub showFJRdetails {
    my @details = @_;
    for (sort {$b->[2] <=> $a->[2]} @details) {
        printf "%.2f %sB/s (total %.2f %sB): ", bytesToHuman($_->[2]), bytesToHuman($_->[1]);
        print "@{$_->[0]}\n";
    }
}


# returns a list of this:
#       1 => blessed ref of job 444257.1
#       7 => undef
sub getJobs {
    my ($qstat, $file) = @_;
    open my $FH, '<', $file or die "$file: $!";
    my %result;
    while (<$FH>) {
        $result{$1} = $qstat->job("$2.$1") if /^(\d+)\t(\d+)$/;
    }
    %result;
}

sub createNJSDirectory {
    my $dir = shift;
    my $symlinkDir = $args{'o'} || $ENV{NJS_OUTPUT};
    if ($symlinkDir) {
        my $newDir = "$symlinkDir/" . strftime("%Y%m%dT%H%M%S_",localtime) . $dir;
        if (-l $dir) {
            warn "Old symlink $dir exists, removing it.\n";
            unlink $dir;
        }
        mkpath $newDir;
        symlink $newDir, $dir or die "Cannot create symlink $dir --> $newDir\n";
    } else {
        mkpath $dir;
    }
}

sub peekIntoJob {
    my $jid = shift;
    my $job = QStat->new()->job($jid);
    if ($job) {
        die "The job is not running\n" unless $job->state() =~ /r/;
        $job->peek();
    } else {
        die "Cannot find job. Please pass jobid.arraynumber (separated by .) of a running job\n";
    }
}

sub logIntoHost {
    my $jid = shift;
    my $job = QStat->new()->job($jid);
    if ($job) {
        die "The job is not running\n" unless $job->state() =~ /r/;
        $job->logIntoHost();
    } else {
        die "Cannot find job. Please pass jobid.arraynumber (separated by .) of a running job\n";
    }
}

# parameter: possible dataset file download by the NAF
sub getConfigTemplate {
    my $ownDataset = shift;
    my $maxEvents = $args{'m'} || -1;
    my $alternativeOutput = $args{'O'}?'True':'False';
    return <<END_OF_TEMPLATE;

import os
from CONFIGFILE import *

$ownDataset
numberOfFiles = len(process.source.fileNames)
numberOfJobs = NUMBER_OF_JOBS
jobNumber = int(os.environ["SGE_TASK_ID"]) - 1

process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
print "running over these files:"
print process.source.fileNames

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32($maxEvents))

if hasattr(process, "options"):
    process.options.wantSummary = cms.untracked.bool(True)
else:
    process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

if jobNumber == 0 and not $alternativeOutput:
    fh = open('FINALOUTPUTPATH/joined.txt', 'w')
    fh.write(eval(process.TFileService.fileName.pythonValue()))
    fh.close

## overwrite TFileService
if not $alternativeOutput:
    process.TFileService = cms.Service("TFileService",
        fileName = cms.string("OUTPUTPATH/CONFIGFILE-" + str(jobNumber + 1) + ".root")
    )

END_OF_TEMPLATE
}

sub getBatchsystemTemplate {
    #$ -l s_vmem=1950M

    my $templ = <<'END_OF_BATCH_TEMPLATE';
#!/bin/zsh
#
#(make sure the right shell will be used)
#$ -S /bin/zsh
#
#(the cpu time for this job)
#(naf2 change: h_cpu to h_rt to account for the new queues)
#$ -l h_rt=__HCPU__
#$ -l s_cpu=__SCPU__
#$ -l s_rt=__SCPU__
#$ -l site=hh
#
#(the maximum memory usage of this job)
#$ -l h_vmem=__HVMEM__M
#$ -l s_vmem=__SVMEM__M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#
# use current dir and current environment
#$ -cwd
#$ -V
#
#$ -o /dev/null
# naf2 changes
#$ -P af-cms

tmp=$(mktemp -d -t njs_XXXXXX)

exec > "$tmp/stdout.txt" 2>&1

echo "Running on host"
hostname

#Creating configuration file
current=`pwd`
perl -pe "s!FINALOUTPUTPATH!$current/naf_DIRECTORY!g; s!OUTPUTPATH!$tmp!g" < $current/naf_DIRECTORY/CONFIGFILE.py > $tmp/run.py

trap '' USR1 XCPU

if [ -e $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.1 ] ; then
    continueOldJobNo=`ls -1 $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.* | wc -l`
    echo "Continuing old job"
    NSkip=$(sumTriggerReports2.pl $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.* | perl -ne 'print($1), exit if /TrigReport\s*Events\stotal\s*=\s*(\d+)/')
    echo "Skipping $NSkip old events"
    if [ -z "CMSRUNPARAMETER" ] ; then
        PARAMS="skipEvents=$NSkip"
    else
        PARAMS="CMSRUNPARAMETER skipEvents=$NSkip"
    fi
    
    PYTHONDONTWRITEBYTECODE=1 cmsRun -j $tmp/jobreport.xml $tmp/run.py $PARAMS
    #DCACHE_CLIENT_ACTIVE=1 PYTHONDONTWRITEBYTECODE=1 cmsRun -j $tmp/jobreport.xml $tmp/run.py $PARAMS
else
    continueOldJobNo=0
    PYTHONDONTWRITEBYTECODE=1 cmsRun -j $tmp/jobreport.xml $tmp/run.py CMSRUNPARAMETER
    #DCACHE_CLIENT_ACTIVE=1 PYTHONDONTWRITEBYTECODE=1 cmsRun -j $tmp/jobreport.xml $tmp/run.py CMSRUNPARAMETER
fi


if [ "$?" = "0" ] ; then
    sync
    thisPart=$(($continueOldJobNo + 1))
    alternativeOutput=ALTERNATIVEOUTPUT
    grep -qP ':signalHandler:\d+ received signal \d+ initiating program termination!' $tmp/stdout.txt
    if [ "$?" = "0" ] ; then
        if [ -z "${alternativeOutput}" ]; then
            set -e
            mv $tmp/jobreport.xml $current/naf_DIRECTORY/jobreport$SGE_TASK_ID.xml.part.$thisPart
            mv $tmp/CONFIGFILE-$SGE_TASK_ID.root $current/naf_DIRECTORY/CONFIGFILE-$SGE_TASK_ID.root.part.$thisPart
            mv $tmp/stdout.txt $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart
            grep TrigReport $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart >/dev/null 2>&1
	    mv $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.$thisPart
            set +e
        else
            #bad luck, not supported!
            exit
        fi
    else
        #job ends with success
        if [ -z "${alternativeOutput}" ]; then
            if [ $thisPart -gt 1 ] ; then
                set -e
                mv $tmp/jobreport.xml $current/naf_DIRECTORY/jobreport$SGE_TASK_ID.xml.part.$thisPart
                mv $tmp/CONFIGFILE-$SGE_TASK_ID.root $current/naf_DIRECTORY/CONFIGFILE-$SGE_TASK_ID.root.part.$thisPart
                mv $tmp/stdout.txt $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart
		grep TrigReport $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart >/dev/null 2>&1
		mv $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt.part.$thisPart $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.$thisPart
                hadd $tmp/hadd.root $current/naf_DIRECTORY/CONFIGFILE-$SGE_TASK_ID.root.part.*
                mv $tmp/hadd.root $current/naf_DIRECTORY/CONFIGFILE-$SGE_TASK_ID.root
                sumTriggerReports2.pl $current/naf_DIRECTORY/out$SGE_TASK_ID.txt.part.* > $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
                set +e
            else
                set -e
                mv $tmp/jobreport.xml $current/naf_DIRECTORY/jobreport$SGE_TASK_ID.xml
                mv $tmp/CONFIGFILE-$SGE_TASK_ID.root $current/naf_DIRECTORY/
                mv $tmp/stdout.txt $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt
		grep TrigReport $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt >/dev/null 2>&1
		mv $current/naf_DIRECTORY/tmp.out$SGE_TASK_ID.txt $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
                set +e
            fi
            if [ -e $current/naf_DIRECTORY/autojoin ] ; then
                NDone=`ls $current/naf_DIRECTORY/out*.txt | wc -l`
                #NDone=$(($NDone + 1))
                if [ "$NDone" = "NUMBER_OF_JOBS" ] ; then
                    joined=`cat $current/naf_DIRECTORY/joined.txt`
                    hadd -f $current/naf_DIRECTORY/`basename $joined`.$SGE_TASK_ID $current/naf_DIRECTORY/CONFIGFILE-*.root
                    if [ "$?" = "0" ] ; then
                        mv -f $current/naf_DIRECTORY/`basename $joined`.$SGE_TASK_ID $current/naf_DIRECTORY/`basename $joined`
                        cp -f $tmp/stdout.txt $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
                        sumTriggerReports2.pl $current/naf_DIRECTORY/out*.txt > $current/naf_DIRECTORY/`basename $joined .root`.txt
                        if [ -e $current/naf_DIRECTORY/autoremove ] ; then
                            rm -f $current/naf_DIRECTORY/CONFIGFILE-*.root*
                        fi
                    fi
                fi
            fi
        else
            mv $tmp/${alternativeOutput} $current/naf_DIRECTORY/`basename ${alternativeOutput} .root`${SGE_TASK_ID}.root && mv $tmp/stdout.txt $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
        fi
    fi
else
    mv $tmp/stdout.txt $current/naf_DIRECTORY/err$SGE_TASK_ID.txt
fi
rm -r $tmp

END_OF_BATCH_TEMPLATE
    my ($hcpu, $scpu) = getCPULimits();
    my ($hvmem, $svmem) = getMemoryLimits();
    $templ =~ s/__HCPU__/$hcpu/g;
    $templ =~ s/__SCPU__/$scpu/g;
    $templ =~ s/__HVMEM__/$hvmem/g;
    $templ =~ s/__SVMEM__/$svmem/g;
    $args{'c'} ||= '';
    $templ =~ s/CMSRUNPARAMETER/$args{'c'}/g;
    $args{'O'} ||= '';
    $templ =~ s/ALTERNATIVEOUTPUT/$args{'O'}/g;
    return $templ;
}

#####
sub getCPULimits {
    my $hlimit = $args{'q'} || $ENV{NJS_QUEUE} || "24:00:00";
    $hlimit .= ":00:00" if $hlimit !~ /:/;
    die "invalid queue format: $hlimit\n" unless $hlimit =~ /^\d{1,2}:\d{2}:\d{2}$/;
    my ($h,$m,$s) = split /:/, $hlimit;
    $m -= 5; #substract 5 minutes for soft limit
    if ($m < 0) { --$h; $m +=60; }
    return ($hlimit, "$h:$m:$s");
}

sub getMemoryLimits {
    my $hlimit = $args{'M'} || $ENV{NJS_MEM} || 3700;
    die "invalid memory requirement: $hlimit\n" unless $hlimit =~ /^\d+$/ && $hlimit > 700 && $hlimit < 20000;
    return ($hlimit, $hlimit - 300);
}

################################################################################################
##  Classes to read qstat
################################################################################################

package Job;
sub new {
    my ($class, $qstatLine) = @_;
    my $self = \$qstatLine;
    bless $self, $class;
}

sub peek {
    my $self = shift;
    die "Job is not running, cannot peek\n" unless $self->state() eq 'r';
    if ($self->queue() =~ /\@(.+)/) {
        print "Please wait, this can take up to a few minutes...\n";
        my $jid = $self->fullId();
        system("qrsh -l h_rt=00:01:00 -l h=$1 -l h_vmem=400M -now n 'cat \$TMP/../$jid.*/njs_*/stdout.txt'");
    } else {
        die "Didn't find hostname\n";
    }
}

sub logIntoHost {
    my $self = shift;
    die "Job is not running, cannot login to same host\n" unless $self->state() eq 'r';
    if ($self->queue() =~ /\@(.+)/) {
        print "Please wait, this can take up to a few minutes...\n";
        my $jid = $self->fullId();
        system("qrsh -l h_rt=00:01:00 -l h=$1 -l h_vmem=400M -now n ");
    } else {
        die "Didn't find hostname\n";
    }
}

sub statusInfo {
    my $self = shift;
    my $command = 'qstat -j ' . $self->fullId();
    `$command`;
}

sub clearError {
    my $self = shift;
    system('qmod -cj ' . $self->fullId());
}

sub extract {
    my ($self, $tag) = @_;
    return unless $$self =~ m!<$tag>(.*?)</$tag>!;
    $1;
}

sub id {
    my $self = shift;
    $self->extract('JB_job_number');
}

sub priority {
    my $self = shift;
    $self->extract('JAT_prio');
}

sub name {
    my $self = shift;
    $self->extract('JB_name');
}

sub user {
    my $self = shift;
    $self->extract('JB_owner');
}

sub state {
    my $self = shift;
    $self->extract('state');
}

sub time {
    my $self = shift;
    $self->extract('JB_submission_time') || $self->extract('JAT_start_time');
}

sub queue {
    my $self = shift;
    $self->extract('queue_name');
}

sub slots {
    my $self = shift;
    $self->extract('slots');
}

sub arrayid {
    my $self = shift;
    $self->extract('tasks') || 0;
}

sub fullId {
    my $self = shift;
    if (my $arr = $self->arrayid()) {
        return $self->id() . ".$arr";
    } else {
        return $self->id();
    }
}

package QStat;
sub new {
    my $class = shift;
    my $self = {};
    my $all = `qstat -xml -g d -u $ENV{USER}`;
    die "qstat has returned something unexpected:\n$all" unless $all =~ m!</job_info>!;
    for ($all =~ m!<job_list.*?>(.*?)</job_list>!sg) {
        my $job = Job->new($_);
        $self->{$job->fullId()} = $job;
    }
    bless $self, $class;
}

sub jobs {
    my ($self) = @_;
    return values %$self;
}

sub job {
    my ($self, $fullId) = @_;
    my ($j) = grep {$fullId eq $_->fullId()} $self->jobs();
    return $j if $j;
    return;
}

1;
