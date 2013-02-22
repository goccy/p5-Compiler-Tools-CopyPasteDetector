var g_file_tree = null;
var g_clone_sets = null;
var g_file_metrics = null;
var g_directory_metrics = null;
var g_scattergram = null;
var g_loading_o = null;
var prev_text = null;
var g_init_num = 0;
var load_rows_size = 50;
var orig_file_tree_area_height;
var cached_scattergram = null;
var filter_regexp = new RegExp();

$("#goto_summary").click(function() {
    location.href = "home";
});
