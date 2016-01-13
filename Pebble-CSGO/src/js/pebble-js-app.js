////////////////////////////////
// Event Listeners for pebble //
////////////////////////////////
Pebble.addEventListener('ready', function () {
    console.log("Pebble JS ready.");
});

Pebble.addEventListener('showConfiguration', function (e) {
    // Show config page
    console.log("Showing page...");
    Pebble.openURL('http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html');
});

Pebble.addEventListener('webviewclosed', function (e) {
    console.log("Page closed.");
    console.log("Closed with " + e.response);
});
