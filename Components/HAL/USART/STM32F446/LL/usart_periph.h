#ifndef __USART_PERIPH_H__
#define __USART_PERIPH_H__

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

#define USART_SR_TXE_S      BIT(7)
#define USART_SR_TC_S       BIT(6)
#define USART_SR_RXNE_S     BIT(5)
#define USART_SR_IDLE_S     BIT(4)
#define USART_SR_ORE_S      BIT(3)
#define USART_SR_NF_S       BIT(2)
#define USART_SR_FE_S       BIT(1)
#define USART_SR_PE_S       BIT(0)

#define USART_CR1_OVER8_S   BIT(15)
#define USART_CR1_UE_S      BIT(13)
#define USART_CR1_M_S       BIT(12)
#define USART_CR1_PCE_S     BIT(10)
#define USART_CR1_PS_S      BIT(9)
#define USART_CR1_PEIE_S    BIT(8)
#define USART_CR1_TXEIE_S   BIT(7)
#define USART_CR1_TCIE_S    BIT(6)
#define USART_CR1_RXNIE_S   BIT(5)
#define USART_CR1_IDLEIE_S  BIT(4)
#define USART_CR1_TE_S      BIT(3)
#define USART_CR1_RE_S      BIT(2)
#define USART_CR1_SBK_S     BIT(0)

#define USART_CR2_STOP_S    (12)
#define USART_CR2_STOP_M    (0x3)

#define USART_CR3_ONEBIT_S  BIT(11)
#define USART_CR3_DMAT_S    BIT(7)
#define USART_CR3_DMAR_S    BIT(6)
#define USART_CR3_EIE_S     BIT(0)

#define USART_BRR_MANTISSA_S    (4)
#define USART_BRR_MANTISSA_M    (0x0FFF)

#define USART_BRR_FRACTION_S    (0)
#define USART_BRR_FRACTION_M    (0x0F)

#define _USART1 ((usart_dev_t) 0x40011000)
#define _USART2 ((usart_dev_t) 0x40004400)
#define _USART3 ((usart_dev_t) 0x40004800)
#define _USART4 ((usart_dev_t) 0x40004C00)
#define _USART5 ((usart_dev_t) 0x40005000)
#define _USART6 ((usart_dev_t) 0x40011400)

typedef struct 
{
    uint32_t sr;
    uint32_t dr;
    uint32_t brr;
    uint32_t cr1;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t gtpr;
} volatile* usart_dev_t;

static const usart_dev_t USART_LL_USARTx[] =
{
    _USART1,
    _USART2,
    _USART3,
    _USART4,
    _USART5,
    _USART6
};

#define USART_LL_DEVS   ((sizeof(USART_LL_USARTx)) / (sizeof(USART_LL_USARTx[0])))

#define USART_LL_GET_HW(num)    ((num) < USART_LL_DEVS ? USART_LL_USARTx[(num)] : NULL)

#endif