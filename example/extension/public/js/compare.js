/* Compare between Namespaces */
function make_cmp_graph(type, time)
{
    var req = new Object();
    req["type"] = type;
    req["time"] = time;
    $.get("compare/get_compare_data", req, function(response) {
        var res = JSON.parse(response)[0];
        var title = "";
        switch (type) {
        case "ads":
            title = "another directories similarity";
            break;
        case "ss":
            title = "self similarity";
            break;
        default:
            title = type;
            break;
        }            
        create_sorted_chart(title + " (" + time + ")", res.name, res.score, {diff: res.diff});
        var xaxis = $(".highcharts-axis-labels")[0];
        var diff = res.diff;
        var i = 0;
        if (diff) {
            $(xaxis).find("text").each(function() {
                if (diff[i] < 0) {
                    $(this).find("tspan").attr("fill", "blue");
                } else if (diff[i] > 0) {
                    $(this).find("tspan").attr("fill", "red");
                }
                i++;
            });
        }
    });
}

function create_sorted_chart(type, name, data, userData)
{
    new Highcharts.Chart({
	    colors: ['#DB843D'], chart: {renderTo: 'sorted_graph_area', type: 'bar'},
        title: {text: type},
        xAxis: {categories: name},
        yAxis: {title: {text: ''}},
        tooltip: {
            formatter: function() {
                var type = this.series.name;
                var value = this.y;
                var view = type + ": " + value;
                var idx = this.point.x;
                var o = this.series.options.userData;
                if (o.diff) {
                    var diff = o.diff[idx];
                    if (diff > 0) {
                        view += "<br><font color='red'>" + diff + " point down</font><br>";
                    } else if (diff < 0) {
                        view += "<br><font color='blue'>" + -diff + " point up</font><br>";
                    }
                }
                return view;
            },
            useHTML: true,
        },
        series: [{name: type, data: data, userData: userData}]
    });
}

window.onload = function() {
    $.get("compare/get_recent_time", function(time) {
        var type = $("#sorted_graph_area").data("name");
        make_cmp_graph(type, time);
    });
    $("#timestamp-list-area div").on("click", function() {
        var type = $("#sorted_graph_area").data("name");
        var time = $(this).data("name");
        make_cmp_graph(type, time);
    });
}
