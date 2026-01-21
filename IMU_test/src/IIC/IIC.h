#ifndef _IIC_H_
#define _IIC_H_
#include "hal_data.h"
#include "IIC/IIC.h"

void SCI_IIC2_Init(uint32_t const slave_address);

void SCI_IIC2_read_bytes(uint8_t *pbuff, uint16_t length);

void SCI_IIC2_write_bytes(uint8_t *pbuff, uint16_t length);

// 读寄存器（默认超时100ms）
void SCI_IIC2_read_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length);

// 写寄存器（默认超时100ms）
void SCI_IIC2_write_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length);

#endif
