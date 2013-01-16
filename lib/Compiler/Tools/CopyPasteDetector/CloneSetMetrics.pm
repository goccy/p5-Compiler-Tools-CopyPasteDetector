package Compiler::Tools::CopyPasteDetector::CloneSetMetrics;
use strict;
use warnings;
use Data::Dumper;

sub new {
    my $class = shift;
    my $clone_set = shift;
    my $self = {clone_set => $clone_set};
    return bless($self, $class);
}

sub get_length_score {
    my ($self) = @_;
    return $self->{clone_set}->[0]->{token_num};
}

sub get_population_score {
    my ($self) = @_;
    return scalar @{$self->{clone_set}};
}

sub get_nif_score {
    my ($self) = @_;
    my %names;
    map {
        $names{$_->{file}}++;
    } @{$self->{clone_set}};
    return scalar keys %names;
}

sub get_radius_score {
    my ($self) = @_;
    my %names;
    map {
        $names{$_->{file}}++;
    } @{$self->{clone_set}};
    my @paths = keys %names;
    my $paths_num = scalar @paths;
    my %shared_parent_map;
    my $max_order = 0;
    foreach my $path (@paths) {
        my @dirs = split(m|/|, $path);
        my $order = 0;
        my $absolute_path = '';
        foreach (@dirs) {
            $absolute_path .= "/$_";
            $shared_parent_map{$absolute_path}->{name} = $_;
            $shared_parent_map{$absolute_path}->{shared}++;
            $shared_parent_map{$absolute_path}->{order} = $order;
            $order++;
        }
        $max_order = $order if ($max_order < --$order);
    }
    my @sorted_shared_parents = sort {
        $shared_parent_map{$b}->{order} <=> $shared_parent_map{$a}->{order}
    } grep {
        $shared_parent_map{$_}->{shared} == $paths_num
    } keys %shared_parent_map;
    my $last_shared_parent_name = $sorted_shared_parents[0];
    my $parent_order = $shared_parent_map{$last_shared_parent_name}->{order};
    my $radius = $max_order - $parent_order;
    return $radius;
}

sub get_kind_of_token_score {
    my ($self) = @_;
    return undef;
    #return $self->{info}->{kind_num};
}

sub get_score {
    my ($self) = @_;
    return {
        length        => $self->get_length_score(),
        population    => $self->get_population_score(),
        nif           => $self->get_nif_score(),
        radius        => $self->get_radius_score(),
        kind_of_token => $self->get_kind_of_token_score()
    };
}

1;
