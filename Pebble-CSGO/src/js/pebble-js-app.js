var server_ip = localStorage.getItem("ipaddr");

////////////////////////////////
// Event Listeners for pebble //
////////////////////////////////

Pebble.addEventListener('ready', function () {
    // console.log("Pebble JS ready.");
    if (!server_ip)
        // console.log("server_ip is not configured.");

        // Initialize app data with initial data
        retrieveCSGO();
});

Pebble.addEventListener('showConfiguration', function (e) {
    // Show config page
    // console.log("Showing page...");

    var url;
    if (server_ip) {
        url = 'http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html?' +
            encodeURIComponent(JSON.stringify({"ipaddr_default" : server_ip.substring(7)}));
    }
    else {
        url = 'http://shotaroikeda.github.io/Pebble-CSGO/configure/conf.html?' +
            encodeURIComponent(JSON.stringify({"ipaddr_default" : "xxx.xxx.x.x:yyyyy"}));
    }

    // console.log(url);
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
    // console.log("Page closed.");
    var jsObj = JSON.parse(e.response);

    if (jsObj["ipaddr"])
    {
        server_ip = jsObj["ipaddr"].match("^(http://)") == null ?
            "http://" + jsObj["ipaddr"] : jsObj["ipaddr"];

        localStorage.setItem("ipaddr", server_ip);
    }
});

Pebble.addEventListener('appmessage', function (e) {
    // retrieve CSGO data from server
    retrieveCSGO();
});

/////////////////////////////////
// App Message CS GO Functions //
/////////////////////////////////

function retrieveCSGO() {
    // console.log("retrieving CSGO Data");
    if (!server_ip) {
        // Bad Initial Configuration
        console.log("Server's IP Address is not properly configured!");
        Pebble.sendAppMessage({
            'CSGO_VALID_SERVER_IP': 0,
            'CSGO_TIME_SINCE_LAST_UPDATE': 0,
            'CSGO_MAP_MODE': "none",
            'CSGO_ROUND_PHASE': "none",
            'CSGO_BOMB_STATUS': "none"
        });
        return;
    }

    var req = new XMLHttpRequest();
    req.open('GET', server_ip);

    req.onload = function () {
        if (req.readyState == 4) {
            if (req.status == 200) {
                // CSGO's Json object is changed around in here.
                var csgo_data = JSON.parse(req.responseText);

                // MARK: Time diff might be screwed up.
                var timeDiff = Math.round((new Date()).getTime() / 1000) - csgo_data.provider["timestamp"];
                // console.log("Time difference: " + timeDiff);

                var csgo_curr_gamemode  = csgo_data.map["mode"];
                var csgo_round_phase    = csgo_data.round["phase"];
                var csgo_bomb_status    = csgo_data.round["bomb"];
                var csgo_ct_score       = csgo_data.map.team_ct.score;
                var csgo_t_score        = csgo_data.map.team_t.score;

                // console.log("Current GameMode: " + csgo_curr_gamemode);
                // console.log("Current RoundPhase: " + csgo_round_phase);
                // console.log("CUrrent BombStatus: " + csgo_bomb_status);

                if (csgo_round_phase == "over" || csgo_round_phase == "freezetime")
                    console.log("buzz!");

                Pebble.sendAppMessage({
                    'CSGO_VALID_SERVER_IP'              : 1,
                    'CSGO_TIME_SINCE_LAST_UPDATE'       : timeDiff,
                    'CSGO_MAP_MODE'                     : csgo_curr_gamemode,
                    'CSGO_ROUND_PHASE'                  : csgo_round_phase,
                    'CSGO_BOMB_STATUS'                  : csgo_bomb_status,
                    'CSGO_TEAM_CT_SCORE'                : csgo_ct_score,
                    'CSGO_TEAM_T_SCORE'                 : csgo_t_score
                });
            }
        }
    };

    req.send();
}
