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
    Pebble.openURL('http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html');
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
