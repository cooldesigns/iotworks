#include "AM2320.h"

AM2320 am2320;

float temperature = 0.0;
float humidity = 0.0;

void setup() {
    Serial.begin(9600);
    am2320.begin();
}

void loop() {
    am2320.read();
    temperature = am2320.getTemperature();
    humidity = am2320.getHumidity();

    Serial.print(temperature);
    Serial.print(" °C, ");
    Serial.print(humidity);
    Serial.println(" %");
    delay(1000);
}
