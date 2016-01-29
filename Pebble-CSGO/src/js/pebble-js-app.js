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

    var url;
    if (server_ip) {
        url = 'http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html?' +
            encodeURIComponent(JSON.stringify({"ipaddr_default" : server_ip.substring(7)}));
    }
    else {
        url = 'http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html?' +
            encodeURIComponent(JSON.stringify({"ipaddr_default" : "xxx.xxx.x.x:yyyyy"}));
    }

    console.log(url);
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
    console.log("Page closed.");
    var jsObj = JSON.parse(e.response);

    if (jsObj["ipaddr"])
    {
        server_ip = jsObj["ipaddr"].match("^(http://)") == null ?
            "http://" + jsObj["ipaddr"] : jsObj["ipaddr"];

        localStorage.setItem("ipaddr", server_ip);
    }
});

/////////////////////////////////
// App Message CS GO Functions //
/////////////////////////////////

function retrieveCSGO() {
    
}
