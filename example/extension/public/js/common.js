$(".cpd-subtitle span").on("click", function() {
    var name = $(this).data("name");
    location.href = name;
});

$("#namespace-list-area div").on("click", function() {
    location.href = "namespace?" + "namespace=" + $(this).data("name");
});
