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
            myHost = iface.address;
    });
});

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
            res.end(JSON.stringify(dataObj));
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
