/* 
 * File:   hih8000.h
 * Author: aaron
 *
 * Created on May 1, 2016, 11:46 PM
 */

#ifndef HIH8000_H
#define	HIH8000_H

#ifdef	__cplusplus
extern "C" {
#endif

#define HIH8000_I2CADDRESS  0x4E
#define HIH8000_SCALAR      16382.0
    
void HIH8000_Init();

uint8_t HIH8000_ReadHumidityTemperature(float *temperature, float *humidity);


#ifdef	__cplusplus
}
#endif

#endif	/* HIH8000_H */

