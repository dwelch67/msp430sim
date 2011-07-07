/* LZSS encoder-decoder  (c) Haruhiko Okumura */

#include "common.h"

extern unsigned short enc_in_byte_done;
extern void enc_out_byte ( unsigned char c );
extern unsigned char enc_in_byte ( void );


void encode(void)
{
    int i, j, f1, x, y, r, s, bufferend, c;

    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    for (i = N - F; i < N * 2; i++)
    {
        //if ((c = fgetc(infile)) == EOF) break;
        c = enc_in_byte(); if(enc_in_byte_done) break;
        buffer[i] = c;
    }
    bufferend = i;  r = N - F;  s = 0;
    while (r < bufferend) {
        f1 = (F <= bufferend - r) ? F : bufferend - r;
        x = 0;  y = 1;  c = buffer[r];
        for (i = r - 1; i >= s; i--)
            if (buffer[i] == c) {
                for (j = 1; j < f1; j++)
                    if (buffer[i + j] != buffer[r + j]) break;
                if (j > y) {
                    x = i;  y = j;
                }
            }
        if (y <= P) output1(c);
        else output2(x & (N - 1), y - 2);
        r += y;  s += y;
        if (r >= N * 2 - F) {
            for (i = 0; i < N; i++) buffer[i] = buffer[i + N];
            bufferend -= N;  r -= N;  s -= N;
            while (bufferend < N * 2)
            {
                //if ((c = fgetc(infile)) == EOF) break;
                c = enc_in_byte(); if(enc_in_byte_done) break;
                buffer[bufferend++] = c;
            }
        }
    }
    flush_bit_buffer();
}
