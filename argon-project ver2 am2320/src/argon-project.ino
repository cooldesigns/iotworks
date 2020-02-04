#include "Particle.h"
#include "math.h"
#include "Wire.h"
#include "lib/AM2320/src/AM2320.h"
// #include <lib/DHT12/DHT12.h>
SYSTEM_MODE(MANUAL);

String str;

// Init DHT12 Temperature and Humudity Sensor
AM2320 am2320;
int timeSinceLastRead = 0;
float temperature = 0.0;
float humidity = 0.0;

// Init LM335 Temperature Sensor
const int sensorPin = A2;
float sensorValue;
float voltageOut;

float temperatureC;
float temperatureF;
float temperatureK;

// Init Servo Motor and Servo Position
Servo myservo;
int pos = 0;
int current_pos;
const int servo_pin = A3;

// UUIDs for service + characteristics
const char *serviceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
const char *red = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
const char *green = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
const char *blue = "6E400004-B5A3-F393-E0A9-E50E24DCCA9E";

// Variables for keeping state
typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_level_t;

// Static level tracking
static led_level_t m_led_level;

// Static function for handling Bluetooth Low Energy callbacks
static void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
{
  // We're only looking for one byte
  if (len != 1)
  {
    return;
  }

  // Sets the global level
  if (context == red)
  {
    m_led_level.red = data[0];
  }
  else if (context == green)
  {
    m_led_level.green = data[0];
  }
  else if (context == blue)
  {
    m_led_level.blue = data[0];
  }

  // Set RGB color
  RGB.color(m_led_level.red, m_led_level.green, m_led_level.blue);
}

// setup() runs once, when the device is first turned on.
void setup()
{
  am2320.begin();

  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  sensorValue = analogRead(sensorPin);
  voltageOut = (sensorValue * 3300) / 1024;

  myservo.attach(servo_pin);

  // Enable app control of LED
  RGB.control(true);

  // Init default level
  m_led_level.red = 0;
  m_led_level.green = 0;
  m_led_level.blue = 0;

  // Set the RGB BLE service
  BleUuid rgbService(serviceUuid);

  // Set the Health Thermometer and Battery Level service
  BleUuid healthThermometerService(0x1809);
  BleUuid batteryLevelService(0x180f);
  // Set up characteristics
  BleCharacteristic temperatureMeasurementCharacteristic("temp", BleCharacteristicProperty::NOTIFY, BleUuid(0x2A1C), healthThermometerService);
  BleCharacteristic batteryLevelCharacteristic("bat", BleCharacteristicProperty::NOTIFY, BleUuid(0x2A19), batteryLevelService);

  BleCharacteristic redCharacteristic("red", BleCharacteristicProperty::WRITE_WO_RSP, red, serviceUuid, onDataReceived, (void *)red);
  BleCharacteristic greenCharacteristic("green", BleCharacteristicProperty::WRITE_WO_RSP, green, serviceUuid, onDataReceived, (void *)green);
  BleCharacteristic blueCharacteristic("blue", BleCharacteristicProperty::WRITE_WO_RSP, blue, serviceUuid, onDataReceived, (void *)blue);

  // Add the characteristics
  BLE.addCharacteristic(temperatureMeasurementCharacteristic);
  BLE.addCharacteristic(batteryLevelCharacteristic);

  BLE.addCharacteristic(redCharacteristic);
  BLE.addCharacteristic(greenCharacteristic);
  BLE.addCharacteristic(blueCharacteristic);

  // Advertising data
  BleAdvertisingData advData;

  // Add the device name
  advData.appendLocalName("Argon");

  // Add the RGB LED service
  advData.appendServiceUUID(rgbService);
  advData.appendServiceUUID(healthThermometerService);
  advData.appendServiceUUID(batteryLevelService);

  // Start advertising!
  BLE.advertise(&advData);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{

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

  /*
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
  */
}