use strict;
use warnings;
use inc::Module::Install;
use Module::Install::XSUtil; # for co-developpers

all_from 'lib/Compiler/Tools/CopyPasteDetector.pm';
use_ppport 3.19;

WriteAll(check_nmake => 0);
