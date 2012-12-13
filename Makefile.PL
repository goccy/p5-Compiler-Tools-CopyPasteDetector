use strict;
use warnings;
use inc::Module::Install;
use Module::Install::XSUtil; # for co-developpers

requires_cplusplus();
all_from qw(lib/Compiler/Tools/CopyPasteDetector.pm lib/Compiler/Tools/CopyPasteDetector/*);
cc_warnings;
cc_append_to_ccflags("-x c++");
use_ppport 3.19;
cc_include_paths qw(include);
cc_src_paths qw(src);
WriteAll(check_nmake => 0);
