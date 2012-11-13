use strict;
use warnings;
use inc::Module::Install;
use Module::Install::XSUtil; # for co-developpers

all_from qw(lib/Compiler/Tools/CopyPasteDetector.pm lib/Compiler/Tools/CopyPasteDetector/*);
use_ppport 3.19;

WriteAll(check_nmake => 0);
