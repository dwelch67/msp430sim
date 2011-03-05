
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int notmain ( void );

void uart_init ( void )
{
}
void hexstring ( unsigned short d )
{
    printf("0x%04X\n",d);
}
unsigned short dummy ( unsigned short d )
{
    return(d);
}


int main ( void )
{
    notmain();
    return(0);
}

