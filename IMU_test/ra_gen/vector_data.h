/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (9)
#endif
/* ISR prototypes */
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);
void sci_i2c_txi_isr(void);
void sci_i2c_tei_isr(void);
void gpt_counter_overflow_isr(void);
void gpt_capture_compare_a_isr(void);
void gpt_capture_compare_b_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_SCI4_RXI ((IRQn_Type) 0) /* SCI4 RXI (Receive data full) */
#define SCI4_RXI_IRQn          ((IRQn_Type) 0) /* SCI4 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI4_TXI ((IRQn_Type) 1) /* SCI4 TXI (Transmit data empty) */
#define SCI4_TXI_IRQn          ((IRQn_Type) 1) /* SCI4 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI4_TEI ((IRQn_Type) 2) /* SCI4 TEI (Transmit end) */
#define SCI4_TEI_IRQn          ((IRQn_Type) 2) /* SCI4 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI4_ERI ((IRQn_Type) 3) /* SCI4 ERI (Receive error) */
#define SCI4_ERI_IRQn          ((IRQn_Type) 3) /* SCI4 ERI (Receive error) */
#define VECTOR_NUMBER_SCI2_TXI ((IRQn_Type) 4) /* SCI2 TXI (Transmit data empty) */
#define SCI2_TXI_IRQn          ((IRQn_Type) 4) /* SCI2 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI2_TEI ((IRQn_Type) 5) /* SCI2 TEI (Transmit end) */
#define SCI2_TEI_IRQn          ((IRQn_Type) 5) /* SCI2 TEI (Transmit end) */
#define VECTOR_NUMBER_GPT2_COUNTER_OVERFLOW ((IRQn_Type) 6) /* GPT2 COUNTER OVERFLOW (Overflow) */
#define GPT2_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 6) /* GPT2 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_A ((IRQn_Type) 7) /* GPT2 CAPTURE COMPARE A (Capture/Compare match A) */
#define GPT2_CAPTURE_COMPARE_A_IRQn          ((IRQn_Type) 7) /* GPT2 CAPTURE COMPARE A (Capture/Compare match A) */
#define VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_B ((IRQn_Type) 8) /* GPT2 CAPTURE COMPARE B (Capture/Compare match B) */
#define GPT2_CAPTURE_COMPARE_B_IRQn          ((IRQn_Type) 8) /* GPT2 CAPTURE COMPARE B (Capture/Compare match B) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (9)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
