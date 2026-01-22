#ifndef __STP_23L_H
#define __STP_23L_H

#include <stdint.h>

// STP23L 传感器 DMA 数据缓冲区配置
#define STP23L_DMABUF_LEN 200

// STP23L 原始数据结构
typedef struct
{
    uint8_t Buf[STP23L_DMABUF_LEN];
    uint16_t DataLen;
} OriData_STP23L_t;

// 全局距离变量
extern float g_stp23l_distance;

// STP23L 数据处理回调函数
// 使用：每接收一个字节调用一次，传入接收的字节和距离输出指针
void stp23l_getdistance_callback(uint8_t recv, float *dis);

#endif /* __STP_23L_H */
