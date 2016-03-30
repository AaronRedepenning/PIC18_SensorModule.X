/* 
 * File:   lps25hb.h
 * Author: Aaron Redepenning
 * 
 * This is the driver use to communicate with the lps25hb pressure sensor.
 * It depends on the functions defined in i2c_master.h/.c
 *
 * Created on February 13, 2016, 1:52 PM
 */

#ifndef LPS25HB_H
#define	LPS25HB_H

/* Slave Address, LSB selected as HIGH */
#define LPS25HB_SLAVE_ADDR 0b10111010

/* Register Mapping */
#define LPS25HB_REF_P_XL        0x08
#define LPS25HB_REF_P_L         0x09
#define LPS25HB_P_H             0x0A
#define LPS25HB_WHO_AM_I        0x0F
#define LPS25HB_RES_CONF        0x10
#define LPS25HB_CTRL_REG1       0x20
#define LPS25HB_CTRL_REG2       0x21
#define LPS25HB_CTRL_REG3       0x22
#define LPS25HB_CTRL_REG4       0x23
#define LPS25HB_INTERRUPT_CFG   0x24
#define LPS25HB_INT_SOURCE      0x25
#define LPS25HB_STATUS_REG      0x27
#define LPS25HB_PRESS_OUT_XL    0x28
#define LPS25HB_PRESS_OUT_L     0x29
#define LPS25HB_PRESS_OUT_H     0x2A
#define LPS25HB_TEMP_OUT_L      0x2B
#define LPS25HB_TEMP_OUT_H      0x2C
#define LPS25HB_FIFO_CTRL       0x2E
#define LPS25HB_FIFO_STATUS     0x2F
#define LPS25HB_THS_P_L         0x30
#define LPS25HB_THS_P_H         0x31
#define LPS25HB_RPDS_L          0x39
#define LPS25HB_RPDS_H          0x3A

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Sets up the lps25hb's configuration registers. Configuration is as follows:
     * 
     */
    void LPS25HB_Init();

    /**
     * Activates the lps25hb by setting the active bit in the CTRL_REG1 register.
     */
    void LPS25HB_Active();

    /**
     * Powers down the lps25hb (low power mode) by clearing the active bit in
     * the CTRL_REG1 register.
     */
    void LPS25HB_PowerDown();

    /**
     * This function reads the WHO_AM_I register of the lps25hb to obtain
     * its i2c address. This function is useful for troubleshooting.
     * Should always return 0xBD (if LSB is selected high)
     * 
     * @return  i2c address of the device
     */
    uint8_t LPS25HB_WhoAmI();

    /**
     * This function gets the temperature read by the lps25hb.
     * @return Temperature in degrees Celsius
     */
    float LPS25HB_ReadTempC();
    
    /**
     * This function gets the temperature from the lps25hb
     * @return Temperature in degrees Fahrenheit
     */
    float LPS25HB_ReadTempF();

    /**
     * This function gets the pressure read by the lps25hb.
     * 
     * @return Pressure in millibars
     */
    float LPS25HB_ReadPressureMillibars();

#ifdef	__cplusplus
}
#endif

#endif	/* LPS25HB_H */

