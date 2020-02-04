#ifndef AM2320_h
#define AM2320_h

#include "application.h"
#include <Wire.h>

#define AM2320_I2C_ADDR          (0xB8 >> 1)
#define AM2320_CMD_READ           0x03
#define AM2320_REG_HUMIDITY_H     0x00
#define AM2320_REG_HUMIDITY_L     0x01
#define AM2320_REG_TEMPERATURE_H  0x02
#define AM2320_REG_TEMPERATURE_L  0x03
#define AM2320_I2C_WAIT           1600

class AM2320 {
  public:
    AM2320(void);
    void begin();
    bool read();
    float getHumidity();
    float getTemperature();
  private:
    uint16_t crc16(uint8_t *ptr, uint8_t len);
    uint8_t buf[8];
};

#endif
