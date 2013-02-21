function create_chart(name, color, title, timestamp, utc_time, description, data, userData)
{
    var new_data = new Array();
    var size = data.length;
    for (var i = 0; i < size; i++) {
        var utc = Number(utc_time[i]);
        data[i] = Number(data[i]);
        new_data.push([utc, data[i]]);
    }
    new Highcharts.StockChart({
        colors: [color],
        chart: {renderTo: name, borderWidth: 2, borderColor: color},
        title: {text: title},
		rangeSelector : {selected : 1, inputEnabled: false},
        navigator : {outlineColor: color},
		tooltip : {valueDecimals : 2, userData: userData,
            formatter: function() {
                var type = this.series.name;
                var value = this.y;
                var o = this.series.tooltipOptions.userData;
                var x = this.x;
                for (; o[x] == null; x += 1000) {}
                var idx = o[x];
                var time = o.time[idx];
                var rev = o.rev[idx];
                var comment = o.comment[idx];
                var view = "<strong>" + time;
                view += "<br><strong>" + type + ": " + value;
                if (comment) {
                    view += "<br><strong>" + "Comment : " + comment;
                }
                return view;
            },
            shared: false,
		},
        plotOptions: {series: {cursor: 'pointer', point: {events: {
            click: function() {
                var o = this.series.tooltipOptions.userData;
                var x = this.x;
                for (; o[x] == null; x += 1000) {}
                var idx = o[x];
                var rev = o.rev[idx];
                popup_gitweb_window(rev);
            }
        }}, marker: {lineWidth: 1}}, area: {dataGrouping: {enabled:false}}},
        series: [{
            name: description,
            data: new_data,
            color: color,
            step: true,
			shadow : true,
			threshold : null,
			fillColor : {linearGradient : {x1: 0, y1: 0, x2: 0, y2: 1},
				stops : [[0, Highcharts.getOptions().colors[0]], [1, 'rgba(0,0,0,0)']]
            }
        }],
    });
}

function make_graph(name, data)
{
    var userData = {rev : data.rev};
    var utc = data.utc;
    var size = utc.length;
    for (var i = 0; i < size; i++) {
        var utc_n = Number(utc[i]);
        userData[utc_n] = i;
    }
    userData["time"] = data.time;
    create_chart(name + '_coverage', '#4572A7', 'Coverage', data.time, data.utc, '', data.coverage, userData);
    create_chart(name + '_ss', '#AA4643', 'Self Similarity', data.time, data.utc, '', data.ss, userData);
    create_chart(name + '_ads', '#92A8CD', 'Another Directories Similarity', data.time, data.utc, '', data.ads, userData);
    create_chart(name + '_neighbor', '#89A54E', 'Neigbor', data.time, data.utc, '', data.neighbor, userData);
}



window.onload = function() {
    var namespace = $("#namespace-title").data("name");
    var req = new Object();
    req["namespace"] = namespace;
    $.get('namespace/get_graph_data', req, function(response) {
        make_graph(namespace, JSON.parse(response)[0]);
    });
}
