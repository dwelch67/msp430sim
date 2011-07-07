/* LZSS encoder-decoder  (c) Haruhiko Okumura */

#include "common.h"

extern unsigned short dec_in_byte_done;
extern void dec_out_byte ( unsigned char c );
extern unsigned char dec_in_byte ( void );

static unsigned char getbit_buf,getbit_mask;

unsigned short getbit(int n) /* get n bits */
{
    unsigned short i, x;

    x = 0;
    for (i = 0; i < n; i++)
    {
        if (getbit_mask == 0)
        {
            getbit_buf = dec_in_byte(); if(dec_in_byte_done) return(0);
            getbit_mask = 128;
        }
        x <<= 1;
        if (getbit_buf & getbit_mask) x++;
        getbit_mask >>= 1;
    }
    return x;
}

void decode(void)
{
    unsigned short i, j, k, r, c;

    getbit_mask = 0;
    getbit_buf = 0;

    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    r = N - F;
    while(1)
    {
        c = getbit(1); if(dec_in_byte_done) break;
        if (c)
        {
            c = getbit(8); if(dec_in_byte_done) break;
            dec_out_byte(c);
            buffer[r++] = c;  r &= (N - 1);
        }
        else
        {
            i = getbit(EI); if(dec_in_byte_done) break;
            j = getbit(EJ); if(dec_in_byte_done) break;
            for (k = 0; k <= j + 1; k++)
            {
                c = buffer[(i + k) & (N - 1)];
                dec_out_byte(c);
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }
}
