const noble = require('noble-mac');
const mqtt = require('mqtt');
var client  = mqtt.connect('mqtt://localhost');
client.subscribe('motor', function (err) {});
client.subscribe('red', function (err) {});
function randomIntInc(low, high) {
  return Math.floor(Math.random() * (high - low + 1) + low)
}

noble.on('stateChange', function(state) {
  if (state === 'poweredOn') {
    noble.startScanning();
  } else {
    noble.stopScanning();
  }
});
noble.on('discover', function(peripheral) {
  console.log('Found device with local name: ' + peripheral.advertisement.localName);
  console.log('advertising the following service uuid\'s: ' + peripheral.advertisement.serviceUuids);
  if (peripheral.advertisement.localName == 'Argon') {
    peripheral.connect(function(error) {
      noble.stopScanning();
      console.log('connected to peripheral: ' + peripheral.uuid);
      peripheral.discoverServices(null, function(error, services) {
        console.log('discovered the following services:');
        for (var s in services) {
          console.log('discovered a service' + s);
          console.log('  ' + s + ' service object: ' + services[s]);
          services[s].discoverCharacteristics(null, function(error, characteristics) {
            console.log('discovered the following characteristics:');
            for (var c in characteristics) {
              console.log(' Characteristic Object: ' + characteristics[c]);
              if (characteristics[c]["properties"].includes("read")) {
                characteristics[c].read(function(error, data) {
                  console.log(' Characteristics[' + characteristics[c]["uuid"] + '] read response int: ' + data.readUInt8(0));
                });
              }
              if (characteristics[c]["properties"].includes("notify")) {
                characteristics[c].subscribe(function(error) {
                  console.log('subscribed to Characteristics[' + characteristics[c]["uuid"] + ']');
                });
                characteristics[c].on('data', function(data, isNotification) {
									if (characteristics[c]["uuid"] == "2a1c"){
										var original_float_value = data.readUInt32LE(0) / 25600.0;
										console.log("Temparture: " + original_float_value);
                    var str = original_float_value.toString();
                    client.publish('temp', str);
									}
									else if (characteristics[c]["uuid"] == "2a19"){
										var original_int_value = data.readUInt8(0);
										console.log("Battery: " + original_int_value +"%");
                    var str = original_int_value.toString();
                    client.publish('battery', str);
									}else if (characteristics[c]["uuid"] == "c00f3d80270a11eaa8402e728ce88125"){
										var original_int_value = data.readUInt32(0);
										console.log("Sayac: " + original_int_value);
                    var str = original_int_value.toString();
                    client.publish('sayac', str);
									}
                });
							}
							if(characteristics[c]["properties"].includes("writeWithoutResponse")){
							  if (characteristics[c]["uuid"] == "6e400002b5a3f393e0a9e50e24dcca9e"){ //blue
							    var red_led_level = 255;
                  client.on('message', function (topic, message) {
                    if (topic == 'blue'){
                      console.log(topic);
                      console.log(message.toString());
                      var hexString = red_led_level.toString(16);
							  	    var buff = Buffer.from(hexString, "hex");
        			  	    characteristics[c].write((buff), true, function(error) {
          		  		    console.log('Set RED led level: ' + red_led_level);
        			  	    });
                    }
                  })
							  }
							  if (characteristics[c]["uuid"] == "6e400003b5a3f393e0a9e50e24dcca9e"){ //green
							    var red_led_level = 255;
                  client.on('message', function (topic, message) {
                    if (topic == 'green'){
                      console.log(topic);
                      console.log(message.toString());
                      var hexString = red_led_level.toString(16);
							  	    var buff = Buffer.from(hexString, "hex");
        			  	    characteristics[c].write((buff), true, function(error) {
          		  		    console.log('Set RED led level: ' + red_led_level);
        			  	    });
                    }
                  })
							  }
							  if (characteristics[c]["uuid"] == "6e400004b5a3f393e0a9e50e24dcca9e"){ //blue
							    var red_led_level = 255;
                  client.on('message', function (topic, message) {
                    if (topic == 'red'){
                      console.log(topic);
                      console.log(message.toString());
                      var hexString = red_led_level.toString(16);
							  	    var buff = Buffer.from(hexString, "hex");
        			  	    characteristics[c].write((buff), true, function(error) {
          		  		    console.log('Set RED led level: ' + red_led_level);
        			  	    });
                    }
                  })
							  }
                if (characteristics[c]["uuid"] == "c00f3c22270a11eaa8402e728ce88125"){ //blue
							    var red_led_level = 255;
                  client.on('message', function (topic, message) {
                    if (topic == 'motor'){
                      console.log(topic);
                      console.log(message.toString());
                      var hexString = 'FFF';
							  	    var buff = Buffer.from(hexString, "hex");
        			  	    characteristics[c].write((buff), true, function(error) {
          		  		    console.log('Set RED led level: ' + red_led_level);
        			  	    });
                    }
                  })
							  }
							}
            }
          });
        }
      });
    });
  }
});