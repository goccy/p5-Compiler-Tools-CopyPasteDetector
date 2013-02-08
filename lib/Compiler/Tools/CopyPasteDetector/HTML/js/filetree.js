function make_file(name, isStrong)
{
    var li = document.createElement("li");
    var span = document.createElement("span");
    var a = document.createElement("a");
    if (isStrong) {
        var strong = document.createElement("strong");
        strong.innerHTML = name;
        a.appendChild(strong);
        span.className = "folder";
    } else {
        a.innerHTML = name;
        span.className = "file";
    }
    span.appendChild(a);
    li.appendChild(span);
    return li;
}

function make_directory(name, list)
{
    var li = make_file(name, 1);
    var ul = document.createElement("ul");
    for (var i = 0; i < list.length; i++) {
        ul.appendChild(list[i]);
    }
    li.appendChild(ul);
    return li;
}

function make_tree(o)
{
    var array = new Array();
    var name = o.name;
    var children = o.children;
    for (var i = 0; i < children.length; i++) {
        var child = children[i];
        if (child.children && child.children.length > 0) {
            var list = make_tree(child);
            if (child.name.match(/\.p[lm]/)) {
                array.push(make_file(child.name, 0));
            } else {
                array.push(make_directory(child.name, list));
            }
        } else {
            if (child.name) array.push(make_file(child.name, 0));
        }
    }
    return array;
}

function load_filetree_data()
{
    $.get("js/output.json", function(file_tree_data) {
        var tree = make_tree(file_tree_data);
        var root = make_directory(file_tree_data.name, tree);
        g_file_tree = root;
    });
}

function setup_sourcecode_highlight()
{
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
}

function make_file_tree() {
    var root = g_file_tree;
    var ul = document.createElement("ul");
    ul.className = "filetree";
    ul.appendChild(root);
    $(".cpd-file-tree").append(ul);
    $(".cpd-file-tree").treeview({
		collapsed: false,
        unique: true,
		animated: "fast",
		control:"#sidetreecontrol",
		persist: "location"
	});
    setTimeout(function() {
        var main_menu_area_height = $(".cpd-main-menu-area").height();
        $(".cpd-main").css({ height: main_menu_area_height + "px" });
    }, 0);
}

function popup_fileview_window(body)
{
    var width = $(window).width();
    var height = $(window).height();
    var window_width = width * 0.9;
    var window_height = height * 0.9;
	var popup_window = document.createElement("div");
    var top = $(window).scrollTop();
    var left = $(window).scrollLeft();
    body.style.width = window_width + "px";
    body.style.height = window_height + "px";

	popup_window.appendChild(body);

    var x = (width - window_width) / 2;
    var y = (height - window_height) / 2;
	popup_window.style.left = left + x + "px";
	popup_window.style.top = top + y + "px";
	popup_window.style.width = window_width + "px";
	popup_window.style.height = window_height + "px";
	popup_window.className = "popup-window";
	$(popup_window).css({"overflow-x": "hidden"});

	var filter = document.createElement("div");
	filter.className = "black-filter";
	filter.style.width = "100%";
	filter.style.height = top + $(document).height() + "px";

    $("body").css({overflow: "hidden"});
	$("body").append(filter);
	$("body").append(popup_window);
	$(".black-filter").click(function(e) {
		$(this).remove();
		$(".popup-window").remove();
        $("body").css({overflow: "auto"});
	});
}
