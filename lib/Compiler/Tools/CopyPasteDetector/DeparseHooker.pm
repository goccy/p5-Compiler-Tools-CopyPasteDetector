package Compiler::Tools::CopyPasteDetector::DeparseHooker;
use B::Deparse;
*B::Deparse::declare_warnings = sub {
    return "";
};
1;
