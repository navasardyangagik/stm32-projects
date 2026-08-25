/*
 * i2c.h
 *
 *  Created on: Aug 20, 2026
 *      Author: dracule
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx.h"
void I2C_WriteMulti (uint8_t *data, uint8_t size);
void I2C_Stop(void);
void I2C_Address_Write(uint8_t address);
void I2C_Address_Read(uint8_t address);
uint8_t I2C_Read(void);
void I2C_Write(uint8_t data); 
void I2C_Start(void);
void I2C_Config(void);
void I2C_Read_Buffer(uint8_t *data, uint8_t size);
uint8_t BME_ReadReg(uint8_t reg);
int32_t BME280_compensate_T_int32(int32_t adc_T);
extern uint16_t dig_T1;
extern int16_t  dig_T2;
extern int16_t  dig_T3;
void BME_ReadBuf(uint8_t addr, uint8_t *data, uint8_t size);
void BME_WriteReg(uint8_t reg, uint8_t data);

#endif /* I2C_H_ */
