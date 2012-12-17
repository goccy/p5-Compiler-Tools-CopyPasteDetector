use strict;
use warnings;
use inc::Module::Install;
use Module::Install::XSUtil; # for co-developpers
use constant DEBUG => 0;

use_ppport 3.19;
all_from qw(lib/Compiler/Tools/CopyPasteDetector.pm lib/Compiler/Tools/CopyPasteDetector/*);
cc_warnings;
requires_cplusplus;
cc_include_paths qw(include);
cc_src_paths qw(src);
makemaker_args->{CC} = 'g++';
makemaker_args->{LD} = 'g++';
if (DEBUG) {
    makemaker_args->{OPTIMIZE} = '-O0';
    makemaker_args->{CCFLAGS} = '-g3 ' . makemaker_args->{CCFLAGS};
}

WriteAll(check_nmake => 0);
