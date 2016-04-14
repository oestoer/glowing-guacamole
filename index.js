var dhtSensor = require('node-dht-sensor');
var sensor = {
    initialize: function () {
        return dhtSensor.initialize(11, 3);
    },
    read: function () {
        var readOut = dhtSensor.read();
        console.log('Temp: ' + readOut.temperature.toFixed(2) + 'C, Humi: ' + readOut.humidity.toFixed(2) + '%');
        setTimeout(function () {
            sensor.read();
        }, 2000);
    }
};

if (sensor.initialize()) {
    sensor.read();
} else {
    console.warn('Failed to initialize sensor');
}
