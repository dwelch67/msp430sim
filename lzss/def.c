
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short notmain ( void );

void uart_init ( void )
{
}
void hexstring ( unsigned short d )
{
    printf("0x%04X\n",d);
}

int main ( void )
{
    if(notmain()) printf("failed\n");
    else printf("pass\n");

    return(0);
}
