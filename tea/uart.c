

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
    unsigned short rb;
    unsigned short rc;

    *((volatile unsigned short *)0x58) = d;


    rb=16;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x0D);
    uart_send(0x0A);
}
//-------------------------------------------------------------------
void hexstrings ( unsigned short d )
{
    unsigned short rb;
    unsigned short rc;

    *((volatile unsigned short *)0x58) = d;


    rb=16;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}
