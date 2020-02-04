const mqtt = require('mqtt');
var client  = mqtt.connect('mqtt://localhost');

client.on('connect', function () {
  client.subscribe('hello', function (err) {
    if (!err) {
      client.publish('hello', 'Hello mqtt');
    }
  })
})
client.on('message', function (topic, message) {
  // message is Buffer
  console.log(message.toString());
  //client.end();
})
