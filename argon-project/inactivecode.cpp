/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/Users/eva/Documents/Particle/argon-project/inactivecode.ino"
void this_is_loop();
#line 1 "/Users/eva/Documents/Particle/argon-project/inactivecode.ino"
void this_is_loop(){
  if (timeSinceLastRead > 2000)
  {
    // read dht12
    am2320.read();
    temperature = am2320.getTemperature();
    humidity = am2320.getHumidity();
    Serial.print("Digital --- ");
    Serial.print(temperature);
    Serial.print(" °C, ");
    Serial.print(humidity);
    Serial.println(" %");

    // calculate temperature for LM335
    temperatureK = voltageOut / 10;
    temperatureC = temperatureK - 273;
    temperatureF = (temperatureC * 1.8) + 32;

    Serial.print("Analog --- Temperature(ºC): ");
    Serial.print(temperatureC);
    Serial.print("  Temperature(ºF): ");
    Serial.print(temperatureF);
    Serial.print("  Voltage(mV): ");
    Serial.println(voltageOut);
    timeSinceLastRead = 0;
  }
  else
  {
    delay(100);
    timeSinceLastRead += 100;
  }
  current_pos = myservo.read();
  Serial.print("Servo Current Position: ");
  Serial.println(current_pos);

  for (pos = 0; pos < 360; pos += 1) // goes from 0 degrees to 360 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    while (pos == myservo.read())
    { 
      // waits until servo to reach the position
      delay(1);
    }
  }
  for (pos = 360; pos >= 1; pos -= 1) // goes from 360 degrees to 0 degrees
  {
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    while (pos == myservo.read())
    {
      // waits until servo to reach the position
      delay(1);
    }
  }
}