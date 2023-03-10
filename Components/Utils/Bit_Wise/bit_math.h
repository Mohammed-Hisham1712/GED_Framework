#ifndef _BIT_MATH_H_
#define _BIT_MATH_H

#include "stm32f446xx.h"

#include <stdint.h>

#define BIT(n)      (1UL << (n))
#define BIT64(n)    (1ULL << (n))

/* To get bit at position n in _r, _b should be BIT(n) or (1UL << n) */
#define GET_BIT(_r, _b) ((_r) & (_b))

/* To set bit at position n in _r, _b should be BIT(n) or (1UL << n) */
#define SET_BIT(_r, _b) ((_r) = (_r) | (_b))

/* To clear bit at position n in _r, _b should be BIT(n) or (1UL << n) */
#define CLR_BIT(_r, _b) ((_r) = (_r) & (~(_b)))

/* Get all bits set in mask shifted by _shift */
#define GET_BITS(_r, _shift, _mask) (((_r) & ((_mask) << (_shift))) >> (_shift))

/* Set all bits set in mask shifted by _shift */
#define SET_BITS(_r, _shift, _mask) ((_r) = (_r) | ((_mask) << (_shift)))

/* Clear all bits set in mask shifted by _shift */
#define CLR_BITS(_r, _shift, _mask) ((_r) = (_r) & (~((_mask) << (_shift))))

/* Set bits in _r in position _shift to equal _v. 
    *Bits included from _v are those set in _mask */
#define WRITE_BITS(_r, _shift, _mask, _v) ((_r) = (((_r) & (~((_mask) << (_shift)))) | \
                                          ((_v & _mask) << (_shift))))

#define REG_GET_BIT(_r, _b)     GET_BIT(_r, _b)
#define REG_SET_BIT(_r, _b)     SET_BIT(_r, _b)
#define REG_CLR_BIT(_r, _b)     CLR_BIT(_r, _b)

#define REG_GET_BITS(_r, _shift, _mask)     GET_BITS(_r, _shift, _mask)
#define REG_SET_BITS(_r, _shift, _mask)     SET_BITS(_r, _shift, _mask)
#define REG_CLR_BITS(_r, _shift, _mask)     CLR_BITS(_r, _shift, _mask)
#define REG_WRITE_BITS(_r, _shift, _mask, _v)   WRITE_BITS(_r, _shift, _mask, _v)

/* ATOMIC OPERATIONS */

#define ATOMIC_SET_BIT(_r, _b)              \
                                    do{     \
                                        uint32_t val;   \
                                        do{             \
                                            val = __LDREXW((volatile uint32_t*) &(_r)) | (_b); \
                                        }while(__STREXW(val, (volatile uint32_t*) &(_r)) != 0);   \
                                    }while(0)

#define ATOMIC_CLEAR_BIT(_r, _b)        \
                                    do{ \
                                        uint32_t val;   \
                                        do{ \
                                            val = __LDREXW((volatile uint32_t*) &(_r)) & ~(_b);  \
                                        }while(__STREXW(val, (volatile uint32_t*) &(_r)) != 0);   \
                                    }while(0)

#endif