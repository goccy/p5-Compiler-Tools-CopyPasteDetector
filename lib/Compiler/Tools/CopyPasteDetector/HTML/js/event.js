function bind_scroll_event()
{
    var proximity = 0.05;
    $(window).bind("scroll", function() {
        var o = $(this);
        var height = $(document).height();
        var pos = $(o).height() + $(o).scrollTop();
        if ((height - pos) / height <= proximity) {
            if (!$(o).data('loading') && !$(".popup-window")[0]) {
                $(o).data('loading', true);
                if (!g_loading_o) g_loading_o = o;
                if ($("#length")[0] && g_clone_sets &&
                    g_clone_sets.length > g_init_num + load_rows_size) {
                    make_clone_set_table();
                    $(o).data('loading', false);
                } else if ($("#coverage")[0] && g_file_metrics &&
                           g_file_metrics.length > g_init_num + load_rows_size) {
                    make_file_metrics_table();
                    $(o).data('loading', false);
                }
            }
        }
    });
}

function bind_key_event()
{
	$(window).keyup(function(e){
        var text = $(".cpd-filter-box").find("input[type='text']").val();
        if (prev_text && text != prev_text) {
            filter_regexp = new RegExp(text);
            var contents_type = $(".selected h3").html().replace(/\s/g, "_");
            var metrics_type = $(".tab-selected").html();
            switch (contents_type) {
            case "clone_set_metrics":
                if (!g_clone_sets) break;
                $(".cpd-main-table").html($("#clone_set_metrics_table_head_tmpl").tmpl({score: metrics_type}));
                g_init_num = 0;
                make_clone_set_table();
                break;
            case "file_metrics":
                if (!g_file_metrics) break;
                $(".cpd-main-table").html($("#file_metrics_table_head_tmpl").tmpl({score: metrics_type}));
                g_init_num = 0;
                make_file_metrics_table();
                break;
            case "directory_metrics":
                if (!g_directory_metrics) break;
                $(".cpd-main-table").html($("#file_metrics_table_head_tmpl").tmpl({score: metrics_type}));
                g_init_num = 0;
                make_directory_metrics_table();
                break;
            default:
                break;
            }
        }
        prev_text = text;
	});
}

function bind_click_event()
{
    $("#scattergram").click(function() {
        switch_to_scattergram_contents();
    });
    $("#clone_set_metrics").click(function() {
        switch_to_clone_set_metrics_contents();
    });
    $("#file_metrics").click(function() {
        switch_to_file_metrics_contents();
    });
    $("#directory_metrics").click(function() {
        switch_to_directory_metrics_contents();
    });
    $("#filetree").click(function() {
        switch_to_file_tree_contents();
    });
}

function bind_global_events()
{
    bind_scroll_event();
    bind_key_event();
    bind_click_event();
}
