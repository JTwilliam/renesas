#include "icm_20948.h"

// ICM20948寄存器
#include "icm_20948_reg.h"
// imu需要iic支持包
#include "IIC/IIC.h"
#include "UART/uart.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

// 复制并移植自 bsp_imu.c，使用 SCI_IIC2_* 接口

static uint8_t ICM20948_Init(void)
{
    uint8_t writebuf = 0;

    // 选择 bank0
    writebuf = REG_VAL_SELECT_BANK_0;
    IIC_Status_t st = UserII2Dev.write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &writebuf, 1, 100);
    if (st != IIC_OK)
    {
        uart_printf(UART_PORT_4, "write_reg REG_BANK_SEL failed: %d\r\n", st);
    }

    // 访问 WHO AM I
    st = UserII2Dev.read_reg(ICM20948_DEV << 1, WHO_AM_I, &writebuf, 1, 100);
    if (st != IIC_OK)
    {
        uart_printf(UART_PORT_4, "read_reg WHO_AM_I failed: %d\r\n", st);
        return 1;
    }
    if (0xEA != writebuf)
    {
        uart_printf(UART_PORT_4, "ICM20948 WHO_AM_I = 0x%x\r\n", writebuf);
        return 1;
    }

    // PWR_MGMT_1 复位
    writebuf = (1 << 7);
    UserII2Dev.write_reg(ICM20948_DEV << 1, PWR_MGMT_1, &writebuf, 1, 100);
    R_BSP_SoftwareDelay(100U, BSP_DELAY_UNITS_MILLISECONDS);

    // user_ctrl 清零
    writebuf = 0x00;
    UserII2Dev.write_reg(ICM20948_DEV << 1, USER_CTRL, &writebuf, 1, 100);

    // 关闭低功耗，使能温度传感器，设置时钟
    writebuf = 0x01;
    UserII2Dev.write_reg(ICM20948_DEV << 1, PWR_MGMT_1, &writebuf, 1, 100);

    // 选择 bank2
    writebuf = REG_VAL_SELECT_BANK_2;
    UserII2Dev.write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &writebuf, 1, 100);

    // 设置 GYRO_SMPLRT_DIV
    writebuf = 0x04;
    UserII2Dev.write_reg(ICM20948_DEV << 1, GYRO_SMPLRT_DIV, &writebuf, 1, 100);

    // GYRO_CONFIG_1
    writebuf = (3 << 1) | (1 << 0) | (3 << 3);
    UserII2Dev.write_reg(ICM20948_DEV << 1, GYRO_CONFIG_1, &writebuf, 1, 100);

    // ACCEL_SMPLRT_DIV_2
    writebuf = 0x04;
    UserII2Dev.write_reg(ICM20948_DEV << 1, ACCEL_SMPLRT_DIV_2, &writebuf, 1, 100);

    // ACCEL_CONFIG
    writebuf = (0 << 1) | (1 << 0) | (5 << 3);
    UserII2Dev.write_reg(ICM20948_DEV << 1, ACCEL_CONFIG, &writebuf, 1, 100);

    // 回到 bank0
    writebuf = REG_VAL_SELECT_BANK_0;
    UserII2Dev.write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &writebuf, 1, 100);


    // 禁用 I2C Master（确保 bypass 模式完全启用）
    writebuf = 0x00;
    UserII2Dev.write_reg(ICM20948_DEV << 1, USER_CTRL, &writebuf, 1, 100);
    R_BSP_SoftwareDelay(10U, BSP_DELAY_UNITS_MILLISECONDS);

    // 配置中断引脚并启用 bypass 模式
    // INT_PIN_CFG = 0x22: bit5=1(INT LATCH), bit1=1(BYPASS I2C)
    writebuf = 0x22;
    UserII2Dev.write_reg(ICM20948_DEV << 1, INT_PIN_CFG, &writebuf, 1, 100);
    R_BSP_SoftwareDelay(50U, BSP_DELAY_UNITS_MILLISECONDS); // 增加延迟以确保 bypass 启用

    // 直接读取 AK09916 的 WIA 寄存器验证 bypass 工作
    st = UserII2Dev.read_reg(AK09916_DEV << 1, WIA, &writebuf, 1, 100);
    if (st != IIC_OK)
    {
        uart_printf(UART_PORT_4, "read_reg AK09916 WIA failed: %d\r\n", st);
        return 1;
    }
    if (0x09 != writebuf)
    {
        uart_printf(UART_PORT_4, "AK09916 WIA = 0x%x\r\n", writebuf);
        return 1;
    }

    // 软复位 AK09916
    writebuf = 0x01;
    UserII2Dev.write_reg(AK09916_DEV << 1, CNTL3, &writebuf, 1, 100);
    R_BSP_SoftwareDelay(10U, BSP_DELAY_UNITS_MILLISECONDS);

    // 配置磁力计为连续模式 100Hz
    writebuf = 0x08;
    UserII2Dev.write_reg(AK09916_DEV << 1, CNTL2, &writebuf, 1, 100);

    return 0;
}

static uint8_t ICM20948_DeInit(void)
{
    uint8_t writebuf = 0;

    // 选择 bank0
    writebuf = REG_VAL_SELECT_BANK_0;
    UserII2Dev.write_reg(ICM20948_DEV << 1, REG_BANK_SEL, &writebuf, 1, 100);

    // 检查 WHO AM I
    UserII2Dev.read_reg(ICM20948_DEV << 1, WHO_AM_I, &writebuf, 1, 100);
    if (0xEA != writebuf)
        return 1;

    // 睡眠
    writebuf = (1 << 6);
    UserII2Dev.write_reg(ICM20948_DEV << 1, PWR_MGMT_1, &writebuf, 1, 100);

    return 0;
}

static IMU_DATA_t ZeroPoint = {0};
static ATTITUDE_DATA_t ZeroAttitude = {0};

static void setZeroPoint_axis(const IMU_DATA_t *point)
{
    memcpy(&ZeroPoint, point, sizeof(IMU_DATA_t));
}

static void setZeroPoint_attitude(const ATTITUDE_DATA_t *attitude)
{
    memcpy(&ZeroAttitude, attitude, sizeof(ATTITUDE_DATA_t));
}

static float Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}

static const float Kp = 0.8f;
static const float Ki = 0.0001f;

static void attitudeUpdate(IMU_DATA_t imudata, ATTITUDE_DATA_t *attitude)
{
    static float eInt[3] = {0};
    static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float q0_t, q1_t, q2_t, q3_t;
    float halftime = 0.005f * 0.5f; // 2ms

    float gx = imudata.gyro.x;
    float gy = imudata.gyro.y;
    float gz = imudata.gyro.z;
    float ax = imudata.accel.x;
    float ay = imudata.accel.y;
    float az = imudata.accel.z;

    //    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    //    float q0q3 = q0 * q3;
    float q1q1 = q1 * q1;
    //    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    //    float q3q3 = q3 * q3;

    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = 1 - 2 * (q1q1 + q2q2);

    norm = Q_rsqrt(ax * ax + ay * ay + az * az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;

    eInt[0] += ex;
    eInt[1] += ey;
    eInt[2] += ez;

    gx += Kp * ex + Ki * eInt[0];
    gy += Kp * ey + Ki * eInt[1];
    gz += Kp * ez + Ki * eInt[2];

    q0_t = (-q1 * gx - q2 * gy - q3 * gz) * halftime;
    q1_t = (q0 * gx + q2 * gz - q3 * gy) * halftime;
    q2_t = (q0 * gy - q1 * gz + q3 * gx) * halftime;
    q3_t = (q0 * gz + q1 * gy - q2 * gx) * halftime;

    q0 += q0_t;
    q1 += q1_t;
    q2 += q2_t;
    q3 += q3_t;

    norm = Q_rsqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= norm;
    q1 *= norm;
    q2 *= norm;
    q3 *= norm;

    attitude->roll = atan2f(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2));
    attitude->pitch = asinf(2 * (q0 * q2 - q3 * q1));
    attitude->yaw = atan2f(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3));
}

static void ImuUpdate(IMU_DATA_t *data)
{
    uint8_t tmpbuf[12];
    uint8_t magnbuf[8];

    // 读取 accel+gyro 12 字节
    UserII2Dev.read_reg(ICM20948_DEV << 1, ACCEL_XOUT_H, tmpbuf, 12, 100);

    data->accel.x = (short)(tmpbuf[0] << 8 | tmpbuf[1]);
    data->accel.y = (short)(tmpbuf[2] << 8 | tmpbuf[3]);
    data->accel.z = (short)(tmpbuf[4] << 8 | tmpbuf[5]);

    data->accel.x *= 0.00059814453125f;
    data->accel.y *= 0.00059814453125f;
    data->accel.z *= 0.00059814453125f;

    data->gyro.x = (short)(tmpbuf[6] << 8 | tmpbuf[7]);
    data->gyro.y = (short)(tmpbuf[8] << 8 | tmpbuf[9]);
    data->gyro.z = (short)(tmpbuf[10] << 8 | tmpbuf[11]);

    data->gyro.x *= 0.06103515625f;
    data->gyro.y *= 0.06103515625f;
    data->gyro.z *= 0.06103515625f;
    data->gyro.x *= 0.01745329252f;
    data->gyro.y *= 0.01745329252f;
    data->gyro.z *= 0.01745329252f;

    data->gyro.x -= ZeroPoint.gyro.x;
    data->gyro.y -= ZeroPoint.gyro.y;
    data->gyro.z -= ZeroPoint.gyro.z;

    // 在 bypass 模式下，直接读取 AK09916 磁力计连续 8 字节数据
    UserII2Dev.read_reg(AK09916_DEV << 1, HXL, magnbuf, 8, 100);
    if (0 == ((magnbuf[6] >> 3) & 0x01)) // ST2 寄存器在 magnbuf[6]，bit3 = overflow
    {
        data->magn.x = (short)(magnbuf[1] << 8 | magnbuf[0]);
        data->magn.y = (short)(magnbuf[3] << 8 | magnbuf[2]);
        data->magn.z = (short)(magnbuf[5] << 8 | magnbuf[4]);

        data->magn.x *= 0.1495361328125f;
        data->magn.y *= 0.1495361328125f;
        data->magn.z *= 0.1495361328125f;
    }
}

// 按照 IMUInterface_t 导出
IMUInterface_t UserICM20948 = {
    .Init = ICM20948_Init,
    .DeInit = ICM20948_DeInit,
    .UpdateZeroPoint_axis = setZeroPoint_axis,
    .UpdateZeroPoint_attitude = setZeroPoint_attitude,
    .Update_9axisVal = ImuUpdate,
    .UpdateAttitude = attitudeUpdate};
