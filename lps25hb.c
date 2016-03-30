/**
 * File: lps25hb.c
 * Author: Aaron Redepenning
 * 
 * Implementation of the lps25hb driver defined in lps25hb.h
 */

#include "main.h"

/* Private Functions */

static void writeRegister(uint8_t regAddr, uint8_t value)
{
    I2CMaster_Start();
    I2CMaster_Write((uint8_t)LPS25HB_SLAVE_ADDR, regAddr, &value, 1);
    I2CMaster_Stop();
}

static uint8_t readRegister(uint8_t regAddr)
{    
    uint8_t result = 0;
    
    I2CMaster_Start();
    I2CMaster_Write(LPS25HB_SLAVE_ADDR, regAddr, NULL, 0);
    I2CMaster_Restart();
    I2CMaster_Read(LPS25HB_SLAVE_ADDR, &result, 1);
    I2CMaster_Stop();
    
    return result;
}

/* Public Function */

void LPS25HB_Init()
{
    writeRegister(LPS25HB_CTRL_REG1, 0x90); // 1Hz Sampling, PD = 1 (Active Mode)
}

void LPS25HB_Active()
{
    uint8_t value = readRegister(LPS25HB_CTRL_REG1);
    writeRegister(LPS25HB_CTRL_REG1, (value | 0x08)); // Set active bit
}

void LPS25HB_PowerDown()
{
    uint8_t value = readRegister(LPS25HB_CTRL_REG1);
    
#pragma warning push
#pragma warning disable 752
    writeRegister(LPS25HB_CTRL_REG1, (value ^ 0x08)); // Clear active bit
#pragma warning pop
    
}

uint8_t LPS25HB_WhoAmI()
{
    uint8_t temp = readRegister(LPS25HB_WHO_AM_I);
    return temp;
}

float LPS25HB_ReadTempC()
{
    int16_t temp;
    float result;
    
    temp = readRegister(LPS25HB_TEMP_OUT_H);
    temp = (temp << 8) | readRegister(LPS25HB_TEMP_OUT_L);
    
    result = (float)temp / 480.0;
    result += 42.5;
    
    return result;
}

float LPS25HB_ReadTempF()
{
    float tempC = LPS25HB_ReadTempC();
    
    /* Convert degrees Celsius to degrees Fahrenheit 
     *      degF = degC * 1.8 + 32
     */
    
    return (tempC * 1.8 + 32);
}

float LPS25HB_ReadPressureMillibars()
{
    uint24_t temp;
    double result;
    
    temp = readRegister(LPS25HB_PRESS_OUT_H);
    temp = (temp << 8) | readRegister(LPS25HB_PRESS_OUT_L);
    temp = (temp << 8) | readRegister(LPS25HB_PRESS_OUT_XL);
    
    result = (double)temp / 4096;
    
    return result;
}