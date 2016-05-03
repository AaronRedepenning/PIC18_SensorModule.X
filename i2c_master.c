#include "main.h"

void I2CMaster_Init(void)
{
    SSPSTATbits.SMP = 1; // Slew rate control disabled
    SSPSTATbits.CKE = 0; // Disable SMBus logic levels
    SSPCON1bits.SSPM = 0b1000; // I2C Master mode, clock = Fosc / [4 * (SSPADD)]
    SSPADD = 0x14;  // 200kHz Clock assuming Fosc = 16 MHz // Old 0x27 - 100kHz
    SSPCON2 = 0x00; // Ensure all events are disabled before enabling the MSSP
    SSPCON1bits.SSPEN = 1; // Enable the MSSP
}

void I2CMaster_Start(void)
{
    while(!I2C_Idle) { }
    
    I2C_Start = 1; // Set the START bit
    while(I2C_Start) { } // Wait until hardware clears the START bit
}

void I2CMaster_Restart(void)
{
    I2C_Restart = 1;
    while(I2C_Restart) { }
}

void I2CMaster_Stop(void)
{
    I2C_Stop = 1;
    while(I2C_Stop) { }
}

void I2CMaster_Write(uint8_t address, uint8_t regAddress, uint8_t *pData, uint8_t len)
{
    I2C_IF = 0;
    
    SSPBUF = address;
    while(!I2C_IF) { }
    I2C_IF = 0;
    
    SSPBUF = regAddress;
    while(!I2C_IF) { }
    I2C_IF = 0;
    
    while(len > 0)
    {
        SSPBUF = *pData;
        len--;
        pData++;
        while(!I2C_IF) { }
        I2C_IF = 0;
    }
}

void I2CMaster_Write8(uint8_t b) {
    I2C_IF = 0;
    
    SSPBUF = b;
    while(!I2C_IF) { }
    I2C_IF = 0;
}

void I2CMaster_Read(uint8_t address, uint8_t *pData, uint8_t count)
{
    I2C_IF = 0;
    
    SSPBUF = (address | 0x01);
    while(!I2C_IF) { }
    I2C_IF = 0;
    
    do {
        SSPCON2bits.RCEN = 1;
        while(!I2C_Idle) { }
        
        *pData = SSPBUF;
        
        SSPCON2bits.ACKDT = count > 1 ? 0 : 1;
        SSPCON2bits.ACKEN = 1;
        while(SSPCON2bits.ACKEN) { }
        SSPCON2bits.ACKDT = 0;
        
        count--;
        pData++;
    } while(count > 0);
}
