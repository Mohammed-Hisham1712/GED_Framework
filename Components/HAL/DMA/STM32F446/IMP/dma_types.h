#ifndef __DMA_TYPES_H__

#define __DMA_TYPES_H__

#include "dma_periph.h"

#include <stdint.h>

typedef enum
{
    DMA_CHANNEL_0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7,
    DMA_CHANNEL_MAX
} dma_channel_t;

typedef enum
{
    DMA_STREAM_0,
    DMA_STREAM_1,
    DMA_STREAM_2,
    DMA_STREAM_3,
    DMA_STREAM_4,
    DMA_STREAM_5,
    DMA_STREAM_6,
    DMA_STREAM_7,
    DMA_STREAM_MAX,
} dma_stream_t;

enum
{
    DMA1,
    DMA2,
    DMA_INV
};

typedef enum
{
    DMA_PRI_LOW,
    DMA_PRI_MED,
    DMA_PRI_HIGH,
    DMA_PRI_VERY_HIGH,
    DMA_PRI_INV
} dma_priority_t;

typedef enum
{
    DMA_MEM_SIZE_BYTE,
    DMA_MEM_SIZE_HALF_WORD,
    DMA_MEM_SIZE_WORD,
    DMA_MEM_SIZE_MAX
} dma_mem_size_t;

typedef enum
{
    DMA_PERIPH_SIZE_BYTE,
    DMA_PERIPH_SIZE_HALF_WORD,
    DMA_PERIPH_SIZE_WORD,
    DMA_PERIPH_SIZE_MAX
} dma_periph_size_t;

typedef enum
{
    DMA_PERIPH_TO_MEM,
    DMA_MEM_TO_PERIPH,
    DMA_MEM_TO_MEM,
} dma_dir_t;

typedef enum
{
    DMA_LEVEL_HALF,
    DMA_LEVEL_COMPLETE
} dma_tf_level_t;

#define DMA_NORMAL  0x00000001
#define DMA_CIRC    0x00000002
#define DMA_FIFO    0x00000004

#define DMA_MODE_DIRECT         (DMA_NORMAL)
#define DMA_MODE_DIRECT_CIRC    (DMA_NORMAL | DMA_CIRC)

#define DMA_MEM_INC_ENABLE      1
#define DMA_MEM_INC_DISABLE     0

#define DMA_PERIPH_INC_ENABLE   1
#define DMA_PERIPH_INC_DISABLE  0

#define DMA_DBM_ENABLE          1
#define DMA_DBM_DISABLE         0

#define DMA_ERROR_NONE          0x00000000
#define DMA_ERROR_TE            0x00000001
#define DMA_ERROR_DME           0x00000002
#define DMA_ERROR_FE            0x00000004

typedef struct 
{
    uint8_t channel;
    uint8_t priority;
    uint8_t mem_data_size;
    uint8_t periph_data_size;
    uint8_t mem_increment;
    uint8_t periph_increment;
    uint8_t mode;
    uint8_t dir;
    uint8_t dbm_enable;    
} dma_init_t;

typedef struct s_dma_hal_context_t
{
    dma_dev_t dev;
    dma_init_t dma_config;
    uint32_t error_code;
    uint32_t state;
    uint16_t remaining;
    uint8_t stream;
    void* parent;
    void (*error_callback) (struct s_dma_hal_context_t*);
    void (*xfer_half_callback) (struct s_dma_hal_context_t*);
    void (*xfer_complete_callback) (struct s_dma_hal_context_t*);
}dma_hal_context_t;

#endif