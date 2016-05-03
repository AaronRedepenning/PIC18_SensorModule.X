#include "main.h"

static uint8_t _buffer[6] = { 0 };

uint8_t HIH8000_ReadHumidityTemperature(float *temperature, float *humidity) {
    uint8_t status;
    uint16_t util;
    
    // Send measurement request command
    I2CMaster_Start();
    I2CMaster_Write8(HIH8000_I2CADDRESS);
    I2CMaster_Stop();
    
    for(int i = 0; i < 10; i++) { __delay_ms(10); } // Give it time to make the reading
    
    I2CMaster_Start();
    I2CMaster_Read(HIH8000_I2CADDRESS, _buffer, 4);
    I2CMaster_Stop();
    
    // Sensor status: 00 -> normal, 01 -> stale data, 10 -> command mode
    status = (_buffer[0] >> 6) & 0x03;
    
    // TODO: Convert sensor readings
    util = (((uint16_t)_buffer[0] & 0x1F) << 8) | _buffer[1];
    *humidity = ((float)util * 100.0) / HIH8000_SCALAR;
    
    util = ((uint16_t)_buffer[2] << 6) | (_buffer[4] >> 2);
    *temperature = (((float)util * 165.0) / HIH8000_SCALAR) - 40; // degrees C
    *temperature = (*temperature * 1.8) + 32.0; // degrees 
    
    return status;
}

void HIH8000_Init() {
    
    I2CMaster_Start();
    I2CMaster_Write8(HIH8000_I2CADDRESS);
    I2CMaster_Stop();
    
}
