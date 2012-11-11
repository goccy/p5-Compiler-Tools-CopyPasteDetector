$(document).ready(function(){
    //$("cpd-code").snippet("perl");
    // Finds <pre> elements with the class "htmlCode"
    // and snippet highlights the HTML code within.
    //$("pre.styles").snippet("css",{style:"greenlcd"});
    // Finds <pre> elements with the class "styles"
    // and snippet highlights the CSS code within
    // using the "greenlcd" styling.
    $("pre").snippet("perl", {style:"emacs", menu:false, transparent:true, showNum:false});
    // Finds <pre> elements with the class "js"
    // and snippet highlights the JAVASCRIPT code within
    // using a random style from the selection of 39
    // with a transparent background
    // without showing line numbers.
});
