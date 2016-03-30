#include "main.h"

static uint8_t _buffer[5];
static TEMPERATURE_UNIT _tUnits;

static void readRegister(uint8_t regAddr, uint8_t numRegisters)
{
    I2CMaster_Start();
    I2CMaster_Write(HDC1000_SLAVE_ADDR, regAddr, NULL, 0);
    
    if((regAddr == HDC1000_TEMP) || (regAddr == HDC1000_HUMI))
    {
        __delay_ms(HDC1000_SAMPLE_DELAY);
    }
    
    I2CMaster_Restart();
    I2CMaster_Read(HDC1000_SLAVE_ADDR, _buffer, (numRegisters << 2));
    I2CMaster_Stop();
}

void writeConfig(uint8_t config)
{
    _buffer[0] = config;
    _buffer[1] = 0;

    I2CMaster_Start();
    I2CMaster_Write(HDC1000_SLAVE_ADDR, HDC1000_CNFG, _buffer, 2);
    I2CMaster_Stop();
}

void HDC1000_Init(uint8_t configBits, TEMPERATURE_UNIT tUnits)
{
    if(configBits)
    {
        writeConfig(configBits);
    }
    
    _tUnits = tUnits;
}
    
void HDC1000_ReadTempHumidity(float *pTemp, float *pHum)
{    
    uint16_t tRaw, hRaw;
    readRegister(HDC1000_TEMP, 2);
    
    tRaw = _buffer[0];
    tRaw = (tRaw << 8) | _buffer[1];
    hRaw = _buffer[2];
    hRaw = (hRaw << 8) | _buffer[3];
    
    *pTemp = (tRaw) * (165.0 / 65536.0) - 40.0;
    *pHum  = (hRaw / 65536.0) * 100.0;
    
    if(_tUnits == DEGREES_F)
    {
        *pTemp = (*pTemp * 1.8) + 32.0;
    }
}

