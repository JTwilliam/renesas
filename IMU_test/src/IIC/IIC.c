#include "IIC/IIC.h"
i2c_master_event_t g_i2c_callback_event;

void sci_i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_i2c_callback_event = p_args->event;
    }
}
void SCI_IIC2_Init(uint32_t const slave_address)
{
    fsp_err_t err;
    err = R_SCI_I2C_Open(&g_i2c2_ctrl, &g_i2c2_cfg);
    assert(err == FSP_SUCCESS);
    // 接受传入的 8-bit 地址（例如 0x68<<1），将其转换为 7-bit 再设置
    err = R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl, (uint32_t)(slave_address >> 1), I2C_MASTER_ADDR_MODE_7BIT);
    assert(err == FSP_SUCCESS);
}

void SCI_IIC2_read_bytes(uint8_t *pbuff, uint16_t length)
{
    unsigned int timeout_ms = 100;
    fsp_err_t err;

    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    err = R_SCI_I2C_Read(&g_i2c2_ctrl, pbuff, length, false);
    assert(FSP_SUCCESS == err);

    /* Since there is nothing else to do, block until Callback triggers */
    while ((I2C_MASTER_EVENT_RX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }
}

void SCI_IIC2_write_bytes(uint8_t *pbuff, uint16_t length)
{
    unsigned int timeout_ms = 100;
    fsp_err_t err;

    err = R_SCI_I2C_Write(&g_i2c2_ctrl, pbuff, length, false);
    assert(FSP_SUCCESS == err);

    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    /* Since there is nothing else to do, block until Callback_triggers*/
    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }
}

// 读寄存器（与 bsp_iic 中的读寄存器格式对应，默认超时100ms）
void SCI_IIC2_read_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length)
{
    unsigned int timeout_ms = 100;
    fsp_err_t err;
    uint8_t mem = (uint8_t)MemAddress;

    /* 可选：设置目标从机地址。接收的 DevAddress 可能为 8-bit（<<1），因此将其右移为 7-bit */
    err = R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl, (uint32_t)(DevAddress >> 1), I2C_MASTER_ADDR_MODE_7BIT);
    assert(err == FSP_SUCCESS);

    /* 先写寄存器地址 */
    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    err = R_SCI_I2C_Write(&g_i2c2_ctrl, &mem, 1, false);
    assert(FSP_SUCCESS == err);
    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }

    /* 再读取寄存器数据 */
    timeout_ms = 100;
    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    err = R_SCI_I2C_Read(&g_i2c2_ctrl, pbuff, length, false);
    assert(FSP_SUCCESS == err);
    while ((I2C_MASTER_EVENT_RX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }
}

// 写寄存器（与 bsp_iic 中的写寄存器格式对应，默认超时100ms）
void SCI_IIC2_write_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pbuff, uint16_t length)
{
    unsigned int timeout_ms = 100;
    fsp_err_t err;
    uint8_t mem = (uint8_t)MemAddress;

    /* 可选：设置目标从机地址。接收的 DevAddress 可能为 8-bit（<<1），因此将其右移为 7-bit */
    err = R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl, (uint32_t)(DevAddress >> 1), I2C_MASTER_ADDR_MODE_7BIT);
    assert(err == FSP_SUCCESS);

    /* 先写寄存器地址 */
    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    err = R_SCI_I2C_Write(&g_i2c2_ctrl, &mem, 1, false);
    assert(FSP_SUCCESS == err);
    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }

    /* 再写寄存器的数据 */
    timeout_ms = 100;
    g_i2c_callback_event = I2C_MASTER_EVENT_ABORTED;
    err = R_SCI_I2C_Write(&g_i2c2_ctrl, pbuff, length, false);
    assert(FSP_SUCCESS == err);
    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }
}
// ----- BSP-style IIC 接口适配 -----

static IIC_Status_t iic_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (NULL == pData || Size == 0)
        return IIC_ERR;

    // 设置目标从机地址（传入可能为 8-bit 格式，转换为 7-bit）
    if (FSP_SUCCESS != R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl, (uint32_t)(DevAddress >> 1), I2C_MASTER_ADDR_MODE_7BIT))
    {
        return IIC_ERR;
    }

    SCI_IIC2_write_bytes(pData, Size);
    return IIC_OK;
}

static IIC_Status_t iic_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (NULL == pData || Size == 0)
        return IIC_ERR;

    if (FSP_SUCCESS != R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl, (uint32_t)(DevAddress >> 1), I2C_MASTER_ADDR_MODE_7BIT))
    {
        return IIC_ERR;
    }

    SCI_IIC2_read_bytes(pData, Size);
    return IIC_OK;
}

static IIC_Status_t iic_write_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (NULL == pData || Size == 0)
        return IIC_ERR;
    SCI_IIC2_write_reg(DevAddress, MemAddress, pData, Size);
    return IIC_OK;
}

static IIC_Status_t iic_read_reg(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)Timeout;
    if (NULL == pData || Size == 0)
        return IIC_ERR;
    SCI_IIC2_read_reg(DevAddress, MemAddress, pData, Size);
    return IIC_OK;
}

// 将延迟固定为 100ms，忽略传入参数
static void iic_delayms(uint16_t ms)
{
    (void)ms;
    R_BSP_SoftwareDelay(100U, BSP_DELAY_UNITS_MILLISECONDS);
}

IICInterface_t UserII2Dev = {
    .write = iic_write,
    .read = iic_read,
    .write_reg = iic_write_reg,
    .read_reg = iic_read_reg,
    .delay_ms = iic_delayms};
