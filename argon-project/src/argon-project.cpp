/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/eva/Documents/Particle/argon-project/src/argon-project.ino"
#include "Particle.h"
// #include "math.h"
// #include "Wire.h"
// #include "lib/AM2320/src/AM2320.h"
// #include "lib/Adafruit_DHT/src/Adafruit_DHT.h"
// Device mac address 30:ae:a4:e5:44:c0
static void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
void setup();
void loop();
#line 7 "/Users/eva/Documents/Particle/argon-project/src/argon-project.ino"
SYSTEM_MODE(MANUAL);
SerialLogHandler logHandler;

String str;
bool sensor_connected = false;

const int sensorPin = A2; // DHT11
const unsigned long UPDATE_INTERVAL_MS = 2000;
unsigned long lastUpdate = 0;
float getTempC();
uint32_t ieee11073_from_float(float temperature);
float lastTemp = 37.0; // 98.6 deg F;
uint8_t lastBattery = 100;

// sayac and servo
uint32_t sayac;
bool updatingMotor = false;
Servo myservo;
const int servo_pin = A3;
unsigned long times;

// motor service
const char *motorServiceUuid = "c00f3984-270a-11ea-a840-2e728ce88125";
const char *motorKarId = "c00f3c22-270a-11ea-a840-2e728ce88125";
const char *sayacKarId = "c00f3d80-270a-11ea-a840-2e728ce88125"; // sayac

// UUIDs for service + characteristics
const char *serviceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
const char *red = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
const char *green = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
const char *blue = "6E400004-B5A3-F393-E0A9-E50E24DCCA9E";

// Set the RGB BLE service
BleUuid rgbService(serviceUuid);

// Set the Health Thermometer and Battery Level service
BleUuid healthThermometerService(0x1809);
BleUuid batteryLevelService(0x180f);

// Init motorService
BleUuid motorService(motorServiceUuid);

// Set up characteristics
BleCharacteristic temperatureMeasurementCharacteristic("temp", BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY, BleUuid(0x2A1C), healthThermometerService);
BleCharacteristic batteryLevelCharacteristic("bat", BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY, BleUuid(0x2A19), batteryLevelService);

BleCharacteristic redCharacteristic("red", BleCharacteristicProperty::WRITE_WO_RSP, red, serviceUuid, onDataReceived, (void *)red);
BleCharacteristic greenCharacteristic("green", BleCharacteristicProperty::WRITE_WO_RSP, green, serviceUuid, onDataReceived, (void *)green);
BleCharacteristic blueCharacteristic("blue", BleCharacteristicProperty::WRITE_WO_RSP, blue, serviceUuid, onDataReceived, (void *)blue);

// motor kar setup
BleCharacteristic motorCharacteristic("mottor", BleCharacteristicProperty::WRITE_WO_RSP, motorKarId, motorServiceUuid, onDataReceived, (void *)motorKarId);

// Inıt sayacCharacteristic
BleCharacteristic sayacCharacteristic("sayyac", BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY, sayacKarId, motorService);

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
  if (context == motorKarId){
    // call motor
    uint8_t flags = data[0];
    uint16_t motor;
    if (flags & 0x01)
    {
      // motor is 16 bits
      memcpy(&motor, &data[1], sizeof(uint16_t));
    }
    else
    {
      // motor is 8 bits (normal case)
      motor = data[1];
    }
    if (!updatingMotor){
      Log.info("motor angle setted=%u", motor);
      times = motor / 0.1756;
      if(times > 10000 ){
        Log.info("Can't turn long than 10s, sended value=%u", motor);
        return;
      }
      updatingMotor = true;
      myservo.write(0); // zero max speed
      Log.info("will turn %", times / 1000);
      delay(times);
      myservo.write(90); // 90 stop
      updatingMotor = false;
      Log.info("motor angle turned=%u", motor);
    }
    return;
  }
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

  Serial.begin(9600);
  // Wait for a USB serial connection for up to 30 seconds
  waitFor(Serial.isConnected, 3000);

  myservo.attach(servo_pin);
  Log.info("Setup Motor read %u", myservo.read());
  // Enable app control of LED
  RGB.control(true);

  // Init default level
  m_led_level.red = 0;
  m_led_level.green = 0;
  m_led_level.blue = 0;

  // Add sayacCharacteristic
  BLE.addCharacteristic(sayacCharacteristic);
  BLE.addCharacteristic(motorCharacteristic);
  // Add the characteristics
  BLE.addCharacteristic(temperatureMeasurementCharacteristic);
  BLE.addCharacteristic(batteryLevelCharacteristic);
  batteryLevelCharacteristic.setValue(&lastBattery, 1);

  BLE.addCharacteristic(redCharacteristic);
  BLE.addCharacteristic(greenCharacteristic);
  BLE.addCharacteristic(blueCharacteristic);

  // Advertising data
  BleAdvertisingData advData;

  // Add the device name
  advData.appendLocalName("Argon");

  // only advertise the sayacService. Other services will be found after connecting.
  advData.appendServiceUUID(motorService);

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
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS)
  {
    lastUpdate = millis();

    if (BLE.connected())
    {
      uint8_t buf[6];
      buf[0] = 0x04;
      uint32_t value = ieee11073_from_float(getTempC());
      memcpy(&buf[1], &value, 4);
      buf[5] = 6; // Mouth
      temperatureMeasurementCharacteristic.setValue(buf, sizeof(buf));
      batteryLevelCharacteristic.setValue(&lastBattery, 1);
      sayac += 1;
      sayacCharacteristic.setValue(sayac);
      if (--lastBattery < 10)
      {
        lastBattery = 100;
      }
    }
  }
}

float getTempC() {
  if (sensor_connected)
  {
    return 0;
  }else 
  {
    if (rand() > (RAND_MAX / 2))
    {
      lastTemp += 0.1;
    }
    else
    {
      lastTemp -= 0.1;
    }
    return lastTemp;
  }
}

uint32_t ieee11073_from_float(float temperature) {
    // This code is from the ARM mbed temperature demo:
    // https://github.com/ARMmbed/ble/blob/master/ble/services/HealthThermometerService.h
    // I'm pretty sure this only works for positive values of temperature, but that's OK for the health thermometer.
    uint8_t  exponent = 0xFE; // Exponent is -2
    uint32_t mantissa = (uint32_t)(temperature * 100);

    return (((uint32_t)exponent) << 24) | mantissa;
}