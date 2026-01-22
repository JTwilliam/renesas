/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI4 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI4 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI4 ERI (Receive error) */
            [4] = sci_i2c_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [5] = sci_i2c_tei_isr, /* SCI2 TEI (Transmit end) */
            [6] = gpt_counter_overflow_isr, /* GPT2 COUNTER OVERFLOW (Overflow) */
            [7] = gpt_capture_compare_a_isr, /* GPT2 CAPTURE COMPARE A (Capture/Compare match A) */
            [8] = gpt_capture_compare_b_isr, /* GPT2 CAPTURE COMPARE B (Capture/Compare match B) */
            [9] = sci_uart_rxi_isr, /* SCI5 RXI (Receive data full) */
            [10] = sci_uart_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [11] = sci_uart_tei_isr, /* SCI5 TEI (Transmit end) */
            [12] = sci_uart_eri_isr, /* SCI5 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI4_RXI,GROUP0), /* SCI4 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,GROUP1), /* SCI4 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,GROUP2), /* SCI4 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI4_ERI,GROUP3), /* SCI4 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP4), /* SCI2 TXI (Transmit data empty) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP5), /* SCI2 TEI (Transmit end) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_GPT2_COUNTER_OVERFLOW,GROUP6), /* GPT2 COUNTER OVERFLOW (Overflow) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_GPT2_CAPTURE_COMPARE_A,GROUP7), /* GPT2 CAPTURE COMPARE A (Capture/Compare match A) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_GPT2_CAPTURE_COMPARE_B,GROUP0), /* GPT2 CAPTURE COMPARE B (Capture/Compare match B) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI5_RXI,GROUP1), /* SCI5 RXI (Receive data full) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TXI,GROUP2), /* SCI5 TXI (Transmit data empty) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TEI,GROUP3), /* SCI5 TEI (Transmit end) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI5_ERI,GROUP4), /* SCI5 ERI (Receive error) */
        };
        #endif
        #endif
