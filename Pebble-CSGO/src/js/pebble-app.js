////////////////////////////////
// Event Listeners for pebble //
////////////////////////////////

Pebble.addEventListener('showConfiguration', function (e) {
    // Show config page
    Pebble.openURL('http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html')
});

Pebble.addEventListener('configurationClosed', function (e) {
    console.log("Closed with " + e.response);
});
