
// ----------------------
//  AMuPLC (mlk4.c)
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
extern MLKFRS *lr;
extern MLKCDE *lc;
extern MECMST *mm;
char dgstr6[] = "             c.p.g : ";
char dgstr7[12];
extern int  fjtb[128];    /* flame jump counter */
extern char dgstr3[];
/*
 Make program change & pitch vend code [20 < 80 _/]
    int  ftype[MAXPT];         phrase type
    int  fchan[MAXPT][4];      phrase channel
    int  fsval[MAXPT][4];      phrase start value
    int  fctno[MAXPT][4];      phrase control number
    int  feval[MAXPT][4];      phrase end value
    long fnval[MAXPT][4];      phrase note value
    int  fincv[MAXPT][4];      phrase increment value
    int  fintv[MAXPT][4];      phrase timecode interval
    int  fcval[MAXPT][4];      phrase current value
    int  fctin[MAXPT][4];      phrase current timer interval
    int  fudfg[MAXPT][4];      phrase up down flag
    int  factf[MAXPT][4];      phrase activ on/off flag
*/
int phrasing_mlk(int p,int f,FILE *fpo)
//int  p;
//int  f;
//FILE *fpo;
{
    int  fctn;

    fctn = lr->fctno[p][f];
    if(lr->fudfg[p][f] == 0 && lr->factf[p][f]) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl) printf("Error! at frasing_mlk()\n");
        return(0);
    }
    if(lr->factf[p][f] == 1) {
        lr->fcval[p][f] = lr->fsval[p][f];  /* set start val */
        lr->factf[p][f] = 2;
    }
    else lr->factf[p][f] = 2;
    if(lr->fudfg[p][f] == 1) {  /* value up phrase */
        if(lr->fintv[p][f] == 1) {
            flasout_mlk(p,f,fpo);
            lr->fcval[p][f] += lr->fincv[p][f];
        }
        else {
            if(!(lr->fctin[p][f] % lr->fintv[p][f])) {
                flasout_mlk(p,f,fpo);
                lr->fcval[p][f] += lr->fincv[p][f];
                lr->fctin[p][f] = 0;
            }
            lr->fctin[p][f]++;
        }
        if(lr->fcval[p][f] >= lr->feval[p][f]) {
            lr->factf[p][f] = 0;        /* act off */
            lr->fctin[p][f] = 0;     /* clear current interval counter */
            if(fctn > 101) fctn = 0;   /* pitch vend (150) */
            fjtb[fctn] = 0;        /* clear flame jump counter */
        }
    }
    if(lr->fudfg[p][f] == 2) {  /* value down phrase */
        if(lr->fintv[p][f] == 1) {
            flasout_mlk(p,f,fpo);
            lr->fcval[p][f] -= lr->fincv[p][f];
        }
        else {
            if(!(lr->fctin[p][f] % lr->fintv[p][f])) {
                flasout_mlk(p,f,fpo);
                lr->fcval[p][f] -= lr->fincv[p][f];
                lr->fctin[p][f] = 0;
            }
            lr->fctin[p][f]++;
        }
        if(lr->fcval[p][f] <= lr->feval[p][f]) {
            lr->factf[p][f] = 0;        /* act off */
            lr->fctin[p][f] = 0;     /* clear current interval counter */
            if(fctn > 101) fctn = 0;   /* pitch vend (150) */
            fjtb[fctn] = 0;        /* clear flame jump counter */
        }
    }
    return(0);
}

/* frasing code out */
/* Controller numbers table
   MODUR 1          modulation depth
   POLTM 5          poltament time
   DENTR 6          data entry (RPN/NRPN)
   VOLUM 7          main volume
   BALAN 8          balance control
   PANPT 10         panpot
   EXPRS 11         expression
   DUMPR 64         hold 1 (dumper)
   POLTA 65         poltament
   SOSTN 66         sosutenute (chord hold)
   SOFTP 67         soft pedal
   HOLD2 69         hold2 (flease)
   EXTEF 91         external efect depth
   TREMO 92         toremolo depth
   CHORS 93         chorus depth
   SELES 94         seleste depth
   FETHR 95         feather depth
*/
int flasout_mlk(int p,int f,FILE *fpo)
//int  p;   /* port */
//int  f;   /* flase */
//FILE *fpo;
{
    int  fctn;

    dgstr7[0] = '\0';
    fctn = lr->fctno[p][f];
    /* printf("lr->fcval[%d][%d]=%d\n",p,f,lr->fcval[p][f]); */
    switch (fctn) {
        case MODUR : strcpy(dgstr7,"modulation");
                     if(!(fjtb[fctn] % R_MODUR)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case POLTM : strcpy(dgstr7,"poltament");
                     if(!(fjtb[fctn] % R_POLTM)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case DENTR : strcpy(dgstr7,"data_entry");
                     if(!(fjtb[fctn] % R_DENTR)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case VOLUM : strcpy(dgstr7,"main_vol.");
                     if(!(fjtb[fctn] % R_VOLUM)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case BALAN : strcpy(dgstr7,"balance");
                     if(!(fjtb[fctn] % R_BALAN)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case PANPT : strcpy(dgstr7,"panpot");
                     if(!(fjtb[fctn] % R_PANPT)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case EXPRS : strcpy(dgstr7,"expression");
                     if(!(fjtb[fctn] % R_EXPRS)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case DUMPR : strcpy(dgstr7,"dumper");
                     if(!(fjtb[fctn] % R_DUMPR)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case POLTA : strcpy(dgstr7,"poltament");
                     if(!(fjtb[fctn] % R_POLTA)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case SOSTN : strcpy(dgstr7,"sosutenute");
                     if(!(fjtb[fctn] % R_SOSTN)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case SOFTP : strcpy(dgstr7,"soft_pedal");
                     if(!(fjtb[fctn] % R_SOFTP)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case HOLD2 : strcpy(dgstr7,"flease");
                     if(!(fjtb[fctn] % R_HOLD2)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case EXTEF : strcpy(dgstr7,"ext.ef.dep");
                     if(!(fjtb[fctn] % R_EXTEF)) flcout_mlk(p,f,fpo);
                     /* external efect depth */
                     fjtb[fctn]++;
                     break;
        case TREMO : strcpy(dgstr7,"toremolo");
                     if(!(fjtb[fctn] % R_TREMO)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case CHORS : strcpy(dgstr7,"chorus");
                     if(!(fjtb[fctn] % R_CHORS)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case SELES : strcpy(dgstr7,"seleste");
                     if(!(fjtb[fctn] % R_SELES)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case FETHR : strcpy(dgstr7,"feather");
                     if(!(fjtb[fctn] % R_FETHR)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
        case PITCH : if(!(fjtb[0] % R_PITCH)) pitch_mlk(p,f,fpo);
                     /* pitch vend */
                     fjtb[0]++;
                     break;
        default    : if(fctn > 102) {
                         ls->errcnt++;
                         ls->msglvl = 3;
                         if(ls->msglvl >= ls->prtlvl)
                         printf("Error! control no. %d\n",fctn);
                         break;
                     }
                     if(!(fjtb[fctn] % 4)) flcout_mlk(p,f,fpo);
                     fjtb[fctn]++;
                     break;
    }
    return(0);
}

int flcout_mlk(int p,int f,FILE *fpo)
//int  p;
//int  f;
//FILE *fpo;
{
    int  bit14;
    int  bx;
    int  cno;
    int  msb;
    int  lsb;
    static int wmsb = 0;
    int  port;

    bit14 = 0;
    if((lr->fsval[p][f] > 127) || (lr->feval[p][f] > 127)) bit14 = 1;
    fputc(0xe5,fpo);   /*  for add port data (back) */
    port = 0x0080 | p; /*  flag set */
    fputc((int)port,fpo);
    lv->occnt = lv->occnt + 2;
    if(ls->otrace == 1) printf("%sE5 %02x / current port=%d\n",
       dgstr6,port,p);
    if(bit14) {   /* 14 bit code */
        bx = 0x0f & lr->fchan[p][f];
        bx |= 0x00b0;
        msb = lr->fcval[p][f] / 128;  /* MSB */
        cno = 0x7f & lr->fctno[p][f];
        if(msb != wmsb) {
            wmsb = msb;
            fputc((char)bx,fpo);   /* Bx */
            fputc((char)cno,fpo);  /* control # */
            fputc((char)msb,fpo);
            lv->occnt = lv->occnt + 3;
        }
        fputc((char)bx,fpo);
        fputc((char)(cno+32),fpo);
        lsb = lr->fcval[p][f] - (msb * 128);  /* LSB */
        fputc((char)lsb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 1) {
            if(msb != wmsb) {
                printf("%s%02x %02x %02x ",dgstr6,bx,cno,msb);
                printf("/ msb=%d %s\n",msb,dgstr7);
                printf("%s%02x %02x %02x ",dgstr6,bx,cno+32,lsb);
                printf("/ fcval[%d][%d]=%d %s\n",p,f,lr->fcval[p][f],dgstr7);
            }
            else {
                printf("%s%02x %02x %02x ",dgstr6,bx,cno+32,lsb);
                printf("/ fcval[%d][%d]=%d %s\n",p,f,lr->fcval[p][f],dgstr7);
            }
        }
    }
    else {
        bx = 0x0f & lr->fchan[p][f];
        bx |= 0x00b0;
        fputc((char)bx,fpo); /* Bx */
        cno = 0x7f & lr->fctno[p][f];
        fputc((char)cno,fpo);  /* control # */
        msb = 0x007f & lr->fcval[p][f]; /* MSB */
        fputc((char)msb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 1) {
            printf("%s%02x ",dgstr6,bx);
            printf("%02x %02x ",cno,msb);
            printf("/ fcval[%d][%d]=%d %s\n",p,f,lr->fcval[p][f],dgstr7);
        }
    }
    if(lv->port != p) {
        fputc(0xe5,fpo);   /*  for add port data (back) */
        port = 0x0080 | lv->port; /*  flag set */
        fputc((int)port,fpo);
        lv->occnt = lv->occnt + 2;
        if(ls->otrace == 1) printf("%sE5 %02x / current port=%d\n",
           dgstr6,port,lv->port);
    }
    return(0);
}

int pitch_mlk(int p,int f,FILE *fpo)     /* pitch vend */
//int  p;
//int  f;
//FILE *fpo;
{
    int  ex;
    int  msb;
    int  lsb;
    int  port;

    fputc(0xe5,fpo);   /*  for add port data (back) */
    port = 0x0080 | p; /*  flag set */
    fputc((int)port,fpo);
    lv->occnt = lv->occnt + 2;
    if(ls->otrace == 1) printf("%sE5 %02x / current port=%d\n",
       dgstr6,port,p);
    ex = 0x0f & lr->fchan[p][f];
    ex |= 0x00e0;
    msb = lr->fcval[p][f] / 128;  /* MSB */
    fputc((char)ex,fpo);   /* Ex */
    fputc((char)msb,fpo);
    lsb = lr->fcval[p][f] - (msb * 128);  /* LSB */
    fputc((char)lsb,fpo);
    lv->occnt = lv->occnt + 3;
    if(ls->otrace == 1) {
        printf("%s%02x %02x %02x ",dgstr6,ex,msb,lsb);
        printf("/ fcval[%d][%d]=%d pitch_vend\n",p,f,lr->fcval[p][f]);
    }
    if(lv->port != p) {
        fputc(0xe5,fpo);   /*  for add port data (back) */
        port = 0x0080 | lv->port; /*  flag set */
        fputc((int)port,fpo);
        lv->occnt = lv->occnt + 2;
        if(ls->otrace == 1) printf("%sE5 %02x / current port=%d\n",
           dgstr6,port,lv->port);
    }
    return(0);
}

int fmerge0_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];

    if(ls->debug) printf("fmerge0_() dfile=%s portnb=%d\n",dfile,portnb);
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

int fmerge1_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge1_() dfile=%s portnb=%d\n",dfile,portnb);
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
    if(lv->mmemb == 0) midfadd_mlk(dfile,afile,bfile,portnb);
    else midfmix_mlk(dfile,afile,bfile,portnb);
    return(0);
}

int fmerge2_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge2_() dfile=%s portnb=%d\n",dfile,portnb);
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
    if(lv->mmemb == 0) midfadd_mlk(dfile,bfile,afile,portnb);
    else midfmix_mlk(dfile,bfile,afile,portnb);
    return(0);
}

int fmerge3_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char afile[STLN];          /* work file a */
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge3_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    ofile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(ofile,".mix");
    if(lv->mmemb == 0) midfadd_mlk(dfile,afile,ofile,portnb);
    else midfmix_mlk(dfile,afile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

int fmerge4_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char bfile[STLN];          /* work file a */
    char ofile[STLN];          /* exe file */

    if(ls->debug) printf("fmerge4_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
    i = 0;
    while(dfile[i] != '_') {
        bfile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    bfile[i] = '\0';
    ofile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    strcat(ofile,".mix");
    if(lv->mmemb == 0) midfadd_mlk(dfile,bfile,ofile,portnb);
    else midfmix_mlk(dfile,bfile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

int fmerge5_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge5_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
    i = 0;
    while(dfile[i] != '_') {
        ofile[i] = dfile[i];
        i++;
    }
    ofile[i] = '\0';
    strcat(ofile,".mix");
    midfconv_mlk(dfile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

int fmerge6_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char ofile[STLN];
    char afile[STLN];

    if(ls->debug) printf("fmerge6_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        ofile[i] = dfile[i];
        afile[i] = dfile[i];
        i++;
    }
    ofile[i] = '\0';
    afile[i] = '\0';
    strcat(ofile,".mix");
    strcat(afile,"_wka.mlk");
    remove(afile);
    rename(ofile,afile);
    remove(ofile);
    midfmix_mlk(dfile,afile,ofile,portnb);
    exsmpte_mlk(ofile,portnb);
    if(ls->exef) mkexecf_mlk(ofile);
    return(0);
}

int fmerge10_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];
    char bfile[STLN];
    char xfile[STLN];         /* port0 mix file */

    if(ls->debug) printf("fmerge10_() dfile=%s portnb=%d\n",dfile,portnb);
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
    strcat(xfile,".mix");
    incmstf_mlk(xfile,bfile,portnb); /* include master file to cfile */
    midfmix_mlk(dfile,bfile,afile,portnb);
    return(0);
}

int fmerge11_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge11_() dfile=%s portnb=%d\n",dfile,portnb);
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

int fmerge12_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char afile[STLN];          /* work file a */
    char bfile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge12_() dfile=%s portnb=%d\n",dfile,portnb);
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

int fmerge13_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char afile[STLN];          /* work file a */
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge13_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
    i = 0;
    while(dfile[i] != '_') {
        afile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    afile[i] = '\0';
    ofile[i] = '\0';
    strcat(afile,"_wka.mlk");
    strcat(ofile,".mix");
    midfmix_mlk(dfile,afile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

int fmerge14_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char bfile[STLN];          /* work file a */
    char ofile[STLN];          /* exe file */

    if(ls->debug) printf("fmerge14_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
    i = 0;
    while(dfile[i] != '_') {
        bfile[i] = dfile[i];
        ofile[i] = dfile[i];
        i++;
    }
    bfile[i] = '\0';
    ofile[i] = '\0';
    strcat(bfile,"_wkb.mlk");
    strcat(ofile,".mix");
    midfmix_mlk(dfile,bfile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

int fmerge15_mlk(char dfile[STLN],int portnb,int eop)
//char dfile[STLN];
//int  portnb;
//int  eop;    /* end port */
{
    int  i;
    char afile[STLN];
    char xfile[STLN];
    char ofile[STLN];          /* work file b */

    if(ls->debug) printf("fmerge15_() dfile=%s portnb=%d eop=%d\n",
        dfile,portnb,eop);
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
    strcat(xfile,".mix");
    ofile[i] = '\0';
    strcat(ofile,".mix");
    incmstf_mlk(xfile,afile,portnb); /* include master file to afile */
    midfmix_mlk(dfile,afile,ofile,portnb);
    if(eop) {
        exsmpte_mlk(ofile,portnb);
        if(ls->exef) mkexecf_mlk(ofile);
    }
    return(0);
}

/* fn_xxx.mex file read and skip line for line select option */
int mlkfrd1_mlk(FILE *fpo,FILE *fpw)
//FILE *fpo;
//FILE *fpw;
{
    unsigned char iword[4];
    unsigned char tword[4];
    int  c;
    int  c1;

    mm->movn = 0;
    mm->bnum = 0;
    while((c = fgetc(fpw)) != EOF) {
        switch (c) {
            case  0xf1 : iword[0] = c;
                         iword[1] = fgetc(fpw);
                         f1dec_mlk(iword);
                         fputc(iword[0],fpo);
                         fputc(iword[1],fpo);
                         lv->occnt = lv->occnt + 2;
                         break;
            case  0xfb : iword[0] = c;           /* fb code check */
                         iword[1] = fgetc(fpw);
                         iword[2] = fgetc(fpw);
                         iword[3] = fgetc(fpw);
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc(iword[0],fpo);
                             fputc(iword[1],fpo);
                             fputc(iword[2],fpo);
                             fputc(iword[3],fpo);
                             lv->occnt = lv->occnt + 4;
                         }
                         else {
                             tword[0] = iword[0];
                             tword[1] = iword[1];
                             tword[2] = iword[2];
                             tword[3] = iword[3];
                             tword[0] &= 0x00;
                             tword[1] &= 0x7f;
                             ctoval_mlk(tword); /* set code to value */
                             fbdec_mlk();
                             if(topchk_mlk(fpo,iword)) {
                                 if(ls->debug == 7)
                                     printf("lv->occnt=%ld\n",lv->occnt);
                                 setqfsv_mlk();
                                 return(0);
                             }
                         }
                         if(ls->debug == 7)
                             printf("lv->occnt=%ld\n",lv->occnt);
                         break;
            default    : fputc(c,fpo);
                         lv->occnt++;
                         break;
        }
    }
    if(ls->debug == 7) printf("lv->occnt=%ld\n",lv->occnt);
    return(0);
}

/* fn_xxx.mex file read and skip line for line select option */
int mlkfrd2_mlk(FILE *fpo,FILE *fpw)
//FILE *fpo;
//FILE *fpw;
{
    unsigned char iword[4];
    unsigned char tword[4];
    int  c;
    int  pf;
    int  c1;

    mm->movn = 0;
    mm->bnum = 0;
    pf = 0;
    while((c = fgetc(fpw)) != EOF) {
        switch (c) {
            case  0xf1 : iword[0] = c;
                         iword[1] = fgetc(fpw);
                         f1dec_mlk(iword);
                         if(pf) {
                             fputc(iword[0],fpo);
                             fputc(iword[1],fpo);
                             lv->occnt = lv->occnt + 2;
                         }
                         break;
            case  0xfb : iword[0] = c;           /* fb code check */
                         iword[1] = fgetc(fpw);
                         iword[2] = fgetc(fpw);
                         iword[3] = fgetc(fpw);
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             if(pf) {
                                 fputc(iword[0],fpo);
                                 fputc(iword[1],fpo);
                                 fputc(iword[2],fpo);
                                 fputc(iword[3],fpo);
                                 lv->occnt = lv->occnt + 4;
                             }
                         }
                         else {
                             tword[0] = iword[0];
                             tword[1] = iword[1];
                             tword[2] = iword[2];
                             tword[3] = iword[3];
                             tword[0] &= 0x00;
                             tword[1] &= 0x7f;
                             ctoval_mlk(tword); /* set code to value */
                             fbdec_mlk();
                             btmchk_mlk(fpo,iword,&pf);
                         }
                         if(ls->debug == 7)
                             printf("lv->occnt=%ld\n",lv->occnt);
                         break;
            default    : if(pf) {
                             fputc(c,fpo);
                             lv->occnt++;
                         }
                         break;
        }
    }
    if(ls->debug == 7) printf("lv->occnt=%ld\n",lv->occnt);
    return(0);
}

/* get .mlk object bar number check & code out */
int topchk_mlk(FILE *fpo,unsigned char iword[4])
//FILE *fpo;
//unsigned char iword[4];
{
    long cbn;
    long sbn;

    switch(mm->brself) {
        case  1 : if(mm->movn >= mm->smovn) return(9);
                  break;
        case  2 : cbn = mm->bnum;
                  sbn = mm->sbarn;
                  if(cbn >= sbn) return(9);
                  break;
        case  3 : cbn = mm->movn * 100000 + mm->bnum;
                  sbn = mm->smovn * 100000 + mm->sbarn;
                  if(cbn >= sbn) return(9);
                  /* if(mm->movn > mm->smovn || cbn >= sbn) return(9); */
                  break;
        default : break;
    }
    fputc(iword[0],fpo);
    fputc(iword[1],fpo);
    fputc(iword[2],fpo);
    fputc(iword[3],fpo);
    lv->occnt = lv->occnt + 4;
    return(0);
}

/* .mlk object bottom bar number check & code out */
int btmchk_mlk(FILE *fpo,unsigned char iword[4],int *pf)
//FILE *fpo;
//unsigned char iword[4];
//int  *pf;
{   /* bottom */
    long cbn;
    long ebn;

    if(*pf == 0) {
        switch(mm->brself) {
            case  1 : if(mm->movn <= mm->emovn) *pf = 1;
                      break;
            case  2 : cbn = mm->bnum;
                      ebn = mm->ebarn;
                      if(cbn >= ebn) *pf = 1;
                      break;
            case  3 : cbn = mm->movn * 100000 + mm->bnum;
                      ebn = mm->emovn * 100000 + mm->ebarn;
                      if(cbn >= ebn) *pf = 1;
                      break;
            default : break;
        }
    }
    if(*pf) {
        fputc(iword[0],fpo);
        fputc(iword[1],fpo);
        fputc(iword[2],fpo);
        fputc(iword[3],fpo);
        lv->occnt = lv->occnt + 4;
    }
    return(0);
}

int mbselck_mlk()
{
    long  cbn;
    long  sbn;
    long  ebn;

    if(mm->byrd == 2) return(0);
    switch(mm->brself) {
        case  1 : if(lv->movn > mm->emovn) mm->byrd = 2;
                  else if(lv->movn >= mm->smovn) mm->byrd = 1;
                  break;
        case  2 : cbn = lv->bnum;   /* for compiler bug ? */
                  sbn = mm->sbarn;
                  ebn = mm->ebarn;
                  if(cbn >= ebn) mm->byrd = 2;
                  else if(cbn >= sbn) mm->byrd = 1;
                  break;
        case  3 : cbn = lv->movn * 100000 + lv->bnum;
                  sbn = mm->smovn * 100000 + mm->sbarn;
                  ebn = mm->emovn * 100000 + mm->ebarn;
                  if(cbn >= ebn) mm->byrd = 2;
                  else if(cbn >= sbn) mm->byrd = 1;
                  break;
        default : break;
    }
    if(ls->debug == 7 && mm->brself) {
        printf("mm->brself=%d cbn=%ld sbn=%ld,ebn=%ld ",
            mm->brself,cbn,sbn,ebn);
        printf("mm->byrd=%d\n",mm->byrd);
    }
    return(0);
}
/* 
MIDI time code decode
SMPTE quoater flame messages
 f1 (message) : System Common status byte
    (message) = 0nnndddd
 f1 0x : (0000 yyyy) flame LS xxx    : undefined
    1x : (0001 xxxy) flame MS yyyyy  : flame no. (0-29)
    2x : (0010 yyyy) sec LS   xx     : undefined
    3x : (0011 xxyy) sec MS   yyyyyy : sec count (0-59)
    4x : (0100 yyyy) min LS   xx     : undefined
    5x : (0101 xxyy) min MS   yyyyyy : min count (0-59)
    6x : (0110 zzzz) hour LS  x      : undefined
    7x : (0111 xyyz)          yy     : timecode type
                                       0 - 24 flame /sec
                                       1 - 25 flame /sec
                                       2 - 30 flame(drop)/sec
                                       3 - 30 flame(non drop)/sec
                              zzzzz  : hour (0-23)
*/
int f1dec_mlk(unsigned char iword[4])
//unsigned char iword[4];
{
    int  k;

    /* if(ls->otrace == 1) printf("%sF1 %02x\n",dgstr3,iword[1]); */
    k = iword[1] & 0xf0;
    k = k >> 4;
    switch(k) {
        case  0 : mm->f1 = iword[1] & 0x0f;
                  break;
        case  1 : mm->f2 = iword[1] & 0x01;
                  break;
        case  2 : mm->s1 = iword[1] & 0x0f;
                  break;
        case  3 : mm->s2 = iword[1] & 0x03;
                  break;
        case  4 : mm->m1 = iword[1] & 0x0f;
                  break;
        case  5 : mm->m2 = iword[1] & 0x03;
                  break;
        case  6 : mm->h1 = iword[1] & 0x0f;
                  break;
        case  7 : mm->h2 = iword[1] & 0x01;
                  mm->tcbit = iword[1] & 0x06;
                  mm->tcbit = mm->tcbit >> 1;
                  mm->f = mm->f2 * 16 + mm->f1;
                  mm->s = mm->s2 * 16 + mm->s1;
                  mm->m = mm->m2 * 16 + mm->m1;
                  mm->h = mm->h2 * 16 + mm->h1;
                  if(ls->debug == 7) {
                      printf("%sSmpte %02d:%02d:%02d/%02d timecode type=%d\n",
                          "             ",mm->h,mm->m,mm->s,mm->f,mm->tcbit);
                  }
                  break;
        default : break;
    }
    mm->flc = k + 1;
    return(0);
}

/* set quoater flame start value */
int setqfsv_mlk()
{
    lv->tctyp = mm->tcbit;
    if(lv->tctyp == 0) lv->sflame = 24;
    if(lv->tctyp == 1) lv->sflame = 25;
    if(lv->tctyp == 2) lv->sflame = 30;
    if(lv->tctyp == 3) lv->sflame = 30;
    mm->smpte = mm->h * 3600 + mm->m * 60 + mm->s;
    mm->smpte = mm->smpte * lv->sflame;
    mm->smpte = (mm->smpte + mm->f) * 100;
    mm->smpte = mm->smpte + (25 * (8 + mm->flc)); /* add quauter flame cnt */
    compsmp1_mlk(mm->smpte);
    mm->fflame = 9999;
    return(0);
}


/*   FB code ....................................
Bar          : FBxxyyyy
 xx(movn)    : movement number
 yyyy(bnum)  : bar number
 ex. -m0100017-_____________..
         xx(movn) = 1
         yyyy(bnum) = 17
*/
int fbdec_mlk()
{
    mm->movn = lc->i[0];
    mm->bnum = lc->i[1];
    if(ls->debug == 7) {
        printf("FB(%02x%02x%02x%02x) Bar data / movn=%d bnum=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],mm->movn,mm->bnum);
    }
    return(0);
}

/* include */
int finclude_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char mixf[STLN];
    char afile[STLN];

    if(ls->debug) printf("finclude_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        mixf[i] = dfile[i];
        afile[i] = dfile[i];
        i++;
    }
    mixf[i] ='\0';
    afile[i] = '\0';
    strcat(mixf,".mix");
    strcat(afile,"_wka.mlk");
    /*
    printf("dfile=%s portnb=%d mixf=%s afile=afile\n",
        dfile,portnb,mixf,afile);
    */
    remove(afile);
    rename(mixf,afile);
    remove(mixf);
    midfmix_mlk(dfile,afile,mixf,portnb);
    exsmpte_mlk(mixf,portnb);
    if(ls->exef) mkexecf_mlk(mixf);
    return(0);
}

/* exclude */
int fexclude_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char mixf[STLN];
    char afile[STLN];

    if(ls->debug) printf("fexclude_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        mixf[i] = dfile[i];
        afile[i] = dfile[i];
        i++;
    }
    mixf[i] ='\0';
    afile[i] = '\0';
    strcat(mixf,".mix");
    strcat(afile,"_wka.mlk");
    /*
    printf("dfile=%s portnb=%d mixf=%s afile=afile\n",
        dfile,portnb,mixf,afile);
    */
    remove(afile);
    rename(mixf,afile);
    remove(mixf);
    mlkexcl_mlk(afile,dfile,mixf,portnb); /* exclude dfile from afile */
    exsmpte_mlk(mixf,portnb);
    if(ls->exef) mkexecf_mlk(mixf);
    return(0);
}

/* replace */
int freplace_mlk(char dfile[STLN],int portnb)
//char dfile[STLN];
//int  portnb;
{
    int  i;
    char mixf[STLN];
    char afile[STLN];

    if(ls->debug) printf("freplace_() dfile=%s portnb=%d\n",dfile,portnb);
    i = 0;
    while(dfile[i] != '_') {
        mixf[i] = dfile[i];
        afile[i] = dfile[i];
        i++;
    }
    mixf[i] ='\0';
    afile[i] = '\0';
    strcat(mixf,".mix");
    strcat(afile,"_wka.mlk");
    /*
    printf("dfile=%s portnb=%d mixf=%s afile=afile\n",
        dfile,portnb,mixf,afile);
    */
    remove(afile);
    rename(mixf,afile);
    remove(mixf);
    mlkexcl_mlk(afile,dfile,mixf,portnb);
    remove(afile);
    rename(mixf,afile);
    remove(mixf);
    midfmix_mlk(dfile,afile,mixf,portnb);
    exsmpte_mlk(mixf,portnb);
    if(ls->exef) mkexecf_mlk(mixf);
    return(0);
}

/* Mlkexf exclude afile from dfile & out to ofile */
int mlkexcl_mlk(char afile[STLN],char dfile[STLN],char ofile[STLN],int portnb)
//char afile[STLN];
//char dfile[STLN];
//char ofile[STLN];
//int  portnb;
{
    FILE *fpi,*fpo;
    unsigned char iword[4];
    int  i,j,k;
    long lword;
    int  c;
    int  dport;
    int  dtrak;
    int  dmemb;
    int  putfg;

    if(ls->debug)
        printf("mlkexcl_() afile=%s dfile=%s ofile=%s portnb=%d\n",
        afile,dfile,ofile,portnb);
    i = 0;
    putfg = 1;
    while(dfile[i] != '_') i++;
    iword[0] = dfile[++i];
    iword[1] = '\0';
    sscanf((char *)iword,"%d",&dtrak);  // 2019.3.30
    iword[0] = dfile[++i];
    iword[1] = dfile[++i];
    iword[2] = '\0';
    sscanf((char *)iword,"%x",&dmemb);  // 2019.3.30
    dport = portnb & 0x7f;
    /* printf("dport=%d dtrak=%d dmemb=%d\n",dport,dtrak,dmemb); */
    fpi = fopen_h(afile,"rb");
    fpo = fopen_h(ofile,"wb");
    k = 0;
    j = 0;
    lv->occnt = 0;
    lword = 0;
    if(ls->trace) ls->trace = 1;
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
                                if(putfg) fputcd_mlk(iword,fpo);
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
                                if(putfg) fputcd_mlk(iword,fpo);
                            }
                            break;
                case 0xf8 : f8decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xfa : fadecode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xdd : dddecode_mlk();
                            putfg = 1;
                            if(lv->port == dport &&
                               lv->trak == dtrak &&
                               lv->memb == dmemb) putfg = 0;
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xe1 : e1decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xe2 : e2decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xe3 : e3decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xe4 : e4decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
                            break;
                case 0xe6 : e6decode_mlk();
                            if(putfg) fputcd_mlk(iword,fpo);
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
        printf("Mlkexf p[%d] %s exclude(%s) -> ",portnb,afile,dfile);
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    return(0);
}

/* optimize fn.mex file */
int optimize_mlk(char inf[STLN])
//char inf[STLN];
{
    FILE *fpi,*fpo;
    char outf[STLN];
    char iword[4];
    int  c;
    int  c1;
    int  ckport;
    int  port;
    int  cktrak;
    int  trak;
    int  ckmemb;
    int  memb;
    long icnt,ocnt;

    ckport = 99;  /* dummy */
    cktrak = 999;
    ckmemb = 999;
    icnt = 0;
    ocnt = 0;
    strcpy(outf,inf);
    strcat(inf,".mex");
    strcat(outf,".tmp");
    fpi = fopen_h(inf,"rb");
    fpo = fopen_h(outf,"wb");
    while(1) {
        if((c = fgetc(fpi)) == EOF) break;
        icnt++;
        switch (c) {
            case  0xe5 : iword[0] = c;           /* fd code check */
                         iword[1] = fgetc(fpi);
                         icnt++;
                         c1 = iword[1] & 0x80;
                         if(c1) {
                             port = iword[1] & 0x7f;
                             if(port == ckport) break;
                             ckport = iword[1] & 0x7f;
                         }
                         fputc(iword[0],fpo);
                         fputc(iword[1],fpo);
                         ocnt += 2;
                         break;
            case  0xfd : if(ls->optm < 2) {
                             fputc(c,fpo);
                             ocnt++;
                             break;
                         }
                         iword[0] = c;           /* fd code check */
                         iword[1] = fgetc(fpi);
                         iword[2] = fgetc(fpi);
                         iword[3] = fgetc(fpi);
                         icnt += 3;
                         c1 = iword[1] & 0x80;
                         if(c1) {
                             port = iword[1] & 0x7f;
                             trak = (int)iword[2];
                             memb = (int)iword[3];
                             if( port == ckport &&
                                 trak == cktrak &&
                                 memb == ckmemb) break;
                             ckport = iword[1] & 0x7f;
                             cktrak = (int)iword[2];
                             ckmemb = (int)iword[3];
                         }
                         fputc(iword[0],fpo);
                         fputc(iword[1],fpo);
                         fputc(iword[2],fpo);
                         fputc(iword[3],fpo);
                         ocnt += 4;
                         break;
            default    : fputc(c,fpo);
                         ocnt++;
                         break;
        }
    }
    fclose(fpi);
    fclose(fpo);
    remove(inf);
    rename(outf,inf);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlkopt %s -> %s %ld Bytes\n",inf,outf,ocnt);
    }
    return(0);
}
