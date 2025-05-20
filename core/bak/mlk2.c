
// ----------------------
//  AMuPLC (mlk2.c)
// ----------------------

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#include "mlk.h"
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKCDE *lc;
extern int slsf;
extern SYNSORT sa[MAXAB];
extern SYNSORT sb[MAXAB];
extern SYNSORT sc[MAXBC];
int  spa;     /* sort point a */
int  spb;     /* sort point b */
int  spc;     /* sort point b */
int  ap;      /* move point a */
int  bp;      /* move point b */
int  cp;      /* move point c */
int  gk;      /* output debug */
/* 2012.12.17 long -> int
union {
    unsigned char  wc[4];
    long sync;
} a,b,c;
long fct[MAXSC];  
*/
union {
    unsigned char  wc[4];
    int  sync;
} a,b,c;
int  fct[MAXSC];   /* sync sort work table */
long lword;
int  eofa;         /* A file eof flag */
int  eofb;         /* B file eof flag */
int  bpassf;       /* B file skip flag */
unsigned char dda[4];  /* ddxxyyzz code sa area */
unsigned char ddb[4];  /* ddxxyyzz code sb area */
int  aport;        /* afile port for ddcode */
int  atrak;        /* afile track for ddcode */
int  amemb;        /* afile member for ddcode */

int cpconv0_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];

    if(ls->debug) printf("cpconv0_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    strcat(afile,"_wka.mlk");
    midfconv_mlk(dfile,afile,portnb);
    return(0);
}

int cpconv1_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv1_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        bfile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    bfile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(bfile,"_wkb.mlk");
    /* midfadd_mlk(dfile,afile,bfile,portnb); */
    if(lv->mmemb == 0) midfadd_mlk(dfile,afile,bfile,portnb);
    else midfmix_mlk(dfile,afile,bfile,portnb);
    return(0);
}

int cpconv2_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv2_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        bfile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    bfile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(bfile,"_wkb.mlk");
    /* midfadd_mlk(dfile,bfile,afile,portnb); */
    if(lv->mmemb == 0) midfadd_mlk(dfile,bfile,afile,portnb);
    else midfmix_mlk(dfile,bfile,afile,portnb);
    return(0);
}

int cpconv3_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    //char wk[4];
    char wk[80];  // 2025.4.8
    char afile[STLN];          /* work file a */
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv3_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    ofile[i] = '\0';
    strcat(afile,"_wka.mlk");
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    if(lv->mmemb == 0) midfadd_mlk(dfile,afile,ofile,portnb);
    else midfmix_mlk(dfile,afile,ofile,portnb);
    /* midfconv_mlk(afile,ofile,portnb); */
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

int cpconv4_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    //char wk[4];
    char wk[80];   // 2025.4.8
    char bfile[STLN];          /* work file a */
    char ofile[STLN];          /* exe file */

    if(ls->debug) printf("cpconv4_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        bfile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    bfile[i] = '\0';
    ofile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    if(lv->mmemb == 0) midfadd_mlk(dfile,bfile,ofile,portnb);
    else midfmix_mlk(dfile,bfile,ofile,portnb);
    /* midfconv_mlk(bfile,ofile,portnb); */
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

int cpconv5_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    //char wk[4];
    char wk[80];      // 2025.4.8
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv5_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        ofile[i] = dfile[i];
        i++;
    }
    ofile[i] = '\0';
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    midfconv_mlk(dfile,ofile,portnb);
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

/* include master file to wkb file */
int inmftowb_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char bfile[STLN];
    char xfile[STLN];         /* port0 mix file */

    if(ls->debug) printf("inmftowb_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        bfile[i] = dfile[i];
        xfile[i] = dfile[i];
        i++;
    }
    bfile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    xfile[i] = '\0';
    strcat(xfile,"_000.mix");
    incmstf_mlk(xfile,bfile,portnb); /* include master file to cfile */
    return(0);
}

int cpconv10_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];
    char bfile[STLN];
    char xfile[STLN];         /* port0 mix file */

    if(ls->debug) printf("cpconv10_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        bfile[i] = dfile[i];
        xfile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    strcat(afile,"_wka.mlk");
    bfile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    xfile[i] = '\0';
    strcat(xfile,"_000.mix");
    incmstf_mlk(xfile,bfile,portnb); /* include master file to cfile */
    midfmix_mlk(dfile,bfile,afile,portnb);
    return(0);
}

int cpconv11_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv11_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        bfile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    bfile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(bfile,"_wkb.mlk");
    midfmix_mlk(dfile,afile,bfile,portnb);
    return(0);
}

int cpconv12_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv12_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        bfile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    bfile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(bfile,"_wkb.mlk");
    midfmix_mlk(dfile,bfile,afile,portnb);
    return(0);
}

int cpconv13_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    //char wk[4];
    char wk[80];    // 2025.4.8
    char afile[STLN];          /* work file a */
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv13_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    ofile[i] = '\0';
    strcat(afile,"_wka.mlk");
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    midfmix_mlk(dfile,afile,ofile,portnb);
    /* midfconv_mlk(afile,ofile,portnb); */
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

int cpconv14_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    // char wk[4];
    char wk[80];    // 2025.4.8
    char bfile[STLN];          /* work file a */
    char ofile[STLN];          /* exe file */

    if(ls->debug) printf("cpconv14_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        bfile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    bfile[i] = '\0';
    ofile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    midfmix_mlk(dfile,bfile,ofile,portnb);
    /* midfconv_mlk(bfile,ofile,portnb); */
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}


int cpconv15_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    //char wk[4];
    char wk[80];    // 2025.4.8
    char afile[STLN];
    char xfile[STLN];
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("cpconv15_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        xfile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    strcat(afile,"_wka.mlk");
    xfile[i] = '\0';
    strcat(xfile,"_000.mix");
    ofile[i] = '\0';
    sprintf(wk,"_%03d.mix",portnb);
    strcat(ofile,wk);
    incmstf_mlk(xfile,afile,portnb); /* include master file to afile */
    midfmix_mlk(dfile,afile,ofile,portnb);
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

int ctoval_mlk(unsigned char iword[4])
//unsigned char iword[4];
{
    union MCODE u;

    if(ls->comptyp == 0) {
        lc->c[0] = iword[0];
        lc->c[1] = iword[1];
        lc->c[2] = iword[2];
        lc->c[3] = iword[3];
        u.wc[0] = iword[1];
        u.wc[1] = iword[0];
        u.wc[2] = iword[3];
        u.wc[3] = iword[2];
        lc->i[0] = (int)iword[1];
        lc->i[1] = u.wi[1];
        lc->s[0] = u.wi[0];
        lc->s[1] = u.wi[1];
        u.wi[0] = lc->s[1];
        u.wi[1] = lc->s[0] & 0x00ff;
        lc->l = u.wl;
    }
    if(ls->comptyp == 1) {
        lc->c[0] = iword[0];
        lc->c[1] = iword[1];
        lc->c[2] = iword[2];
        lc->c[3] = iword[3];
        u.wc[0] = iword[0];
        u.wc[1] = iword[1];
        u.wc[2] = iword[2];
        u.wc[3] = iword[3];
        lc->i[0] = (int)u.wc[1];
        lc->i[1] = u.wi[1];
        lc->s[0] = u.wi[0];
        lc->s[1] = u.wi[1];
        u.wc[0] &= 0x00;
        lc->l = u.wl;
    }
    if(ls->comptyp == 2) {
        lc->c[0] = iword[0];
        lc->c[1] = iword[1];
        lc->c[2] = iword[2];
        lc->c[3] = iword[3];
        u.wc[0] = iword[1];
        u.wc[1] = iword[0];
        u.wc[2] = iword[3];
        u.wc[3] = iword[2];
        lc->i[0] = (int)iword[1];
        lc->i[1] = u.wi[1];
        lc->s[0] = u.wi[0];
        lc->s[1] = u.wi[1];
        u.wi[0] = lc->s[1];
        u.wi[1] = lc->s[0] & 0x00ff;
        lc->l = u.wl;
    }
    return(0);
}

/* Mlkcnv */
int midfconv_mlk(char dfile[STLN],char ofile[STLN],int portnb)
//char dfile[STLN];
//char ofile[STLN];
//int  portnb;
{
    FILE *fpi,*fpo;
    unsigned char iword[4];
    int  j,k;
    long lword;
    int  c;

    if(ls->debug) printf("midfconv_() dfile=%s ofile=%s portnb=%d\n",
        dfile,dfile,portnb);
    fpi = fopen_h(dfile,"rb");
    fpo = fopen_h(ofile,"wb");
    k = 0;
    j = 0;
    lv->occnt = 0;
    lword = 0;
    mkptmscd_mlk(dfile,portnb);  /* make ddb(DD port track member) code */
    fputc(ddb[0],fpo);
    fputc(ddb[1],fpo);
    fputc(ddb[2],fpo);
    fputc(ddb[3],fpo);
    lv->occnt = lv->occnt + 4;
    if(ls->trace) ls->trace = 3;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            switch (iword[0]) {
                case 0xf1 : f1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xb1 : b1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf2 : f2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfb : mfbdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            /* smptubit_mlk(&u,fpo); */
                            break;
                case 0xfc : mfcdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfd : fddecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf8 : f8decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf4 : f4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf6 : f6decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfa : fadecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfe : fedecode_mlk();
                            break;
                case 0xdd : dddecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe1 : e1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe2 : e2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe3 : e3decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe4 : e4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe6 : e6decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                default   : break;
            }
            lword++;
        }
    }
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    lv->occnt = lv->occnt + 4;
    fclose(fpi);
    fclose(fpo);
    if(ls->odump == 1) outcdump_mlk(ofile);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
      printf("Mlkcnv p[%d] %s -> ",portnb,dfile);
      printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    return(0);
}

/* Mlkinc include master file to ofile */
int incmstf_mlk(char dfile[STLN],char ofile[STLN],int portnb)
//char dfile[STLN];
//char ofile[STLN];
//int  portnb;
{
    FILE *fpi,*fpo;
    unsigned char iword[4];
    int  j,k;
    long lword;
    int  c;

    if(ls->debug) printf("incmst_() dfile=%s ofile=%s portnb=%d\n",
        dfile,dfile,portnb);
    fpi = fopen_h(dfile,"rb");
    fpo = fopen_h(ofile,"wb");
    k = 0;
    j = 0;
    lv->occnt = 0;
    lword = 0;
    if(ls->trace) ls->trace = 3;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            switch (iword[0]) {
                case 0xf1 : f1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xb1 : b1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfd : if(ls->mergef) {   /* include option */
                                fddecode_mlk();
                                fputcd_mlk(iword,fpo);
                            }
                            break;
                case 0xf2 : f2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfb : mfbdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfc : mfcdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf4 : f4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf6 : if(ls->mergef) {
                                f6decode_mlk();
                                fputcd_mlk(iword,fpo);
                            }
                            break;
                case 0xf8 : f8decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfa : fadecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xdd : dddecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe1 : e1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe2 : e2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe3 : e3decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe4 : e4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe6 : e6decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                default   : break;
            }
            lword++;
        }
    }
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    lv->occnt = lv->occnt + 4;
    fclose(fpi);
    fclose(fpo);
    if(ls->odump == 1) outcdump_mlk(ofile);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlkinc p[%d] %s -> ",portnb,dfile);
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    return(0);
}

/* Mlkadd */
int midfadd_mlk(char dfile[STLN],char mfile[STLN],char ofile[STLN],int portnb)
//char dfile[STLN];    /* data file */
//char mfile[STLN];    /* mix input file (work file) */
//char ofile[STLN];    /* output file */
//int  portnb;
{
    FILE *fpi,*fpm,*fpo;
    unsigned char iword[4];
    int  j,k;
    long lword;
    int  c;

    if(ls->debug)
        printf("midfadd_() dfile=%s mfile=%s ofile=%s portnb=%d\n",
        dfile,mfile,dfile,portnb);
    fpi = fopen_h(dfile,"rb");
    fpm = fopen_h(mfile,"rb");
    fpo = fopen_h(ofile,"wb");
    k = 0;
    j = 0;
    lv->occnt = 0;
    lword = 0;
    /*
    mkptmscd_mlk(mfile,portnb);
    fputc(ddb[0],fpo);
    fputc(ddb[1],fpo);
    fputc(ddb[2],fpo);
    fputc(ddb[3],fpo);
    lv->occnt = lv->occnt + 4;
    */
    if(ls->trace) ls->trace = 3;
    while((c = fgetc(fpm)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            mfocsel_mlk(iword,fpo);
            lword++;
        }
    }
    fclose(fpm);
    mkptmscd_mlk(dfile,portnb);  /* make ddb(DD port track member) code */
    fputc(ddb[0],fpo);
    fputc(ddb[1],fpo);
    fputc(ddb[2],fpo);
    fputc(ddb[3],fpo);
    lv->occnt = lv->occnt + 4;
    k = 0;
    j = 0;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            mfocsel_mlk(iword,fpo);
            lword++;
        }
    }
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    lv->occnt = lv->occnt + 4;
    fclose(fpi);
    fclose(fpo);
    if(ls->odump == 1) outcdump_mlk(ofile);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlkadd p[%d] %s + %s -> ",portnb,dfile,mfile);
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    return(0);
}

int mfocsel_mlk(unsigned char iword[4],FILE *fpo)
//unsigned char iword[4];
//FILE *fpo;
{
    switch (iword[0]) {
        case 0xf1 : f1decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xb1 : b1decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xf2 : f2decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xfb : mfbdecode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xfc : mfcdecode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xfd : fddecode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xf8 : f8decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xf4 : f4decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xf6 : f6decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xfa : fadecode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xfe : fedecode_mlk();
                    break;
        case 0xdd : dddecode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xe1 : e1decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xe2 : e2decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xe3 : e3decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xe4 : e4decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        case 0xe6 : e6decode_mlk();
                    fputcd_mlk(iword,fpo);
                    break;
        default   : break;
    }
    return(0);
}

/*
fputw_mlk(c,fpo)
char c;
FILE *fpo;
{
    fputc(0xad,fpo);
    fputc(lv->trak,fpo);
    fputc(lv->memb,fpo);
    fputc(c,fpo);
    lv->occnt = lv->occnt + 4;
    return(0);
}
fgetw_mlk(fpo)
FILE *fpo;
{
    int c;
    int dd;

    dd = fgetc(fpo);
    if(dd != 0xdd) return(-1);
    lv->mtrak = fgetc(fpo);
    lv->mmemb = fgetc(fpo);
    c = fgetc(fpo);
    return(c);
}
*/

int fputcd_mlk(unsigned char iword[4],FILE *fpo)
//unsigned char iword[4];
//FILE *fpo;
{
    fputc(iword[0],fpo);
    fputc(iword[1],fpo);
    fputc(iword[2],fpo);
    fputc(iword[3],fpo);
    lv->occnt = lv->occnt + 4;
    return(0);
}

int outcdump_mlk(char dfile[STLN])
//char dfile[STLN];
{
    int  c1,c2,c3,c4;
    long l;
    long oct;
    int  mod;
    FILE *fpi;

    oct = 0;
    mod = 0;
    l = 0;
    fpi = fopen_h(dfile,"rb");
    while(1) {
        c1 = fgetc(fpi);
        c2 = fgetc(fpi);
        c3 = fgetc(fpi);
        c4 = fgetc(fpi);
        if(c1 == 0xed) break;
        if(!mod) printf("%07ld",oct);
        printf(" %02x%02x%02x%02x",c1,c2,c3,c4);
        l++;
        oct++;
        mod = oct % 8;
        if(!mod) printf("\n");
    }
    printf("\n");
    fclose(fpi);
    return(0);
}

/* make ddb (port,track,member select) code */
int mkptmscd_mlk(char bfile[STLN],int portnb)
//char bfile[STLN];    /* b file */
//int  portnb;
{
    int  i;
    //unsigned char iword[4];
    char iword[4];
    int  bport;
    int  btrak;
    int  bmemb;

    /* make ddb (port,track,member select) code */
    if(ls->debug) printf("mkptmscd_mlk() bfile=%s,portnb=%d\n",bfile,portnb);
    i = 0;
    while(bfile[i] != '_') i++;
    iword[0] = bfile[++i];
    iword[1] = '\0';
    sscanf(iword,"%d",&btrak);
    iword[0] = bfile[++i];
    iword[1] = bfile[++i];
    iword[2] = '\0';
    sscanf(iword,"%x",&bmemb);
    ddb[0] = 0xdd;
    bport = portnb & 0x7f;
    ddb[1] = bport;
    ddb[2] = btrak;
    ddb[3] = bmemb;
    if(ls->debug == 6) {
        printf("\"%s\" ddb=DD%02x%02x%02x port=%d trak=%d memb=%d\n",
        bfile,ddb[1],ddb[2],ddb[3],bport,btrak,bmemb);
    }
    return(0);
}

/* Mlkmix MIX A & B file */
/* dfile = A mfile = B */
int midfmix_mlk(char dfile[STLN],char mfile[STLN],char ofile[STLN],int portnb)
//char dfile[STLN];    /* data file */
//char mfile[STLN];    /* mix input file (work file) */
//char ofile[STLN];    /* output file */
//int  portnb;
{
    FILE *fpa,*fpb,*fpo;
    unsigned char iword[4];
    //char iword[4];  // 2019.3.30
    int  i,j,k;
    int  c;
    int  fchain;
    long synchain;

    if(ls->debug)
        printf("midfmix_() dfile=%s mfile=%s ofile=%s portnb=%d\n",
        dfile,mfile,dfile,portnb);
    fpa = fopen_h(dfile,"rb");
    fpb = fopen_h(mfile,"rb");
    fpo = fopen_h(ofile,"wb");
    /* make dda(DD port track member) code */
    i = 0;
    while(dfile[i] != '_') i++;
    iword[0] = dfile[++i];
    iword[1] = '\0';
    sscanf((char *)iword,"%d",&atrak);  // 2019.3.30
    iword[0] = dfile[++i];
    iword[1] = dfile[++i];
    iword[2] = '\0';
    //sscanf(iword,"%x",&amemb);
    sscanf((char *)iword,"%d",&amemb);  // 2019.3.30
    dda[0] = 0xdd;
    aport = portnb & 0x7f;
    dda[1] = aport;
    dda[2] = atrak;
    dda[3] = amemb;
    if(ls->debug == 6) {
        printf("\"%s\" dda=DD%02x%02x%02x port=%d trak=%d memb=%d\n",
            dfile,dda[1],dda[2],dda[3],aport,atrak,amemb);
    }
    k = 0;
    j = 0;
    lv->mmeas = 0;
    lv->meas = 99999999;
    lv->occnt = 0;
    bpassf = 0;
    spa = 0;
    spb = 0;
    eofa = 0;
    eofb = 0;
    lword = 0;
    fchain = 0;
    synchain = 88888888;
    savema_mlk(dda);  /* ddcode save */
    while((c = fgetc(fpa)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            if(ls->trace) ls->trace = 1;
            switch (iword[0]) {
                case 0xff : ffdecode_mlk();
                            break;
                case 0xf1 : f1decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xb1 : b1decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xf2 : f2decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xfb : mfbdecode_mlk();
                            savema_mlk(iword);
                            fchain = 0;
                            if(lv->mmeas == lv->meas) abtsort_mlk(fpo);
                            if(!eofb) bfschout_mlk(fpb,fpo);
                            if(lv->mmeas == lv->meas) abtsort_mlk(fpo);
                            if(lv->mmeas < lv->meas) atsmout_mlk(fpo);
                            else {
                                if(eofb) atsmout_mlk(fpo);
                            }
                            break;
                case 0xfc : mfcdecode_mlk();
                            /* Add fcain bug patch for 0xfc data chain */
                            if(lv->msync == synchain && fchain >= 1) break; 
                            savema_mlk(iword); /* 1996.10.13 */
                            synchain = lv->msync;
                            fchain++;
                            break;
                case 0xfd : fddecode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xf8 : f8decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xf4 : f4decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xf6 : f6decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xfa : fadecode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xfe : fedecode_mlk();
                            /* savema_mlk(iword); */
                            break;
                case 0xdd : dddecode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xe1 : e1decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xe2 : e2decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xe3 : e3decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xe4 : e4decode_mlk();
                            savema_mlk(iword);
                            break;
                case 0xe6 : e6decode_mlk();
                            savema_mlk(iword);
                            break;
                default   : /* fchain = 0; */
                            break;
            }
            lword++;
        }
        if(eofa) break;
    }
    if(lv->mmeas <= lv->meas) {
        atsmout_mlk(fpo);
        if(!eofb) bfsmout_mlk(fpb,fpo);
    }
    else {
        if(!eofb) bfsmout_mlk(fpb,fpo);
        atsmout_mlk(fpo);
    }
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    fputc(0xed,fpo);
    lv->occnt = lv->occnt + 4;
    fclose(fpb);
    fclose(fpa);
    fclose(fpo);
    if(ls->trace) {
        k = 0;
        j = 0;
        fpo = fopen_h(ofile,"rb");
        while((c = fgetc(fpo)) != EOF) {
            j = k % 4;
            iword[j] = c;
            k++;
            if(j == 3) {
                k = 0;
                ctoval_mlk(iword);   /* set code to value */
                if(ls->trace) ls->trace = 3;
                switch (iword[0]) {
                    case 0xff : ffdecode_mlk();
                                break;
                    case 0xf1 : f1decode_mlk();
                                break;
                    case 0xb1 : b1decode_mlk();
                                break;
                    case 0xf2 : f2decode_mlk();
                                break;
                    case 0xfb : mfbdecode_mlk();
                                break;
                    case 0xfc : mfcdecode_mlk();
                                break;
                    case 0xfd : fddecode_mlk();
                                break;
                    case 0xf8 : f8decode_mlk();
                                break;
                    case 0xf4 : f4decode_mlk();
                                break;
                    case 0xf6 : f6decode_mlk();
                                break;
                    case 0xfa : fadecode_mlk();
                                break;
                    case 0xfe : fedecode_mlk();
                                break;
                    case 0xdd : dddecode_mlk();
                                break;
                    case 0xe1 : e1decode_mlk();
                                break;
                    case 0xe2 : e2decode_mlk();
                                break;
                    case 0xe3 : e3decode_mlk();
                                break;
                    case 0xe4 : e4decode_mlk();
                                break;
                    case 0xe6 : e6decode_mlk();
                                break;
                    default   : break;
                }
            }
        }
        fclose(fpo);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlkmix p[%d] %s + %s -> ",portnb,dfile,mfile);
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    spa = 0; /* add 98.1 */
    spb = 0; /* add 98.1 */
    return(0);
}

/*  B file out */
int bfsmout_mlk(FILE *fpb,FILE *fpo)
//FILE *fpb,*fpo;
{
    unsigned char iword[4];
    int  j,k;
    int  c;

    k = 0;
    fputc(0xdd,fpo);
    fputc((char)lv->port,fpo);
    fputc((char)lv->trak,fpo);
    fputc((char)lv->memb,fpo);
    while((c = fgetc(fpb)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            if(ls->trace) ls->trace = 2;
            switch (iword[0]) {
                case 0xff : ffdecode_mlk();
                            break;
                case 0xf1 : f1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xb1 : b1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf2 : f2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfb : mfbdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfc : mfcdecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfd : fddecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf8 : f8decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf4 : f4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xf6 : f6decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfa : fadecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xfe : fedecode_mlk();
                            /* fputcd_mlk(iword,fpo); */
                            break;
                case 0xdd : dddecode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe1 : e1decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe2 : e2decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe3 : e3decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe4 : e4decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                case 0xe6 : e6decode_mlk();
                            fputcd_mlk(iword,fpo);
                            break;
                default   : break;
            }
        }
    }
    eofb = 1;
    return(0);
}

/* B file serch & out */
int bfschout_mlk(FILE *fpb,FILE *fpo)
//FILE *fpb,*fpo;
{
    unsigned char iword[4];
    int  j,k;
    int  c;
    int  fchain;
    long synchain;

    if(bpassf && (lv->mmeas < lv->meas)) return(1);
    k = 0;
    j = 0;
    fchain = 0;
    synchain = 88888888;
    while((c = fgetc(fpb)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            if(ls->trace) ls->trace = 2;
            switch (iword[0]) {
                case 0xff : ffdecode_mlk();
                            break;
                case 0xf1 : f1decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xb1 : b1decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xf2 : f2decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xfb : fbdecode_mlk();
                            savemb_mlk(iword);
                            bpassf = 1;
                            if(lv->mmeas <= lv->meas) return(0);
                            btsmout_mlk(fpo);
                            fchain = 0;
                            break;
                case 0xfc : fcdecode_mlk();
                            /* Add fcain bug patch for 0xfc data chain 98.1 */
                            if(lv->sync == synchain && fchain >= 1) break; 
                            savemb_mlk(iword);
                            synchain = lv->sync;
                            fchain++;
                            break;
                case 0xfd : fddecode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xf8 : f8decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xf4 : f4decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xf6 : f6decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xfa : fadecode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xfe : fedecode_mlk();
                            /* savemb_mlk(iword); */
                            fchain = 0;
                            break;
                case 0xdd : dddecode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xe1 : e1decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xe2 : e2decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xe3 : e3decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xe4 : e4decode_mlk();
                            savemb_mlk(iword);
                            break;
                case 0xe6 : e6decode_mlk();
                            savemb_mlk(iword);
                            break;
                default   : /* fchain = 0; */
                            break;
            }
            lword++;
        }
        if(eofb) break;
    }
    btsmout_mlk(fpo);
    eofb = 1;
    return(0);
}

/* A file data table out */
int atsmout_mlk(FILE *fpo)
//FILE *fpo;
{
    int  i;

    if(ls->debug == 2) {
        printf("### Atb out spa=%d eofb=%d A_meas.=%ld B_meas.=%ld\n",
            spa,eofb,lv->mmeas,lv->meas);
    }
    fputc(dda[0],fpo);
    fputc(dda[1],fpo);
    fputc(dda[2],fpo);
    fputc(dda[3],fpo);
    for(i = 0; i < spa; i++) {
        fputc(sa[i].c0,fpo);
        fputc(sa[i].c1,fpo);
        fputc(sa[i].c2,fpo);
        fputc(sa[i].c3,fpo);
        lv->occnt = lv->occnt + 4;
    }
    spa = 0;
    return(0);
}

/* B file data table out */
int btsmout_mlk(FILE *fpo)
//FILE *fpo;
{
    int  i;

    if(ls->debug == 2) {
        printf("### Btb out spb=%d eofa=%d A_meas.=%ld B_meas.=%ld\n",
            spb,eofa,lv->mmeas,lv->meas);
    }
    fputc(0xdd,fpo);
    fputc((char)lv->port,fpo);
    fputc((char)lv->trak,fpo);
    fputc((char)lv->memb,fpo);
    for(i = 0; i < spb; i++) {
        fputc(sb[i].c0,fpo);
        fputc(sb[i].c1,fpo);
        fputc(sb[i].c2,fpo);
        fputc(sb[i].c3,fpo);
        lv->occnt = lv->occnt + 4;
    }
    spb = 0;
    return(0);
}

int savema_mlk(unsigned char iword[4])
//unsigned char iword[4];
{
    sa[spa].c0 = iword[0];
    sa[spa].c1 = iword[1];
    sa[spa].c2 = iword[2];
    sa[spa].c3 = iword[3];
    spa++;
    if(spa >= MAXAB) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! Too many data in meas at savema(spa=%d). limit %d\n",
	    spa,MAXAB);
        spa = 0;
        return(-1);
    }
    return(0);
}

int savemb_mlk(unsigned char iword[4])
//unsigned char iword[4];
{
    sb[spb].c0 = iword[0];
    sb[spb].c1 = iword[1];
    sb[spb].c2 = iword[2];
    sb[spb].c3 = iword[3];
    spb++;
    if(spb >= MAXAB) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! Too many data in meas at savemb(spb=%d). limit %d\n",
	    spb,MAXAB);
        spb = 0;
        return(-1);
    }
    return(0);
}

/* sort merge sa & sb tabel to sc tabel */
int abtsort_mlk(FILE *fpo)
//FILE *fpo;
{
    int  i,j;
    int  rt;
    int  rc;
    long fcwk;
    int  fc;

    a.sync = 0;
    ap = 0;
    bp = 0;
    cp = 0;
    rc = -1;
    if(ls->debug == 2) {
        for(i = 0; i < spa; i++) {
            if((i % 8) == 0) printf("\nA ");
            printf("%02x%02x%02x%02x ",
                sa[i].c0, sa[i].c1, sa[i].c2, sa[i].c3);
        }
        for(i = 0; i < spb; i++) {
            if((i % 8) == 0) printf("\nB ");
            printf("%02x%02x%02x%02x ",
                sb[i].c0, sb[i].c1, sb[i].c2, sb[i].c3);
        }
        printf("\n");
    }
    baddtoc_mlk(-1);   /* put btb until 0xfc data */
    for(i = 0; i < spa; i++) {
        if(sa[i].c0 == 0xfb) {
            if(rc != -1) baddtoc_mlk(rc);
            sortmovb_mlk();
            break;
        }
        if(rc != -1) baddtoc_mlk(rc);
        rc = -1;
        if(ddasave_mlk()) break;
        sc[cp].c0 = sa[ap].c0;
        sc[cp].c1 = sa[ap].c1;
        sc[cp].c2 = sa[ap].c2;
        sc[cp].c3 = sa[ap].c3;
        ap++;
        cp++;
        if(ap >= spa) break;
        if(cp >= MAXBC) {
            ls->errcnt++;
            ls->msglvl = 3;
            if(ls->msglvl >= ls->prtlvl)
                printf(
	        "Error! Too many data in meas at abtsort(cp=%d). limit %d\n",
		cp,MAXBC);
            break;
        }
        if(sa[i].c0 == 0xfc) {
            if(ls->comptyp == 0) {
                a.wc[0] = sa[i].c3;
                a.wc[1] = sa[i].c2;
                a.wc[2] = sa[i].c1;
                a.wc[3] = 0x00;
            }
            if(ls->comptyp == 1) {
                a.wc[3] = sa[i].c3;
                a.wc[2] = sa[i].c2;
                a.wc[1] = sa[i].c1;
                a.wc[0] = 0x00;
            }
            if(ls->comptyp == 2) {
                a.wc[0] = sa[i].c3;
                a.wc[1] = sa[i].c2;
                a.wc[2] = sa[i].c1;
                a.wc[3] = 0x00;
            }
            rc = bsynchk_mlk();
        }
        if(ls->debug == 2) {
            printf("rc=%d i=%d a.sync=%d b.sync=%d ap=%d bp=%d cp=%d\n",
            rc, i, a.sync, b.sync, ap, bp, cp);
        }
    }
    if(ls->debug == 2) {
        for(i = 0; i < cp; i++) {
            if((i % 8) == 0) printf("\nC ");
            printf("%02x%02x%02x%02x ",
                sc[i].c0, sc[i].c1, sc[i].c2, sc[i].c3);
        }
    }
    /* make sync table fct */
    fc = 0;
    for(i = 0; i < cp; i++) {
        if(sc[i].c0 == 0xfc) {
            if(ls->comptyp == 0) {
                c.wc[0] = sc[i].c3;
                c.wc[1] = sc[i].c2;
                c.wc[2] = sc[i].c1;
                c.wc[3] = 0x00;
            }
            if(ls->comptyp == 1) {
                c.wc[3] = sc[i].c3;
                c.wc[2] = sc[i].c2;
                c.wc[1] = sc[i].c1;
                c.wc[0] = 0x00;
            }
            if(ls->comptyp == 2) {
                c.wc[0] = sc[i].c3;
                c.wc[1] = sc[i].c2;
                c.wc[2] = sc[i].c1;
                c.wc[3] = 0x00;
            }
            fct[fc] = c.sync;
            fc++;
            if(fc >= MAXSC) {
                ls->errcnt++;
                ls->msglvl = 3;
                if(ls->msglvl >= ls->prtlvl)
                printf("Error! fc over than MAXSC %d\n",MAXSC);
                return(-1);
            }
        }
    }
    /* sort sync table (fct) */
    fcwk = 0;
    for(i = 0; i < fc-1; i++) {
        for(j = i+1; j < fc; j++) {
            if(fct[i] < fct[j]) {
                fcwk = fct[i];
                fct[i] = fct[j];
                fct[j] = fcwk;
            }
        }
    }
    /* out sc data to file */
    gk = 0;   /* gloval k */
    for(i = 0; i < cp; i++) {
        if(sc[i].c0 == 0xfc) break;
        if(sc[i].c0 == 0xfb) break;
        fputsc_mlk(i,fpo);
    }
    for(i = 0; i < fc; i++) {
        /* printf("fct[i]=%ld\n",fct[i]); */
        if(fct[i] != 0) outsctf_mlk(fpo,fct[i]);
    }
    fputsc_mlk(cp - 1,fpo);     /* last fb data */
    if(ls->debug == 2) printf("\n");
    spa = 0;
    spb = 0;
    return(0);
}

/* out sc data to file */
int outsctf_mlk(FILE *fpo,int isync)
//FILE *fpo;
//int  isync;
{
    int  i;
    int  j;

    c.sync = -1; /* add 98.1 */
    for(i = 0; i < cp; i++) {
        if(sc[i].c0 == 0xfc) {
            if(ls->comptyp == 0) {
                c.wc[0] = sc[i].c3;
                c.wc[1] = sc[i].c2;
                c.wc[2] = sc[i].c1;
                c.wc[3] = 0x00;
            }
            if(ls->comptyp == 1) {
                c.wc[3] = sc[i].c3;
                c.wc[2] = sc[i].c2;
                c.wc[1] = sc[i].c1;
                c.wc[0] = 0x00;
            }
            if(ls->comptyp == 2) {
                c.wc[0] = sc[i].c3;
                c.wc[1] = sc[i].c2;
                c.wc[2] = sc[i].c1;
                c.wc[3] = 0x00;
            }
        }
        /* printf("c.sync=%d isync=%d\n",c.sync,isync); */
        if(c.sync == isync) {
            fputsc_mlk(i,fpo);
            for(j = i+1; j < cp; j++) {
                if(sc[j].c0 == 0xfc) break;
                if(sc[j].c0 == 0xfb) break;
                fputsc_mlk(j,fpo);
            }
            return(0);
        }
    }
    ls->errcnt++;
    ls->msglvl = 3;
    if(ls->msglvl >= ls->prtlvl)
    printf("Error! isync not equal to sc table c.sync\n");
    return(-1);
}

int fputsc_mlk(int i,FILE *fpo)
//int  i;
//FILE *fpo;
{
    if(sc[i].c0 == 0xdd && sc[i+1].c0 == 0xdd) return(0);
    fputc(sc[i].c0,fpo);
    fputc(sc[i].c1,fpo);
    fputc(sc[i].c2,fpo);
    fputc(sc[i].c3,fpo);
    lv->occnt = lv->occnt + 4;
    if(ls->debug == 2) {
        if((gk % 8) == 0) printf("\nO ");
        printf("%02x%02x%02x%02x ",
            sc[i].c0, sc[i].c1, sc[i].c2, sc[i].c3);
    }
    gk++;
    return(0);
}

int bsynchk_mlk()
{
    int  i;

    b.sync = 0;
    for(i = 0; i < spb; i++) {
        if(sb[i].c0 == 0xfc) {
            if(ls->comptyp == 0) {
                b.wc[0] = sb[i].c3;
                b.wc[1] = sb[i].c2;
                b.wc[2] = sb[i].c1;
                b.wc[3] = 0x00;
            }
            if(ls->comptyp == 1) {
                b.wc[3] = sb[i].c3;
                b.wc[2] = sb[i].c2;
                b.wc[1] = sb[i].c1;
                b.wc[0] = 0x00;
            }
            if(ls->comptyp == 2) {
                b.wc[0] = sb[i].c3;
                b.wc[1] = sb[i].c2;
                b.wc[2] = sb[i].c1;
                b.wc[3] = 0x00;
            }
            if(a.sync == b.sync) {
                sb[i].c0 = 0x00;
                return(i);
            }
        }
    }
    return(-1);
}

/* merge sb tabe to sc table */
int baddtoc_mlk(int sp)
//int  sp;
{
    int  i;

    if(ddbsave_mlk()) return(0);  /* save ddbcode to sc table */
    for(i = sp+1 ; i < spb; i++) {
        if(sb[i].c0 == 0xfb) break;
        if(sb[i].c0 == 0xfc) break;
        if(sb[i].c0 == 0xdd) {
            lv->port = sb[i].c1;
            lv->trak = sb[i].c2;
            lv->memb = sb[i].c3;
        }
        if(sb[i].c0 != 0x00) {
            sc[cp].c0 = sb[i].c0;
            sc[cp].c1 = sb[i].c1;
            sc[cp].c2 = sb[i].c2;
            sc[cp].c3 = sb[i].c3;
            sb[i].c0 = 0x00;        /* clear flag  used at sortmovb_mlk() */
            cp++;
            if(cp >= MAXBC) {
                ls->errcnt++;
                ls->msglvl = 3;
                if(ls->msglvl >= ls->prtlvl)
                printf(
	        "Error! Too many data in meas at baddtoc(cp=%d). limit %d\n",
		    cp,MAXBC);
                return(-1);
            }
        }
    }
    return(0);
}

/*
int sortmova_mlk()
{
    sc[cp].c0 = sa[ap].c0;
    sc[cp].c1 = sa[ap].c1;
    sc[cp].c2 = sa[ap].c2;
    sc[cp].c3 = sa[ap].c3;
    ap++;
    cp++;
    if(ap >= spa) return(9);
    if(cp >= MAXBC) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! Too many data in meas at sortmova(cp=%d). limit %d\n",
	    cp,MAXBC);
        return(-1);
    }
    return(0);
}
*/

int sortmovb_mlk()
{
    int  i;

    if(ddbsave_mlk()) return(0);  /* save ddb code to sc table */
    for(i = 0 ; i < spb; i++ ) {
        if(sb[i].c0 == 0xdd) {
            lv->port = sb[i].c1;
            lv->trak = sb[i].c2;
            lv->memb = sb[i].c3;
        }
        if(sb[i].c0 != 0x00) {
            sc[cp].c0 = sb[i].c0;
            sc[cp].c1 = sb[i].c1;
            sc[cp].c2 = sb[i].c2;
            sc[cp].c3 = sb[i].c3;
            sb[i].c0 = 0x00;        /* clear flag  used at sortmovb_mlk() */
            cp++;
            if(cp >= MAXBC) {
                ls->errcnt++;
                ls->msglvl = 3;
                if(ls->msglvl >= ls->prtlvl)
                printf(
		"Error! Too many data in meas at sortmovb(cp=%d). limit %d\n",
		    cp,MAXBC);
                return(-1);
            }
        }
    }
    return(0);
}

/* save 0xdd aport atrak amemb code to sc table */
int ddasave_mlk()
{
    sc[cp].c0 = dda[0];
    sc[cp].c1 = dda[1];
    sc[cp].c2 = dda[2];
    sc[cp].c3 = dda[3];
    cp++;
    if(cp >= MAXBC) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! Too many data in meas at ddasave(cp=%d). limit %d\n",
	    cp,MAXBC);
        return(-1);
    }
    return(0);
}

int ddbsave_mlk()
{
    sc[cp].c0 = 0xdd;
    sc[cp].c1 = (char)lv->port;
    sc[cp].c2 = (char)lv->trak;
    sc[cp].c3 = (char)lv->memb;
    cp++;
    if(cp >= MAXBC) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! Too many data in meas at ddbsave(cp=%d). limit %d\n",
	    cp,MAXBC);
        return(-1);
    }
    return(0);
}
