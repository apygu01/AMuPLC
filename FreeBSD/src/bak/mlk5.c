
// ----------------------
//  AMuPLC (mlk5.c)
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
/* mlk5.c  -old option SMF .mex */
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKCDE *lc;
extern MLKFRS *lr;
extern MLKPHS *ph;
extern int  iv[16][5];
extern long cdt; /* Current dl */
extern long pdt;
extern int  pdf; /* pdt on/off flag */
extern int  ppf;
unsigned char MXhd[14] = {
   0x4d, 0x58, 0x68, 0x64,  /* MXhd */
   0x00, 0x00, 0x00, 0x06,  /* chunk length */
   0x00, 0x00,              /* format 0 */
   0x00, 0x01,              /* one track */
   0xe2, 0x78               /* SMPTE 30 flame per sec resolution 120 */
};
unsigned char MXrk[8] = {
   0x4d, 0x58, 0x72, 0x6b,  /* MXrk */
   0x00, 0x00, 0x00, 0x00   /* chunk length 0 bytes */
};
long dt_all; /* dt count */

/* make mex file fn.mex resolution 1/3000 sec */
int mkdtx(char ifile[STLN])
//char ifile[STLN];
{
    char ofile[STLN];
    char wfile[STLN];
    char tfile[STLN];
    FILE *fpi,*fpo,*fpw;
    int  i;
    int  hh,mm,ss,ff;

    if(ls->debug) printf("mkdtx() ifile=%s\n",ifile);
    dt_all = 0;
    lv->occnt = 0;
    lv->qfcnt = 0;
    lv->qfsmp = 0;
    i = 0;
    while(ifile[i] != '.') {
        ofile[i] = ifile[i];
        wfile[i] = ifile[i];
        tfile[i] = ifile[i];
        i++;
    }
    ifile[i] = '\0';
    ofile[i] = '\0';
    wfile[i] = '\0';
    tfile[i] = '\0';
    strcat(ifile,".mlk");
    strcat(wfile,".wrk");
    strcat(ofile,".mex");
    strcat(tfile,".tmp");
    fpi = fopen_h(ifile,"rb");
    fpo = fopen_h(wfile,"wb");
    dtxsub1(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close wfile */
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlkdef %s -> %s %ld Bytes dt_all %ld ",
        ifile,ofile,lv->occnt,dt_all);
        printf("Resolution 1/%.1f Sec\n",lv->mexres);
    }
    if(ls->optm == 0) {
        fpi = fopen_h(wfile,"rb");
        fpo = fopen_h(ofile,"wb");
        dtxsub2(fpi,fpo);
        fclose(fpi);   /* close ifile */
        fclose(fpo);   /* close ofile */
        remove(ifile);
        remove(wfile);
    }
    else {
        fpi = fopen_h(wfile,"rb");
        fpo = fopen_h(tfile,"wb");
        nexsub2(fpi,fpo);
        fclose(fpi);   /* close ifile */
        fclose(fpo);   /* close ofile */

        fpi = fopen_h(tfile,"rb");
        fpo = fopen_h(ofile,"wb");
        dtxsub2(fpi,fpo);
        fclose(fpi);   /* close ifile */
        fclose(fpo);   /* close ofile */

        remove(ifile);
        remove(wfile);
        remove(tfile);
    }
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        dtconv_mlk(dt_all,&hh,&mm,&ss,&ff);
        printf("Mlk end -old %s %ld Bytes Time %d:%d:%d/%d ",
        ofile,lv->occnt,hh,mm,ss,ff);
        printf("Resolution 1/%.1f Sec\n",lv->mexres);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    return(0);
}

int dtconv_mlk(long dt,int *hh,int *mm,int *ss,int *ff)
//long dt;
//int  *hh;
//int  *mm;
//int  *ss;
//int  *ff;
{
    long res;
    long h; 
    long m; 

    /* dt = dt /lv->mlp; */
    /* *hh = dt / 10800000;  (3600 * 3000) = 10800000 */
    /* *mm = (dt - (*hh * 10800000))/ 180000;   (60 * 3000) = 180000 */
    /*
    *ss = (dt - (*hh * 10800000) - (*mm * 180000))/3000;
    *ff = dt - (*hh * 10800000) - (*mm * 180000) - (*ss * 3000);
    *ff = *ff/100;
    */
    /* 30 Flame / sec */

    /* res = lv->sflame * lv->mtcres; */
    res = lv->mexres;
    h = 3600 * res;
    m = 60 * res;
    *hh = dt / h; 
    *mm = (dt - (*hh * h))/m;
    *ss = (dt - (*hh * h) - (*mm * m))/res;
    *ff = (dt - (*hh * h) - (*mm * m) - (*ss * res))/lv->mtcres;
    *ff = dt - (*hh * h) - (*mm * m) - (*ss * res);
    *ff = *ff/lv->mtcres;  
    return(0);
}

int dtxsub1(FILE *fpi,FILE *fpo)
//FILE *fpi;
//FILE *fpo;
{
    unsigned char wc[4];
    unsigned char iword[4];
    int  i,j,k;
    int  e;
    int  c;
    int  fflg;
    long fv;    /* phrase value */
    long fl;    /* phrase length */
    int  p;
    long dt;    /* delta time */
    int  f,ff;
    int  divx;  /* divx = 1: default 1/100 flame */
    int  vcf;   /* velocity change flag */

    lv->mtcres = lv->mexres / lv->sflame;
    if(lv->mtcres > 127) {
        lv->mtcres = 120;
        /* lv->sflame = lv->mexres / 127; */
        ls->wrncnt++; 
        ls->msglvl = 4;
        if(ls->msglvl >= ls->prtlvl)
        printf("Worning! Resolution over 1/%.1f Sec set default 120\n",lv->mexres);
    }
    if(ls->debug) printf("lv->sflame=%d lv->mtcres=%d lv->mexres=%.1f\n",
        lv->sflame,lv->mtcres,lv->mexres); 
    MXhd[12] = -lv->sflame; /* MXhd division SMPTE format */
    MXhd[13] = lv->mtcres;  /* MTC resolution */
    divx = 1;
    if(lv->mtcres == 10) divx = 10;
    if(ls->debug == 8) {
        ls->dchk = 8;    /* SMF 0 debug */
        printf("MXhd division =%02x %02x\n",MXhd[12],MXhd[13]);
    }
    for(i = 0; i < 14; i++) fputc(MXhd[i],fpo); /* Put SMF Header Format 0 */
    lv->occnt = 14;
    if(ls->otrace == 8) {
        printf("\n           %02x %02x %02x %02x    MXhd\n",
            MXhd[0],MXhd[1],MXhd[2],MXhd[3]);
        printf("           %02x %02x %02x %02x    chunk length\n",
            MXhd[4],MXhd[5],MXhd[6],MXhd[7]);
        printf("           %02x %02x          format 0\n",
            MXhd[8],MXhd[9]);
        printf("           %02x %02x          one track\n",
            MXhd[10],MXhd[11]);
        printf("           %02x %02x          division\n\n",
            MXhd[12],MXhd[13]);
    }
    for(i = 0; i < 8; i++) fputc(MXrk[i],fpo);  /* PutSMF_Track_chunk */
    lv->occnt = lv->occnt + 8;
    if(ls->otrace == 8) {
        printf("           %02x %02x %02x %02x    MXrk\n",
            MXrk[0],MXrk[1],MXrk[2],MXrk[3]);
        printf("           %02x %02x %02x %02x    chunk length (Set After)\n",
            MXrk[4],MXrk[5],MXrk[6],MXrk[7]);
        printf("\n            Delta-time    Event        Comments\n");
    }

    for(p = 0; p < MAXPT; p++) {
        lr->ftype[p] = 0;
        for(ff = 0; ff < 4; ff++) {
            lr->fchan[p][ff] = 0;     /* phrase channel */
            lr->fsval[p][ff] = 0;     /* phrase start value */
            lr->fctno[p][ff] = 0;     /* phrase control number */
            lr->feval[p][ff] = 0;     /* phrase end value */
            lr->fnval[p][ff] = 0;     /* phrase note value */
        }
    }
    for(e = 0; e < 16; e++) {
        for(f = 0; f < 3; f++) {
            ph->cn[e][f] = 0;       /* phrase control number */
            ph->fl[e][f] = 0;       /* phrase length */
            ph->iv[e][f] = 0;       /* phrase increment value */
            ph->dl[e][f] = 0;       /* phrase time interval */
            ph->cv[e][f] = 0;       /* phrase current value */
            ph->ev[e][f] = 0;       /* phrase end value */
            ph->cp[e][f] = 0;       /* phrase current point */
            ph->af[e][f] = 0;       /* phrase activ on/off flag */
        }
    }
    k = 0;
    j = 0;
    dt = 0;
    pdt = 0;
    pdf = 0;
    ppf = 0;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            switch (iword[0]) {
                case 0xb1 : b1decode_mlk();
                            fflg = lc->c[1] & 0x80;
                            if(!fflg) {
                                LagOut(dt,fpo);
                                smptfull_mlk(fpo);
                            }
                            break;
                case 0xf2 : f2decode_mlk();
                            break;
                case 0xfb : fbdecode_mlk();
                            iword[1] |= 0x80;     /* fb code flag set */
                            fputc(iword[0],fpo);
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            if(ls->otrace == 8) {
                                printf("-m%02d%05d-___   ",
                                    lv->movn,lv->bnum);
                                printf("          %02x %02x %02x %02x\n",
                                    iword[0],iword[1],iword[2],iword[3]);
                            }
                            break;
                case 0xf4 : /* Delta time */
                            f4decode_mlk();
                            dt = dt + lv->stlag/divx;
                            /* Default 1/100 flame */
                            break;
                case 0xfd : /* fddecord_mlk(); */
                            LagOut(dt,fpo);
                            dt = 0;
                            vcf = velochg_mlk(iword);
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            if(ls->otrace == 8) {
                                printf("    %02x %02x",iword[1],iword[2]);
                            }
                            if(iword[3] != 0xff) {
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 3;
                                if(ls->otrace == 8) printf(" %02x",iword[3]);
                            }
                            else {
                                lv->occnt = lv->occnt + 2;
                            }
                            if(vcf) velomsg_mlk(); 
                            else fddec_mlk();
                            break;
                case 0xf6 : 
                            f6decode_mlk();
                            if(iword[1] == 0x02) {
                                if(iword[2] & 0x80) {
                                    if(iword[2] != 0xf7) {  /* Sys Ex. eof ? */
                                        LagOut(dt,fpo);
                                        dt = 0;
                                    }
                                }
                                fputc(iword[2],fpo);
                                if(iword[3] & 0x80) {
                                    if(iword[3] != 0xf7) {  /* Sys Ex. eof ? */
                                        LagOut(dt,fpo);
                                        dt = 0;
                                    }
                                }
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 2;
                                if(ls->otrace == 8) {
                                    if(iword[2] & 0x80) {
                                        printf("    %02x %02x",
                                            iword[2],iword[3]);
                                    }
                                    else {
                                        printf(" %02x %02x",iword[2],iword[3]);
                                    }
                                }
                            }
                            if(iword[1] == 0x01) {
                                if(iword[2] & 0x80) {
                                    if(iword[2] != 0xf7) {  /* Sys Ex. eof ? */
                                        LagOut(dt,fpo);
                                        dt = 0;
                                    }
                                }
                                fputc(iword[2],fpo);
                                lv->occnt++;
                                if(ls->otrace == 8) {
                                    printf(" %02x\n",iword[2]);
                                }
                            }
                            if(iword[1] == 0x00) {
                                if(ls->otrace == 8) printf("\n");
                            }
                            break;
                case 0xf8 : f8decode_mlk();
                            break;
                case 0xfa : fadecode_mlk();
                            break;
                case 0xfe : fedecode_mlk();
                            break;
                case 0xe1 : e1decode_mlk();
                            break;
                case 0xe2 : e2decode_mlk();
                            break;
                case 0xe3 : 
                            e3decode_mlk();
                            p = lv->port;     /* port */
                            if(p >= MAXPT) break;
                            ff = lr->ftype[p];
                            f = ff;
                            if(f == 3) f = 1;  /* cont. chg -> modulation */
                            e = lr->fchan[p][ff] - 1;
                            if(e > 15 || e < 0) e = 0; /* Channel Error */
                            ph->cn[e][f] = lr->fctno[p][ff];
                            fv = lr->feval[p][ff] - lr->fsval[p][ff];
                            ph->iv[e][f] = 1;
                            if( fv < 0 ) {
                                ph->iv[e][f] = -1;
                                fv = lr->fsval[p][ff] - lr->feval[p][ff];
                            }
                            fl = lv->smfdiv * lr->fnval[p][ff] / 100000;
                            ph->fl[e][f] = fl;
                            if( fv > fl) {
                                ph->iv[e][f] = ph->iv[e][f]*fv/fl; 
                                ph->dl[e][f] = 1;  /* dt = 1 */
                            }
                            if( fl > fv ) {
                                ph->dl[e][f] = fl/fv; /* dt */
                            }
                            ph->cv[e][f] = lr->fsval[p][ff];
                            ph->ev[e][f] = lr->feval[p][ff];
                            ph->af[e][f] = 1;
                            if(ls->otrace == 8) {
                              if(f == 0) printf("Volume");
                              if(f == 1) ph_outmsg(ph->cn[e][f]);
                              if(f == 2) printf("Pitch vend");
                              printf("[%d] Ch.%d St/End:%d/%d ",
                                  p,e+1,ph->cv[e][f],ph->ev[e][f]);
                              printf("Inc.%d dl.%d Len.%ld\n",
                                  ph->iv[e][f],ph->dl[e][f],ph->fl[e][f]);
                            }
                            break;
                case 0xe6 : /* Execution code for mex */
                            e6decode_mlk();
                            iword[1] |= 0x80;    /* e6 code flag set */
                            fputc(iword[0],fpo);
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            if(ls->otrace == 8) {
                                printf("Execution Controle data");
                                printf("    %02x %02x %02x %02x\n",
                                    iword[0],iword[1],iword[2],iword[3]);
                            }
                            break;
                case 0xdd : /* port , channel, member */
                            dddecode_mlk();      /* change dd to fd code */
                            if(ls->otrace == 8) {
                                printf("                        ");
                                printf("  fd %02x %02x %02x",
                                    iword[1] | 0x80,iword[2],iword[3]);
                                /*
                                printf("  Port[%d] Track %d Member %d\n",
                                    iword[1],iword[2],iword[3]);
                                */
                                // 2024.11.20 Member cut
                                printf("  Port[%d] Track %d\n",
                                    iword[1],iword[2]);
                            }
                            fputc(0xfd,fpo);
                            iword[1] |= 0x80;      /* fd code flag set */
                            fputc(iword[1],fpo);
                            fputc(iword[2],fpo);
                            fputc(iword[3],fpo);
                            lv->occnt = lv->occnt + 4;
                            break;
                default   : break;
            }
        }
    }
    fputc(0x00,fpo);    /* Put end of track */
    fputc(0xff,fpo);    /* Put end of track */
    fputc(0x2f,fpo);    /* Put end of track */
    fputc(0x00,fpo);    /* Put end of track */
    lv->occnt = lv->occnt + 4;
    if(ls->otrace == 8) {
        printf("       0 - 00             ff 2f 00     end of track\n");
    }
    return(0);
}

int dtxsub2(FILE *fpi,FILE *fpo)
//FILE *fpi;
//FILE *fpo;
{
    int  c;
    unsigned char wc[4];
    long k;

    k = 0;
    while((c = fgetc(fpi)) != EOF) {
        fputc(c,fpo);    /* Put data */
        k++;
        if(k == 18) {
            fgetc(fpi); /* length dummy read */
            fgetc(fpi); /* length dummy read */
            fgetc(fpi); /* length dummy read */
            fgetc(fpi); /* length dummy read */
            get_length(lv->occnt-22,wc);
            fputc(wc[0],fpo);
            fputc(wc[1],fpo);
            fputc(wc[2],fpo);
            fputc(wc[3],fpo);
            k = k + 4;
        }
    }
    return(0);
}

int LagOut(long dt,FILE *fpo)
//long dt;
//FILE *fpo;
{
    unsigned char wc[4];

    if(dt <= 0) {
        if(ls->otrace == 8) {
            printf("                      ");
        }
        return(0);
    }
    get_length(dt,wc);
    /*
    printf("len=%ld %02x %02x %02x %02x\n",
        lv->occnt,wc[0],wc[1],wc[2],wc[3]);
    */
    fputc(0xf1,fpo);
    wc[1] |= 0x80;    /* F1 code flag set */
    fputc(wc[1],fpo);
    fputc(wc[2],fpo);
    fputc(wc[3],fpo);
    lv->occnt = lv->occnt + 4;
    if(ls->otrace == 8) {
        printf("%8ld - f1 %02x %02x %02x",dt,wc[1],wc[2],wc[3]);
    }
    dt_all = dt_all + dt;  /* dt count check */
    return(0);
}

int velochg_mlk(unsigned char iword[4])
//unsigned char iword[4];
{

    int  i;
    int  pfg;
    float vr;
    int  vcf;

    lv->fdtyp = lc->c[1] & 0xf0;
    lv->fdtyp = lv->fdtyp >> 4;
    lv->fdch = lc->c[1] & 0x0f;
    vcf = 0;
    switch (lv->fdtyp) {
        case  9 : lv->keyp = lc->c[2] & 0xff;
                  lv->veon = lc->c[3] & 0xff;
                  vr = lv->mvelo / 1000.0;
                  lv->veon = lv->veon * vr;
                  if(lv->veon > 127) lv->veon = 127;
                  if(lv->veon < 0) lv->veon = 0;
                  iword[3] = lv->veon & 0xff;
                  vcf = 1;
                  break;
        default : break;
    }
    return(vcf);
}

int velomsg_mlk()
{
    float vr;

    vr = lv->mvelo / 10.0;
    if(ls->otrace == 8) {
        if(lv->veon) printf("     Ch.%d  Note on ",lv->fdch+1);
        else printf("     Ch.%d  Note off",lv->fdch+1);
        printf(" key# %d Val.%d(%4.1f%c)\n",lv->keyp,lv->veon,vr,'%');
    }
    return(0);
}
