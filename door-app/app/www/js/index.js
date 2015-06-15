
var app = {
    // Application Constructor
    initialize: function() {
        this.bindEvents();
    },

    // Bind Event Listeners
    //
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
    },


    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    //
    // Here is also where we get access to the plugin objects
    onDeviceReady: function() {

        // Create a web socket for connecting to the door
        chrome.sockets.udp.create({}, function (createInfo) {
            console.log('Created socket iwth id: ' + createInfo.socketId);

            // Need to bind in order to call send()
            chrome.sockets.udp.bind(createInfo.socketId, '::', 0, function (result) {
                if (result < 0) {
                    console.log('ERROR calling bind');
                } else {

                    // Add a click handler to the button
                    document.getElementById('unlock').addEventListener('click', function () {
                        console.log('Unlock!!');

                        // Prepare and send the packet
                        var data = str2ab('unlock4908');
                        chrome.sockets.udp.send(createInfo.socketId,
                                                data,
                                                '2607:f018:800:10f:c298:e552:5048:d86e',
                                                4999,
                                                function (sendInfo) {
                                                    console.log('Door unlocked?');
                                                });

                    });
                }
            });
        });

    }
};

app.initialize();