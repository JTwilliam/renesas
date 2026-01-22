#ifndef _IIC_H_
#define _IIC_H_
#include "hal_data.h"
#include "IIC/IIC.h"

typedef enum
{
    IIC_OK = 0x00U,
    IIC_ERR = 0x01U,
    IIC_BUSY = 0x02U,
    IIC_TIMEOUT = 0x03U
} IIC_Status_t;

typedef struct
{
    // 参数：设备地址，要写入的数据，要写入的数据长度，超时时间
    IIC_Status_t (*write)(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    IIC_Status_t (*read)(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

    // 参数：设备地址，要访问的寄存器地址，要写入的数据，要写入的数据长度，超时时间
    IIC_Status_t (*write_reg)(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    IIC_Status_t (*read_reg)(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

    void (*delay_ms)(uint16_t ms);
} IICInterface_t, *pIICInterface_t;
void SCI_IIC2_Init(uint32_t const slave_address);


void SCI_IIC2_read_bytes(uint8_t *pbuff, uint16_t length);


void SCI_IIC2_write_bytes(uint8_t *pbuff, uint16_t length);

// 读寄存器（与 bsp_iic 中的读寄存器格式对应，默认超时100ms）
void SCI_IIC2_read_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length);


// 写寄存器（与 bsp_iic 中的写寄存器格式对应，默认超时100ms）
void SCI_IIC2_write_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length);
extern IICInterface_t UserII2Dev;

#endif
