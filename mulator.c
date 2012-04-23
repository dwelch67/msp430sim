
////Copyright (C) 2002, David Welch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHOWREGS  0
#define SHOWFETCH 0
#define SHOWREAD  0
#define SHOWWRITE 0

#define SHOWTOPREGSWRITE 0

#define SHOWDISS  0

#define ROMSTART 0x8000

#define CBIT 0x01
#define ZBIT 0x02
#define NBIT 0x04
#define VBIT 0x10


unsigned long instructions;
unsigned long fetches;
unsigned long reads;
unsigned long writes;

#define ROMADDMASK 0xFFFF
#define RAMADDMASK 0xFFFF

#define ROMSIZE (ROMADDMASK+1)
#define RAMSIZE (RAMADDMASK+1)

unsigned char rom[ROMSIZE];
unsigned char ram[RAMSIZE];


unsigned int reg[16];

unsigned int pcop;
unsigned int pc;
unsigned int inst;
unsigned int sx;
unsigned int dx;

unsigned int op;
unsigned int ad;
unsigned int as;
unsigned int bw;
unsigned int src;
unsigned int dst;

unsigned int srcval;
unsigned int dstval;




FILE *fp;

#define EI_MAG0      0
#define EI_MAG1      1
#define EI_MAG2      2
#define EI_MAG3      3
#define EI_CLASS     4
#define EI_DATA      5
#define EI_VERSION   6
#define EI_PAD       7
#define EI_NIDENT    16

#define EV_NONE     0
#define EV_CURRENT  1

#define ELFMAG0 0x7F
#define ELFMAG1 0x45
#define ELFMAG2 0x4C
#define ELFMAG3 0x46

#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define SHF_WRITE       0x1
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4

#define SHN_UNDEF       0x0000
#define SHN_LORESERVE   0xFF00

struct
{
    unsigned char e_ident[EI_NIDENT];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned int e_shoff;
    unsigned int e_flags;
    unsigned short e_ehsize;
    unsigned short e_phentsize;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
    unsigned short e_shstrndx;
} elfheader;

struct
{
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned int sh_flags;
    unsigned int sh_addr;
    unsigned int sh_offset;
    unsigned int sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned int sh_addralign;
    unsigned int sh_entsize;
} sectionheader;
unsigned int shnamebase;
unsigned int shnamesize;
unsigned char shname[256];

struct
{
    unsigned int st_name;
    unsigned int st_value;
    unsigned int st_size;
    unsigned char st_info;
    unsigned char st_other;
    unsigned short st_shndx;
} symbolheader;

struct
{
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_paddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
} progheader;
unsigned int prog;








int readelf ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;
    unsigned int nsh;
    unsigned int hi,lo;

    if(52!=sizeof(elfheader))
    {
        printf("Compiler error [elfheader]\n");
        return(1);
    }
    if(40!=sizeof(sectionheader))
    {
        printf("Compiler error [sectionheader]\n");
        return(1);
    }
    if(16!=sizeof(symbolheader))
    {
        printf("Compiler error [sectionheader]\n");
        return(1);
    }

    if(fread(&elfheader,1,sizeof(elfheader),fp)!=sizeof(elfheader))
    {
        printf("Error reading ELF header\n");
        return(1);
    }

    printf("e_ident[EI_MAG0   ] 0x%02X\n",elfheader.e_ident[EI_MAG0   ]);
    printf("e_ident[EI_MAG1   ] 0x%02X\n",elfheader.e_ident[EI_MAG1   ]);
    printf("e_ident[EI_MAG2   ] 0x%02X\n",elfheader.e_ident[EI_MAG2   ]);
    printf("e_ident[EI_MAG3   ] 0x%02X\n",elfheader.e_ident[EI_MAG3   ]);
    printf("e_ident[EI_CLASS  ] 0x%02X\n",elfheader.e_ident[EI_CLASS  ]);
    printf("e_ident[EI_DATA   ] 0x%02X\n",elfheader.e_ident[EI_DATA   ]);
    printf("e_ident[EI_VERSION] 0x%02X\n",elfheader.e_ident[EI_VERSION]);

    printf("e_type       0x%08X %u\n",elfheader.e_type     ,elfheader.e_type      );
    printf("e_machine    0x%08X %u\n",elfheader.e_machine  ,elfheader.e_machine   );
    printf("e_version    0x%08X %u\n",elfheader.e_version  ,elfheader.e_version   );
    printf("e_entry      0x%08X %u\n",elfheader.e_entry    ,elfheader.e_entry     );
    printf("e_phoff      0x%08X %u\n",elfheader.e_phoff    ,elfheader.e_phoff     );
    printf("e_shoff      0x%08X %u\n",elfheader.e_shoff    ,elfheader.e_shoff     );
    printf("e_flags      0x%08X %u\n",elfheader.e_flags    ,elfheader.e_flags     );
    printf("e_ehsize     0x%08X %u\n",elfheader.e_ehsize   ,elfheader.e_ehsize    );
    printf("e_phentsize  0x%08X %u\n",elfheader.e_phentsize,elfheader.e_phentsize );
    printf("e_phnum      0x%08X %u\n",elfheader.e_phnum    ,elfheader.e_phnum     );
    printf("e_shentsize  0x%08X %u\n",elfheader.e_shentsize,elfheader.e_shentsize );
    printf("e_shnum      0x%08X %u\n",elfheader.e_shnum    ,elfheader.e_shnum     );
    printf("e_shstrndx   0x%08X %u\n",elfheader.e_shstrndx ,elfheader.e_shstrndx  );

    ra=0;
    if(elfheader.e_version!=EV_CURRENT) ra++;
    if(elfheader.e_ident[EI_MAG0]!=ELFMAG0) ra++;
    if(elfheader.e_ident[EI_MAG1]!=ELFMAG1) ra++;
    if(elfheader.e_ident[EI_MAG2]!=ELFMAG2) ra++;
    if(elfheader.e_ident[EI_MAG3]!=ELFMAG3) ra++;
    //if(elfheader.e_ident[EI_CLASS]==ELFCLASSNONE) ra++;
    switch(elfheader.e_ident[EI_CLASS])
    {
        case ELFCLASSNONE: ra++; break;
        case ELFCLASS32: printf("ELFCLASS32\n"); break;
        case ELFCLASS64: printf("ELFCLASS64\n"); break;
    }
    //if(elfheader.e_ident[EI_DATA]==ELFDATANONE) ra++;
    switch(elfheader.e_ident[EI_DATA])
    {
        case ELFDATANONE: ra++; break;
        case ELFDATA2LSB: printf("ELFDATA2LSB\n"); break;
        case ELFDATA2MSB: printf("ELFDATA2MSB\n"); break;
    }
    if(elfheader.e_ident[EI_VERSION]!=EV_CURRENT) ra++;
    if(ra)
    {
        printf("Not an ELF format file\n");
        return(1);
    }



    if(elfheader.e_phentsize!=sizeof(progheader))
    {
        printf("e_phentsize is not %u\n",(unsigned int)sizeof(progheader));
        //return(1);
    }
    else
    {
        if
        (   (elfheader.e_phoff!=0) &&
            (elfheader.e_phnum!=0) &&
            (elfheader.e_phentsize!=0)
        )
        {

            for(prog=0;prog<elfheader.e_phnum;prog++)
            {
                fseek(fp,elfheader.e_phoff+(prog*sizeof(progheader)),0);
                fread(&progheader,1,sizeof(progheader),fp);
                printf("\n----- program header 0x%03X %u %lu -----\n",prog,prog,ftell(fp));

                printf("p_type   0x%08X (%u)\n",progheader.p_type  ,progheader.p_type  );
                printf("p_offset 0x%08X (%u)\n",progheader.p_offset,progheader.p_offset);
                printf("p_vaddr  0x%08X (%u)\n",progheader.p_vaddr ,progheader.p_vaddr );
                printf("p_paddr  0x%08X (%u)\n",progheader.p_paddr ,progheader.p_paddr );
                printf("p_filesz 0x%08X (%u)\n",progheader.p_filesz,progheader.p_filesz);
                printf("p_memsz  0x%08X (%u)\n",progheader.p_memsz ,progheader.p_memsz );
                printf("p_flags  0x%08X (%u)\n",progheader.p_flags ,progheader.p_flags );
                printf("p_align  0x%08X (%u)\n",progheader.p_align ,progheader.p_align );
            }
        }
    }

    printf("\n-----\n");

    if(elfheader.e_ehsize!=sizeof(elfheader))
    {
        printf("e_ehsize is not %u\n",(unsigned int)sizeof(elfheader));
        return(1);
    }

    if
    (   (elfheader.e_shoff!=0) &&
        (elfheader.e_shnum!=0) &&
        (elfheader.e_shentsize!=0)
    )
    {
        shnamebase=0;
        if(elfheader.e_shstrndx>SHN_UNDEF)
        if(elfheader.e_shstrndx<SHN_LORESERVE)
        if(elfheader.e_shstrndx<elfheader.e_shnum)
        {
            fseek(fp,elfheader.e_shoff+(elfheader.e_shstrndx*sizeof(sectionheader)),0);
            if(fread(&sectionheader,1,sizeof(sectionheader),fp)!=sizeof(sectionheader))
            {
                printf("Error reading sectionheader\n");
                return(1);
            }
            //printf("sh_offset     0x%08X %u\n",sectionheader.sh_offset    ,sectionheader.sh_offset    );
            //printf("sh_size       0x%08X %u\n",sectionheader.sh_size      ,sectionheader.sh_size      );
            shnamebase=sectionheader.sh_offset;
            shnamesize=sectionheader.sh_size;
            printf("shnamebase 0x%08X %u\n",shnamebase,shnamebase);
            printf("shnamesize 0x%08X %u\n",shnamesize,shnamesize);
        }
        if(elfheader.e_shentsize!=sizeof(sectionheader))
        {
            printf("e_shentsize is not %u\n",(unsigned int)sizeof(sectionheader));
            return(1);
        }
        for(nsh=0;nsh<elfheader.e_shnum;nsh++)
        {
            fseek(fp,elfheader.e_shoff+(nsh*sizeof(sectionheader)),0);
            printf("\n----- section header 0x%03X %u %lu -----\n",nsh,nsh,ftell(fp));
            if(fread(&sectionheader,1,sizeof(sectionheader),fp)!=sizeof(sectionheader))
            {
                printf("Error reading sectionheader\n");
                return(1);
            }

            shname[0]=0;
            if(shnamebase)
            {
                fseek(fp,shnamebase+sectionheader.sh_name,0);
                fread(shname,1,sizeof(shname),fp);
                shname[sizeof(shname)-1]=0;
            }

            printf("sh_name       0x%08X %u [%s]\n",sectionheader.sh_name      ,sectionheader.sh_name,shname);
            printf("sh_type       0x%08X %u\n",sectionheader.sh_type      ,sectionheader.sh_type      );
            printf("sh_flags      0x%08X %u  ",sectionheader.sh_flags     ,sectionheader.sh_flags     );
                if(sectionheader.sh_flags&SHF_WRITE) printf("[SHF_WRITE]");
                if(sectionheader.sh_flags&SHF_ALLOC) printf("[SHF_ALLOC]");
                if(sectionheader.sh_flags&SHF_EXECINSTR) printf("[SHF_EXECINSTR]");
                printf("\n");
            printf("sh_addr       0x%08X %u\n",sectionheader.sh_addr      ,sectionheader.sh_addr      );
            printf("sh_offset     0x%08X %u\n",sectionheader.sh_offset    ,sectionheader.sh_offset    );
            printf("sh_size       0x%08X %u\n",sectionheader.sh_size      ,sectionheader.sh_size      );
            printf("sh_link       0x%08X %u\n",sectionheader.sh_link      ,sectionheader.sh_link      );
            printf("sh_info       0x%08X %u\n",sectionheader.sh_info      ,sectionheader.sh_info      );
            printf("sh_addralign  0x%08X %u\n",sectionheader.sh_addralign ,sectionheader.sh_addralign );
            printf("sh_entsize    0x%08X %u\n",sectionheader.sh_entsize   ,sectionheader.sh_entsize   );

            if(sectionheader.sh_flags&SHF_ALLOC)
            {
                if((sectionheader.sh_flags&SHF_EXECINSTR)||(sectionheader.sh_flags&SHF_WRITE))
                {
                    rb=sectionheader.sh_size;
                    if(sectionheader.sh_addr>sizeof(rom))
                    {
                        rc=0;
                    }
                    else
                    {
                        rc=sizeof(rom)-sectionheader.sh_addr;
                    }
                    if(rb>rc) rb=rc;
                    if(rb)
                    {

                        fseek(fp,sectionheader.sh_offset,0);
                        rc=fread(&rom[sectionheader.sh_addr],1,rb,fp);
                        if(rc!=rb)
                        {
                            printf("Error reading data\n");
                            return(1);
                        }
                        rc=sectionheader.sh_addr;
                        printf("0x%04X 0x%04X %u\n",rc,rb,rb);
                    }
                }
            }
        }
    }
    else
    {
        printf("No section headers defined\n");
    }

    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
unsigned int fetch ( unsigned int addr )
{
    unsigned int ra;

    if(addr&1)
    {
        fprintf(stderr,"Error fetch from odd address 0x%04X\n",addr);
        exit(1);
    }
    if(addr<ROMSTART)
    {
        fprintf(stderr,"Error trying to fetch from ram 0x%04X\n",addr);
        exit(1);
    }
    ra=rom[addr+1];
    ra<<=8;
    ra|=rom[addr+0];
if(SHOWFETCH) printf("fetch [%04X] = %04X\n",addr,ra);
    fetches++;
    return(ra);
}
//-------------------------------------------------------------------
unsigned int read8 ( unsigned int addr )
{
    unsigned int ra;

    if(addr<0x200)
    {
        //put peripherals here
        ra=ram[addr]&0xFF;
    }
    else if(addr<ROMSTART)
    {
        ra=ram[addr]&0xFF;
    }
    else
    {
        ra=rom[addr]&0xFF;
    }
if(SHOWREAD) printf("read8 [%04X] = %02X\n",addr,ra);
    return(ra);
}
//-------------------------------------------------------------------
unsigned int read16 ( unsigned int addr )
{
    unsigned int ra;

    if(addr&1)
    {
        fprintf(stderr,"Error read from an odd address 0x%04X\n",addr);
        exit(1);
    }
        ra=read8(addr+0);
        ra|=read8(addr+1)<<8;
    return(ra);
}
//-------------------------------------------------------------------
void write8 ( unsigned int addr, unsigned int data )
{
    if(addr<0x200)
    {
        //put peripherals here
        ram[addr]=data&0xFF;

        if(addr==0x77)
        {
            //UTXBUF
            printf("uart out: 0x%02X\n",data&0xFF);
        }


    }
    else if(addr<ROMSTART)
    {
        ram[addr]=data&0xFF;
    }
    else
    {
        fprintf(stderr,"Error cannot write to rom 0x%04X pc 0x%X\n",addr,pc);
        exit(1);
    }
if(SHOWWRITE) printf("write8 [%04X] = %02X\n",addr,data);
}
//-------------------------------------------------------------------
void write16 ( unsigned int addr, unsigned int data )
{
    if(addr&1)
    {
        fprintf(stderr,"Error write to an odd address 0x%04X\n",addr);
        exit(1);
    }

    switch(addr)
    {
        default:
        {
            write8(addr+0,(data>>0)&0xFF);
            write8(addr+1,(data>>8)&0xFF);
            break;
        }
        case 0x58:
        {
            printf("hexstring: 0x%04X\n",data);
            break;
        }
    }
}
//-------------------------------------------------------------------
unsigned int read_register ( unsigned int r )
{
    unsigned int ra;

    ra=reg[r&0xF]&0xFFFF;
if(SHOWREGS) printf("read_register(%u) = 0x%04X\n",r,ra);
    return(ra);
}
//-------------------------------------------------------------------
void write_register ( unsigned int r, unsigned int v )
{
    v&=0xFFFF;
if(SHOWREGS) printf("write_register(%u,0x%04X);\n",r,v);
if(SHOWTOPREGSWRITE)
{
    if(r>11) printf("write_register(%u,0x%04X);\n",r,v);
}
    reg[r&0xF]=v;
}
//-------------------------------------------------------------------
int do_sx ( void )
{
    switch(as)
    {
        case 0:
        case 2:
        {
            break;
        }
        case 1:
        {
            if(src!=0x3)
            {
                sx=fetch(pc); pc+=2; write_register(0,pc);
            }
            break;
        }
        case 3:
        {
            if(src==0x0)
            {
                sx=fetch(pc); pc+=2; write_register(0,pc);
            }
            break;
        }
    }
    return(0);
}
//-------------------------------------------------------------------
int do_dx ( void )
{
    if(ad)
    {
        if(dst!=0x3)
        {
            dx=fetch(pc); pc+=2; write_register(0,pc);
        }
    }
    return(0);
}//-----------------------------------------------------------------
int get_source ( void )
{
    //get source
    switch(as)
    {
        case 0:
        {
            switch(src)
            {
                case 0x0: srcval=pc; break;
                case 0x3: srcval=0x0000; break;
                default: srcval=read_register(src); break;
            }
            break;
        }
        case 1:
        {
            switch(src)
            {
                case 0x0:
                {
//printf("sx %X pc %X\n",sx,pc);
                    srcval=sx+pc-2;
                    //if(bw) srcval=read8(sx+pc);
                    //else   srcval=read16(sx+pc);
//printf("check label 0x%04X\n",srcval);
                    return(0);
                }
                case 0x2:
                {
                    if(bw) srcval=read8(sx);
                    else   srcval=read16(sx);
                    break;
                }
                case 0x3: srcval=0x0001; break;
                default:
                {
                    srcval=read_register(src);
                    if(bw) srcval=read8(srcval+sx);
                    else   srcval=read16(srcval+sx);
                    break;
                }
            }
            break;
        }
        case 2:
        {
            switch(src)
            {
                case 0x0:
                {
                    if(bw) srcval=read8(pc);
                    else   srcval=read16(pc);
                    break;
                }
                case 0x2: srcval=0x0004; break;
                case 0x3: srcval=0x0002; break;
                default:
                {
                    srcval=read_register(src);
                    if(bw) srcval=read8(srcval);
                    else   srcval=read16(srcval);
                    break;
                }
            }
            break;
        }
        case 3:
        {
            switch(src)
            {
                case 0x0:
                {
                    srcval=sx;
                    break;
                }
                case 0x2: srcval=0x0008; break;
                case 0x3: srcval=0xFFFF; break;
                default:
                {
                    srcval=read_register(src);
                    if(bw)
                    {
                        write_register(src,srcval+1);
                        srcval=read8(srcval);
                    }
                    else
                    {
                        write_register(src,srcval+2);
                        srcval=read16(srcval);
                    }
                    break;
                }
            }
            break;
        }
    }
    return(0);
}
//-----------------------------------------------------------------
int get_destination ( void )
{
    unsigned int ra;

    if(ad==0)
    {
        switch(dst)
        {
            case 0x0: dstval=pc; break;
            case 0x3: dstval=0x0000; break;
            default: dstval=read_register(dst); break;
        }
    }
    else
    {
        switch(dst)
        {
            case 0x0:
            {
                if(bw) dstval=read8(dx+pc);
                else   dstval=read16(dx+pc);
                printf("check label 0x%04X\n",dstval);
                return(1);
            }
            case 0x2:
            {
                if(bw) dstval=read8(dx);
                else   dstval=read16(dx);
                break;
            }
            case 0x3: dstval=0x0001; break;
            default:
            {
                ra=read_register(dst);
                if(bw) dstval=read8(ra+dx);
                else   dstval=read16(ra+dx);
                break;
            }
        }
    }
    return(0);
}
//-------------------------------------------------------------------
int store_source ( void )
{
    unsigned int ra;

    switch(as)
    {
        case 0:
        {
            switch(src)
            {
                case 0x0:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x3:
                {
                    printf("todo\n");
                    return(1);
                }
                default:
                {
                    write_register(src,dstval);
                    break;
                }
            }
            break;
        }
        case 1:
        {
            switch(src)
            {
                case 0x0:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x2:
                {
                    if(bw) write8(sx,dstval);
                    else   write16(sx,dstval);
                    break;
                }
                case 0x3:
                {
                    printf("todo\n");
                    return(1);
                }
                default:
                {
                    ra=read_register(src);
                    if(bw) write8(ra+sx,dstval);
                    else   write16(ra+sx,dstval);
                    break;
                }
            }
            break;
        }
        case 2:
        {
            switch(src)
            {
                case 0x0:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x2:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x3:
                {
                    printf("todo\n");
                    return(1);
                }
                default:
                {
                    read_register(src);
                    if(bw) write8(ra,dstval);
                    else   write16(ra,dstval);
                    break;
                }
            }
            break;
        }
        case 3:
        {
            switch(src)
            {
                case 0x0:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x2:
                {
                    printf("todo\n");
                    return(1);
                }
                case 0x3:
                {
                    printf("todo\n");
                    return(1);
                }
                default:
                {
                    read_register(src);
                    if(bw) { write8(ra,dstval); ra++; }
                    else   { write16(ra,dstval); ra+=2; }
                    write_register(src,ra);
                    break;
                }
            }
            break;
        }
    }
    return(0);
}
//-------------------------------------------------------------------
int store_destination ( void )
{
    unsigned int ra;

    if(ad==0)
    {
        switch(dst)
        {
            case 0x0:
            {
                //printf("todo pc 0x%04X\n",dstval);
                write_register(dst,dstval);
                return(0);
            }
            case 0x3:
            {
                printf("todo\n");
                return(1);
            }
            default:
            {
                write_register(dst,dstval);
                break;
            }
        }
    }
    else
    {
        switch(dst)
        {
            case 0x0:
            {
                printf("todo\n");
                return(1);
            }
            case 0x2:
            {
                if(bw) write8(dx,dstval);
                else   write16(dx,dstval);
                break;
            }
            case 0x3:
            {
                printf("todo\n");
                return(1);
            }
            default:
            {
                ra=read_register(dst);
                if(bw) write8(ra+dx,dstval);
                else   write16(ra+dx,dstval);
                break;
            }
        }
    }
    return(0);
}
//-------------------------------------------------------------------
void do_zflag ( unsigned int x )
{
    unsigned int ra;

    if(bw) x&=0xFF; else x&=0xFFFF;

    ra=read_register(2);
    if(x==0) ra|=ZBIT; else ra&=(~ZBIT);
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_nflag ( unsigned int x )
{
    unsigned int ra;

    if(bw) x&=0x80; else x&=0x8000;

    ra=read_register(2);
    if(x) ra|=NBIT; else ra&=(~NBIT);
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_cflag ( unsigned int a, unsigned int b, unsigned int c )
{
    unsigned int ra;
    unsigned int rc;

    if(bw)
    {
        rc=(a&0xFF)+(b&0xFF)+c;
        rc>>=8;
    }
    else
    {
        rc=(a&0xFFFF)+(b&0xFFFF)+c;
        rc>>=16;
    }
    ra=read_register(2);
    if(rc&1) ra|=CBIT; else ra&=~CBIT;
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_cbit ( unsigned int x )
{
    unsigned int ra;

    ra=read_register(2);
    if(x) ra|=CBIT; else ra&=~CBIT;
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_cnotzero ( void )
{
    unsigned int ra;

    ra=read_register(2);
    if(ra&ZBIT) ra&=~CBIT; else ra|=CBIT;
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_vflag ( unsigned int a, unsigned int b, unsigned int c )
{
    unsigned int ra;
    unsigned int rc;
    unsigned int rd;

    if(bw)
    {
        rc=(a&0x7F)+(b&0x7F)+c; //carry in
        rc>>=7; //carry in in lsbit
        rd=(rc&1)+((a>>7)&1)+((b>>7)&1); //carry out
        rd>>=1; //carry out in lsbit
        rc=(rc^rd)&1;
    }
    else
    {
        rc=(a&0x7FFF)+(b&0x7FFF)+c; //carry in
        rc>>=15; //carry in in lsbit
        rd=(rc&1)+((a>>15)&1)+((b>>15)&1);
        rd>>=1; //carry out in lsbit
        rc=(rc^rd)&1;
    }
    ra=read_register(2);
    if(rc) ra|=VBIT; else ra&=~VBIT;
    write_register(2,ra);
}
//-------------------------------------------------------------------
void do_vbit ( unsigned int x )
{
    unsigned int ra;

    ra=read_register(2);
    if(x) ra|=VBIT; else ra&=~VBIT;
    write_register(2,ra);
}
//-------------------------------------------------------------------
int execute ( void )
{
    unsigned int ra;
    unsigned int rc;
    unsigned int res;
    unsigned int cond;
    unsigned int joff;

    instructions++;
    pc=read_register(0);
    if(pc&1)
    {
        fprintf(stderr,"invalid program counter 0x%04X\n",pc);
        return(1);
    }
    pcop=pc;
    inst=fetch(pc);
    pc+=2; write_register(0,pc);

    if(inst&0xC000)
    {
        //two operand arithmetic
        op =(inst>>12)&0xF;
        src=(inst>>8)&0xF;
        ad =(inst>>7)&1;
        bw =(inst>>6)&1;
        as =(inst>>4)&3;
        dst=(inst>>0)&0xF;

        if(do_sx()) return(1);
        if(do_dx()) return(1);
        if(get_source()) return(1);
        if(op==0x4) //mov ----
        {
if(SHOWDISS) printf("[%04X] mov\n",pcop);
            dstval=srcval;
            if(bw) dstval&=0xFF; else dstval&=0xFFFF;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0x5) //add vnzc
        {
if(SHOWDISS) printf("[%04X] add\n",pcop);
            if(get_destination()) return(1);
            res=dstval+srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cflag(dstval,srcval,0);
            do_vflag(dstval,srcval,0);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0x6) //addc vncz
        {
if(SHOWDISS) printf("[%04X] addc\n",pcop);
            if(get_destination()) return(1);
            rc=read_register(2);
            if(rc&CBIT) rc=1; else rc=0;
            res=dstval+srcval+rc;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cflag(dstval,srcval,rc);
            do_vflag(dstval,srcval,rc);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0x7) //subc vnzc
        {
if(SHOWDISS) printf("[%04X] subc\n",pcop);
            if(get_destination()) return(1);
            rc=read_register(2);
            if(rc&CBIT)
            {
                rc=0;
                res=dstval-srcval-1;
            }
            else
            {
                rc=1;
                res=dstval-srcval-0;
            }
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cflag(dstval,~srcval,rc);
            do_vflag(dstval,~srcval,rc);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0x8) //sub vnzc
        {
if(SHOWDISS) printf("[%04X] sub\n",pcop);
            if(get_destination()) return(1);
            res=dstval-srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cflag(dstval,~srcval,1);
            do_vflag(dstval,~srcval,1);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0x9) //cmp vnzc
        {
if(SHOWDISS) printf("[%04X] cmp\n",pcop);
            if(get_destination()) return(1);
            res=dstval-srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cflag(dstval,~srcval,1);
            do_vflag(dstval,~srcval,1);
            return(0);
        }
        if(op==0xA) //dadd vnzc
        {
if(SHOWDISS) printf("[%04X] dadd\n",pcop);
        }
        if(op==0xB) //bit 0nzc
        {
if(SHOWDISS) printf("[%04X] bit\n",pcop);
            if(get_destination()) return(1);
            res=dstval&srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cnotzero();
            do_vbit(0);
            return(0);
        }
        if(op==0xC) //bic ----
        {
if(SHOWDISS) printf("[%04X] bic\n",pcop);
            if(get_destination()) return(1);
            res=dstval&(~srcval);
            if(bw) res&=0xFF; else res&=0xFFFF;
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0xD) //bis ----
        {
if(SHOWDISS) printf("[%04X] bis\n",pcop);
            if(get_destination()) return(1);
            res=dstval|srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0xE) //xor vnzc
        {
if(SHOWDISS) printf("[%04X] xor\n",pcop);
            if(get_destination()) return(1);
            res=dstval^srcval;
            if(bw)
            {
                res&=0xFF;
                rc&=0x80;
            }
            else
            {
                res&=0xFFFF;
                rc&=0x8000;
            }
            do_zflag(res);
            do_nflag(res);
            do_cnotzero();
            do_vbit(rc);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
        if(op==0xF) //and 0nzc
        {
if(SHOWDISS) printf("[%04X] and\n",pcop);
            if(get_destination()) return(1);
            res=dstval&srcval;
            if(bw) res&=0xFF; else res&=0xFFFF;
            do_zflag(res);
            do_nflag(res);
            do_cnotzero();
            do_vbit(0);
            dstval=res;
            if(store_destination()) return(1);
            return(0);
        }
    }
    else
    {
        if(inst&0xE000)
        {
            //conditional jump
            cond=(inst>>10)&7;
            switch(cond)
            {
                case 0: //jne/jnz
                {
if(SHOWDISS) printf("[%04X] jne\n",pcop);
                    ra=read_register(2);
                    if((ra&ZBIT)==0)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jne true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 1: //jeq/jz
                {
if(SHOWDISS) printf("[%04X] jeq\n",pcop);
                    ra=read_register(2);
                    if(ra&ZBIT)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jeq true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 2: //jnc/jlo
                {
if(SHOWDISS) printf("[%04X] jnc\n",pcop);
                    ra=read_register(2);
                    if((ra&CBIT)==0)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jnc true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 3: //jc/jhs
                {
if(SHOWDISS) printf("[%04X] jc\n",pcop);
                    ra=read_register(2);
                    if(ra&CBIT)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jc true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 4: //jn
                {
if(SHOWDISS) printf("[%04X] jn\n",pcop);
                    ra=read_register(2);
                    if(ra&NBIT)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jn true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 5: //jge
                {
if(SHOWDISS) printf("[%04X] jge\n",pcop);
                    ra=read_register(2);
                    if(ra&NBIT) rc=1; else rc=0;
                    if(ra&VBIT) ra=1; else ra=0;
                    ra=ra^rc;
                    if(ra==0)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jge true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 6: //jl
                {
if(SHOWDISS) printf("[%04X] jl\n",pcop);
                    ra=read_register(2);
                    if(ra&NBIT) rc=1; else rc=0;
                    if(ra&VBIT) ra=1; else ra=0;
                    ra=ra^rc;
                    if(ra==1)
                    {
                        joff=inst&0x3FF;
                        joff<<=1;
                        if(joff&0x400) joff|=0xF800;
                        joff=(joff+pc)&0xFFFF;
                        write_register(0,joff);
if(SHOWDISS) printf("[%04X] jl true 0x%04X\n",pcop,joff);
                    }
                    return(0);
                }
                case 7: //unconditional
                {
if(SHOWDISS) printf("[%04X] jmp\n",pcop);
                    joff=inst&0x3FF;
                    joff<<=1;
                    if(joff&0x400) joff|=0xF800;
                    joff=(joff+pc)&0xFFFF;
                    write_register(0,joff);
                    return(0);
                }
            }
        }
        else
        {
            if((inst&0xFC00)==0x1000)
            {
                //one operand arithmetic
                op =(inst>>7)&0x7;
                src=(inst>>0)&0xF;
                bw =(inst>>6)&1;
                as =(inst>>4)&3;
                if(do_sx()) return(1);

                if(op==0x0) //RRC
                {
if(SHOWDISS) printf("[%04X] rrc\n",pcop);
                    if(get_source()) return(1);
                    ra=read_register(2);
                    dstval=srcval>>1;
                    if(bw)
                    {
                        dstval&=0x7F;
                        if(ra&CBIT) dstval|=0x80;
                    }
                    else
                    {
                        dstval&=0x7FFF;
                        if(ra&CBIT) dstval|=0x8000;
                    }
                    do_zflag(dstval);
                    do_nflag(dstval);
                    do_cbit(srcval&1);
                    do_vbit(0);
                    if(store_source()) return(1);
                    return(0);
                }
                if(op==0x1) //SWPB ----
                {
                    if(bw==0)
                    {
if(SHOWDISS) printf("[%04X] swpb\n",pcop);
                        if(get_source()) return(1);
                        dstval=(srcval<<8)|(srcval>>8);
                        dstval&=0xFFFF;
                        //no flags
                        if(store_source()) return(1);
                        return(0);
                    }
                }
                if(op==0x2) //RRA
                {
if(SHOWDISS) printf("[%04X] rra\n",pcop);
                    if(get_source()) return(1);
                    ra=read_register(2);
                    dstval=srcval>>1;
                    if(bw)
                    {
                        dstval&=0x7F;
                        dstval|=srcval&0x80;
                    }
                    else
                    {
                        dstval&=0x7FFF;
                        dstval|=srcval&0x8000;
                    }
                    do_zflag(dstval);
                    do_nflag(dstval);
                    do_cbit(srcval&1);
                    do_vbit(0);
                    if(store_source()) return(1);
                    return(0);
                }
                if(op==0x3) //SXT 0nzc
                {
                    if(bw==0)
                    {
if(SHOWDISS) printf("[%04X] sxt\n",pcop);
                        if(get_source()) return(1);
                        if(srcval&0x80) dstval=srcval|0xFF00;
                        else            dstval=srcval;
                        dstval&=0xFFFF;
                        do_zflag(dstval);
                        do_nflag(dstval);
                        do_cnotzero();
                        do_vbit(0);
                        if(store_source()) return(1);
                        return(0);
                    }
                }
                if(op==0x4) //push
                {
if(SHOWDISS) printf("[%04X] push\n",pcop);
                    if(get_source()) return(1);
                    ra=read_register(1);
                    ra-=2;
                    if(bw) srcval&=0x00FF;
                    write16(ra,srcval);
                    write_register(1,ra);
                    return(0);
                }
                if(op==0x5) //call
                {
if(SHOWDISS) printf("[%04X] call\n",pcop);
                    if(bw==0)
                    {
                        if(get_source()) return(1);
                        //push pc
                        ra=read_register(1);
                        ra-=2;
                        write16(ra,pc);
                        write_register(1,ra);
                        //move srcval to pc
                        pc=srcval;
                        write_register(0,pc);
                        return(0);
                    }
                }
                if(inst==0x1300) //reti
                {
if(SHOWDISS) printf("[%04X] reti\n",pcop);
                    ra=read_register(1);
                    srcval=read16(ra); ra+=2;
                    write_register(2,srcval);
                    pc=read16(ra); ra+=2;
                    write_register(0,pc);
                    write_register(1,ra);
                }
            }
            else
            {
            }
        }
    }


    printf("Unknown instruction [%04X] 0x%04X\n",pcop,inst);

    return(1);
}

//-------------------------------------------------------------------
void dump_counters ( void )
{
    printf("\n\n");
    printf("instructions %lu\n",instructions);
    printf("fetches      %lu\n",fetches);
    printf("reads        %lu\n",reads);
    printf("writes       %lu\n",writes);
    printf("memcycles    %lu\n",fetches+reads+writes);
}
//-------------------------------------------------------------------
int reset ( void )
{

    printf("\n----- reset -----\n\n");

    memset(ram,0xFF,sizeof(ram));
    memset(reg,0x00,sizeof(reg));
    reg[0]=fetch(0xFFFE);
    reg[1]=ROMSTART;

    instructions=0;
    fetches=0;
    reads=0;
    writes=0;

    return(0);
}
//-------------------------------------------------------------------
int run ( void )
{
    reset();
    while(1)
    {
        if(execute()) break;
    }
    dump_counters();
    return(0);
}
//-------------------------------------------------------------------
int main ( int argc, char *argv[] )
{
    int ret;
    unsigned int ra,rb,rc;

    if(argc<2)
    {
        printf(".ELF file not specified\n");
        return(1);
    }

    memset(rom,0xFF,sizeof(rom));

    fp=fopen(argv[1],"rb");
    if(fp==NULL)
    {
        printf("Error opening file %s\n",argv[1]);
        return(1);
    }
    ret=readelf();
    fclose(fp);
    if(ret) return(1);

    run();

//0000fc00 <reset>:
    //fc00: 3f 40 d2 04     mov #1234,  r15 ;#0x04d2

//0000fc04 <inner>:
    //fc04: 1f 83           dec r15
    //fc06: fe 23           jnz $-2         ;abs 0xfc04
    //fc08: fb 3f           jmp $-8         ;abs 0xfc00

//0000fc0a <hang>:
    //fc0a: ff 3f           jmp $+0


    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
