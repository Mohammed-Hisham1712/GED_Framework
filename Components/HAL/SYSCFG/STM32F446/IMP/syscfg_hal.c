#include "syscfg_hal_ext.h"
#include "syscfg_ll.h"

#include "rcc_hal_ext.h"

void syscfg_hal_init(void)
{
    rcc_hal_context_t rcc;

    RCC_HAL_GET_HW(&rcc, RCC);
    rcc_hal_apb2_en_clk(&rcc, RCC_HAL_SYSCFG);
}

void syscfg_hal_set_exti_port(syscfg_hal_context_t* hal, uint32_t line, uint32_t port)
{
    ASSERT(hal);

    syscfg_ll_set_exti_port(hal->dev, line, port);
}