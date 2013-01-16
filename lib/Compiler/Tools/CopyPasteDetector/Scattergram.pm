package Compiler::Tools::CopyPasteDetector::Scattergram;
use strict;
use warnings;
use POSIX;
use HTML::Template;
use Data::Dumper;

sub new {
    my $class = shift;
    my $root = shift;
    my $self = {
        root => $root,
        name_elements_map => +{},
        x => 100,
        y => 100,
        width  => 20,
        height => 20,
        objects => {
            paths => [],
            rects => [],
            texts => []
        }
    };
    return bless($self, $class);
}

sub render {
    my ($self) = @_;
    $self->__make_scattergram_objects();
    delete $self->{root};
    delete $self->{name_elements_map};
    my $tmpl = <<'__TMPL__';
<svg x="0" y="0" width="1200" height="1200">
  <TMPL_LOOP NAME=paths>
    <path d="M<TMPL_VAR NAME=x1>,<TMPL_VAR NAME=y1>L<TMPL_VAR NAME=x2>,<TMPL_VAR NAME=y2>" stroke="<TMPL_VAR NAME=color>" fill="none" stroke-width="1"></path>
  </TMPL_LOOP>
  <TMPL_LOOP NAME=rects>
    <rect class="<TMPL_VAR NAME=class>" fill-opacity="<TMPL_VAR NAME=fill_opacity>" x="<TMPL_VAR NAME=x>" y="<TMPL_VAR NAME=y>" width="<TMPL_VAR NAME=width>" height="<TMPL_VAR NAME=height>" r="0" rx="0" ry="0" fill="<TMPL_VAR NAME=color>" data-name="<TMPL_VAR NAME=name>" stroke="<TMPL_VAR NAME=stroke>" stroke-width="<TMPL_VAR NAME=stroke_width>"></rect>
  </TMPL_LOOP>
  <TMPL_LOOP NAME=texts>
    <text id="<TMPL_VAR NAME=id>" text-anchor="middle" display="none" stroke="none" fill="#000000" font-family="Arial" x="<TMPL_VAR NAME=x>" y="<TMPL_VAR NAME=y>" font-size="<TMPL_VAR NAME=font_size>"><tspan dy="5"><TMPL_VAR NAME=name></tspan></text>
  </TMPL_LOOP>
</svg>
__TMPL__
    my $t = HTML::Template->new(scalarref => \$tmpl);
    $t->param($self->{objects});
    return $t->output;
}

sub __get_random_color {
    my $rand_color = "#";
    for (my $i = 0; $i < 3; $i++) {
        my $rand = floor(rand() * 155);
        $rand += 100;
        $rand_color .= sprintf("%X", $rand);
    }
    return $rand_color;
}

sub __get_children_size {
    my ($self, $root) = @_;
    my $ret = 0;
    my $children = $root->{children};
    foreach my $child (@$children) {
        if (ref $child->{children} eq 'ARRAY') {
            $ret += $self->__get_children_size($child);
        } else {
            my $clone_num = scalar keys %$child;
            $ret += $clone_num;
        }
    }
    return $ret;
}

sub __draw_directory_area {
    my ($self, $root, $offset) = @_;
    my $x = $self->{x};
    my $y = $self->{y};
    my $width = $self->{width};
    my $height = $self->{height};
    my $children = $root->{children};
    my $i = $offset;
    my $texts = $self->{objects}->{texts};
    my $rects = $self->{objects}->{rects};
    my $name_elements_map = $self->{name_elements_map};
    foreach my $child (@$children) {
        if (defined($child->{children})) {
            my $files_num = $self->__get_children_size($child);
            my $length = $width * $files_num;
            my $draw_pos = $i * $width;
            my $name = $child->{name};
            my $replaced_name = $name;
            $replaced_name =~ s|[/.]|_|g;
            unless (exists $name_elements_map->{$name}) {
                push(@$texts, {
                    x           => $x + $draw_pos,
                    y           => $y + $draw_pos,
                    name        => $name,
                    id          => $replaced_name,
                    'font_size' => '14px',
                });
                $name_elements_map->{$name}++;
            }
            my $rcolor = $self->__get_random_color();
            my $stroke_width = 3.0;
            $stroke_width = $width if ($width < $stroke_width);
            push(@$rects, {
                x              => $x + $draw_pos,
                'y'            => $y + $draw_pos,
                width          => $length,
                height         => $length,
                stroke         => $rcolor,
                color          => $rcolor,
                name           => $replaced_name,
                class          => 'area',
                fill_opacity   => '0',
                'stroke_width' => $stroke_width
            });
            $self->__draw_directory_area($child, $i);
            $i += $files_num;
        } else {
            my $clone_num = scalar keys %$child;
            $i += $clone_num;
        }
    }
}

sub __make_scattergram_objects {
    my ($self) = @_;
    my $size = 0;
    my $children = $self->__serialize_children($self->__traverse($self->{root}));
    $size += scalar keys %$_ foreach (@$children);
    my $change_length = 1200 / $size;
    my $x = $self->{x};
    my $y = $self->{y};
    my $width = $self->{width};
    my $height = $self->{height};
    $width = $change_length if ($change_length < $width);
    $height = $change_length if ($change_length < $height);
    $self->{width} = $width;
    $self->{height} = $height;
    my $pos_x = $x + $size * $width;
    my $pos_y = $y + $size * $height;
    my $paths = $self->{objects}->{paths};
    push(@$paths, {x1 => $x, y1 => $y, x2 => $x, y2 => $pos_y, color => '#808080'});
    push(@$paths, {x1 => $x, y1 => $y, x2 => $pos_x, y2 => $y, color => '#808080'});
    my $i = 0;
    foreach my $child (@$children) {
        foreach my $hash (keys %$child) {
            my $count = $child->{$hash};
            $self->__add_rect($children, $i, $hash);
            $i++;
        }
        my $pos_x = $x + $i * $width;
        my $pos_y = $y + $size * $height;
        push(@$paths, {x1 => $pos_x, y1 => $y, x2 => $pos_x, y2 => $pos_y, color => '#808080'});
        push(@$paths, {x1 => $x, y1 => $pos_x, x2 => $pos_y, y2 => $pos_x, color => '#808080'});
    }
    $self->__draw_directory_area($self->{root}, 0);
}

sub __add_rect {
    my ($self, $children, $i, $hash) = @_;
    my $x      = $self->{x};
    my $y      = $self->{y};
    my $width  = $self->{width};
    my $height = $self->{height};
    my $rects = $self->{objects}->{rects};
    my $hash_count = 0;
    foreach my $child (@$children) {
        foreach my $hash2 (keys %$child) {
            if ($hash eq $hash2) {
                push(@$rects, {
                    x         => $x + $i * $width,
                    y         => $y + $hash_count * $height,
                    color => 'red',
                    width     => $width,
                    height    => $height,
                    fill_opacity => 1,
                    class     => '',
                    stroke    => 'red',
                    stroke_width => 0
                });
                push(@$rects, {
                    x         => $x + $hash_count * $width,
                    y         => $y + $i * $height,
                    color => "red",
                    width     => $width,
                    height    => $height,
                    fill_opacity => 1,
                    class     => '',
                    stroke    => 'red',
                    stroke_width => 0
                });
            }
            $hash_count++;
        }
    }
}

sub __serialize_children {
    my ($self, $children_) = @_;
    my @children;
    foreach my $child (@$children_) {
        if (ref $child eq 'ARRAY') {
            my $ch = $self->__serialize_children($child);
            foreach (@$ch) {
                push(@children, $_);
            }
        } else {
            push(@children, $child);
        }
    }
    return \@children;
}

sub __traverse {
    my ($self, $root) = @_;
    my @array;
    my $name = $root->{name};
    my $children = $root->{children};
    foreach my $child (@$children) {
        if (defined($child->{children}) && scalar @{$child->{children}} >= 0) {
            push(@array, $self->__traverse($child));
        } else {
            push(@array, $child);
        }
    }
    return \@array;
}

1;
