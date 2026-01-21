#ifndef _OLED_H_
#define _OLED_H_
#include "hal_data.h"
#include "IIC/IIC.h"
/****************************************************
 * LOCAL MACROS AND DEFINITIONS
 ******************************************************************************/
#define OLED_DEBUG 1
#define OLED_ADDRESS 0x3c

typedef enum
{
   FONT_16 = 0,
   FONT_12 = 1,
} OLED_CHARSIZE;

typedef enum
{
   PARA = 0,
   DATA = 0x40,
} OLED_PARE_REG;

typedef enum
{
   X_WIDTH = 128,
   Y_WIDTH = 64,
} OLED_SCREEN_SIZE;

/******************************************************************************
 * EXPORTED MACROS AND DEFINITIONS
 ******************************************************************************/

/******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************/
void OLED_Clear(void);
void OLED_Init(void);
void OLED_UpScreenOn(void);

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *str);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t val);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_SetCharSize(uint8_t val);
void OLED_ShowMultiChar(uint8_t x, uint8_t y, uint8_t *str, uint8_t length);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
#endif
