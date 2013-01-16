function footer() {
	var footerId = "footer";
	function footerFixed(){
		var dh = document.getElementsByTagName("body")[0].clientHeight;
		document.getElementById(footerId).style.top = "0px";
		var ft = document.getElementById(footerId).offsetTop;
		var fh = document.getElementById(footerId).offsetHeight;
		if (window.innerHeight) {
			var wh = window.innerHeight;
		} else if (document.documentElement && document.documentElement.clientHeight != 0){
			var wh = document.documentElement.clientHeight;
		}
		if (ft+fh<wh) {
			document.getElementById(footerId).style.position = "relative";
			document.getElementById(footerId).style.top = (wh-fh-ft-1)+"px";
		}
	}
	function checkFontSize(func){
		var e = document.createElement("div");
		var s = document.createTextNode("S");
		e.appendChild(s);
		e.style.visibility="hidden"
		e.style.position="absolute"
		e.style.top="0"
		document.body.appendChild(e);
		var defHeight = e.offsetHeight;
		function checkBoxSize(){
			if (defHeight != e.offsetHeight) {
				func();
				defHeight= e.offsetHeight;
			}
		}
		setInterval(checkBoxSize,1000)
	}
	function addEvent(elm,listener,fn) {
		try {
			elm.addEventListener(listener,fn,false);
		} catch(e) {
			elm.attachEvent("on"+listener,fn);
		}
	}
	addEvent(window,"load",footerFixed);
	addEvent(window,"load",function(){
		checkFontSize(footerFixed);
	});
	addEvent(window,"resize",footerFixed);
}

footer();
