

extern void uart_init ( void );
extern void hexstring ( unsigned short d );
extern unsigned short dummy ( unsigned short );

volatile unsigned char data[256];

//-------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra,rb,rc;

    uart_init();
    hexstring(0x1234);


    rb=0;
    for(ra=0,rc=0;rc<10;ra++) rc=dummy(ra);
    rb+=ra;
    for(ra=0,rc=0;rc<=13;ra++) rc=dummy(ra);
    rb+=ra;
    for(ra=12,rc=0;rc>4;ra--) rc=dummy(ra);
    rb+=ra;
    for(ra=15,rc=0;rc;ra--) rc=dummy(ra);
    rb+=ra;
    hexstring(rb);

    for(ra=0;ra<256;ra++) data[ra]=ra;

    return(1);
}





