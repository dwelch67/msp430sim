
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STATE_ZERO              0
#define STATE_RESET             1
#define STATE_FETCH_RESET       2
#define STATE_OPCODE_FETCH      3
#define STATE_OPCODE_DECODE     4
#define STATE_SOURCE_ADD        5
#define STATE_SOURCE_DATA       6
#define STATE_DEST_ADD          7
#define STATE_DEST_DATA         8



#define STATE_HALT              111

FILE *vcdout;

#define MAXVCD 1000
unsigned int vcdlast[MAXVCD];
unsigned int vcdnew[MAXVCD];
unsigned int vcdmask[MAXVCD];
unsigned int vcdbits[MAXVCD];
char vcdname[MAXVCD][64];
unsigned int nvcds;
unsigned int vcdclock;

unsigned int in_clock;
unsigned int in_reset;
unsigned int xstate;
unsigned int xstate_next;

unsigned int opcode ;
unsigned int op_bw  ;
unsigned int op_src ;
unsigned int op_as  ;
unsigned int op_dst ;
unsigned int op_ad  ;
unsigned int src_add;
unsigned int src_val;
unsigned int dst_add;
unsigned int dst_val;

unsigned int mem_bw;
unsigned int mem_rd;
unsigned int mem_wr;
unsigned int mem_add;
unsigned int mem_fetch;
unsigned int mem_in;
unsigned int mem_out;

unsigned short ram[0x80>>1]; //0x200 to 0x280
unsigned short rom[0x1000>>1]; //0xF000 to 0xFFFF

unsigned short reg[16];


char xstring[80];



unsigned char port1dir;
unsigned char port1out;

void register_vcd ( char *name, unsigned int bits, unsigned int mask )
{
    if(bits<1)
    {
        printf("register_vcd error\n");
        exit(1);
    }
    strcpy(vcdname[nvcds],name);
    vcdbits[nvcds]=bits;
    vcdmask[nvcds]=mask;
    nvcds++;
    if(nvcds>=MAXVCD)
    {
        printf("too many vcd items\n");
        exit(1);
    }
}

void vcd_update ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int first;

    vcdclock++;
    first=1;
    for(ra=0;ra<nvcds;ra++)
    {
        vcdlast[ra]&=vcdmask[ra];
        vcdnew[ra]&=vcdmask[ra];
        if(vcdlast[ra]==vcdnew[ra]) continue;

        vcdlast[ra]=vcdnew[ra];
        if(first)
        {
            first=0;
            fprintf(vcdout,"#%u\n",vcdclock);
        }
        if(vcdbits[ra]==1)
        {
            fprintf(vcdout,"%uv%03u\n",vcdnew[ra]&1,ra);
        }
        else
        {
            fprintf(vcdout,"b");
            rb=1<<vcdbits[ra];
            for(rb>>=1;rb;rb>>=1) if(rb&vcdnew[ra]) fprintf(vcdout,"1"); else fprintf(vcdout,"0");
            fprintf(vcdout," v%03u\n",ra);
        }
    }
    fflush(vcdout);
}








int main ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int temp;
    //read elf or hex file:
    memset(rom,0xFF,sizeof(rom));
    rom[(0xffe0 - 0xF000)>>1]=0xfc26;
    rom[(0xffe2 - 0xF000)>>1]=0xfc26;
    rom[(0xffe4 - 0xF000)>>1]=0xfc26;
    rom[(0xffe6 - 0xF000)>>1]=0xfc26;
    rom[(0xffe8 - 0xF000)>>1]=0xfc26;
    rom[(0xffea - 0xF000)>>1]=0xfc26;
    rom[(0xffec - 0xF000)>>1]=0xfc26;
    rom[(0xffee - 0xF000)>>1]=0xfc26;
    rom[(0xfff0 - 0xF000)>>1]=0xfc26;
    rom[(0xfff2 - 0xF000)>>1]=0xfc26;
    rom[(0xfff4 - 0xF000)>>1]=0xfc26;
    rom[(0xfff6 - 0xF000)>>1]=0xfc26;
    rom[(0xfff8 - 0xF000)>>1]=0xfc26;
    rom[(0xfffa - 0xF000)>>1]=0xfc26;
    rom[(0xfffc - 0xF000)>>1]=0xfc26;
    rom[(0xfffe - 0xF000)>>1]=0xfc00;
    rom[(0xfc00 - 0xF000)>>1]=0x40b2;
    rom[(0xfc02 - 0xF000)>>1]=0x5a80;
    rom[(0xfc04 - 0xF000)>>1]=0x0120;
    rom[(0xfc06 - 0xF000)>>1]=0xd0f2;
    rom[(0xfc08 - 0xF000)>>1]=0x0041;
    rom[(0xfc0a - 0xF000)>>1]=0x0022;
    rom[(0xfc0c - 0xF000)>>1]=0xd3d2;
    rom[(0xfc0e - 0xF000)>>1]=0x0021;
    rom[(0xfc10 - 0xF000)>>1]=0xc0f2;
    rom[(0xfc12 - 0xF000)>>1]=0x0040;
    rom[(0xfc14 - 0xF000)>>1]=0x0021;
    rom[(0xfc16 - 0xF000)>>1]=0xe0f2;
    rom[(0xfc18 - 0xF000)>>1]=0x0041;
    rom[(0xfc1a - 0xF000)>>1]=0x0021;
    rom[(0xfc1c - 0xF000)>>1]=0x4305;
    rom[(0xfc1e - 0xF000)>>1]=0x4326;
    rom[(0xfc20 - 0xF000)>>1]=0x8315;
    rom[(0xfc22 - 0xF000)>>1]=0x23fe;
    rom[(0xfc24 - 0xF000)>>1]=0x3ff8;
    rom[(0xfc26 - 0xF000)>>1]=0x3fff;

    nvcds=0;
    vcdclock=0;


    register_vcd("in_clock",1,1);
    register_vcd("in_reset",1,1);
    register_vcd("xstate",16,0xFFFF);
    register_vcd("xstate_next",16,0xFFFF);
    register_vcd("opcode",16,0xFFFF);
    register_vcd("op_bw",1,1);
    register_vcd("op_src",4,0xF);
    register_vcd("op_as",2,0x3);
    register_vcd("op_dst",4,0xf);
    register_vcd("op_ad",1,0x1);
    register_vcd("src_add",16,0xFFFF);
    register_vcd("src_val",16,0xFFFF);
    register_vcd("dst_add",16,0xFFFF);
    register_vcd("dst_val",16,0xFFFF);
    register_vcd("mem_bw",1,1);
    register_vcd("mem_rd",1,1);
    register_vcd("mem_wr",1,1);
    register_vcd("mem_add",16,0xFFFF);
    register_vcd("mem_fetch",1,1);
    register_vcd("mem_in",16,0xFFFF);
    register_vcd("mem_out",16,0xFFFF);

    register_vcd("reg_00",16,0xFFFF);
    register_vcd("reg_01",16,0xFFFF);
    register_vcd("reg_02",16,0xFFFF);
    register_vcd("reg_03",16,0xFFFF);
    register_vcd("reg_04",16,0xFFFF);
    register_vcd("reg_05",16,0xFFFF);
    register_vcd("reg_06",16,0xFFFF);
    register_vcd("reg_07",16,0xFFFF);
    register_vcd("reg_08",16,0xFFFF);
    register_vcd("reg_09",16,0xFFFF);
    register_vcd("reg_10",16,0xFFFF);
    register_vcd("reg_11",16,0xFFFF);
    register_vcd("reg_12",16,0xFFFF);
    register_vcd("reg_13",16,0xFFFF);
    register_vcd("reg_14",16,0xFFFF);
    register_vcd("reg_15",16,0xFFFF);
    for(ra=0;ra<(0x80>>1);ra++)
    {
        sprintf(xstring,"mem_2%02X",ra<<1);
        register_vcd(xstring,16,0xFFFF);
    }
    register_vcd("port1dir",8,0xFF);
    register_vcd("port1out",8,0xFF);


    vcdout=fopen("sim.vcd","wt");
    if(vcdout==NULL)
    {
        printf("Error creating vcd file\n");
        return(1);
    }
    fprintf(vcdout,"$timescale 1ns $end\n");
    fprintf(vcdout,"$scope module mspvcdsim $end\n");
    for(ra=0;ra<nvcds;ra++)
    {
        fprintf(vcdout,"$var wire %2u v%03u %s $end\n",vcdbits[ra],ra,vcdname[ra]);
    }
    fprintf(vcdout,"$upscope $end\n");
    fprintf(vcdout,"$enddefinitions $end\n");

    memset(vcdlast,0xFF,sizeof(vcdlast));
    memset(vcdnew,0x00,sizeof(vcdnew));
    vcd_update();

    memset(reg,0,sizeof(reg));
    in_reset=0;
    in_clock=0;
    xstate = STATE_ZERO;
    xstate_next = STATE_ZERO;
    opcode  = 0;
    op_bw   = 0;
    op_src  = 0;
    op_as   = 0;
    op_dst  = 0;
    op_ad   = 0;
    src_add = 0;
    src_val = 0;
    dst_add = 0;
    dst_val = 0;

    while(vcdclock<30)
    {
        in_clock=(in_clock+1)&1;
        if(vcdclock==12) in_reset = 1;
printf("%u %u\n",in_reset,xstate);
        if(in_reset)
        {
            if(in_clock == 1) //rising edge
            {
                //latch stuff, xstate last
                switch(xstate)
                {
                    case STATE_FETCH_RESET:
                    {
                        reg[0]=mem_out;
                        break;
                    }
                    case STATE_OPCODE_FETCH:
                    {
                        reg[0]+=2;
                        opcode=mem_out;
                        break;
                    }
                    case STATE_OPCODE_DECODE:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                xstate = xstate_next;

                mem_bw=0;
                mem_rd=0;
                mem_wr=0;
                mem_add=0;
                mem_fetch=0;
                mem_in=0;
                mem_out=0;
            }
            else
            {
                mem_bw=0;
                mem_rd=0;
                mem_wr=0;
                mem_add=0;
                mem_fetch=0;
                mem_in=0;
                mem_out=0;
                switch(xstate)
                {
                    case STATE_ZERO:
                    {
                        xstate_next = STATE_RESET;
                        break;
                    }
                    case STATE_RESET:
                    {
                        xstate_next = STATE_FETCH_RESET;
                        break;
                    }
                    case STATE_FETCH_RESET:
                    {
                        mem_fetch=1;
                        mem_bw=0;
                        mem_rd=1;
                        mem_add=0xFFFE;
                        xstate_next = STATE_OPCODE_FETCH;
                        break;
                    }
                    case STATE_OPCODE_FETCH:
                    {
                        mem_fetch=1;
                        mem_bw=0;
                        mem_rd=1;
                        mem_add=reg[0];
                        xstate_next = STATE_OPCODE_DECODE;
                        break;
                    }
                    default:
                    {
                        xstate_next = STATE_HALT;
                        break;
                    }
                }


                if(mem_rd)
                {
                    if((mem_add&0xF000)==0xF000)
                    {
                        mem_out=rom[((mem_add&0xFFFE)-0xF000)>>1];
                        if(mem_bw)
                        {
                            if(mem_add&1) mem_out>>=8;
                            mem_out&=0xFF;
                        }
                    }
                    else
                    {
                        if((mem_add&0xFF80)==0x0200)
                        {
                            if(mem_fetch)
                            {
                                printf("Really?  a fetch from 0x%04X\n",mem_add);
                            }
                            mem_out=ram[((mem_add&0xFFFE)-0x200)>>1];
                            if(mem_bw)
                            {
                                if(mem_add&1) mem_out>>=8;
                                mem_out&=0xFF;
                            }
                        }
                        else
                        {
                            switch(mem_add)
                            {
                                case 0x21:
                                {
                                    if(mem_bw==0)
                                    {
                                        printf("warning word access to 0x21\n");
                                    }
                                    port1out&=port1dir;
                                    mem_out=port1out;
                                    break;
                                }
                                case 0x22:
                                {
                                    if(mem_bw==0)
                                    {
                                        printf("warning word access to 0x22\n");
                                    }
                                    mem_out=port1dir;
                                    break;
                                }
                                default:
                                {
                                    printf("reading unknown address 0x%04X\n",mem_add);
                                    break;
                                }
                            }
                        }
                    }
                }

                if(mem_wr)
                {
                    if((mem_add&0xF000)==0xF000)
                    {
                        printf("error cannot write to rom 0x%04X\n",mem_add);
                    }
                    else
                    {
                        if((mem_add&0xFF80)==0x0200)
                        {
                            if(mem_bw)
                            {
                                temp=ram[((mem_add&0xFFFE)-0x200)>>1];
                                if(mem_add&1)
                                {
                                    temp&=0x00FF;
                                    temp|=mem_in<<8;
                                }
                                else
                                {
                                    temp&=0xFF00;
                                    temp|=mem_in&0x00FF;
                                }
                                temp&=0xFFFF;
                                ram[((mem_add&0xFFFE)-0x200)>>1]=temp;
                            }
                            else
                            {
                                ram[((mem_add&0xFFFE)-0x200)>>1]=mem_in;
                            }
                        }
                        else
                        {
                            switch(mem_add)
                            {
                                case 0x21:
                                {
                                    if(mem_bw==0)
                                    {
                                        printf("warning word access to 0x21\n");
                                    }
                                    port1out=port1dir&mem_in;
                                    break;
                                }
                                case 0x22:
                                {
                                    if(mem_bw==0)
                                    {
                                        printf("warning word access to 0x22\n");
                                    }
                                    port1dir=mem_in&0xFF;
                                    break;
                                }
                                default:
                                {
                                    printf("writing unknown address 0x%04X\n",mem_add);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        //-------------------------------------
        vcdnew[ra++]=in_clock;
        vcdnew[ra++]=in_reset;
        vcdnew[ra++]=xstate;
        vcdnew[ra++]=xstate_next;
        vcdnew[ra++]=opcode;
        vcdnew[ra++]=op_bw;
        vcdnew[ra++]=op_src;
        vcdnew[ra++]=op_as;
        vcdnew[ra++]=op_dst;
        vcdnew[ra++]=op_ad;
        vcdnew[ra++]=src_add;
        vcdnew[ra++]=src_val;
        vcdnew[ra++]=dst_add;
        vcdnew[ra++]=dst_val;

        vcdnew[ra++]=mem_bw;
        vcdnew[ra++]=mem_rd;
        vcdnew[ra++]=mem_wr;
        vcdnew[ra++]=mem_add;
        vcdnew[ra++]=mem_fetch;
        vcdnew[ra++]=mem_in;
        vcdnew[ra++]=mem_out;

        vcdnew[ra++]=reg[ 0];
        vcdnew[ra++]=reg[ 1];
        vcdnew[ra++]=reg[ 2];
        vcdnew[ra++]=reg[ 3];
        vcdnew[ra++]=reg[ 4];
        vcdnew[ra++]=reg[ 5];
        vcdnew[ra++]=reg[ 6];
        vcdnew[ra++]=reg[ 7];
        vcdnew[ra++]=reg[ 8];
        vcdnew[ra++]=reg[ 9];
        vcdnew[ra++]=reg[10];
        vcdnew[ra++]=reg[11];
        vcdnew[ra++]=reg[12];
        vcdnew[ra++]=reg[13];
        vcdnew[ra++]=reg[14];
        vcdnew[ra++]=reg[15];
        for(rb=0;rb<(0x80>>1);rb++)
        {
            vcdnew[ra++]=ram[rb];
        }
        vcdnew[ra++]=port1dir;
        vcdnew[ra++]=port1out;
        vcd_update();
    }
    fclose(vcdout);
    return(0);
}

