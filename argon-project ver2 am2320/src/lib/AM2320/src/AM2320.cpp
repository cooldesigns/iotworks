#include "AM2320.h"
AM2320::AM2320(void) {}

void AM2320::begin() {
    Wire.begin();
}

bool AM2320::read(void) {
    // Wakeup
    Wire.beginTransmission(AM2320_I2C_ADDR);
    Wire.endTransmission();
    // Write command: read humidity and temperature register
    Wire.beginTransmission(AM2320_I2C_ADDR);
    Wire.write(AM2320_CMD_READ); //command for read register
    Wire.write(0x00); //start from register 0x00
    Wire.write(0x04); //read 4 registers. i.e. 0x00 ~ 0x03
    Wire.endTransmission();
    // Wait
    delayMicroseconds(AM2320_I2C_WAIT); //>1.5ms
    // Get data: function_code(1) + counts(1) + data(4) + crc(2) = 8 bytes
    Wire.requestFrom(AM2320_I2C_ADDR, 8);
	 uint8_t i = 0;
    for (i=0; i<6; i++)
        buf[i] = Wire.read();

    uint16_t crc = 0;
    crc = Wire.read(); //crc low byte
    crc |= Wire.read() << 8; //crc high byte
    if (crc == crc16(buf, 6))
        return true;
    return false;
}

uint16_t AM2320::crc16(uint8_t *ptr, uint8_t len)
{
    uint16_t crc = 0xFFFF;
    uint8_t i = 0;
    while(len--) {
        crc ^= *ptr++;
        for(i=0; i<8; i++) {
            if(crc & 0x01) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

float AM2320::getHumidity(void) {
    return (float) (buf[2] * 256 + buf[3]) / 10.0;
}

float AM2320::getTemperature(void) {
    return (float) (buf[4] * 256 + buf[5]) / 10.0;
}
