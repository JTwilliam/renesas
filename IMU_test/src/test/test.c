#include "hal_data.h"
#include "UART/uart.h"
#include "IIC/IIC.h"
#include "ICM_20948/icm_20948.h"
#include "ICM_20948/icm_20948_reg.h"
#include "test.h"

static void dump_regs(uint16_t dev8, uint8_t start, uint8_t len)
{
    uint8_t buf[32];
    for (uint8_t ofs = 0; ofs < len; ofs += 8)
    {
        uint8_t to_read = (len - ofs) > 8 ? 8 : (len - ofs);
        // Use UserII2Dev.read_reg which expects Dev<<1 style; pass dev8 as provided
        UserII2Dev.read_reg(dev8, start + ofs, buf, to_read, 100);
        uart_printf(UART_PORT_4, "DEV0x%x @0x%x:", dev8 >> 1, (unsigned int)(start + ofs));
        for (uint8_t i = 0; i < to_read; i++)
        {
            uart_printf(UART_PORT_4, " %x", buf[i]);
        }
        uart_printf(UART_PORT_4, "\r\n");
    }
}

void test_icm20948(void)
{
    /* 初始化串口与 I2C */
    uart_init(UART_PORT_4);
    SCI_IIC2_Init(0x68 << 1);
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS); // 等待设备稳定

    pIMUInterface_t imu = &UserICM20948;
    // imu->DeInit();
    // R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    // if (imu->Init())
    // {
    //     uart_printf(UART_PORT_4, "ICM20948 init failed\r\n");
    //     // 失败时转储 ICM (bank0) 和 AK09916 寄存器便于诊断
    //     uart_printf(UART_PORT_4, "Dump ICM20948 registers (0x%x)...\r\n", (ICM20948_DEV << 1) >> 1);
    //     // ICM20948 bank0 regs 0x00..0x0F
    //     dump_regs(ICM20948_DEV << 1, 0x00, 16);
    //     R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    //     uart_printf(UART_PORT_4, "Dump AK09916 registers (0x%x)...\r\n", (AK09916_DEV << 1) >> 1);
    //     // AK09916 regs 0x00..0x0F
    //     dump_regs(AK09916_DEV << 1, 0x00, 16);
    //     return;
    // }

    IMU_DATA_t imu_ori = {0};
    ATTITUDE_DATA_t attitude_data = {0};
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

    while (1)
    {
        imu->Update_9axisVal(&imu_ori);
        imu->UpdateAttitude(imu_ori, &attitude_data);

        uart_printf(UART_PORT_4, "%.2f %.2f %.2f \r\n",
                    (float)attitude_data.pitch * 53.7f,
                    (float)attitude_data.roll * 53.7f,
                    (float)attitude_data.yaw * 53.7f);

        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    }
}
uint8_t Check_AK09916_WHO_AM_I(void)
{
    pIICInterface_t iicdev = &UserII2Dev; // iic设备

    uint8_t write_buf = 0;

    // 选择bank3
    write_buf = REG_VAL_SELECT_BANK_3;
    iicdev->write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &write_buf, 1, 100);

    // 写入要使用icm20948去访问的设备地址
    write_buf = AK09916_DEV | (1 << 7);
    iicdev->write_reg(ICM20948_DEV << 1, I2C_SLV0_ADDR, &write_buf, 1, 100);

    // 写入要使用icm20948去访问的寄存器地址
    write_buf = WIA;
    iicdev->write_reg(ICM20948_DEV << 1, I2C_SLV0_REG, &write_buf, 1, 100);

    // 使能从机访问功能，并指定读取多少字节
    write_buf = (1 << 7) | (1 << 0); // 使能读取，读取2字节
    iicdev->write_reg(ICM20948_DEV << 1, I2C_SLV0_CTRL, &write_buf, 1, 100);

    // 选择bank0
    write_buf = REG_VAL_SELECT_BANK_0;
    iicdev->write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &write_buf, 1, 100);

    // 使能IIC MST
    iicdev->read_reg(ICM20948_DEV << 1, USER_CTRL, &write_buf, 1, 100);  // 读取当前值
    write_buf |= (1 << 5);                                               // 使能IIC MST,且不修改寄存器的其他值
    iicdev->write_reg(ICM20948_DEV << 1, USER_CTRL, &write_buf, 1, 100); // 写入寄存器

    // 等待读取数据,不延迟无法得到数据(请使用适用的延迟方式)
    R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MILLISECONDS);

    // 失能I2C MST
    iicdev->read_reg(ICM20948_DEV << 1, USER_CTRL, &write_buf, 1, 100);  // 读取当前值
    write_buf &= ~(1 << 5);                                              // 失能IIC MST,且不修改寄存器的其他值
    iicdev->write_reg(ICM20948_DEV << 1, USER_CTRL, &write_buf, 1, 100); // 写入寄存器

    // 读取2字节(ICM20948将SLV0访问到的值放在EXT_SLV_SENS_DATA_00寄存器)
    iicdev->read_reg(ICM20948_DEV << 1, EXT_SLV_SENS_DATA_00, &write_buf, 1, 100);

    uart_printf(UART_PORT_4, "0x:%x", write_buf);
    // 0x09为AK09916 WHO AM I寄存器值
    if (write_buf == 0x09)
        write_buf = 1;
    else
        write_buf = 0;

    return write_buf;
}
