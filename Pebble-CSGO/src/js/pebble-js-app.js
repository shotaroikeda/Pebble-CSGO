var server_ip = localStorage.getItem("ipaddr");

////////////////////////////////
// Event Listeners for pebble //
////////////////////////////////

Pebble.addEventListener('ready', function () {
    console.log("Pebble JS ready.");
    if (!server_ip)
	console.log("server_ip is not configured.");
});

Pebble.addEventListener('showConfiguration', function (e) {
    // Show config page
    console.log("Showing page...");
    var url = 'http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html?' +
	    encodeURIComponent(JSON.stringify({"ipaddr_default" : server_ip}));
    console.log(url);
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
    console.log("Page closed.");
    var jsObj = JSON.parse(e.response);

    if (jsObj["ipaddr"])
    {
	localStorage.setItem("ipaddr", jsObj["ipaddr"]);
	server_ip = jsObj["ipaddr"];
    }
});
