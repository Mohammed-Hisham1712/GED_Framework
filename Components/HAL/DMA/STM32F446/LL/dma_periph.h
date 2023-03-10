#ifndef __DMA_PERIPH_H__

#define __DMA_PERIPH_H__

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>


#define DMA_LL_STREAMS  8

#define _DMA1   ((dma_dev_t) 0x40026000)
#define _DMA2   ((dma_dev_t) 0x40026400)

#define SxCR(_hw, _st)      ((_hw)->stream[(_st)].sxcr)
#define SxNDTR(_hw, _st)    ((_hw)->stream[(_st)].sxndtr)
#define SxPAR(_hw, _st)     ((_hw)->stream[(_st)].sxpar)
#define SxMAR0(_hw, _st)    ((_hw)->stream[(_st)].sxmar0)
#define SxMAR1(_hw, _st)    ((_hw)->stream[(_st)].sxmar1)
#define SxFCR(_hw, _st)     ((_hw)->stream[(_st)].sxfcr)

#define DMA_SxCR_CHSEL_S    (25)
#define DMA_SxCR_CHSEL_M    (0x07)

#define DMA_SxCR_MBURST_S   (23)
#define DMA_SxCR_MBURST_M   (0x03)

#define DMA_SxCR_PBURST_S   (21)
#define DMA_SxCR_PBURST_M   (0x03)

#define DMA_SxCR_CT_S       BIT(19)

#define DMA_SxCR_DBM_S      BIT(18)

#define DMA_SxCR_PL_S       (16)
#define DMA_SxCR_PL_M       (0x03)

#define DMA_SxCR_MSIZE_S    (13)
#define DMA_SxCR_MSIZE_M    (0x03)

#define DMA_SxCR_PSIZE_S    (11)
#define DMA_SxCR_PSIZE_M    (0x03)

#define DMA_SxCR_MINC_S     BIT(10)

#define DMA_SxCR_PINC_S     BIT(9)

#define DMA_SxCR_CIRC_S     BIT(8)

#define DMA_SxCR_DIR_S      (6)
#define DMA_SxCR_DIR_M      (0x03)

#define DMA_SxCR_TCIE_S     BIT(4)
#define DMA_SxCR_HTIE_S     BIT(3)
#define DMA_SxCR_TEIE_S     BIT(2)
#define DMA_SxCR_DMEIE_S    BIT(1)

#define DMA_SxCR_EN_S       BIT(0)

#define DMA_SxFCR_FEIE_S    BIT(7)
#define DMA_SxFCR_DMDIS_S   BIT(2)

#define DMA_ISR_FEI_S       BIT(0)
#define DMA_ISR_DMEI_S      BIT(2)
#define DMA_ISR_TEI_S       BIT(3)
#define DMA_ISR_HTI_S       BIT(4)
#define DMA_ISR_TCI_S       BIT(5)

#define DMA_ISR_ALL_S       (DMA_ISR_FEI_S | DMA_ISR_DMEI_S | DMA_ISR_TEI_S \
                                | DMA_ISR_HTI_S | DMA_ISR_TCI_S)       

typedef struct 
{
    uint32_t lisr;
    uint32_t hisr;
    uint32_t lifcr;
    uint32_t hifcr;
    struct 
    {
        uint32_t sxcr;
        uint32_t sxndtr;
        uint32_t sxpar;
        uint32_t sxmar0;
        uint32_t sxmar1;
        uint32_t sxfcr;
    } stream [DMA_LL_STREAMS];
}volatile * dma_dev_t;

static const dma_dev_t DMA_LL_DMAx[] = 
{
    _DMA1,
    _DMA2
};

#define DMA_LL_DEVS             ((sizeof(DMA_LL_DMAx)) / sizeof(dma_dev_t))
#define DMA_LL_GET_HW(num)      (((num) < DMA_LL_DEVS) ? DMA_LL_DMAx[(num)] : NULL)


#endif