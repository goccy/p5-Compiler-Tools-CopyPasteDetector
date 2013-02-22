function get_auto_pixel_size(el, prop)
{
    var elem = el.clone().css({"width":"auto"}).appendTo("body");
    var ret = elem.css(prop);
    elem.remove();
    return ret;
}
