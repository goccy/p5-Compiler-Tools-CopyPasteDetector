package Compiler::Tools::CopyPasteDetector::DirectoryMetrics;
use strict;
use warnings;
use List::Util qw(sum);
use List::MoreUtils qw(any);
use Data::Dumper;

sub new {
    my ($class, $root, $filemap) = @_;
    my $self = {diretory_map => +{}, filemap => $filemap};
    my $ret = bless($self, $class);
    $ret->add_filename_to_directory_map($root);
    return $ret;
}

sub add_filename_to_directory_map {
    my ($self, $root) = @_;
    my $name = $root->{name};
    my $children = $root->{children};
    my @names;
    foreach my $child (@$children) {
        if (defined($child->{children})) {
            my $child_name = $child->{name};
            my $names_ref = $self->add_filename_to_directory_map($child);
            push(@names, @$names_ref);
            push(@{$self->{diretory_map}->{$name}}, @$names_ref) if ($name ne '');
        } else {
            push(@names, { $name => $child });
        }
    }
    return \@names;
}

sub get_neighbor_score {
    my ($self, $dirname) = @_;
    my $map = $self->{diretory_map};
    my %neighbors;
    foreach my $info (@{$map->{$dirname}}) {
        my @names = keys %$info;
        my $filename = $names[0];
        my $clones = $info->{$filename};
        foreach my $clone (keys %$clones) {
            map {
                $neighbors{$_}++;
            } grep {
                $_ !~ $dirname || $_ eq "$dirname.pm"
            } keys %{$clones->{$clone}->{from_names}};
        }
    }
    return scalar(keys %neighbors);
}

sub get_another_directories_similarity_score {
    my ($self, $dirname) = @_;
    return $self->__get_ratio_of_target_token_to_all_token($dirname,
        sub {
            my ($value, $dirname, $from_names) = @_;
            return any { $_ !~ "$dirname/" || $_ eq "$dirname.pm" } @$from_names;
        });
}

sub get_self_similarity_score {
    my ($self, $dirname) = @_;
    return $self->__get_ratio_of_target_token_to_all_token($dirname,
        sub {
            my ($value, $dirname, $from_names) = @_;
            return sum(map {
                $value->{from_names}->{$_}
            } grep {
                $_ =~ "$dirname/" && $_ ne "$dirname.pm"
            } @$from_names) > 1;
        });
}

sub get_coverage_score {
    my ($self, $dirname) = @_;
    return $self->__get_ratio_of_target_token_to_all_token($dirname, sub { 1; });
}

sub get_directory_map {
    my $self = shift;
    return $self->{diretory_map};
}

sub __get_ratio_of_target_token_to_all_token {
    my ($self, $dirname, $cond) = @_;
    my $map = $self->{diretory_map};
    my $all_token_num = 0;
    my $token_num = 0;
    my %parents;
    my %evaluated_hashs;
    foreach my $info (@{$map->{$dirname}}) {
        my @names = keys %$info;
        my $filename = $names[0];
        my $clones = $info->{$filename};
        foreach (values %$clones) {
            $parents{$_}++ foreach @{$_->{parents}};
        }
        foreach my $hash (keys %$clones) {
            next if exists $parents{$hash};
            next if exists $evaluated_hashs{$hash};
            my $value = $clones->{$hash};
            my @names = keys %{$value->{from_names}};
            if (&$cond($value, $dirname, \@names)) {
                my $count = sum map {
                    $value->{from_names}->{$_}
                } grep {
                    $_ =~ "$dirname/" && $_ ne "$dirname.pm"
                } @names;
                $token_num += $count * $value->{token_num};
            }
            $evaluated_hashs{$hash}++;
        }
        $all_token_num += $self->{filemap}->{$filename}->{token_num};
    }
    return ($all_token_num > 0) ? ($token_num / $all_token_num) * 100 : 0;
}

sub get_score {
    my ($self, $dirname) = @_;
    my $score = {
        another_directories_similarity => $self->get_another_directories_similarity_score($dirname),
        self_similarity                => $self->get_self_similarity_score($dirname),
        coverage                       => $self->get_coverage_score($dirname),
        neighbor                       => $self->get_neighbor_score($dirname)
    };
    return $score;
}

1;
