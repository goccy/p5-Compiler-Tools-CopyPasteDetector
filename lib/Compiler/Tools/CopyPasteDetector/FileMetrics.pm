package Compiler::Tools::CopyPasteDetector::FileMetrics;
use strict;
use warnings;
use Data::Dumper;

sub new {
    my $class = shift;
    my $finfo = shift;
    my $self = {info => $finfo};
    return bless($self, $class);
}

sub get_neighbor_score {
    my ($self) = @_;
    my $filename = $self->{info}->{name};
    my $clones = $self->{info}->{clones};
    my %neighbors;
    foreach my $clone (keys %$clones) {
        map {
            $neighbors{$_}++;
        } grep {
            $_ !~ $filename
        } keys %{$clones->{$clone}->{from_names}};
    }
    return scalar(keys %neighbors);
}

sub get_another_files_similarity_score {
    my ($self) = @_;
    return $self->__get_ratio_of_target_token_to_all_token(sub { shift == 1; });
}

sub get_self_similarity_score {
    my ($self) = @_;
    return $self->__get_ratio_of_target_token_to_all_token(sub { shift > 1; });
}

sub get_coverage_score {
    my ($self) = @_;
    return $self->__get_ratio_of_target_token_to_all_token(sub { 1; });
}

sub __get_ratio_of_target_token_to_all_token {
    my ($self, $cond) = @_;
    my $filename = $self->{info}->{name};
    my $clones = $self->{info}->{clones};
    my $token_num = 0;
    my %parents;
    foreach (values %$clones) {
        $parents{$_}++ foreach @{$_->{parents}};
    }
    foreach my $hash (keys %$clones) {
        next if exists $parents{$hash};
        my $value = $clones->{$hash};
        my $count = $value->{from_names}->{$filename};
        $token_num += $count * $value->{token_num} if (&$cond($count));
    }
    my $all_token_num = $self->{info}->{all_token_num};
    return ($all_token_num > 0) ? $token_num / $all_token_num * 100 : 0;
}

sub get_score {
    my ($self) = @_;
    return {
        another_files_similarity => $self->get_another_files_similarity_score(),
        self_similarity          => $self->get_self_similarity_score(),
        coverage                 => $self->get_coverage_score(),
        neighbor                 => $self->get_neighbor_score()
    };
}

1;
