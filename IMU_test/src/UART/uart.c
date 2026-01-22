#include <UART/uart.h>
#include "stdio.h"
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
/* 初始化指定串口 */
void uart_init(uart_port_t port)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (port)
    {
    case UART_PORT_4:
        err = R_SCI_UART_Open(&g_uart4_ctrl, &g_uart4_cfg);
        assert(FSP_SUCCESS == err);
        break;
    case UART_PORT_5:
           err = R_SCI_UART_Open(&g_uart5_ctrl, &g_uart5_cfg);
           assert(FSP_SUCCESS == err);
           break;
    default:
        /* 未配置的串口：不做处理（可扩展其他 g_uartX） */
        break;
    }
}

/* 发送完成标志 */
volatile bool uart_send_complete_flag = false;

/* 串口中断回调 */
void g_uart4_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
    case UART_EVENT_RX_CHAR:
    {

        break;
    }
    case UART_EVENT_TX_COMPLETE:
    {
        uart_send_complete_flag = true;
        break;
    }
    default:
        break;
    }
}
void g_uart5_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
    case UART_EVENT_RX_CHAR:
    {

        break;
    }
    case UART_EVENT_TX_COMPLETE:
    {
        uart_send_complete_flag = true;
        break;
    }
    default:
        break;
    }
}
// 简单的整数次幂（m^n），n 为非负
static unsigned long m_pow_n(unsigned long m, unsigned long n)
{
    unsigned long i = 0, ret = 1;
    for (i = 0; i < n; i++)
    {
        ret *= m;
    }
    return ret;
}

// 将格式化内容写入 outbuf，返回写入长度（不包含终止 \0）
// 支持: %d, %o, %x, %b, %s, %c, %f(6位小数), %%
static int format_to_buffer(char *outbuf, size_t maxlen, const char *fmt, va_list args)
{
    if (outbuf == NULL || fmt == NULL || maxlen == 0)
        return -1;

    size_t outpos = 0;
    const char *pStr = fmt;

    while (*pStr != '\0' && outpos + 1 < maxlen) // 保留1字节给终止\0
    {
        if (*pStr != '%')
        {
            // 支持在格式字符串中使用反斜杠转义序列写入控制字符：\r, \n, \t, \\
            if (*pStr == '\\' && *(pStr + 1) != '\0')
            {
                char next = *(pStr + 1);
                char putc = 0;
                if (next == 'r')
                    putc = '\r';
                else if (next == 'n')
                    putc = '\n';
                else if (next == 't')
                    putc = '\t';
                else if (next == '\\')
                    putc = '\\';

                if (putc != 0)
                {
                    if (outpos + 1 < maxlen)
                        outbuf[outpos++] = putc;
                    pStr += 2; // 跳过两个字符
                    continue;
                }
                // 未识别的转义序列则按原样输出反斜杠
            }

            outbuf[outpos++] = *pStr++;
            continue;
        }

        // 遇到 '%' 解析格式
        pStr++;
        if (*pStr == '\0')
            break;

        if (*pStr == '%')
        {
            outbuf[outpos++] = '%';
            pStr++;
            continue;
        }

        // 解析可选的精度格式，如 "%.3f"，仅支持点号后跟数字的精度
        int precision = -1; // -1 表示未指定，后续默认使用 6
        if (*pStr == '.')
        {
            const char *q = pStr + 1;
            int prec_val = 0;
            int digits = 0;
            while (*q && isdigit((unsigned char)*q))
            {
                prec_val = prec_val * 10 + (*q - '0');
                q++;
                digits++;
            }
            if (digits > 0)
            {
                precision = prec_val;
                pStr = q; // 跳过精度描述，pStr 指向格式字母
            }
        }

        int ArgIntVal = 0;
        unsigned long ArgHexVal = 0;
        char *ArgStrVal = NULL;
        double ArgFloVal = 0.0;
        unsigned long val_seg = 0;
        unsigned long val_temp = 0;
        int cnt = 0;

        switch (*pStr)
        {
        case 'c':
            ArgIntVal = va_arg(args, int);
            if (outpos + 1 < maxlen)
                outbuf[outpos++] = (char)ArgIntVal;
            pStr++;
            break;
        case 'd':
            ArgIntVal = va_arg(args, int);
            if (ArgIntVal < 0)
            {
                if (outpos + 1 < maxlen)
                    outbuf[outpos++] = '-';
                // 处理最小值会在取反时溢出，转换为 unsigned 处理
                val_temp = (unsigned long)(-(long)ArgIntVal);
            }
            else
            {
                val_temp = (unsigned long)ArgIntVal;
            }

            // 计算位数
            if (val_temp)
            {
                unsigned long t = val_temp;
                while (t)
                {
                    cnt++;
                    t /= 10;
                }
            }
            else
                cnt = 1;

            // 输出每位
            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = val_temp / m_pow_n(10, cnt - 1);
                val_temp %= m_pow_n(10, cnt - 1);
                outbuf[outpos++] = (char)val_seg + '0';
                cnt--;
            }
            pStr++;
            break;
        case 'o':
            ArgIntVal = va_arg(args, int);
            if (ArgIntVal < 0)
            {
                if (outpos + 1 < maxlen)
                    outbuf[outpos++] = '-';
                val_temp = (unsigned long)(-(long)ArgIntVal);
            }
            else
                val_temp = (unsigned long)ArgIntVal;

            if (val_temp)
            {
                unsigned long t = val_temp;
                while (t)
                {
                    cnt++;
                    t /= 8;
                }
            }
            else
                cnt = 1;

            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = val_temp / m_pow_n(8, cnt - 1);
                val_temp %= m_pow_n(8, cnt - 1);
                outbuf[outpos++] = (char)val_seg + '0';
                cnt--;
            }
            pStr++;
            break;
        case 'x':
            ArgHexVal = va_arg(args, unsigned long);
            if (ArgHexVal)
            {
                unsigned long t = ArgHexVal;
                while (t)
                {
                    cnt++;
                    t /= 16;
                }
            }
            else
                cnt = 1;

            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = ArgHexVal / m_pow_n(16, cnt - 1);
                ArgHexVal %= m_pow_n(16, cnt - 1);
                if (val_seg <= 9)
                    outbuf[outpos++] = (char)val_seg + '0';
                else
                    outbuf[outpos++] = (char)(val_seg - 10) + 'A';
                cnt--;
            }
            pStr++;
            break;
        case 'b':
            ArgIntVal = va_arg(args, int);
            val_temp = (unsigned long)ArgIntVal;
            if (val_temp)
            {
                unsigned long t = val_temp;
                while (t)
                {
                    cnt++;
                    t /= 2;
                }
            }
            else
                cnt = 1;

            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = val_temp / m_pow_n(2, cnt - 1);
                val_temp %= m_pow_n(2, cnt - 1);
                outbuf[outpos++] = (char)val_seg + '0';
                cnt--;
            }
            pStr++;
            break;
        case 's':
            ArgStrVal = va_arg(args, char *);
            if (ArgStrVal == NULL)
                ArgStrVal = "(null)";
            while (*ArgStrVal && outpos + 1 < maxlen)
            {
                outbuf[outpos++] = *ArgStrVal++;
            }
            pStr++;
            break;
        case 'f':
            ArgFloVal = va_arg(args, double);
            if (ArgFloVal < 0)
            {
                if (outpos + 1 < maxlen)
                    outbuf[outpos++] = '-';
                ArgFloVal = -ArgFloVal;
            }
            val_seg = (unsigned long)ArgFloVal; // 整数部分
            val_temp = val_seg;

            // 整数部分位数
            if (val_seg)
            {
                unsigned long t = val_seg;
                while (t)
                {
                    cnt++;
                    t /= 10;
                }
            }
            else
                cnt = 1;

            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = val_temp / m_pow_n(10, cnt - 1);
                val_temp %= m_pow_n(10, cnt - 1);
                outbuf[outpos++] = (char)val_seg + '0';
                cnt--;
            }

            // 小数点
            if (outpos + 1 < maxlen)
                outbuf[outpos++] = '.';

            // 输出小数部分，不四舍五入，位数由 precision 决定，默认 6
            if (precision < 0)
                precision = 6;
            ArgFloVal = ArgFloVal - (unsigned long)(ArgFloVal);
            // 乘以 10^precision
            {
                unsigned long mult = 1;
                for (int ii = 0; ii < precision; ii++)
                    mult *= 10u;
                ArgFloVal *= (double)mult;
                val_temp = (unsigned long)ArgFloVal;
                cnt = precision;
            }
            while (cnt && outpos + 1 < maxlen)
            {
                val_seg = val_temp / m_pow_n(10, cnt - 1);
                val_temp %= m_pow_n(10, cnt - 1);
                outbuf[outpos++] = (char)val_seg + '0';
                cnt--;
            }
            pStr++;
            break;
        default:
            // 未识别格式，输出一个空格占位
            if (outpos + 1 < maxlen)
                outbuf[outpos++] = ' ';
            pStr++;
            break;
        }
    }

    outbuf[outpos] = '\0';
    return (int)outpos;
}

// 串口与上位机通讯：可变参数 printf 风格，格式化到缓冲区后一次性发送
void uart_printf(uart_port_t uart_num, const char *fmt, ...)
{
    if (fmt == NULL)
        return;

    char buf[512];
    va_list args;
    va_start(args, fmt);
    int len = format_to_buffer(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len <= 0)
        return;

    if (len >= (int)sizeof(buf))
        len = (int)sizeof(buf) - 1;

    uart_send_complete_flag = false;
    switch (uart_num)
    {
    case UART_PORT_4:
    {
        fsp_err_t err = R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)buf, (uint32_t)len);
        if (err == FSP_SUCCESS)
        {
            // 等待完成，但增加超时以避免死循环
            unsigned int wait_cnt = 0;
            const unsigned int wait_max = 1000000u;
            while ((uart_send_complete_flag == false) && (wait_cnt++ < wait_max))
            {
                ;
            }
            uart_send_complete_flag = false;
        }
        else
        {
            // 写入失败（可能为忙等），退回到逐字节发送，带超时保护
            for (int i = 0; i < len; i++)
            {
                uart_send_complete_flag = false;
                (void)R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)&buf[i], 1u);
                unsigned int wait_cnt = 0;
                const unsigned int wait_max = 100000u;
                while ((uart_send_complete_flag == false) && (wait_cnt++ < wait_max))
                {
                    ;
                }
                uart_send_complete_flag = false;
            }
        }
    }
    break;
    default:
        break;
    }
}
