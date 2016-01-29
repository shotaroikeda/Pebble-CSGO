var http = require('http');

///////////////////////////////////////////
// Prerequisites for running the server. //
///////////////////////////////////////////

// Obtain local ip address to create an appropriate server.

var os = require('os');
var ifaces = os.networkInterfaces();

var myHost = null;

Object.keys(ifaces).forEach(function (ifname) {
    ifaces[ifname].forEach(function (iface) {
        if (iface.internal || iface.family == 'IPv6')
            return;
        else
        {
            if (!myHost)
                myHost = iface.address;
        }
    });
});

/////////////////////////
// Filtering functions //
/////////////////////////

function filterCSGOData(dataObj)
{
    var dataMap = {
	provider : {},
	map : {},
	round : {},
	/*
	  As of now, not required...maybe in the future for gui fanciness
	  player :
	*/ 
    };

    if (dataObj.provider)
    {
	dataMap.provider["timestamp"] = dataObj.provider.timestamp;
    }
    if (dataObj.map)
    {
	dataMap.map["mode"] = dataObj.map.mode || "none";
    }

    if (dataObj.round)
    {
	dataMap.round["phase"] = dataObj.round.phase || "none";
	dataMap.round["bomb"] = dataObj.round.bomb || "none";
    }

    return dataMap;
}

////////////////////
// Server Portion //
////////////////////

// TODO: Find an open port to use
port = 8099;

var dataObj = null;

server = http.createServer( function(req, res) {
    if (req.method == 'POST')
    {
        res.writeHead(200, {'Content-Type': 'text/html'});

        var body = '';
        req.on('data', function (data) {
            body += data;
        });

        req.on('end', function () {
            dataObj = JSON.parse(body);
        });
    }
    else
    {
        res.writeHead(200, {'Content-Type': 'application/json'});

        if (dataObj)
        {
            res.end(JSON.stringify(filterCSGOData(dataObj)));
        }
        else
        {
            res.end(JSON.stringify({ data: null }));
        }
    }
});

try {
    // Listen to both external ips and internal ips
    // External ip used by Device connected with Pebble
    server.listen(port, myHost);

    // Internal ip used by CSGO to connect.
    server.listen(port);

    // Verbose statements
    console.log('Listening at http://' + myHost + ':' + port);
    console.log('Please go to the pebble watch settings and insert: http://' + myHost + ':' + port);
}
catch (e)
{
    console.log('Could not obtain an open port at ' + port + '. Please try again.');
}
