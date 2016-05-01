/**************************************************************************/
/*!
    @file     Adafruit_MPL3115A2.h
    @author   Aaron Redepenning (rede3704@stthomas.edu)

    This is a library for the Adafruit MPL3115A2 breakout board
    ----> https://www.adafruit.com/products/1893

    It was based off of the Adafuit_MPL3115A2 library for the arduino
    written by adafruit. This repository of the arduino library is:
    https://github.com/adafruit/Adafruit_MPL3115A2_Library.git.

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#include "main.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define MPL3115A2_ADDRESS                       (0xC0)    // 11000000
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    #define MPL3115A2_REGISTER_STATUS               (0x00)
    #define MPL3115A2_REGISTER_STATUS_TDR 0x02
    #define MPL3115A2_REGISTER_STATUS_PDR 0x04
    #define MPL3115A2_REGISTER_STATUS_PTDR 0x08

    #define MPL3115A2_REGISTER_PRESSURE_MSB         (0x01)
    #define MPL3115A2_REGISTER_PRESSURE_CSB         (0x02)
    #define MPL3115A2_REGISTER_PRESSURE_LSB         (0x03)

    #define MPL3115A2_REGISTER_TEMP_MSB             (0x04)
    #define MPL3115A2_REGISTER_TEMP_LSB             (0x05)

    #define MPL3115A2_REGISTER_DR_STATUS            (0x06)

    #define MPL3115A2_OUT_P_DELTA_MSB               (0x07)
    #define MPL3115A2_OUT_P_DELTA_CSB               (0x08)
    #define MPL3115A2_OUT_P_DELTA_LSB               (0x09)

    #define MPL3115A2_OUT_T_DELTA_MSB               (0x0A)
    #define MPL3115A2_OUT_T_DELTA_LSB               (0x0B)

    #define MPL3115A2_WHOAMI                        (0x0C)

#define MPL3115A2_PT_DATA_CFG 0x13
#define MPL3115A2_PT_DATA_CFG_TDEFE 0x01
#define MPL3115A2_PT_DATA_CFG_PDEFE 0x02
#define MPL3115A2_PT_DATA_CFG_DREM 0x04

#define MPL3115A2_CTRL_REG1                     (0x26)
#define MPL3115A2_CTRL_REG1_SBYB 0x01
#define MPL3115A2_CTRL_REG1_OST 0x02
#define MPL3115A2_CTRL_REG1_RST 0x04
#define MPL3115A2_CTRL_REG1_OS1 0x00
#define MPL3115A2_CTRL_REG1_OS2 0x08
#define MPL3115A2_CTRL_REG1_OS4 0x10
#define MPL3115A2_CTRL_REG1_OS8 0x18
#define MPL3115A2_CTRL_REG1_OS16 0x20
#define MPL3115A2_CTRL_REG1_OS32 0x28
#define MPL3115A2_CTRL_REG1_OS64 0x30
#define MPL3115A2_CTRL_REG1_OS128 0x38
#define MPL3115A2_CTRL_REG1_RAW 0x40
#define MPL3115A2_CTRL_REG1_ALT 0x80
#define MPL3115A2_CTRL_REG1_BAR 0x00
#define MPL3115A2_CTRL_REG2                     (0x27)
#define MPL3115A2_CTRL_REG3                     (0x28)
#define MPL3115A2_CTRL_REG4                     (0x29)
#define MPL3115A2_CTRL_REG5                     (0x2A)

    #define MPL3115A2_REGISTER_STARTCONVERSION      (0x12)
/*=========================================================================*/

bool Adafruit_MPL3115A2_Init();
float Adafruit_MPL3115A2_getPressure();
float Adafruit_MPL3115A2_getAltitude();
float Adafruit_MPL3115A2_getTemperature();
