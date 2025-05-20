
// ----------------------
//  AMuPLC (mlk3.c)
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
extern int slsf;
SYNSORT sa[MAXAB];
SYNSORT sb[MAXAB];
SYNSORT sc[MAXBC];
extern int  spa;      /* sa table point */
extern int  spb;      /* sb table point */
extern int  spc;      /* sc table point */
long pxsyn;           /* pre xsync */
long pysyn;           /* pre ysync */
long pxflam;          /* pre xflame */
long pyflam;          /* pre yflame */
long xsync;
long ysync;
long xflame;
long yflame;
long plag;
long pds;
long pdsc;
long dsc;
long lag;
long ds;
char psxyflg;
char pfxyflg;
char sxyflg;
char fxyflg;
char abflg;
char dgstr1[] = "             m.m.c : ";
char dgstr2[] = "             s.f.m : ";
char dgstr3[] = "             q.f.m : ";
char dgstr4[] = "             p.t.m : ";
char dgstr5[] = "             b.n.c : ";  /* bar number & output count */
int  ckport;
int  cktrak;
int  ckmemb;
int  fjtb[128];    /* flame jump counter */

/* Mlksmp */
int exsmpte_mlk(char ifile[STLN],int portnb)
//char ifile[STLN];
//int  portnb;
{
    char ofile[STLN];          /* exe file */
    FILE *fpi,*fpo;
    unsigned char iword[4];
    unsigned char f8word[4];
    unsigned char faword[4];
    int  i,j,k;
    long lword;
    int  c;
    int  fflg;
    int  wtracef;

    if(ls->debug) printf("exsmpte_() ifile = %s portnb=%d\n",ifile,portnb);
    i = 0;
    while(ifile[i] != '.') {
        ofile[i] = ifile[i];
        i++;
    }
    ofile[i] = '\0';
    strcat(ofile,".mlk");
    fpi = fopen_h(ifile,"rb");
    fpo = fopen_h(ofile,"wb");
    k = 0;
    j = 0;
    ckport = 0x0f;
    cktrak = 0xff;
    ckmemb = 0xff;
    lv->occnt = 0;
    lv->smpte = 0;
    lv->sync = 0;
    lword = 0;
    pxflam = 0;
    pyflam = 0;
    pxsyn = 0;
    pysyn = 0;
    xflame = 0;
    yflame = 0;
    xsync = 0;
    ysync = 0;
    pdsc = 0;
    spa = 0;
    spb = 0;
    spc = 0;
    abflg = 'a';
    psxyflg = 'x';
    pfxyflg = 'x';
    sxyflg = 'x';
    fxyflg = 'x';
    wtracef = ls->trace;
    if(ls->trace) ls->trace = 1;
    if(ls->outfm == 0) lv->mlp = 1; /* old.mex  Ignore -fres -div option */
    if(ls->outfm == 2) lv->mlp = 1; /* smf0.mid Ignore -fres -div option */
    if(ls->outfm == 3) lv->mlp = 1; /* smf1.mid Ignore -fres -div option */
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set iword[4] code to value */
            k = 0;
            switch (iword[0]) {
                case 0xf1 : /* f1decode_mlk(); */
                            fflg = lc->c[1] & 0x80;
                            if(abflg == 'a') {
                                savema_mlk(iword);
                                if(fflg) {
                                    abflg = 'b';
                                    spachk_mlk(fpo);
                                    spa = 0;
                                }
                            }
                            else {  /* abflg == 'b' */
                                savemb_mlk(iword);
                                if(fflg) {
                                    abflg = 'a';
                                    spbchk_mlk(fpo);
                                    spb = 0;
                                }
                            }
                            break;
                case 0xb1 : /* b1decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xf2 : /* f2decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xfb : /* fbdecode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xfc : /* fcdecode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xfd : /* fddecode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xf8 : /* f8decode_mlk(); */
                            if(iword[0] == f8word[0] &&
                               iword[1] == f8word[1] &&
                               iword[2] == f8word[2] &&
                               iword[3] == f8word[3]) break; 
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            f8word[0] = iword[0];
                            f8word[1] = iword[1];
                            f8word[2] = iword[2];
                            f8word[3] = iword[3];
                            break;
                case 0xf4 : /* f4decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xf6 : /* f6decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xfa : /* fadecode_mlk(); */
                            if(iword[0] == faword[0] &&
                               iword[1] == faword[1] &&
                               iword[2] == faword[2] &&
                               iword[3] == faword[3]) break; 
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            faword[0] = iword[0];
                            faword[1] = iword[1];
                            faword[2] = iword[2];
                            faword[3] = iword[3];
                            break;
                case 0xfe : /* fedecode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xdd : /* dddecode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xe1 : /* e1decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xe2 : /* e2decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xe3 : /* e3decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xe4 : /* e4decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
                            break;
                case 0xe6 : /* e6decode_mlk(); */
                            if(abflg == 'a') savema_mlk(iword);
                            if(abflg == 'b') savemb_mlk(iword);
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
    ls->trace = wtracef;
    if(ls->odump == 1) outcdump_mlk(ofile);
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlksmp p[%d] %s -> ",portnb,ifile);
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    return(0);
}

int spachk_mlk(FILE *fpo)
//FILE *fpo;
{
    int  i;
    unsigned char iword[4];
    unsigned char cword[4];
    int  fflg;

    if(ls->debug == 3) printf("///spa table/// spa = %d\n",spa);
    if(ls->trace || ls->debug == 3) ls->trace = 1;
    for(i = 0; i < spa; i++) {
        iword[0] = sa[i].c0;
        iword[1] = sa[i].c1;
        iword[2] = sa[i].c2;
        iword[3] = sa[i].c3;
        ctoval_mlk(iword);   /* set code to value */
        switch (iword[0]) {
            case 0xf1 : f1decode_mlk();
                        fflg = lc->c[1] & 0x80;
                        if(!fflg) break;
                        /* smptfull_mlk(fpo); */
                        if(fxyflg == 'x') {
                            pxflam = lv->smpte;
                            if(pxflam > pyflam) plag = pxflam - pyflam;
                            else plag = 0;
                            fxyflg = 'y';
                            if(ls->debug == 3) {
                                printf("pxflam=%ld pyflam=%ld",
                                    pxflam,pyflam);
                                printf(" plag=%ld pdsc=%ld\n",plag,pdsc);
                            }
                        }
                        else  { /* fxyflg == 'y' */
                            pyflam = lv->smpte;
                            if(pyflam > pxflam) plag = pyflam - pxflam;
                            else plag = 0;
                            fxyflg = 'x';
                            if(ls->debug == 3) {
                                printf("pyflam=%ld pxflam=%ld",
                                    pyflam,pxflam);
                                printf(" plag=%ld pdsc=%ld\n",plag,pdsc);
                            }
                        }
                        dsc = pdsc;
                        pdsc = 0;
                        break;
            case 0xfc : fcdecode_mlk();
                        if(sxyflg == 'x') {
                            pxsyn = lv->sync;
                            if(pysyn < pxsyn) pds = pysyn;
                            else pds = pysyn - pxsyn;
                            sxyflg = 'y';
                            pdsc = pdsc + pds;
                            /*
                            if(ls->debug == 3) {
                                printf("pysyn=%ld pxsyn=%ld",
                                    pysyn,pxsyn);
                                printf(" pdsc=%ld pds=%ld\n",pdsc,pds);
                                break;
                            }
                            */
                        }
                        if(sxyflg == 'y') {
                            pysyn = lv->sync;
                            if(pxsyn < pysyn) pds = pxsyn;
                            else pds = pxsyn - pysyn;
                            sxyflg = 'x';
                            pdsc = pdsc + pds;
                            /*
                            if(ls->debug == 3) {
                                printf("pxsyn=%ld pysyn=%ld",
                                    pxsyn,pysyn);
                                printf(" pdsc=%ld pds=%ld\n",pdsc,pds);
                                break;
                            }
                            */
                        }
                        break;
            default   : break;
        }
    }
    /* check fc code and mix */
    for(i = 0; i < spa - 1 ; i++) {
        if(sa[i].c0 == 0xfc && sa[i+1].c0 == 0xfc) sa[i].c0 = 0xcc;
    }
    if(ls->trace || ls->debug == 3) ls->trace = 2;
    for(i = 0; i < spa; i++) {
        iword[0] = sa[i].c0;
        iword[1] = sa[i].c1;
        iword[2] = sa[i].c2;
        iword[3] = sa[i].c3;
        ctoval_mlk(iword);   /* set code to value */
        switch (iword[0]) {
            case 0xf1 : f1decode_mlk();
                        fflg = lc->c[1] & 0x80;
                        /* if(fflg) break; */
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xb1 : b1decode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xf2 : f2decode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xfb : fbdecode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xfc : fcdecode_mlk();
                        if(psxyflg == 'x') {
                            xsync = lv->sync;
                            if(ysync < xsync) ds = ysync;
                            else ds = ysync - xsync;
			    ds = ds * lv->mlp;
                            if(dsc == 0) lag = ds * plag;
                            else lag = ds * plag / dsc;
                            psxyflg = 'y';
                            if(lag < 0) lag = 0;
                            ltocdec_mlk(cword,0xf4,lag);
                            fputcd_mlk(cword,fpo);
                            if(ls->debug == 3) {
                                printf("%sF4(%02x%02x%02x%02x)",
                                    "             ",
                                    cword[0],cword[1],cword[2],cword[3]);
                                printf(" lag(ds*plag/dsc)=%ld\n",lag);
                                printf("%sysn=%ld xsn=%ld",
                                    "             ",ysync,xsync);
                                printf(" ds=%ld plag=%ld dsc=%ld\n",
                                    ds,plag,dsc);
                            }
                            break;
                        }
                        if(psxyflg == 'y') {
                            ysync = lv->sync;
                            if(xsync < ysync) ds = xsync;
                            else ds = xsync - ysync;
			    ds = ds * lv->mlp;
                            if(dsc == 0) lag = ds * plag;
                            else lag = ds * plag / dsc;
                            psxyflg = 'x';
                            if(lag < 0) lag = 0;
                            ltocdec_mlk(cword,0xf4,lag);
                            fputcd_mlk(cword,fpo);
                            if(ls->debug == 3) {
                                printf("%sF4(%02x%02x%02x%02x)",
                                    "             ",
                                    cword[0],cword[1],cword[2],cword[3]);
                                printf(" lag(ds*plag/dsc)=%ld\n",lag);
                                printf("%sysn=%ld xsn=%ld",
                                    "             ",ysync,xsync);
                                printf(" ds=%ld plag=%ld dsc=%ld\n",
                                    ds,plag,dsc);
                            }
                        }
                        break;
            case 0xcc : fcdecode_mlk();
                        break;
            case 0xfa : fadecode_mlk();
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
            case 0xfe : fedecode_mlk();
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
            case 0xdd : dddecode_mlk();
                        /* Comment out 2002.9 */
                        /*
                        if(sa[i].c1 == ckport && sa[i].c2 == cktrak &&
                        sa[i].c3 == ckmemb) break;
                        ckport = sa[i].c1;
                        cktrak = sa[i].c2;
                        ckmemb = sa[i].c3;
                        */
                        if(sa[i+1].c0 != 0xdd) fputcd_mlk(iword,fpo);
                        break;
            default   : break;
        }
    }
    return(0);
}

int spbchk_mlk(FILE *fpo)
//FILE *fpo;
{
    int  i;
    unsigned char iword[4];
    unsigned char cword[4];
    int  fflg;

    if(ls->debug == 3) printf("///spb table/// spb = %d\n",spb);
    if(ls->trace || ls->debug == 3) ls->trace = 1;
    for(i = 0; i < spb; i++) {
        iword[0] = sb[i].c0;
        iword[1] = sb[i].c1;
        iword[2] = sb[i].c2;
        iword[3] = sb[i].c3;
        ctoval_mlk(iword);   /* set code to value */
        switch (iword[0]) {
            case 0xf1 : f1decode_mlk();
                        fflg = lc->c[1] & 0x80;
                        if(!fflg) break;
                        if(fxyflg == 'x') {
                            pxflam = lv->smpte;
                            if(pxflam > pyflam) plag = pxflam - pyflam;
                            else plag = 0;
                            fxyflg = 'y';
                            if(ls->debug == 3) {
                                printf("pxflam=%ld pyflam=%ld",
                                    pxflam,pyflam);
                                printf(" plag=%ld pdsc=%ld\n",plag,pdsc);
                            }
                        }
                        else  { /* fxyflg == 'y' */
                            pyflam = lv->smpte;
                            if(pyflam > pxflam) plag = pyflam - pxflam;
                            else plag = 0;
                            fxyflg = 'x';
                            if(ls->debug == 3) {
                                printf("pyflam=%ld pxflam=%ld",
                                    pyflam,pxflam);
                                printf(" plag=%ld pdsc=%ld\n",plag,pdsc);
                            }
                        }
                        dsc = pdsc;
                        pdsc = 0;
                        break;
            case 0xfc : fcdecode_mlk();
                        if(sxyflg == 'x') {
                            pxsyn = lv->sync;
                            if(pysyn < pxsyn) pds = pysyn;
                            else pds = pysyn - pxsyn;
                            sxyflg = 'y';
                            pdsc = pdsc + pds;
                            /*
                            if(ls->debug == 3) {
                                printf("pysyn=%ld pxsyn=%ld",
                                    pysyn,pxsyn);
                                printf(" pdsc=%ld pds=%ld\n",pdsc,pds);
                                break;
                            }
                            */
                        }
                        if(sxyflg == 'y') {
                            pysyn = lv->sync;
                            if(pxsyn < pysyn) pds = pxsyn;
                            else pds = pxsyn - pysyn;
                            sxyflg = 'x';
                            pdsc = pdsc + pds;
                            /*
                            if(ls->debug == 3) {
                                printf("pxsyn=%ld pysyn=%ld",
                                    pxsyn,pysyn);
                                printf(" pdsc=%ld pds=%ld\n",pdsc,pds);
                                break;
                            }
                            */
                        }
                        break;
            default   : break;
        }
    }
    /* check fc code and mix */
    for(i = 0; i < spb - 1 ; i++) {
        if(sb[i].c0 == 0xfc && sb[i+1].c0 == 0xfc) sb[i].c0 = 0xcc;
    }
    if(ls->trace || ls->debug == 3) ls->trace = 2;
    for(i = 0; i < spb; i++) {
        iword[0] = sb[i].c0;
        iword[1] = sb[i].c1;
        iword[2] = sb[i].c2;
        iword[3] = sb[i].c3;
        ctoval_mlk(iword);   /* set code to value */
        switch (iword[0]) {
            case 0xf1 : f1decode_mlk();
                        fflg = lc->c[1] & 0x80;
                        /* if(fflg) break; */
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xb1 : b1decode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xf2 : f2decode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xfb : fbdecode_mlk();
                        fputcd_mlk(iword,fpo);
                        break;
            case 0xfc : fcdecode_mlk();
                        if(psxyflg == 'x') {
                            xsync = lv->sync;
                            if(ysync < xsync) ds = ysync;
                            else ds = ysync - xsync;
			    ds = ds * lv->mlp;
                            if(dsc == 0) lag = ds * plag;
                            else lag = ds * plag / dsc;
                            psxyflg = 'y';
                            if(lag < 0) lag = 0;
                            ltocdec_mlk(cword,0xf4,lag);
                            fputcd_mlk(cword,fpo);
                            if(ls->debug == 3) {
                                printf("%sF4(%02x%02x%02x%02x)",
                                    "             ",
                                    cword[0],cword[1],cword[2],cword[3]);
                                printf(" lag(ds*plag/dsc)=%ld\n",lag);
                                printf("%sysn=%ld xsn=%ld",
                                    "             ",ysync,xsync);
                                printf(" ds=%ld plag=%ld dsc=%ld\n",
                                    ds,plag,dsc);
                            }
                            break;
                        }
                        if(psxyflg == 'y') {
                            ysync = lv->sync;
                            if(xsync < ysync) ds = xsync;
                            else ds = xsync - ysync;
			    ds = ds * lv->mlp;
                            if(dsc == 0) lag = ds * plag;
                            else lag = ds * plag / dsc;
                            psxyflg = 'x';
                            if(lag < 0) lag = 0;
                            ltocdec_mlk(cword,0xf4,lag);
                            fputcd_mlk(cword,fpo);
                            if(ls->debug == 3) {
                                printf("%sF4(%02x%02x%02x%02x)",
                                    "             ",
                                    cword[0],cword[1],cword[2],cword[3]);
                                printf(" lag(ds*plag/dsc)=%ld\n",lag);
                                printf("%sysn=%ld xsn=%ld",
                                    "             ",ysync,xsync);
                                printf(" ds=%ld plag=%ld dsc=%ld\n",
                                    ds,plag,dsc);
                            }
                        }
                        break;
            case 0xcc : fcdecode_mlk();
                        break;
            case 0xfa : fadecode_mlk();
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
            case 0xfe : fedecode_mlk();
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
            case 0xdd : dddecode_mlk();
                        /* Comment out 2002.9 */
                        /*
                        if(sb[i].c1 == ckport && sb[i].c2 == cktrak &&
                        sb[i].c3 == ckmemb) break;
                        ckport = sb[i].c1;
                        cktrak = sb[i].c2;
                        ckmemb = sb[i].c3;
                        */
                        if(sb[i+1].c0 != 0xdd) fputcd_mlk(iword,fpo);
                        break;
            default   : break;
        }
    }
    return(0);
}

/* make midi file fn_xxx.exe */
int mkmidif_mlk(char ifile[STLN])
//char ifile[STLN];
{
    char ofile[STLN];
    char wfile[STLN];
    FILE *fpi,*fpo,*fpw;
    int  i;
    int  rtn;

    if(ls->debug) printf("mkmidif() ifile=%s\n",ifile);
    mm->flc = 0;    /* quoater flame position count */
    mm->f1 = 0;
    mm->f2 = 0;
    mm->s1 = 0;
    mm->s2 = 0;
    mm->m1 = 0;
    mm->m2 = 0;
    mm->h1 = 0;
    mm->h2 = 0;
    mm->h = 0;
    mm->s = 0;
    mm->m = 0;
    mm->f = 0;
    mm->tcbit = 3;  /* ? */
    mm->fflame = 0;
    mm->smpte = 0;
    lv->occnt = 0;
    lv->qfcnt = 0;
    lv->qfsmp = 0;
    i = 0;
    while(ifile[i] != '.') {
        ofile[i] = ifile[i];
        wfile[i] = ifile[i];
        i++;
    }
    ifile[i] = '\0';
    ofile[i] = '\0';
    wfile[i] = '\0';
    strcat(ifile,".mlk");
    strcat(ofile,".mex");
    strcat(wfile,".bak");
    fpi = fopen_h(ifile,"rb");
    if(mm->brself) {
        mm->byrd = 0;
        fpo = fopen_h(wfile,"wb");
        fpw = fopen_h(ofile,"rb");  /* read xxx.mex file for include */
        mlkfrd1_mlk(fpo,fpw);
    }
    else {
        fpo = fopen_h(ofile,"wb");
        mm->byrd = 1;
    }
    rtn = submkmid_mlk(fpi,fpo,fpw);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close wfile */
    if(mm->brself) {
        fclose(fpw);   /* close ofile */
        remove(ofile);
        if(rtn == 3) rename(wfile,ofile); /* rename .bak to .mex */
    }
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlk end mtc format ");
        printf("%s %ld Bytes\n",ofile,lv->occnt);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    return(0);
}

int submkmid_mlk(FILE *fpi,FILE *fpo,FILE *fpw)
//FILE *fpi;
//FILE *fpo;
//FILE *fpw;
{
    unsigned char iword[4];
    int  i,j,k;
    int  c;
    int  trace;
    int  fflg;
    long dv;    /* delta value */
    int  p;
    int  qf;    /* quoater flame  SMPTE * 4 */
    long qfs;
    long df;    /* delta falme */
    int  f;
    int  vcf;   /* velocity change flag */
    float vr;

    k = 0;
    j = 0;
    trace = ls->trace;
    if(ls->trace || ls->debug == 4) ls->trace = 1;
    for(p = 0; p < MAXPT; p++) {
        lr->ftype[p] = 0;
        for(f = 0; f < 4; f++) {
            lr->fchan[p][f] = 0;     /* phrase channel */
            lr->fsval[p][f] = 0;     /* phrase start value */
            lr->fctno[p][f] = 0;     /* phrase control number */
            lr->feval[p][f] = 0;     /* phrase end value */
            lr->fnval[p][f] = 0;     /* phrase note value */
            lr->fincv[p][f] = 0;     /* phrase increment value */
            lr->fintv[p][f] = 0;     /* phrase timecode interval */
            lr->fcval[p][f] = 0;     /* phrase current value */
            lr->fctin[p][f] = 0;     /* phrase current timer interval */
            lr->fudfg[p][f] = 0;     /* phrase up down flag */
            lr->factf[p][f] = 0;     /* phrase activ on/off flag */
        }
    }
    for(i = 0; i < 128; i++) fjtb[i] = 0;    /* flame jump counter */
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            switch (iword[0]) {
                case 0xf1 : if(mm->byrd != 1) break;
                            f1decode_mlk();
                            if(!lv->qfcnt) {
                                lv->qfsmp = lv->smpte;
                                compsmp1_mlk(lv->smpte);
                            }
                            lv->qfcnt++;
                            break;
                case 0xb1 : if(mm->byrd != 1) break;
                            b1decode_mlk();
                            /* compsmp1_mlk(lv->smpte); */
                            fflg = lc->c[1] & 0x80;
                            if(!fflg) smptfull_mlk(fpo);
                            break;
                case 0xf2 : if(mm->byrd != 1) break;
                            f2decode_mlk();
                            break;
                case 0xfb : fbdecode_mlk();
                            if(ls->lnkmon) {
                                printf("%02d%05d occnt=%ld\r",
                                    lv->movn,lv->bnum,lv->occnt);
                            }
                            mbselck_mlk();
                            if(mm->byrd != 1) break;
                            iword[1] |= 0x80;         /* fb code flag set */
                            fputc(iword[0],fpo);
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            if(ls->debug == 7)
                                printf("lv->bnum=%d lv->occnt=%ld\n",
                                    lv->bnum,lv->occnt);
                            if(ls->otrace == 1) {
                                printf("%s%02x %02x %02x %02x\n",
                                dgstr5,iword[0],iword[1],iword[2],iword[3]);
                            }
                            break;
                case 0xf4 : if(mm->byrd != 1) break;
                            f4decode_mlk();
                            quatflag_mlk(fpo,0);
                            break;
                case 0xfd : if(mm->byrd != 1) break;
                            vcf = velochg_mlk(iword);
                            fddecode_mlk();
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            if(iword[3] != 0xff) {
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 3;
                            }
                            else lv->occnt = lv->occnt + 2;
                            if(ls->otrace == 1) {
                                if(vcf) {
                                    vr = lv->mvelo / 10.0;
                                    printf("%s%02x %02x %02x (%4.1f%c)\n",
                                    dgstr1,iword[1],iword[2],iword[3],vr,'%');
                                }
                                else printf("%s%02x %02x %02x\n",
                                    dgstr1,iword[1],iword[2],iword[3]);
                            }
                            break;
                case 0xf6 : if(mm->byrd != 1) break;
                            f6decode_mlk();
                            if(iword[1] == 0x02) {
                                fputc(iword[2],fpo);
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 2;
                                if(ls->otrace == 1) {
                                    printf("%s%02x %02x\n",
                                        dgstr1,iword[2],iword[3]);
                                }
                            }
                            if(iword[1] == 0x01) {
                                fputc(iword[2],fpo);
                                lv->occnt++;
                                if(ls->otrace == 1) {
                                    printf("%s%02x\n",
                                        dgstr1,iword[2]);
                                }
                            }
                            break;
                case 0xf8 : if(mm->byrd == 1) f8decode_mlk();
                            break;
                case 0xfa : if(mm->byrd == 1) fadecode_mlk();
                            break;
                case 0xfe : if(mm->byrd == 1) fedecode_mlk();
                            break;
                case 0xe1 : if(mm->byrd == 1) e1decode_mlk();
                            break;
                case 0xe2 : if(mm->byrd == 1) e2decode_mlk();
                            break;
                case 0xe3 : if(mm->byrd != 1) break;
                            e3decode_mlk();
                            p = lv->port;     /* port */
                            if(p >= MAXPT) break;
                            f = lr->ftype[p];
                            dv = lr->feval[p][f] - lr->fsval[p][f];
                            lr->fudfg[p][f] = 1;
                            if( dv < 0 ) {
                                lr->fudfg[p][f] = 2;
                                dv = lr->fsval[p][f] - lr->feval[p][f];
                            }
                            qf = lv->sflame * 4;
                            qfs = (lr->fnval[p][f] * qf) / 100000;
                            df = (qfs * 6000) / lv->temp;
                            if( dv > df ) {
                                lr->fincv[p][f] = dv/df;
                                lr->fintv[p][f] = 1;
                            }
                            if( dv < df ) {
                                lr->fintv[p][f] = df/dv;
                                lr->fincv[p][f] = 1;
                            }
                            lr->factf[p][f] = 1;
                            if(ls->debug == 4) {
                                /* printf("             "); */
                                printf("dv=%ld qfs=%ld df=%ld ",dv,qfs,df);
                                printf("fincv[%d][%d]=%d fintv[%d][%d]=%d ",
                                    p,f,lr->fincv[p][f],p,f,lr->fintv[p][f]);
                                printf("fnval[%d][%d]=%ld\n",
                                    p,f,lr->fnval[p][f]);
                            }
                            break;
                case 0xe6 : if(mm->byrd != 1) break;
                            e6decode_mlk();
                            iword[1] |= 0x80;         /* e6 code flag set */
                            fputc(iword[0],fpo);
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            if(ls->otrace == 1) {
                                printf("%s%02x %02x %02x %02x\n",
                                dgstr5,iword[0],iword[1],iword[2],iword[3]);
                            }
                            break;
                case 0xdd : if(mm->byrd != 1) break;
                            dddecode_mlk();      /* change dd to fd code */
                            fputc(0xfd,fpo);
                            iword[1] |= 0x80;      /* fd code flag set */
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            if(ls->otrace == 1) {
                                printf("%s%02x %02x %02x %02x\n",
                                dgstr4,iword[0],iword[1],iword[2],iword[3]);
                            }
                            break;
                default   : break;
            }
        }
    }
    if(mm->flc) quatflag_mlk(fpo,1);
    if(mm->byrd == 2) mlkfrd2_mlk(fpo,fpw);
    if(mm->byrd == 1) {
        fputc(0xed,fpo);
        fputc(0xed,fpo);
        fputc(0xed,fpo);
        fputc(0xed,fpo);
        lv->occnt = lv->occnt + 4;
    }
    ls->trace = trace;
    return(3);
}

/*
SMPTE full message
 f0 7f : Universal System xclusive Real Time Header
 7f    : channel (message intended for entire system)
 01    : sub-id #1 (MIDI time code)
 01    : sub-i2 #2 (full time code message)
 hr (0yyzzzzz) : Hours and type
         yy    : type
         00    : 24 flames /second
         01    : 25 flames /second
         02    : 30 flames/second(drop frame)
         03    : 30 flames/second(non drop frame)
         zzzzz : Hours (0-23)
 mn    : Minites(0-59)
 sc    : Seconds(0-59)
 fr    : Flames(0-29)
 f7    : EOX
*/
int smptfull_mlk(FILE *fpo)
//FILE *fpo;
{
    unsigned char v,z;
    long fm;
    int  h,m,s,f;

    fm = lv->smpte / 100;
    h = fm / 3600;
    h = h / lv->sflame;
    /* h = fm / (3600 * lv->sflame); */
    m = (fm - (h * 3600 * lv->sflame)) / 60;
    m = m / lv->sflame;
    /* m = (fm - (h * 3600 * lv->sflame)) / (60 * lv->sflame); */
    s = fm - (h * 3600 * lv->sflame);
    s = s - (m * 60 * lv->sflame);
    s = s / lv->sflame;
    /* s = (fm - (h * 3600 * lv->sflame) - (m * 60 * lv->sflame))
            / FLAME; */
    f = fm - (h * 3600 * lv->sflame) - (m * 60 *lv->sflame)
        - (s * lv->sflame);
    fputc(0xf0,fpo);
    if(ls->outfm == 2) fputc(0x09,fpo);
    if(ls->outfm == 3) fputc(0x09,fpo);
    fputc(0x7f,fpo);
    /* v = lv->chan & 0xff; */
    v = 0x7f;    /* channel */
    fputc(v,fpo);
    fputc(0x01,fpo);
    fputc(0x01,fpo);
    if(ls->otrace == 1) printf("%sf0 7f %02x 01 01 ",dgstr2,v);
    if(ls->otrace == 8) {
	 if(ls->outfm == 2 || ls->outfm == 3) {  /* .mid */
	     printf("    f0 09 7f %02x 01 01 ",v);
	 }
	 else  { /* .mex */
	     printf("      f0 7f %02x 01 01 ",v);
	 }
    }
    v = lv->tctyp & 0x0f;
    v = v << 5;
    z = h & 0x001f;
    v |= z;
    fputc(v,fpo);
    if(ls->otrace == 1) printf("%02x ",v);
    if(ls->otrace == 8) printf("%02x ",v);
    v = m & 0x007f;
    fputc(v,fpo);
    if(ls->otrace == 1) printf("%02x ",v);
    if(ls->otrace == 8) printf("%02x ",v);
    v = s & 0x007f;
    if(ls->otrace == 1) printf("%02x ",v);
    if(ls->otrace == 8) printf("%02x ",v);
    fputc(v,fpo);
    v = f & 0x007f;
    fputc(v,fpo);
    fputc(0xf7,fpo);
    if(ls->otrace == 1) printf("%02x f7\n",v);
    if(ls->otrace == 8) printf("%02x f7    SMPTE full msg\n",v);
    if(ls->outfm == 2) lv->occnt = lv->occnt + 11;
    else lv->occnt = lv->occnt + 10;
    return(0);
}

/* 
SMPTE quoater flame time lag get and set time code
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
                              zzzz   : hour (0-23)
*/
int quatflag_mlk(FILE *fpo,int last)
//FILE *fpo;
//int  last;
{
    long qlag;
    int  i,j;
    float ff;
    long sfm;

    qlag = lv->stlag;  /* lag : (1/100) flame */
    qlag = qlag * 4;   /* qlag : (4/100) flame */
    if(mm->fflame == 9999) {
        if(ls->debug == 7)
            printf("lv->qfsmp=%ld mm->smpte=%ld\n",lv->qfsmp,mm->smpte);
        lv->qfsmp = mm->smpte;
        mm->fflame = 0;
    }
    qlag = qlag + mm->fflame;
    if(last) qlag = (8 - mm->flc) * 100;
    while(qlag >= 50) {  /* rount 0.5/120 sec */
        compsmp2_mlk(lv->qfsmp);
        lv->qfcnt++;
        fputc(0xf1,fpo);
        switch (mm->flc) {
            case   0 : fputc((char)mm->f1,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->f1);
                       break;
            case   1 : mm->f2 = mm->f2 | 0x0010;
                       fputc((char)mm->f2,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->f2);
                       break;
            case   2 : mm->s1 = mm->s1 | 0x0020;
                       fputc((char)mm->s1,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->s1);
                       break;
            case   3 : mm->s2 = mm->s2 | 0x0030;
                       fputc((char)mm->s2,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->s2);
                       break;
            case   4 : mm->m1 = mm->m1 | 0x0040;
                       fputc((char)mm->m1,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->m1);
                       break;
            case   5 : mm->m2 = mm->m2 | 0x0050;
                       fputc((char)mm->m2,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->m2);
                       break;
            case   6 : mm->h1 = mm->h1 | 0x0060;
                       fputc((char)mm->h1,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->h1);
                       break;
            case   7 : mm->h2 = mm->h2 | 0x0070;
                       mm->tcbit = lv->tctyp << 1;
                       mm->tcbit &= 0x0006;
                       mm->h2 = mm->h2 | mm->tcbit;
                       fputc((char)mm->h2,fpo);
                       if(ls->otrace == 1) printf("%sF1 %02x",
                           dgstr3,mm->h2);
                       break;
            default  : break;
        }
        if(ls->otrace == 1) {
            sfm = lv->sflame * 100;
            ff = lv->qfsmp - ((long)mm->h * 3600 * sfm)
                  - ((long)mm->m * 60 * sfm) - ((long)mm->s * sfm);
            ff = ff / 100.0;
            printf(" / %02d:%02d:%02d/%05.2f flame=%ld",
                mm->h,mm->m,mm->s,ff,lv->qfsmp);
            if(mm->flc == 7) printf(" tctyp=%d\n",lv->tctyp);
            else printf("\n");
        }
        mm->flc++;
        mm->flc = mm->flc % 8;
        qlag = qlag - 100;    /* 1/4 flame dec. */
        lv->qfsmp += 25;      /* 25 = 100 / 4 */
        lv->occnt = lv->occnt + 2;
        /* flase code E1 E2 E3 */
        for(i = 0 ; i < MAXPT; i++) {
            for(j = 0; j < 4; j++) if(lr->factf[i][j]) phrasing_mlk(i,j,fpo);
        }
    }
    mm->fflame = 0;
    if(qlag > 0 && qlag < 50) mm->fflame = qlag;
    if(qlag > 50 && qlag < 100) mm->fflame = 50 - qlag; /* negativ value */
    return(0);
}

int compsmp1_mlk(long fm)
//long fm;
{
    long sfm;
    long h,m,s,f;
    float ff;

    sfm = lv->sflame * 100;
    h = fm / 3600;
    h = h / sfm;
    m = (fm - (h * 3600 * sfm)) / 60;
    m = m / sfm;
    s = fm - (h * 3600 * sfm);
    s = s - (m * 60 * sfm);
    s = s / sfm;
    f = fm - (h * 3600 * sfm)
            - (m * 60 * sfm) - (s * sfm);
    f = f / 100;
    mm->h = h;
    mm->m = m;
    mm->s = s;
    mm->f = f;
    mm->f1 = mm->f & 0x000f;
    mm->f2 = mm->f & 0x00f0;
    mm->f2 = mm->f2 >> 4;
    mm->s1 = mm->s & 0x000f;
    mm->s2 = mm->s & 0x00f0;
    mm->s2 = mm->s2 >> 4;
    mm->m1 = mm->m & 0x000f;
    mm->m2 = mm->m & 0x00f0;
    mm->m2 = mm->m2 >> 4;
    mm->h1 = mm->h & 0x000f;
    mm->h2 = mm->h & 0x00f0;
    mm->h2 = mm->h2 >> 4;
    if(ls->debug == 4) {
        ff = fm - (h * 3600 * sfm)
              - (m * 60 * sfm) - (s * sfm);
        ff = ff / 100.0;
        /* printf("             Smpte "); */
        printf("%02d:%02d:%02d/%05.2f flame=%ld\n",
            mm->h,mm->m,mm->s,ff,fm);
    }
    return(0);
}

int compsmp2_mlk(long fm)
//long fm;
{
    long sfm;
    long h,m,s,f;

    sfm = lv->sflame * 100;
    h = fm / 3600;
    h = h / sfm;
    m = (fm - (h * 3600 * sfm)) / 60;
    m = m / sfm;
    s = fm - (h * 3600 * sfm);
    s = s - (m * 60 * sfm);
    s = s / sfm;
    f = fm - (h * 3600 * sfm)
            - (m * 60 * sfm) - (s * sfm);
    f = f / 100;
    mm->h = h;
    mm->m = m;
    mm->s = s;
    mm->f = f;
    mm->f1 = mm->f & 0x000f;
    mm->f2 = mm->f & 0x00f0;
    mm->f2 = mm->f2 >> 4;
    mm->s1 = mm->s & 0x000f;
    mm->s2 = mm->s & 0x00f0;
    mm->s2 = mm->s2 >> 4;
    mm->m1 = mm->m & 0x000f;
    mm->m2 = mm->m & 0x00f0;
    mm->m2 = mm->m2 >> 4;
    mm->h1 = mm->h & 0x000f;
    mm->h2 = mm->h & 0x00f0;
    mm->h2 = mm->h2 >> 4;
    return(0);
}

int ltocdec_mlk(unsigned char cword[4],unsigned char fcode,long lval)
//unsigned char cword[4];
//unsigned char fcode;
//long lval;
{
    union MCODE u;
    union {
        unsigned long lv;
        unsigned short iv[2];
    } m;

    if(ls->comptyp == 0) {
        m.lv = lval;
        u.wi[0] = m.iv[1];
        u.wi[1] = m.iv[0];
        cword[0] = fcode;
        cword[1] = u.wc[0];
        cword[2] = u.wc[3];
        cword[3] = u.wc[2];
    }
    if(ls->comptyp == 1) {
        m.lv = lval;
        u.wi[0] = m.iv[0];
        u.wi[1] = m.iv[1];
        cword[0] = fcode;
        cword[1] = u.wc[1];
        cword[2] = u.wc[2];
        cword[3] = u.wc[3];
    }
    if(ls->comptyp == 2) {
        m.lv = lval;
        u.wi[0] = m.iv[1];
        u.wi[1] = m.iv[0];
        cword[0] = fcode;
        cword[1] = u.wc[0];
        cword[2] = u.wc[3];
        cword[3] = u.wc[2];
    }
    return(0);
}
