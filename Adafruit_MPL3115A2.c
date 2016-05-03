#include "Adafruit_MPL3115A2.h"
#include "i2c_master.h"

uint8_t mode;

void write8(uint8_t reg, uint8_t data);
uint8_t read8(uint8_t reg);

/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
bool Adafruit_MPL3115A2_Init() {
    uint8_t whoami = read8(MPL3115A2_WHOAMI);
    if(whoami != 0xC4) {
        return false;
    }
    
    write8(MPL3115A2_CTRL_REG1,
	    MPL3115A2_CTRL_REG1_SBYB |
	    MPL3115A2_CTRL_REG1_OS128 |
        MPL3115A2_CTRL_REG1_ALT);
    write8(MPL3115A2_PT_DATA_CFG, 
        MPL3115A2_PT_DATA_CFG_TDEFE |
        MPL3115A2_PT_DATA_CFG_PDEFE |
        MPL3115A2_PT_DATA_CFG_DREM);
    
    // First reading is always bad for some reason
    Adafruit_MPL3115A2_getPressure();
    
    return true;
}

/**************************************************************************/
/*!
    @brief  Gets the floating-point pressure level in kPa
*/
/**************************************************************************/
float Adafruit_MPL3115A2_getPressure() {
    uint32_t pressure = 0;
    uint8_t buf[3];
    
    // Tell MPL3115A2 to read pressure
    write8(MPL3115A2_CTRL_REG1, 
	    MPL3115A2_CTRL_REG1_SBYB |
	    MPL3115A2_CTRL_REG1_OS128 |
	    MPL3115A2_CTRL_REG1_BAR);
      
    // Wait for data to be ready
    uint8_t sta = 0;
    while (! (sta & MPL3115A2_REGISTER_STATUS_PDR)) {
        sta = read8(MPL3115A2_REGISTER_STATUS);
        __delay_ms(10);
    }
    
    // Set register pointer
    I2CMaster_Start();
    I2CMaster_Write(MPL3115A2_ADDRESS, MPL3115A2_REGISTER_PRESSURE_MSB, NULL, 0);
    I2CMaster_Restart();
    
    // Read data (3 bytes)
    I2CMaster_Read(MPL3115A2_ADDRESS, buf, 3);
    I2CMaster_Stop();
    
    // Calculate pressure
    pressure = buf[0];
    pressure = (pressure << 8) | buf[1];
    pressure = (pressure << 8) | buf[2];
                   
    float baro = (float)(pressure);
    baro /= 6400; // Hectopascals
    return baro;
}

float Adafruit_MPL3115A2_getAltitude() { return 0.0; }
float Adafruit_MPL3115A2_getTemperature() { return 0.0; }


/*********************************************************************************/
// Utility Functions
/********************************************************************************/

static uint8_t _buffer[3] = { 0 };

void write8(uint8_t reg, uint8_t data) {
    I2CMaster_Start(); // Send I2C Start Condition
    I2CMaster_Write(MPL3115A2_ADDRESS, reg, &data, 1); // Write data to the desired register
    I2CMaster_Stop(); // Send stop condition
}

uint8_t read8(uint8_t reg) {    
    
    I2CMaster_Start();
    I2CMaster_Write(MPL3115A2_ADDRESS, reg, NULL, 0);
    
    I2CMaster_Restart();
    I2CMaster_Read(MPL3115A2_ADDRESS, _buffer, 1);
    I2CMaster_Stop();
    return _buffer[0];
}