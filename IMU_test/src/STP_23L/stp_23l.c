#include "stp_23l.h"
#include <string.h>

// STP23L 高度数据
float g_stp23l_distance = 0;

// 单个点云数据结构 (12 字节)
#pragma pack(1)
typedef struct
{
    int16_t distance;   // 距离 (2 bytes)
    uint16_t noise;     // 环境噪声 (2 bytes)
    uint32_t peak;      // 强度 (4 bytes)
    uint8_t confidence; // 置信度 (1 byte)
    uint32_t intg;      // 积分次数 (4 bytes) - 实际3字节，但为了对齐用4字节
} LidarPointTypedef;
#pragma pack()

// 完整一帧点云数据结构 (154 字节)
#pragma pack(1)
typedef struct
{
    LidarPointTypedef PointCloud[12]; // 12 个点云数据
    uint32_t timestamp;               // 时间戳
} STP23L_OneFrame_t;
#pragma pack()

static STP23L_OneFrame_t stp23l_frame;                          // 定义 1 帧 STP23L 数据
static const uint8_t stp23l_BufLen = sizeof(STP23L_OneFrame_t); // 原始数据长度
static uint8_t stp23l_buf[sizeof(STP23L_OneFrame_t)];           // 存储原始数据的数组
static uint8_t stp23l_counts = 0;                               // 接收数据的计数值

/**
 * @brief STP23L 距离处理回调函数
 * @param recv 接收到的字节
 * @param dis 距离输出指针
 *
 * 帧格式：
 * [10字节帧头] [154字节数据] [1字节校验和]
 * 帧头：0xAA 0xAA 0xAA 0xAA 0x00 0x02 0x00 0x00 0xB8 0x00
 */
void stp23l_getdistance_callback(uint8_t recv, float *dis)
{
    static uint8_t checkcode = 0; // 校验码

    // 状态机定义
    enum
    {
        Wait_HEAD = 0, // 等待帧头阶段
        Handle_DATA,   // 处理数据阶段
        END_DATA,      // 数据校验处理阶段
    };

    static uint8_t state_machine = Wait_HEAD; // 初始化状态机
    static uint8_t headlen = 0;               // 帧头计数

    switch (state_machine)
    {
    case Wait_HEAD:
    {
        // 帧头 10 字节：0xAA 0xAA 0xAA 0xAA 0x00 0x02 0x00 0x00 0xB8 0x00
        headlen++;
        if (10 == headlen)
        {
            // 帧头接收完成，初始校验码为：0x02 + 0xB8 = 0xBA
            headlen = 0;
            state_machine = Handle_DATA;
            checkcode = 0xBA; // 校验码初始值（0x02 + 0xB8）
        }
        break;
    }

    case Handle_DATA:
    {
        // 累积校验码
        checkcode += recv;

        // 接收数据
        stp23l_buf[stp23l_counts++] = recv;

        // 检查数据是否接收完毕
        if (stp23l_BufLen == stp23l_counts)
        {
            stp23l_counts = 0;
            state_machine = END_DATA;
        }
        break;
    }

    case END_DATA:
    {
        // 校验数据
        if (recv == checkcode)
        {
            // 数据赋值和解码
            memcpy(&stp23l_frame, stp23l_buf, stp23l_BufLen);

            // 计算 12 个点云数据的平均距离
            float all_dist = 0;
            for (uint8_t i = 0; i < 12; i++)
            {
                all_dist += stp23l_frame.PointCloud[i].distance;
            }

            // 平均距离，单位转换 mm → m
            if (dis != NULL)
            {
                *dis = all_dist / 12.0f; // 计算平均值
                *dis /= 1000.0f;         // 单位转换
            }

            // 更新全局距离
            g_stp23l_distance = all_dist / 12.0f / 1000.0f;
        }
        else
        {
            // 数据校验错误，不处理此帧数据
            // 可在此添加错误处理逻辑（如计数器等）
        }

        // 重置状态机
        checkcode = 0;
        state_machine = Wait_HEAD;
        break;
    }

    default:
        break;
    }
}
