#include "stdio_public.h"

int putch_(char ch)
{
    unsigned char u_ch;

    u_ch = (unsigned char) ch;

    putchar_(ch);

    return u_ch;
}

int puts_(const char* s)
{
    int cnt;

    if(!s)
    {
        return EOF;
    }

    cnt = 0;

    while(*s)
    {
        putchar_(*s++);
        cnt++;
    }

    putchar_('\r');
    putchar_('\n');

    return cnt;
}