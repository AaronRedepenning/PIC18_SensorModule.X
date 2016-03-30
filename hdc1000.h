/* 
 * File:   hdc1000.h
 * Author: Aaron Redepenning
 *
 * Created on February 16, 2016, 4:58 PM
 */

#ifndef HDC1000_H
#define	HDC1000_H

/* I2C Address - ADDR Select Pins both tied to GND */
#define HDC1000_SLAVE_ADDR  0x80

/* Register Mapping */
#define HDC1000_TEMP 0x00 // Temperature measurement output
#define HDC1000_HUMI 0x01 // Humidity measurement output
#define HDC1000_CNFG 0x02 // Configuration Register
#define HDC1000_SER1 0xFB // First two bytes of serial ID
#define HDC1000_SER2 0xFB // Mid two bytes of serial ID
#define HDC1000_SER3 0xFD // Last byte of serial ID
#define HDC1000_MFID 0xFE // Manufacturer ID
#define HDC1000_DVID 0xFF // Device ID for HDC1000

/* Bit masks for configuration register */
#define HDC1000_RST                 0x80
#define HDC1000_HEAT_ON             0x20
#define HDC1000_HEAT_OFF            0x00
#define HDC1000_BOTH_TEMP_HUMI      0x10
#define HDC1000_SINGLE_MEASUR       0x00
#define HDC1000_TEMP_HUMI_14BIT     0x00
#define HDC1000_TEMP_11BIT          0x04
#define HDC1000_HUMI_11BIT          0x01
#define HDC1000_HUMI_8BIT           0x02

#define HDC1000_SAMPLE_DELAY        20      // Delay in milliseconds
#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum temp_unit {
        DEGREES_F,
        DEGREES_C
    } TEMPERATURE_UNIT;
    
    void HDC1000_Init(uint8_t configBits, TEMPERATURE_UNIT tUnits);
    
    void HDC1000_ReadTempHumidity(float *pTemp, float *pHum);

#ifdef	__cplusplus
}
#endif

#endif	/* HDC1000_H */

