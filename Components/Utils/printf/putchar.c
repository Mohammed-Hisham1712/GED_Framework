#include "serial.h"
#include "assert.h"

#ifdef CONFIG_PRINTF_SERIAL_PORT
#define PUTCH_SERIAL_PORT   CONFIG_PRINTF_SERIAL_PORT
#else
#error No serial port specified to use by putch_
#endif /* CONFIG_PRINTF_SERIAL_PORT */

void putchar_(char ch)
{
    uint16_t sent;

    do
    {
        serial_tx(PUTCH_SERIAL_PORT, (const uint8_t*) &ch, sizeof(ch), &sent);
    } while(sent != sizeof(ch));
}