#include "assert.h"

#include <stdint.h>

#define SRAM_START	0x20000000U
#define SRAM_SIZE	(128 * 1024)	/* 128kB */
#define SRAM_END	((SRAM_START) + (SRAM_SIZE))
#define STACK_START	(SRAM_END)

#define RESET_VECTOR_SIZE 112

extern uint8_t _la_data;
extern uint8_t _sdata;
extern uint8_t _edata;

extern uint8_t _sbss;
extern uint8_t _ebss;

/* prototypes */
int main(void);

/* reset vector prototype functions */
void reset_handler(void);
void nmi_handler(void) __attribute__((weak, alias("default_handler")));
void svc_handler(void) __attribute__((weak, alias("default_handler")));
void pendsv_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void) __attribute__((weak, alias("default_handler")));

void exti0_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti1_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti2_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti3_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti4_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti9_5_irq_handler(void) __attribute__((weak, alias("default_handler")));
void exti15_10_irq_handler(void) __attribute__((weak, alias("default_handler")));

void dma1_stream0_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream1_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream2_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream3_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream4_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream5_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream6_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_stream7_irq_handler(void) __attribute__((weak, alias("default_handler")));

void dma2_stream0_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream1_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream2_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream3_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream4_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream5_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream6_irq_handler(void) __attribute__((weak, alias("default_handler")));
void dma2_stream7_irq_handler(void) __attribute__((weak, alias("default_handler")));

void usart1_irq_handler(void) __attribute__((weak, alias("default_handler")));
void usart2_irq_handler(void) __attribute__((weak, alias("default_handler")));
void usart3_irq_handler(void) __attribute__((weak, alias("default_handler")));

void tim6_irq_handler(void)	__attribute__((weak, alias("default_handler")));
void tim7_irq_handler(void)	__attribute__((weak, alias("default_handler")));

__attribute__((section(".reset_vector")))
uint32_t reset_vector[RESET_VECTOR_SIZE] = 
{
	(uint32_t) STACK_START,
	(uint32_t) &reset_handler,
	(uint32_t) &nmi_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &svc_handler,
	0,
	0,
	(uint32_t) &pendsv_handler,
	(uint32_t) &systick_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &exti0_irq_handler,
	(uint32_t) &exti1_irq_handler,
	(uint32_t) &exti2_irq_handler,
	(uint32_t) &exti3_irq_handler,
	(uint32_t) &exti4_irq_handler,
	(uint32_t) &dma1_stream0_irq_handler,
	(uint32_t) &dma1_stream1_irq_handler,
	(uint32_t) &dma1_stream2_irq_handler,
	(uint32_t) &dma1_stream3_irq_handler,
	(uint32_t) &dma1_stream4_irq_handler,
	(uint32_t) &dma1_stream5_irq_handler,
	(uint32_t) &dma1_stream6_irq_handler,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &exti9_5_irq_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &usart1_irq_handler,
	(uint32_t) &usart2_irq_handler,
	(uint32_t) &usart3_irq_handler,
	(uint32_t) &exti15_10_irq_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &dma1_stream7_irq_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &tim6_irq_handler,
	(uint32_t) &tim7_irq_handler,
	(uint32_t) &dma2_stream0_irq_handler,
	(uint32_t) &dma2_stream1_irq_handler,
	(uint32_t) &dma2_stream2_irq_handler,
	(uint32_t) &dma2_stream3_irq_handler,
	(uint32_t) &dma2_stream4_irq_handler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(uint32_t) &dma2_stream5_irq_handler,
	(uint32_t) &dma2_stream6_irq_handler,
	(uint32_t) &dma2_stream7_irq_handler,
};

void default_handler(void)
{
	ASSERT(0);
	
	while (1);
}

__attribute__((noreturn))
void reset_handler(void)
{
	/* Init data section in SRAM */
	uint32_t size = (uint32_t) &_edata - (uint32_t) &_sdata;
	uint8_t* sptr = &_la_data;
	uint8_t* dptr = &_sdata;
	uint32_t i; 

	for(i = 0; i < size; i++)
	{
		*dptr = *sptr;
		sptr++;
		dptr++;
	}

	/* Init bss section in SRAM */
	size = (uint32_t) &_ebss - (uint32_t) &_sbss;
	dptr = &_sbss;
	for(i = 0; i < size; i++)
	{
		*dptr = 0;
		dptr++;
	}
	
	main();

	while(1);
}

void _exit(int x)
{
	(void) x;
	while(1);
}
