function load_clone_set_data()
{
    $.ajax({
        type: "GET",
        url:  "js/clone_set_data.json",
        async: false,
        success: function(clone_sets) {
            g_clone_sets = clone_sets;
        }
    });
}

function load_file_data()
{
    $.get("js/file_data.json", function(file_metrics) {
        g_file_metrics = file_metrics;
    });
}

function load_directory_data()
{
    $.get("js/directory_data.json", function(directory_metrics) {
        g_directory_metrics = directory_metrics;
    });
}

function make_file_metrics_table()
{
    var file_metrics = g_file_metrics;
    var table_data = new Array();
    var init_num = g_init_num;
    var size = (file_metrics.length > init_num + 100) ? init_num + 100 : file_metrics.length;
    for (var i = init_num; i < size; i++) {
        var file = file_metrics[i];
        var score = file.score;
        var name = file.name;
        if (name.match(filter_regexp)) {
            var data = new Object();
            data.name = name;
            data.score = score;
            table_data.push(data);
        }
    }
    g_init_num = size;
    $("#file_metrics_table_tmpl").tmpl(table_data).appendTo(".cpd-main-table");
    if (size >= 100) {
        setTimeout(function() {
            var main_menu_area_height = $(".cpd-main-menu-area").height();
            var file_tree_area_height = $(".cpd-filt-tree-area").height();
            if (file_tree_area_height > main_menu_area_height) {
                $(".cpd-filt-tree-area").css({ height: main_menu_area_height + "px" });
            }
            $(".cpd-main").css({ height: main_menu_area_height + "px" });
        }, 0);
    }
}

function make_directory_metrics_table()
{
    var directory_metrics = g_directory_metrics;
    var table_data = new Array();
    var init_num = g_init_num;
    var size = (directory_metrics.length > init_num + 100) ? init_num + 100 : directory_metrics.length;
    for (var i = init_num; i < size; i++) {
        var dir = directory_metrics[i];
        var score = dir.score;
        var name = dir.name;
        if (name.match(filter_regexp)) {
            var data = new Object();
            data.name = name;
            data.score = score;
            table_data.push(data);
        }
    }
    g_init_num = size;
    $("#file_metrics_table_tmpl").tmpl(table_data).appendTo(".cpd-main-table");
    if (size >= 100) {
        setTimeout(function() {
            var main_menu_area_height = $(".cpd-main-menu-area").height();
            var file_tree_area_height = $(".cpd-filt-tree-area").height();
            if (file_tree_area_height > main_menu_area_height) {
                $(".cpd-filt-tree-area").css({ height: main_menu_area_height + "px" });
            }
            $(".cpd-main").css({ height: main_menu_area_height + "px" });
        }, 0);
    }
}

function make_clone_set_table()
{
    var clone_sets = g_clone_sets;
    var table_data = new Array();
    var init_num = g_init_num;
    var added_num = 0;
    for (var i = init_num; added_num < 100; i++) {
        if (clone_sets.length <= i) break;
        var clone_set = clone_sets[i];
        var metrics = clone_set.metrics;
        var score = clone_set.score;
        var set = clone_set.set;
        var data = new Object();
        data.score = score;
        var added_flag = false;
        for (var j = 0; j < set.length; j++) {
            if (set[j].file.match(filter_regexp)) {
                added_flag = true;
                break;
            }
        }
        if (added_flag) {
            if (set.length > 4) {
                data.capacity_over = 1;
                data.short_location = set.slice(0, 3);
            }
            if (set.length > 0) {
                data.location = set;
                data.classname = init_num;
                data.src = set[0].src;
                data.hash = set[0].hash;
                table_data.push(data);
                added_num++;
            }
        }
    }
    g_init_num = init_num + 100;
    $("#cpd_main_table_tmpl").tmpl(table_data).appendTo(".cpd-main-table");
    setTimeout(function() {
        $(".cpd-main").css({ height: $(".cpd-main-menu-area").height() + "px" });
    }, 0);
    $("pre." + init_num).snippet("perl", {style:"emacs", menu:false, transparent:true, showNum:false});
    $(".cpd-hit").hover(function(e) {
        if (!$(this).hasClass("expanded")) {
            var detail_location = $(this).find("ul").prev()[0];
            var short_location = $(this).find("ul")[0];
            var hidden_location = $(this).find(".cpd-hidden-location")[0];
            var div = document.createElement("div");
            div.className = "cpd-hidden-location";
            div.appendChild(detail_location);
            div.appendChild(short_location);
            var elem = $(hidden_location).clone();
            elem.append($(div));
            $(this).html(elem.html());
            $(this).addClass("expanded");
        }
    }, function(e) {
        if ($(this).hasClass("expanded")) {
            var detail_location = $(this).find("ul").prev()[0];
            var short_location = $(this).find("ul")[0];
            var hidden_location = $(this).find(".cpd-hidden-location")[0];
            var div = document.createElement("div");
            div.className = "cpd-hidden-location";
            div.appendChild(detail_location);
            div.appendChild(short_location);
            var elem = $(hidden_location).clone();
            elem.append($(div));
            $(this).html(elem.html());
            $(this).removeClass("expanded");
        }
    });
    $(".cpd-location,.cpd-hit").click(function(e) {
        var div = $(this).find("div")[0];
        var hash = $(div).attr("class");
        var ul = $(div).siblings("ul");
        var filenames = new Array();
        var locations = new Array();
        ul.find("li").each(function() {
            var location = $(this).html();
            var name = location.split(":")[0].replace(/\s+$/, "");
            locations.push(location);
            filenames.push(name)
        });
        var frames = document.createElement("div");
        for (var i in filenames) {
            $.ajax({
                type: "GET",
                url:  "data/" + filenames[i],
                async: false,
                success: function(response) {
                    var code = document.createElement("code");
                    code.innerHTML = response;
                    var pre = document.createElement("pre");
                    pre.appendChild(code);
                    var h = document.createElement("h3");
                    h.innerHTML = locations[i];
                    var src_view = document.createElement("div");
                    src_view.className = "src-view";
                    src_view.appendChild(pre);
                    var contents = document.createElement("div");
                    contents.className = "detail-contents";
                    contents.appendChild(h);
                    contents.appendChild(src_view);
                    $(contents).css({zoom: "70%"});
                    frames.appendChild(contents);
                }
            });
        }
        popup_fileview_window(frames);
        $(".popup-window pre").snippet("perl", {style:"emacs", menu:false, transparent:true, showNum:true});
        $(".code-clone-start").each(function() {
            var li = $(this).parent("li");
            var src = li.find("code").html();
            var classname = $(this).attr("class");
            li.html(src + $(this).html()).before("<div class='" + classname + "'/>");
        });
        $(".code-clone-end").each(function() {
            var classname = $(this).attr("class");
            var li = $(this).parent("li");
            var src = li.find("code").html() || "";
            li.html(src + $(this).html()).before("<div class='" + classname + "'/>");
        });
        var starts = $(".code-clone-start");
        var ends = $(".code-clone-end");
        for (var i = 0; i < starts.length; i++) {
            $(starts[i]).nextUntil($(ends[i])).css({"background-color" : "pink"});
        }
        $(".src-view").each(function() {
            var offset = $($(this).find($(".code-clone-start." + hash))[0]).offset().top;
            $(this).scrollTop(offset - $(window).scrollTop() - 200);
        });
    });
}

function change_order(contents_type, metrics_type)
{
    var replaced_metrics_name = metrics_type.replace(/\s/g, "_");
    var all_data;
    switch (contents_type) {
    case "clone_set_metrics":
        all_data = g_clone_sets;
        break;
    case "file_metrics":
        all_data = g_file_metrics;
        break;
    case "directory_metrics":
        all_data = g_directory_metrics;
        break;
    default:
        break;
    }
    var init_num = 0;
    for (var i = 0; i < all_data.length; i++) {
        var data = all_data[i];
        var metrics = data.metrics;
        all_data[i].score = metrics[replaced_metrics_name];
    }
    all_data.sort(
	    function (a, b) {
		    var a = a["score"];
		    var b = b["score"];
		    if (parseFloat(b) < parseFloat(a)) return -1;
		    if (parseFloat(b) > parseFloat(a)) return 1;
		    return 0;
    });
    switch (contents_type) {
    case "clone_set_metrics":
        g_clone_sets = all_data;
        $(".cpd-main-table-wrapper").html($("#clone_set_metrics_table_head_tmpl").tmpl({score: metrics_type}));
        make_clone_set_table();
        break;
    case "file_metrics":
        g_file_metrics = all_data;
        $(".cpd-main-table-wrapper").html($("#file_metrics_table_head_tmpl").tmpl({score: metrics_type}));
        make_file_metrics_table();
        break;
    case "directory_metrics":
        g_directory_metrics = all_data;
        $(".cpd-main-table-wrapper").html($("#file_metrics_table_head_tmpl").tmpl({score: metrics_type}));
        make_directory_metrics_table();
        break;
    default:
        break;
    }
}
