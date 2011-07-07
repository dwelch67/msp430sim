/* LZSS encoder-decoder  (c) Haruhiko Okumura */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "twain.h"

FILE *fp;

unsigned int def_len;
unsigned int inf_len;
unsigned char def_data[0x8000];
unsigned char inf_data[0x8000];

unsigned char in_data[0x8000];
unsigned char out_data[0x8000];

#define EI 11  /* typically 10..13 */
#define EJ  4  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + P)  /* lookahead buffer size */

unsigned int out_data_index, in_data_index;
unsigned int in_data_len;

int inbyte_done;
int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;
unsigned char buffer[N * 2];

void out_byte ( unsigned char c )
{
    out_data[out_data_index++]=c;
}

unsigned char in_byte ( void )
{
    if(in_data_index<in_data_len)
    {
        inbyte_done=0;
        return(in_data[in_data_index++]);
    }
    else
    {
        inbyte_done=1;
        return(0);
    }
}


void putbit1(void)
{
    bit_buffer |= bit_mask;
    if ((bit_mask >>= 1) == 0)
    {
        out_byte(bit_buffer);
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void putbit0(void)
{
    if ((bit_mask >>= 1) == 0)
    {
        out_byte(bit_buffer);
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void flush_bit_buffer(void)
{
    if (bit_mask != 128)
    {
        out_byte(bit_buffer);
        codecount++;
    }
}

void output1(int c)
{
    int mask;

    putbit1();
    mask = 256;
    while (mask >>= 1) {
        if (c & mask) putbit1();
        else putbit0();
    }
}

void output2(int x, int y)
{
    int mask;

    putbit0();
    mask = N;
    while (mask >>= 1) {
        if (x & mask) putbit1();
        else putbit0();
    }
    mask = (1 << EJ);
    while (mask >>= 1) {
        if (y & mask) putbit1();
        else putbit0();
    }
}

void encode(void)
{
    int i, j, f1, x, y, r, s, bufferend, c;

    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    for (i = N - F; i < N * 2; i++)
    {
        //if ((c = fgetc(infile)) == EOF) break;
        c = in_byte(); if(inbyte_done) break;
        buffer[i] = c;  textcount++;
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
                c = in_byte(); if(inbyte_done) break;
                buffer[bufferend++] = c;  textcount++;
            }
        }
    }
    flush_bit_buffer();
    printf("text:  %ld bytes\n", textcount);
    printf("code:  %ld bytes (%ld%%)\n",
        codecount, (codecount * 100) / textcount);
}

int getbit(int n) /* get n bits */
{
    int i, x;
    static int buf, mask = 0;

    x = 0;
    for (i = 0; i < n; i++) {
        if (mask == 0)
        {

            //if ((buf = fgetc(infile)) == EOF) return EOF;
            buf = in_byte(); if(inbyte_done) return(0);
            mask = 128;
        }
        x <<= 1;
        if (buf & mask) x++;
        mask >>= 1;
    }
    return x;
}

void decode(void)
{
    int i, j, k, r, c;

    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    r = N - F;
    while(1)
    {
        c = getbit(1); if(inbyte_done) break;

        if (c)
        {
            c = getbit(8); if(inbyte_done) break;
            out_byte(c);
            buffer[r++] = c;  r &= (N - 1);
        } else {
            i = getbit(EI); if(inbyte_done) break;
            j = getbit(EJ); if(inbyte_done) break;
            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                out_byte(c);
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }
}

int main ( void )
{
    unsigned int ra,rb;

    in_data_len = TEST_DATA_LEN;
    memcpy(in_data,test_data,in_data_len);

    in_data_index=0;
    out_data_index=0;

    encode();

    printf("%u %u\n",in_data_index,out_data_index);


fp=fopen("def_data.h","wt");
if(fp==NULL)
{
    return(1);
}
    fprintf(fp,"\n");
    fprintf(fp,"#define DEF_DATA_LEN %u\n",out_data_index);
    fprintf(fp,"const unsigned char def_data[DEF_DATA_LEN]=\n");
    fprintf(fp,"{");
    for(ra=0;ra<out_data_index;ra++)
    {
        if((ra&7)==0) fprintf(fp,"\n  ");
        fprintf(fp,"0x%02X,",out_data[ra]);
    }
    fprintf(fp,"\n");
    fprintf(fp,"};\n");
    fprintf(fp,"\n");
rb=0;
for(ra=0;ra<in_data_index;ra++) rb+=in_data[ra];
    fprintf(fp,"#define INF_DATA_CHECKSUM 0x%04X\n",rb&0xFFFF);
    fprintf(fp,"#define INF_DATA_LEN      %u\n",in_data_index);
    fclose(fp);


rb=0; for(ra=0;ra<TEST_DATA_LEN;ra++) rb+=test_data[ra]; printf("0x%04X\n",rb);


    in_data_len=out_data_index;
    memcpy(in_data,out_data,out_data_index);
    in_data_index=0;
    out_data_index=0;

    decode();

    printf("%u %u\n",in_data_index,out_data_index);


fp=fopen("test_data.h","wt");
if(fp==NULL)
{
    return(1);
}
    fprintf(fp,"\n");
    fprintf(fp,"#define TEST_DATA_LEN %u\n",out_data_index);
    fprintf(fp,"const unsigned char test_data[TEST_DATA_LEN]=\n");
    fprintf(fp,"{");
    for(ra=0;ra<out_data_index;ra++)
    {
        if((ra&7)==0) fprintf(fp,"\n  ");
        fprintf(fp,"0x%02X,",out_data[ra]);
    }
    fprintf(fp,"\n");
    fprintf(fp,"};\n");
    fprintf(fp,"\n");
rb=0;
for(ra=0;ra<in_data_len;ra++) rb+=in_data[ra];
    fprintf(fp,"#define DEF_DATA_CHECKSUM 0x%04X\n",rb&0xFFFF);
    fprintf(fp,"#define DEF_DATA_LEN      %u\n",in_data_len);
    fclose(fp);

    for(ra=0;ra<TEST_DATA_LEN;ra++) if(out_data[ra]!=test_data[ra]) break;
    if(ra<TEST_DATA_LEN)
    {
        printf("fail\n");
        return(1);
    }
    else
    {
        printf("match\n");
    }

    return 0;
}
