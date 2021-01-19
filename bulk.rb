require 'rubygems'
require 'mqtt'
require 'date'
require 'json'

memory = Hash.new
while true do
	sleep 3
  devices = ['Ms5Z50jZUtK3oTuhd8qY', 'E8eChakXTLdzPT752Oco', 'swQuTzX5mHarnCxXu4B3', 'J5I1SS4bdLAo0d1qq7Vf', '5MyBt6xvtPx58qJE2cUs']
	devices.each do |username|
		if memory[username].nil?
  	  hash = {
  	    "ts": DateTime.now.strftime("%Q").to_i,
  	    "values": {
  	      "k_temperature": rand(1.2...32.9),
  	      "k_humudity": rand(1.2...89.9),
  	      "k_battery": rand(5.2...99.9),
  	      "k_voltage": rand(0.2...59.9),
  	      "k_amper": rand(0.1...19.9)
  	    }
			}
		else
			hash = {
  	    "ts": DateTime.now.strftime("%Q").to_i,
  	    "values": {
  	      "k_temperature": memory[username][:values][:k_temperature] + rand(-0.5...0.5),
  	      "k_humudity": memory[username][:values][:k_humudity] + rand(-0.5...0.5),
  	      "k_battery": memory[username][:values][:k_battery] + rand(-0.5...0.5),
  	      "k_voltage": memory[username][:values][:k_voltage] + rand(-1.0...1.0),
  	      "k_amper": memory[username][:values][:k_amper] + rand(-1.0...1.0)
  	    }
  	  }
		end
		memory[username] = hash
  	MQTT::Client.connect("mqtt://#{username}@51.11.242.41") do |c|
  	    c.publish('v1/devices/me/telemetry', hash.to_json)
  	end
	end
end