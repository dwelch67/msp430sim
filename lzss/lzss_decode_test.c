
extern void uart_init ( void );
extern void hexstring ( unsigned short d );

extern void decode(void);

unsigned short inf_data_len;
unsigned char inf_data[0x4000];

unsigned short dec_in_byte_done;
unsigned short dec_in_byte_len;

#include "def_data.h"

void dec_out_byte ( unsigned char c )
{
    inf_data[inf_data_len++]=c;
}

unsigned char dec_in_byte ( void )
{
    if(dec_in_byte_len<DEF_DATA_LEN)
    {
        dec_in_byte_done=0;
        return(def_data[dec_in_byte_len++]);
    }
    else
    {
        dec_in_byte_done=1;
        return(0);
    }
}

unsigned short run_def_test ( void )
{
    unsigned short ra,rb;

    inf_data_len=0;
    dec_in_byte_len=0;

    decode();

    hexstring(inf_data_len);
    if(inf_data_len!=INF_DATA_LEN) return(1);
    rb=0; for(ra=0;ra<inf_data_len;ra++) rb+=inf_data[ra]; rb&=0xFFFF;
    hexstring(rb);
    if(rb!=INF_DATA_CHECKSUM) return(1);

    return(0);
}


unsigned short notmain ( void )
{
    unsigned short ret;
    uart_init();
    hexstring(0x1234);
    ret=run_def_test();
    hexstring(ret);
    return(ret);
}


