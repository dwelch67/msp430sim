
////Copyright (C) 2002, David Welch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fpout;

#define ROMADDMASK 0xFFFF

#define ROMSIZE (ROMADDMASK+1)

unsigned char rom[ROMSIZE];
unsigned char hitlist[ROMSIZE];
unsigned char lablist[ROMSIZE];

unsigned int hits;

unsigned int op;
unsigned int ad;
unsigned int as;
unsigned int bw;
unsigned int src;
unsigned int dst;

unsigned int dx;
unsigned int sx;
unsigned int cond;
unsigned int joff;
unsigned int pc;

unsigned int inst;

unsigned int pcop;

unsigned int opwords;
char opstring[32];
char sstring[32];
char dstring[32];


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
                        //for(ra=0;ra<rb;ra++) printf("0x%04X : 0x%02X\n",rc+ra,rom[rc+ra]);
                    }
                }
            }
            if((sectionheader.sh_flags&SHF_ALLOC)&&(sectionheader.sh_flags&SHF_WRITE))
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
                    //for(ra=0;ra<rb;ra++) printf("0x%04X : 0x%02X\n",rc+ra,rom[rc+ra]);
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
unsigned int fetch ( unsigned int addr, unsigned char type )
{
    unsigned int ra;

//addr&=ROMADDMASK;

if(addr>ROMADDMASK)
{
    printf("fetch(0x%X) too big\n",addr);
    exit(1);
}



    if(hitlist[addr]!=type)
    {
        if(hitlist[addr])
        {
            fprintf(stderr,"Error collision 0x%04X %u %u\n",addr,hitlist[addr],type);
        }
        hitlist[addr+0]=type;
        hitlist[addr+1]=type;
        hits++;

printf("add hit 0x%04X 0x%04X %u\n",pcop,addr,type);

    }
    ra=rom[addr+1];
    ra<<=8;
    ra|=rom[addr+0];
    return(ra);
}
//-------------------------------------------------------------------
unsigned int read8 ( unsigned int addr )
{
    unsigned int ra;


if(addr>ROMADDMASK)
{
    printf("read8(0x%X) too big\n",addr);
    exit(1);
}

    ra=rom[addr]&0xFF;
    return(ra);
}
//-------------------------------------------------------------------
unsigned int read16 ( unsigned int addr )
{
    unsigned int ra;

if(addr>ROMADDMASK)
{
    printf("read16(0x%X) too big\n",addr);
    exit(1);
}


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
                sx=fetch(pc,2); pc+=2;
            }
            break;
        }
        case 3:
        {
            if(src==0x0)
            {
                sx=fetch(pc,2); pc+=2;
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
            dx=fetch(pc,2); pc+=2;
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
                //case 0x0: srcval=pc; break;
                case 0x3: sprintf(sstring,"#0x0000"); return(1);
                default: sprintf(sstring,"r%u",src); break;
            }
            break;
        }
        case 1:
        {
            switch(src)
            {
                case 0x0:
                {
                    sprintf(sstring,"L%04X",(sx+pc-2)&ROMADDMASK);
                    break;
                }
                case 0x2:
                {
                    sprintf(sstring,"&0x%04X",sx);
                    break;
                }
                case 0x3: sprintf(sstring,"#0x0001"); return(1);
                default:
                {
                    sprintf(sstring,"%u(r%u)",sx,src);
                    break;
                }
            }
            break;
        }
        case 2:
        {
            switch(src)
            {
                case 0x2: sprintf(sstring,"#0x0004"); return(1);
                case 0x3: sprintf(sstring,"#0x0002"); return(1);
                default:
                {
                    sprintf(sstring,"@r%u",src);
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
                    sprintf(sstring,"#0x%04X",sx);
                    return(1);
                }
                case 0x2: sprintf(sstring,"#0x0008"); return(1);
                case 0x3: sprintf(sstring,"#0xFFFF"); return(1);
                default:
                {
                    sprintf(sstring,"@r%u+",src);
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
            case 0x3: sprintf(dstring,"#0x0000"); return(1);
            default: sprintf(dstring,"r%u",dst); break;
        }
    }
    else
    {
        switch(dst)
        {
            case 0x0:
            {
                sprintf(dstring,"L%04X",dx+pc);
                printf("check label\n");
                return(1);
            }
            case 0x2:
            {
                sprintf(dstring,"&0x%04X",dx);
                break;
            }
            case 0x3: sprintf(dstring,"#0x0001"); return(1);
            default:
            {
                sprintf(dstring,"%u(r%u)",dx,dst);
                break;
            }
        }
    }
    return(0);
}

//-------------------------------------------------------------------
int find_hits ( void )
{
    unsigned int ra;
    unsigned int rc;
    unsigned int res;
    unsigned int cond;
    unsigned int joff;

    pcop=pc;
    inst=fetch(pc,1);
    pc+=2;

    if(inst&0xC000)
    {
        //two operand arithmetic
        op =(inst>>12)&0xF;
        src=(inst>>8)&0xF;
        ad =(inst>>7)&1;
        bw =(inst>>6)&1;
        as =(inst>>4)&3;
        dst=(inst>>0)&0xF;

        do_sx();
        do_dx();
        get_source();
        if(op>=0x4)
        {


            if((as==1)&&(src==0)) lablist[sx+pc-2]=1;



            if(dst)
            {
                fetch(pc,1); //mark hit on next inst
            }
            return(0);
        }
    }
    else
    {
        if(inst&0xE000)
        {
            //conditional jump
            cond=(inst>>10)&7;

            joff=inst&0x3FF;
            joff<<=1;
            if(joff&0x400) joff|=0xF800;
            joff=(joff+pc)&0xFFFF;
            if(cond<7) fetch(pc,1); //mark hit
            fetch(joff,1);  //mark hit
            lablist[joff]=1;
            return(0);
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

                do_sx();
                if(op==0x0) //RRC
                {
                    fetch(pc,1); //mark hit
                    return(0);
                }
                if(op==0x1) //SWPB ----
                {
                    if(bw==0)
                    {
                        fetch(pc,1); //mark hit
                        return(0);
                    }
                }
                if(op==0x2) //RRA
                {
                    fetch(pc,1); //mark hit
                    return(0);
                }
                if(op==0x3) //SXT 0nzc
                {
                    if(bw==0)
                    {
                        fetch(pc,1); //mark hit
                        return(0);
                    }
                }
                if(op==0x4) //push
                {
                    fetch(pc,1); //mark hit
                    return(0);
                }
                if(op==0x5) //call
                {
                    if(bw==0)
                    {
                        fetch(pc,1); //mark hit
                        if((as==1)&&(src==0))
                        {
                            joff=(sx+pc-2)&ROMADDMASK;
                            fetch(joff,1);
                            lablist[joff]=1;
                        }
                        else if((as==3)&&(src==0))
                        {
                            fetch(sx,1);
                            lablist[sx]=1;
                        }
                        else
                        {
                            printf("Incomplete call dest 0x%04X %u %u\n",pcop,as,src);
                        }
                        return(0);
                    }
                }
                if(inst==0x1300) //reti
                {
                    return(0);
                }
            }
            else
            {
                if(inst==0x0FFF) //will call this a halt
                {
                    return(0);
                }
            }
        }
    }


    printf("Unknown instruction [%04X] 0x%04X\n",pcop,inst);

    return(1);
}
//-------------------------------------------------------------------
int codegen ( void )
{
    unsigned int ret;

    pcop=pc;
    inst=fetch(pc,1);
    pc+=2;


    opwords=0;
    sprintf(opstring,"UNKNOWN");
    sstring[0]=0;
    dstring[0]=0;
    if(inst&0xC000)
    {
        //two operand arithmetic
        op =(inst>>12)&0xF;
        src=(inst>>8)&0xF;
        ad =(inst>>7)&1;
        bw =(inst>>6)&1;
        as =(inst>>4)&3;
        dst=(inst>>0)&0xF;

        if(op==0x4) //mov ----
        {
            opwords++;
            sprintf(opstring,"mov");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0x5) //add vnzc
        {
            opwords++;
            sprintf(opstring,"add");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0x6) //addc vncz
        {
            opwords++;
            sprintf(opstring,"addc");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0x7) //subc vnzc
        {
            opwords++;
            sprintf(opstring,"subc");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0x8) //sub vnzc
        {
            opwords++;
            sprintf(opstring,"sub");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0x9) //cmp vnzc
        {
            opwords++;
            sprintf(opstring,"cmp");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xA) //dadd vnzc
        {
            opwords++;
            sprintf(opstring,"dadd");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xB) //bit 0nzc
        {
            opwords++;
            sprintf(opstring,"bit");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xC) //bic ----
        {
            opwords++;
            sprintf(opstring,"bic");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xD) //bis ----
        {
            opwords++;
            sprintf(opstring,"bis");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xE) //xor vnzc
        {
            opwords++;
            sprintf(opstring,"xor");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
        if(op==0xF) //and 0nzc
        {
            opwords++;
            sprintf(opstring,"and");
            if(bw) strcat(opstring,".b");
            do_sx();
            do_dx();
            get_source();
            ret=get_destination();
            if(ret) opwords=0;
            return(0);
        }
    }
    else
    {
        if(inst&0xE000)
        {
            //conditional jump
            cond=(inst>>10)&7;
            joff=inst&0x3FF;
            joff<<=1;
            if(joff&0x400) joff|=0xF800;
            pc+=joff;
            pc&=0xFFFF;
            sprintf(sstring,"L%04X ; pc+0x%04X",pc,joff);
            opwords++;
            switch(cond)
            {
                case 0: //jne/jnz
                {
                    sprintf(opstring,"jnz");
                    return(0);
                }
                case 1: //jeq/jz
                {
                    sprintf(opstring,"jz");
                    return(0);
                }
                case 2: //jnc/jlo
                {
                    sprintf(opstring,"jnc");
                    return(0);
                }
                case 3: //jc/jhs
                {
                    sprintf(opstring,"jc");
                    return(0);
                }
                case 4: //jn
                {
                    sprintf(opstring,"jn");
                    return(0);
                }
                case 5: //jge
                {
                    sprintf(opstring,"jge");
                    return(0);
                }
                case 6: //jl
                {
                    sprintf(opstring,"jl");
                    return(0);
                }
                case 7: //unconditional
                {
                    sprintf(opstring,"jmp");
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
                if(op==0x0) //RRC
                {
                    opwords++;
                    sprintf(opstring,"rrc");
                    if(bw) strcat(opstring,".b");
                    do_sx();
                    ret=get_source();
                    if(ret) opwords=0;
                    return(0);
                }
                if(op==0x1) //SWPB ----
                {
                    if(bw==0)
                    {
                        opwords++;
                        sprintf(opstring,"swpb");
                        do_sx();
                        ret=get_source();
                        if(ret) opwords=0;
                        return(0);
                    }
                }
                if(op==0x2) //RRA
                {
                    opwords++;
                    sprintf(opstring,"rra");
                    if(bw) strcat(opstring,".b");
                    do_sx();
                    ret=get_source();
                    if(ret) opwords=0;
                    return(0);
                }
                if(op==0x3) //SXT 0nzc
                {
                    if(bw==0)
                    {
                        opwords++;
                        sprintf(opstring,"sxt");
                        do_sx();
                        ret=get_source();
                        if(ret) opwords=0;
                        return(0);
                    }
                }
                if(op==0x4) //push
                {
                    opwords++;
                    sprintf(opstring,"push");
                    if(bw) strcat(opstring,".b");
                    do_sx();
                    ret=get_source();
                    if(ret) opwords=0;
                    return(0);
                }
                if(op==0x5) //call
                {
                    if(bw==0)
                    {
                        opwords++;
                        sprintf(opstring,"call");
                        do_sx();
                        ret=get_source();
                        if(ret) opwords=0;
                        return(0);
                    }
                }
                if(inst==0x1300) //reti
                {
                    opwords++;
                    sprintf(opstring,"reti");
                    do_sx();
                    ret=get_source();
                    if(ret) opwords=0;
                    return(0);
                }
            }
            else
            {
                if(inst==0x0FFF)
                {
                    sprintf(opstring,".word 0x0FFF");
                    sprintf(sstring,"; halt");
                }
            }
        }
    }

    return(1);
}
//-------------------------------------------------------------------
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

    memset(hitlist,0,sizeof(hitlist));
    memset(lablist,0,sizeof(lablist));

    for(ra=0xFFE0;ra<0x10000;ra+=2)
    {
        rb=read16(ra);
        if(rb<0xFFE0)
        {
            if(rb&1) ; else
            {
                fetch(rb,1); //add hit
                lablist[rb]=1;
            }
        }
    }
    while(1)
    {
        hits=0;
        for(ra=0;ra<0xFFE0;ra+=2)
        {
            if(hitlist[ra]==1)
            {
                pc=ra;
                find_hits();
            }
        }
        printf("hits %u\n",hits);
        if(hits==0) break;
    }


    fpout=fopen("xdiss.s","wt");
    if(fpout==NULL)
    {
        printf("Error creating file xdiss.s\n");
        return(1);
    }

    for(ra=0;ra<0xFFE0;ra+=2)
    {
        for(rb=0xFFE0;rb<0x10000;rb+=2)
        {
            if(read16(rb)==ra)
            {
                fprintf(fpout,".global L%04X\n",ra);
                break;
            }
        }
        if(lablist[ra]) fprintf(fpout,"L%04X:\n",ra);
        if(hitlist[ra])
        {
            if(hitlist[ra]==1)
            {
                pc=ra;
                codegen();
                if(dstring[0]) fprintf(fpout,"        %s %s,%s\n",opstring,sstring,dstring);
                else           fprintf(fpout,"        %s %s\n",opstring,sstring);
            }
        }
        else
        {
            rb=read16(ra);
            if(rb!=0xFFFF)
            {
                fprintf(fpout,"L%04X: .word 0x%04X\n",ra,rb);
            }
        }
    }
    fclose(fpout);


    fpout=fopen("xvect.s","wt");
    if(fpout==NULL)
    {
        printf("Error creating file xvect.s\n");
        return(1);
    }
    fprintf(fpout,"vector_table:\n");
    for(ra=0xFFE0;ra<0x10000;ra+=2)
    {
        rb=read16(ra);
        fprintf(fpout,"L%04X: .word L%04X\n",ra,rb);
    }
    fclose(fpout);


    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
