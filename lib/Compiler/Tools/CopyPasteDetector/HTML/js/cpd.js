function set_scattergram_event()
{
    $("rect.area").hover(function(e) {
        var name = $(this).attr("data-name");
        var text = $('#' + name);
        text.show();
        $(this).attr({"fill-opacity": 0.5});
    }, function(e) {
        var name = $(this).attr("data-name");
        $("#" + name).hide();
        $(this).attr({"fill-opacity": 0});
    });
}

function switch_to_scattergram_contents() {
    $(".selected").removeClass("selected");
    $(".metrics-tab-group").html("");
    if (cached_scattergram) {
        $(".cpd-main-table-wrapper").html(cached_scattergram);
        set_scattergram_event();
    } else {
        $(".cpd-main-table-wrapper").html('');
        $.get("scattergram.html", function(response) {
            $(".cpd-main-table-wrapper").html(response);
            set_scattergram_event();
            cached_scattergram = $(".cpd-main-table-wrapper").html();
        });
    }
    $("#scattergram").addClass("selected");
}

function switch_to_clone_set_metrics_contents() {
    refresh();
    $(".selected").removeClass("selected");
    $(".metrics-tab-group").html($("#clone_set_metrics_tab_tmpl").tmpl());
    set_metrics_tab_event();
    $(".cpd-main-table-wrapper").html($("#clone_set_metrics_table_head_tmpl").tmpl({score: 'length'}));
    make_clone_set_table();
    $("#clone_set_metrics").addClass("selected");
}

function switch_to_file_metrics_contents() {
    refresh();
    $(".selected").removeClass("selected");
    $(".metrics-tab-group").html($("#file_metrics_tab_tmpl").tmpl());
    set_metrics_tab_event();
    $(".cpd-main-table-wrapper").html($("#file_metrics_table_head_tmpl").tmpl({score: 'coverage'}));
    make_file_metrics_table();
    $("#file_metrics").addClass("selected");
}

function switch_to_directory_metrics_contents() {
    refresh();
    $(".selected").removeClass("selected");
    $(".metrics-tab-group").html($("#directory_metrics_tab_tmpl").tmpl());
    set_metrics_tab_event();
    $(".cpd-main-table-wrapper").html($("#file_metrics_table_head_tmpl").tmpl({score: 'coverage'}));
    make_directory_metrics_table();
    $("#directory_metrics").addClass("selected");
}

function refresh()
{
    g_init_num = 0;
    if (g_loading_o) g_loading_o.data('loading', false);
}

function set_metrics_tab_event()
{
    $(".metrics-tab-group span").click(function() {
        refresh();
        $(".tab-selected").removeClass("tab-selected");
        $(this).addClass("tab-selected");
        var contents_type = $(".selected h3").html().replace(/\s/g, "_");
        var metrics_type = $(this).html();
        change_order(contents_type, metrics_type);
    });
}

function init()
{
    load_file_data();
    load_clone_set_data();
    load_directory_data();
}

$(document).ready(function() {
    init();
    $("#clone_set_metrics_table_head_tmpl").tmpl({score: 'length'}).appendTo(".cpd-main-table-wrapper");
    make_clone_set_table();
    make_file_tree();
    $(".metrics-tab-group").html($("#clone_set_metrics_tab_tmpl").tmpl());
    $("#clone_set_metrics").addClass("selected");
    set_metrics_tab_event();
    bind_global_events();
});
