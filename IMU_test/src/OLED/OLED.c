
#include "OLED.h"
/*******************************************************************************
 * LOCAL INCLUDE FILES
 *******************************************************************************/
#include "OLED_Front.h"

static uint8_t charSize;
/******************************************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************************************/
static void OLED_WriteRegister(uint8_t val, uint8_t reg);

/******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************/
void OLED_Init(void)
{
    SCI_IIC2_Init(OLED_ADDRESS);

    OLED_WriteRegister(0xAE, PARA); //--display off
    OLED_WriteRegister(0x00, PARA); //---set low column address
    OLED_WriteRegister(0x10, PARA); //---set high column address
    OLED_WriteRegister(0x40, PARA); //--set start line address
    OLED_WriteRegister(0xB0, PARA); //--set page address
    OLED_WriteRegister(0x81, PARA); // contract control
    OLED_WriteRegister(0xFF, PARA); //--128
    OLED_WriteRegister(0xA1, PARA); // set segment remap
    OLED_WriteRegister(0xA6, PARA); //--normal / reverse
    OLED_WriteRegister(0xA8, PARA); //--set multiplex ratio(1 to 64)
    OLED_WriteRegister(0x3F, PARA); //--1/32 duty
    OLED_WriteRegister(0xC8, PARA); // Com scan direction
    OLED_WriteRegister(0xD3, PARA); //-set display offset
    OLED_WriteRegister(0x00, PARA); //

    OLED_WriteRegister(0xD5, PARA); // set osc division

    OLED_WriteRegister(0x80, PARA); //

    OLED_WriteRegister(0xD8, PARA); // set area color mode off
    OLED_WriteRegister(0x05, PARA); //

    OLED_WriteRegister(0xD9, PARA); // Set Pre-Charge Period
    OLED_WriteRegister(0xF1, PARA); //

    OLED_WriteRegister(0xDA, PARA); // set com pin configuartion
    OLED_WriteRegister(0x12, PARA); //

    OLED_WriteRegister(0xDB, PARA); // set Vcomh
    OLED_WriteRegister(0x30, PARA); //

    OLED_WriteRegister(0x8D, PARA); // set charge pump enable
    OLED_WriteRegister(0x14, PARA); //

    OLED_WriteRegister(0xAF, PARA); //--turn on oled panel

    OLED_Clear();

    // #if OLED_DEBUG
    OLED_SetCharSize(FONT_16);
    //     OLED_ShowString(6,0,(uint8_t*)"0.96' OLED TEST");

    //     OLED_SetCharSize( FONT_12 );
    //     OLED_ShowString(2,6,(uint8_t*)"ASCII:");
    //     OLED_ShowString(60,6,(uint8_t*)"-CODE:");
    // #endif
}

void OLED_SetCharSize(uint8_t val)
{
    charSize = val;
}

void OLED_SetCursor(uint8_t x, uint8_t y)
{
    OLED_WriteRegister(0xb0 + y, PARA);
    OLED_WriteRegister(((x & 0xf0) >> 4) | 0x10, PARA);
    OLED_WriteRegister((x & 0x0f), PARA);
}

void OLED_DisplayOn(void)
{
    OLED_WriteRegister(0X8D, PARA);
    OLED_WriteRegister(0X14, PARA);
    OLED_WriteRegister(0XAF, PARA);
}

void OLED_DisplayOff(void)
{
    OLED_WriteRegister(0X8D, PARA);
    OLED_WriteRegister(0X10, PARA);
    OLED_WriteRegister(0XAE, PARA);
}

void OLED_Clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        OLED_WriteRegister(0xb0 + i, PARA);
        OLED_WriteRegister(0x00, PARA);
        OLED_WriteRegister(0x10, PARA);

        for (n = 0; n < 128; n++)
        {
            OLED_WriteRegister(0, DATA);
        }
    }
}

void OLED_UpScreenOn(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        OLED_WriteRegister(0xb0 + i, PARA);
        OLED_WriteRegister(0x00, PARA);
        OLED_WriteRegister(0x10, PARA);

        for (n = 0; n < 128; n++)
        {
            OLED_WriteRegister(1, DATA);
        }
    }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t val)
{
    /* x: �� (1~4), y: �� (1~16) */
    uint8_t character = 0;
    uint8_t i = 0;
    uint8_t col_pixel;
    uint8_t page;

    character = val - ' ';

    /* �� ��/�� ת��Ϊ ������ �� ҳ(address page) */
    col_pixel = (uint8_t)((y - 1) * 8); /* ÿ�п� 8 ���� */
    page = (uint8_t)((x - 1) * 2);      /* ÿ��ռ 2 �� page (16 ����) */

    if (charSize == FONT_16)
    {
        /* �ϰ벿�� */
        OLED_SetCursor(col_pixel, page);
        for (i = 0; i < 8; i++)
        {
            OLED_WriteRegister(F8X16[character * 16 + i], DATA);
        }

        /* �°벿�� */
        OLED_SetCursor(col_pixel, page + 1);
        for (i = 0; i < 8; i++)
        {
            OLED_WriteRegister(F8X16[character * 16 + i + 8], DATA);
        }
    }
    else
    {
        /* 6x8 �������һ�� page �ڣ���������λ��д�� */
        OLED_SetCursor(col_pixel, page);
        for (i = 0; i < 6; i++)
        {
            OLED_WriteRegister(F6x8[character * 6 + i], DATA);
        }
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *str)
{
    /* x: �� (1~4), y: ��ʼ�� (1~16) */
    uint8_t index = 0;

    while (str[index] != '\0')
    {
        OLED_ShowChar(x, y + index, str[index]);
        index++;
    }
}

void OLED_ShowMultiChar(uint8_t x, uint8_t y, uint8_t *str, uint8_t length)
{
    /* x: ��, y: ��ʼ��, length: �ַ��� */
    for (uint8_t index = 0; index < length; index++)
    {
        OLED_ShowChar(x, y + index, str[index]);
    }
}

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/
static void OLED_WriteRegister(uint8_t val, uint8_t reg)
{
    fsp_err_t err;
    unsigned char buff[2];

    err = R_SCI_I2C_SlaveAddressSet(&g_i2c2_ctrl,
                                    OLED_ADDRESS,
                                    I2C_MASTER_ADDR_MODE_7BIT);
    assert(FSP_SUCCESS == err);

    buff[0] = reg;
    buff[1] = val;
    SCI_IIC2_write_bytes(buff, 2);
}
