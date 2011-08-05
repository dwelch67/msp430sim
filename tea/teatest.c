
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include "testdata.h"

unsigned short sedata[TESTDATALEN];

void hexstring ( unsigned short d ); //printf("%08X\n");
void hexstrings ( unsigned short d ); //printf("%08X ");
void hexstringcr ( unsigned short d ); //printf("%08X\r");

void short_add32 ( unsigned short *hi, unsigned short *lo, unsigned short h, unsigned short l, unsigned short carry_in )
{
    unsigned short carry;
    unsigned short ll,hh;


//unsigned int ax,bx;
//unsigned short lx,hx;

//lx=*lo;
//hx=*hi;

//ax=hx;
//ax<<=16;
//ax|=lx;
//bx=h;
//bx<<=16;
//bx|=l;

//ax+=bx+carry_in;


    ll=*lo;
    hh=*hi;

    carry = (ll&0x7FFF)+(l&0x7FFF)+carry_in;
    carry >>= 15;
    carry += (ll>>15);
    carry += (l>>15);

    ll=ll+l+carry_in;
    hh=hh+h;

    if(carry&2) hh++;

    *lo=ll;
    *hi=hh;

//if( (hh!=((ax>>16)&0xFFFF)) || (ll!=((ax>> 0)&0xFFFF)) )
//{
    //printf("Error 0x%08X 0x%04X%04X %u %u\n",ax,hh,ll,carry,carry_in);
//}

}

//void encrypt (unsigned int* v, unsigned int* k) {
    //unsigned int v0=v[0], v1=v[1], sum=0, i;           /* set up */
    //unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    //unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    //for (i=0; i < 32; i++) {                       /* basic cycle start */
        //sum += delta;
        //v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        //v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    //}                                              /* end cycle */
    //v[0]=v0; v[1]=v1;
//}

void short_encrypt (unsigned short* v, unsigned short* k)
{
    unsigned short i;
    unsigned short sumhi,sumlo;
    unsigned short v0hi ,v0lo;
    unsigned short v1hi ,v1lo;
    unsigned short vahi,valo;
    unsigned short vbhi,vblo;

  //  unsigned int v0=v[0], v1=v[1], sum=0, i;           /* set up */
    //unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
//    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */

    sumhi=0; sumlo=0;
    v0hi=v[0]; v0lo=v[1];
    v1hi=v[2]; v1lo=v[3];

    for (i=0; i < 32; i++)                        /* basic cycle start */
    {
        //sum += delta;
        short_add32(&sumhi,&sumlo,0x9E37,0x79B9,0);
        //v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        vahi=(v1hi<<4)|(v1lo>>(16-4));
        valo=(v1lo<<4);
        short_add32(&vahi,&valo,k[0],k[1],0);
        vbhi=v1hi;
        vblo=v1lo;
        short_add32(&vbhi,&vblo,sumhi,sumlo,0);
        vahi^=vbhi;
        valo^=vblo;
        vbhi=(v1hi>>5);
        vblo=(v1hi<<(16-5))|(v1lo>>5);
        short_add32(&vbhi,&vblo,k[2],k[3],0);
        vahi^=vbhi;
        valo^=vblo;
        short_add32(&v0hi,&v0lo,vahi,valo,0);
        //v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        vahi=(v0hi<<4)|(v0lo>>(16-4));
        valo=(v0lo<<4);
        short_add32(&vahi,&valo,k[4],k[5],0);
        vbhi=v0hi;
        vblo=v0lo;
        short_add32(&vbhi,&vblo,sumhi,sumlo,0);
        vahi^=vbhi;
        valo^=vblo;
        vbhi=(v0hi>>5);
        vblo=(v0hi<<(16-5))|(v0lo>>5);
        short_add32(&vbhi,&vblo,k[6],k[7],0);
        vahi^=vbhi;
        valo^=vblo;
        short_add32(&v1hi,&v1lo,vahi,valo,0);
    }                                              /* end cycle */
    //v[0]=v0; v[1]=v1;
    v[0]=v0hi;
    v[1]=v0lo;
    v[2]=v1hi;
    v[3]=v1lo;
}

//void decrypt (unsigned int* v, unsigned int* k) {
    //unsigned int v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    //unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    //unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    //for (i=0; i<32; i++) {                         /* basic cycle start */
        //v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        //v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        //sum -= delta;
    //}                                              /* end cycle */
    //v[0]=v0; v[1]=v1;
//}

void short_decrypt (unsigned short *v, unsigned short *k)
{
    //unsigned int v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    //unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    //unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    unsigned short i;
    unsigned short sumhi,sumlo;
    unsigned short v0hi ,v0lo;
    unsigned short v1hi ,v1lo;
    unsigned short vahi,valo;
    unsigned short vbhi,vblo;

    sumhi=0xC6EF; sumlo=0x3720;
    v0hi=v[0]; v0lo=v[1];
    v1hi=v[2]; v1lo=v[3];
    for (i=0; i<32; i++)                          /* basic cycle start */
    {
        //v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        vahi=(v0hi<<4)|(v0lo>>(16-4));
        valo=(v0lo<<4);
        short_add32(&vahi,&valo,k[4],k[5],0);
        vbhi=v0hi;
        vblo=v0lo;
        short_add32(&vbhi,&vblo,sumhi,sumlo,0);
        vahi^=vbhi;
        valo^=vblo;
        vbhi=(v0hi>>5);
        vblo=(v0hi<<(16-5))|(v0lo>>5);
        short_add32(&vbhi,&vblo,k[6],k[7],0);
        vahi^=vbhi;
        valo^=vblo;
        short_add32(&v1hi,&v1lo,~vahi,~valo,1);
        //v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        vahi=(v1hi<<4)|(v1lo>>(16-4));
        valo=(v1lo<<4);
        short_add32(&vahi,&valo,k[0],k[1],0);
        vbhi=v1hi;
        vblo=v1lo;
        short_add32(&vbhi,&vblo,sumhi,sumlo,0);
        vahi^=vbhi;
        valo^=vblo;
        vbhi=(v1hi>>5);
        vblo=(v1hi<<(16-5))|(v1lo>>5);
        short_add32(&vbhi,&vblo,k[2],k[3],0);
        vahi^=vbhi;
        valo^=vblo;
        short_add32(&v0hi,&v0lo,~vahi,~valo,1);
        //sum -= delta;
        short_add32(&sumhi,&sumlo,0x61C8,0x8646,1);

    }                                              /* end cycle */
    //v[0]=v0; v[1]=v1;
    v[0]=v0hi;
    v[1]=v0lo;
    v[2]=v1hi;
    v[3]=v1lo;
}


int run_tea_test ( void )
{
    unsigned short ra;
    unsigned short errors;
    unsigned short sdata[4];
    unsigned short skey[8];

    errors=0;

    skey[0]=0x1111; skey[1]=1111;
    skey[2]=0x2222; skey[3]=2222;
    skey[4]=0x3333; skey[5]=3333;
    skey[6]=0x4444; skey[7]=4444;

    hexstring(0x1234);

    if((TESTDATALEN&3))
    {
        hexstring(0xBAD0);
        return(1);
    }

    for(ra=0;ra<TESTDATALEN;ra+=4)
    {
        sdata[0]=testdata[ra+0];
        sdata[1]=testdata[ra+1];
        sdata[2]=testdata[ra+2];
        sdata[3]=testdata[ra+3];
        short_encrypt(sdata,skey);
        sedata[ra+0]=sdata[0];
        sedata[ra+1]=sdata[1];
        sedata[ra+2]=sdata[2];
        sedata[ra+3]=sdata[3];
    }

    for(ra=0;ra<TESTDATALEN;ra+=4)
    {
        sdata[0]=sedata[ra+0];
        sdata[1]=sedata[ra+1];
        sdata[2]=sedata[ra+2];
        sdata[3]=sedata[ra+3];
        short_decrypt(sdata,skey);
        if(sdata[0]!=testdata[ra+0])
        {
            errors++;
            hexstrings(ra); hexstrings(sdata[0]); hexstring(testdata[ra+0]);
        }
        if(sdata[1]!=testdata[ra+1])
        {
            errors++;
            hexstrings(ra); hexstrings(sdata[1]); hexstring(testdata[ra+1]);
        }
        if(sdata[2]!=testdata[ra+2])
        {
            errors++;
            hexstrings(ra); hexstrings(sdata[2]); hexstring(testdata[ra+2]);
        }
        if(sdata[3]!=testdata[ra+3])
        {
            errors++;
            hexstrings(ra); hexstrings(sdata[3]); hexstring(testdata[ra+3]);
        }
        if(errors>20) break;
    }
    hexstring(errors);
    if(errors)
    {
        hexstring(0xBAD9);
        return(1);
    }
    hexstring(0x1234);
    return(0);
}

