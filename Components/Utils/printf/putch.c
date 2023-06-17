#include "serial.h"
#include "assert.h"

#ifdef CONFIG_PRINTF_SERIAL_PORT
#define PUTCH_SERIAL_PORT   CONFIG_PRINTF_SERIAL_PORT
#else
#error No serial port specified to use by putch_
#endif /* CONFIG_PRINTF_SERIAL_PORT */

void putch_(char ch)
{
    ASSERT(PUTCH_SERIAL_PORT < SERIAL_PORT_MAX);
    
    serial_tx(PUTCH_SERIAL_PORT, (const uint8_t*) &ch, sizeof(ch), NULL);
}