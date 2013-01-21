package Compiler::Tools::CopyPasteDetector::DeparseHooker;
use B qw(svref_2object class);
use B::Deparse;
*B::Deparse::declare_warnings = sub { ''; };
*B::Deparse::declare_hinthash = sub { ''; };
*B::Deparse::stash_subs = sub {
    my ($self, $pack) = @_;
    my (@ret, $stash);
    if (!defined $pack) {
        $pack = '';
        $stash = \%::;
    } else {
        $pack =~ s/(::)?$/::/;
        no strict 'refs';
        $stash = \%{"main::$pack"};
    }
    my %stash = svref_2object($stash)->ARRAY;
    while (my ($key, $val) = each %stash) {
        my $class = class($val);
        if ($class eq "PV") {
            # Just a prototype. As an ugly but fairly effective way
            # to find out if it belongs here is to see if the AUTOLOAD
            # (if any) for the stash was defined in one of our files.
            my $A = $stash{"AUTOLOAD"};
            if (defined ($A) && class($A) eq "GV" && defined($A->CV)
                && class($A->CV) eq "CV") {
                my $AF = $A->FILE;
                next unless $AF eq $0 || exists $self->{'files'}{$AF};
            }
            push @{$self->{'protos_todo'}}, [$pack . $key, $val->PV];
        } elsif ($class eq "IV" && !($val->FLAGS & SVf_ROK)) {
            # Just a name. As above.
            # But skip proxy constant subroutines, as some form of perl-space
            # visible code must have created them, be it a use statement, or
            # some direct symbol-table manipulation code that we will Deparse
            my $A = $stash{"AUTOLOAD"};
            if (defined ($A) && class($A) eq "GV" && defined($A->CV)
                && class($A->CV) eq "CV") {
                my $AF = $A->FILE;
                next unless $AF eq $0 || exists $self->{'files'}{$AF};
            }
            ## --------------<< HOOK POINT >>---------------- ##
            #push @{$self->{'protos_todo'}}, [$pack . $key, undef];
            ## ---------------------------------------------- ##
        } elsif ($class eq "GV") {
            if (class(my $cv = $val->CV) ne "SPECIAL") {
                next if $self->{'subs_done'}{$$val}++;
                next if $$val != ${$cv->GV};   # Ignore imposters
                $self->todo($cv, 0);
            }
            if (class(my $cv = $val->FORM) ne "SPECIAL") {
                next if $self->{'forms_done'}{$$val}++;
                next if $$val != ${$cv->GV};   # Ignore imposters
                $self->todo($cv, 1);
            }
            if (class($val->HV) ne "SPECIAL" && $key =~ /::$/) {
                $self->stash_subs($pack . $key)
                    unless $pack eq '' && $key eq 'main::';
                # avoid infinite recursion
            }
        }
    }
};

1;
