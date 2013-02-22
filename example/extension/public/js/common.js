$(".cpd-subtitle span").on("click", function() {
    var name = $(this).data("name");
    location.href = name;
});

$("#goto_detail").on("click", function() {
    location.href = "detail";
});

$("#namespace-list-area div").on("click", function() {
    location.href = "namespace?" + "namespace=" + $(this).data("name");
});
