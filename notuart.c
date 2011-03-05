

void uart_init ( void )
{
}


void uart_send ( unsigned char c )
{
    //for the simulator dont need to bother waiting for the txbuf to
    //have room for another character.
    *((volatile unsigned char *)0x77) = c;
}

//-------------------------------------------------------------------
void hexstring ( unsigned short d )
{
    *((volatile unsigned short *)0x58) = d;
}
