#ifndef __EXTI_TYPES_H__

#define __EXTI_TYPES_H__

#include "exti_periph.h"

typedef enum
{
    EXTI_LINE_0,
    EXTI_LINE_1,
    EXTI_LINE_2,
    EXTI_LINE_3,
    EXTI_LINE_4,
    EXTI_LINE_5,
    EXTI_LINE_6,
    EXTI_LINE_7,
    EXTI_LINE_8,
    EXTI_LINE_9,
    EXTI_LINE_10,
    EXTI_LINE_11,
    EXTI_LINE_12,
    EXTI_LINE_13,
    EXTI_LINE_14,
    EXTI_LINE_15,
    EXTI_LINE_16,
    EXTI_LINE_17,
    EXTI_LINE_18,
    EXTI_LINE_19,
    EXTI_LINE_20,
    EXTI_LINE_21,
    EXTI_LINE_22,
    EXTI_MAX_LINES
} exti_line_t;

typedef enum
{
    EXTI_PORT_A,
    EXTI_PORT_B,
    EXTI_PORT_C,
    EXTI_PORT_D,
    EXTI_PORT_E,
    EXTI_PORT_F,
    EXTI_PORT_G,
    EXTI_PORT_H,
    EXTI_MAX_PORTS,
} exti_port_t;

typedef enum
{
    EXTI = 0
} exti_t;

#define EXTI_GET_LINE(_pin)         ((exti_line_t) (_pin))

#define EXTI_GET_PORT(_port)        ((exti_port_t) (_port))

#endif