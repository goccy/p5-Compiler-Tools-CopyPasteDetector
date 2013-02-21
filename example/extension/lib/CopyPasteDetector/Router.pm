package CopyPasteDetector::Router;
use strict;
use warnings;

sub dispatch {
    my $routes = shift;
    $routes->get('/')->to('home#index');
    $routes->get('/home')->to('home#index');
    $routes->get('/coverage')->to('compare#coverage');
    $routes->get('/ads')->to('compare#another_directories_similarity');
    $routes->get('/ss')->to('compare#self_similarity');
    $routes->get('/neighbor')->to('compare#neighbor');
    $routes->get('/namespace')->to('namespace#index');
    $routes->get('/namespace/get_graph_data')->to('namespace#get_graph_data');
    $routes->get('/compare/get_compare_data')->to('compare#get_compare_data');
    $routes->get('/compare/get_recent_time')->to('compare#get_recent_time');
}

1;
