/* 
 * File:   i2c_master.h
 * Author: Aaron Redepenning
 *
 * Created on March 5, 2016, 12:46 PM
 */

#ifndef I2C_MASTER_H
#define	I2C_MASTER_H

#define I2C_Idle    !(SSPSTATbits.R_nW | (0x1F & SSPCON2))
#define I2C_Start   (SSPCON2bits.SEN)
#define I2C_Restart (SSPCON2bits.RSEN)
#define I2C_Stop    (SSPCON2bits.PEN)
#define I2C_IF      (PIR1bits.SSPIF)

#ifdef	__cplusplus
extern "C" {
#endif
    
    void I2CMaster_Init(void);
    
    void I2CMaster_Start(void);
    
    void I2CMaster_Restart(void);
    
    void I2CMaster_Stop(void);
    
    void I2CMaster_Write(uint8_t address, uint8_t regAddress, uint8_t *pData, uint8_t len);
    
    void I2CMaster_Write8(uint8_t b);
    
    void I2CMaster_Read(uint8_t address, uint8_t *pData, uint8_t count);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_MASTER_H */

