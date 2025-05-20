
// ----------------------
//  AMuPLC (mlk7.c)
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
/* mlk7.c */
/* Not use 99.6 Backup */
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKCDE *lc;
extern MECMST *mm;
extern MLKFRS *lr;
extern MLKPHS *ph;
extern int  iv[16][5];
extern long cdt; /* Current dl */
extern long pdt;
extern int  pdf; /* pdt on/off flag */
extern int  ppf;
extern unsigned char MThd[14];
extern unsigned char MTrk[8];

/* make file fn.mid -dtxx optin variable tempo */
int mksmf0_val(char ifile[STLN])
//char ifile[STLN];
{
    char ofile[STLN];
    char wfile[STLN];
    FILE *fpi,*fpo,*fpw;
    int  i;

    if(ls->debug) printf("mksmf0_val() ifile=%s\n",ifile);
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
    strcat(wfile,".tmp");
    strcat(ofile,".mid");
    fpi = fopen_h(ifile,"rb");
    fpo = fopen_h(wfile,"wb");
    smf0sub1_dt(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close wfile */
    fpi = fopen_h(wfile,"rb");
    fpo = fopen_h(ofile,"wb");
    smf0sub2_dt(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close ofile */
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlksmf -sc %s -> %s %ld Bytes\n",ifile,ofile,lv->occnt);
    }
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlk(Variable Tempo) end SMF Format 0 Resolution %d\n",
        lv->smfdiv);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    return(0);
}

int smf0sub1_dt(FILE *fpi,FILE *fpo)
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
    float divx;  /* divx = 3000 / 2 * lv->smfdiv */
    int  nn,dd,cc,bb; /* Time signature */
    long tttt;   /* tempo micro sec */
    float tra;
    float rate;
    int  slag;

    divx = 5;   /* default 1/600 sec  dt = 300 */
    if(lv->smfdiv < 0) {
        MThd[12] = -lv->sflame; /* MThd division SMPTE format */
        MThd[13] = lv->mtcres;  /* MTC resolution */
        divx = 1;
    }
    else {
        if(lv->smfdiv == 3000) divx = 1;
        else divx = (float)1500 / lv->smfdiv;
            /* division = dt
              dt = lv->stlag/divx; 
                            _/ = 60     _/ = 120
              divx = 10    1/300 sec   (dt = 150)
              divx =  5    1/600 sec   (dt = 300)
              divx =  2    1/1000 sec  (dt = 500)
              divx =  1    1/3000 sec  (dt = 1500)

              dt = 60 * 600 / tempo 
              tempo : _/ = 120 -> ff 51 03 500000 
              ex. _/ = 60  : dt = 600 
                  _/ = 90  : dt = 400
                  _/ = 120 : dt = 300  <- default setting
           */
        get_length((long)6,wc); /* MThd length */
        /* get_length((long)0xff000000,wc); MThd length (test) */
        MThd[4] = wc[0];
        MThd[5] = wc[1];
        MThd[6] = wc[2];
        MThd[7] = wc[3];
        get_length((long)lv->smfdiv,wc);
        /*
        printf("len=%ld %02x %02x %02x %02x\n",
            (long)lv->smfdiv,wc[0],wc[1],wc[2],wc[3]);
        */
        MThd[12] = wc[2];  /* MThd division quarter-note format */
        MThd[13] = wc[3];
    }
    if(ls->debug == 8) {
        ls->dchk = 8;    /* SMF 0 debug */
        printf("MThd division =%02x %02x \n",MThd[12],MThd[13]);
    }
    get_length(lv->occnt,wc);
    /*
    printf("len=%ld %02x %02x %02x %02x\n",
        lv->occnt,wc[0],wc[1],wc[2],wc[3]);
    */
    MTrk[4] = wc[0];
    MTrk[5] = wc[1];
    MTrk[6] = wc[2];
    MTrk[7] = wc[3];
    for(i = 0; i < 14; i++) fputc(MThd[i],fpo); /* Put SMF Header Format 0 */
    lv->occnt = 14;
    if(ls->otrace == 8) {
        printf("\n           %02x %02x %02x %02x    MThd\n",
            MThd[0],MThd[1],MThd[2],MThd[3]);
        printf("           %02x %02x %02x %02x    chunk length\n",
            MThd[4],MThd[5],MThd[6],MThd[7]);
        printf("           %02x %02x          format 0\n",
            MThd[8],MThd[9]);
        printf("           %02x %02x          one track\n",
            MThd[10],MThd[11]);
        printf("           %02x %02x          division\n\n",
            MThd[12],MThd[13]);
    }
    for(i = 0; i < 8; i++) fputc(MTrk[i],fpo);  /* PutSMF_Track_chunk */
    lv->occnt = lv->occnt + 8;
    if(ls->otrace == 8) {
        printf("           %02x %02x %02x %02x    MTrk\n",
            MTrk[0],MTrk[1],MTrk[2],MTrk[3]);
        printf("           %02x %02x %02x %02x    chunk length (Set After)\n",
            MTrk[4],MTrk[5],MTrk[6],MTrk[7]);
        printf("\n   Delta-time             Event        Comments\n");
    }
    get_length(500000,wc);
    /*
    printf("len=%ld %02x %02x %02x %02x\n",
        lv->occnt,wc[0],wc[1],wc[2],wc[3]);
    */
    fputc(0x00,fpo);  /* delta time */
    fputc(0xff,fpo);  /* tempo */
    fputc(0x51,fpo);
    fputc(0x03,fpo);
    fputc(wc[1],fpo);
    fputc(wc[2],fpo);
    fputc(wc[3],fpo);
    lv->occnt = lv->occnt + 7;
    if(ls->otrace == 8) {
        printf("       0 - 00             ff 51 03 %02x %02x %02x ",
            wc[1],wc[2],wc[3]);
        printf("    tempo _/ = 120\n");
    }
    /* Title test
    fputc(0x00,fpo);
    fputc(0xff,fpo);
    fputc(0x03,fpo); 
    fputc(0x04,fpo);
    fputc('T',fpo);
    fputc('e',fpo);
    fputc('s',fpo);
    fputc('t',fpo);
    lv->occnt = lv->occnt + 8;
    */
    for(p = 0; p < MAXPT; p++) {
        lr->ftype[p] = 0;
        for(ff = 0; ff < 5; ff++) {
            lr->fchan[p][ff] = 0;     /* phrase channel */
            lr->fsval[p][ff] = 0;     /* phrase start value */
            lr->fctno[p][ff] = 0;     /* phrase control number */
            lr->feval[p][ff] = 0;     /* phrase end value */
            lr->fnval[p][ff] = 0;     /* phrase note value */
        }
    }
    for(e = 0; e < 16; e++) {
        for(f = 0; f < 5; f++) {
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
    rate = 1.0; /* start tempo _/ = 120  */
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            switch (iword[0]) {
                case 0xf1 : f1decode_mlk();
                            if(!lv->qfcnt) {
                                lv->qfsmp = lv->smpte;
                                compsmp1_mlk(lv->smpte);
                            }
                            lv->qfcnt++;
                            break;
                case 0xb1 : b1decode_mlk();
                            /* compsmp1_mlk(lv->smpte); */
                            fflg = lc->c[1] & 0x80;
                            if(!fflg) {
                                VarLen(dt,fpo);
                                smptfull_mlk(fpo);
                            }
                            break;
                case 0xf2 : f2decode_mlk();
                            break;
                case 0xfb : fbdecode_mlk();
                            if(ls->otrace == 8) {
                                printf("-m%02d%05d-___\n",
                                    lv->movn,lv->bnum);
                            }
                            break;
                case 0xf4 : /* Delta time */
                            f4decode_mlk();
                            slag = lv->stlag * rate;
                            dt = dt + slag/divx;
                            if(ls->debug) {
                                printf("dt=%ld lv->temp=%d rate=%f\n",
                                dt,lv->temp,rate);
                            }
                            /* 1/3000 sec -> 1/600 sec */
                            break;
                case 0xfd : /* fddecord_mlk(); */
                            VarLen(dt,fpo);
                            dt = 0;
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
                            fddec_mlk();
                            break;
                case 0xf6 : 
                            f6decode_mlk();
                            if(iword[1] == 0x02) {
                                if(iword[2] & 0x80) {
                                    if(iword[2] != 0xf7) {  /* Sys Ex. eof ? */
					 VarLen(dt,fpo);
                                         dt = 0;
				    }
                                }
                                fputc(iword[2],fpo);
                                if(iword[3] & 0x80) {
                                    if(iword[3] != 0xf7) {  /* Sys Ex. eof ? */
					 VarLen(dt,fpo);
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
					 VarLen(dt,fpo);
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
                            /*           
                            printf("lv->ritm = %d lv->time = %d\n",
                                lv->ritm,lv->time);
                            */  
                            fputc(0x00,fpo);    /* Put dt 0 */
                            fputc(0xff,fpo);    /* Put Time signature */
                            fputc(0x58,fpo);
                            fputc(0x04,fpo);
                            nn = 0x7f & lv->ritm;   
                            dd = 0x7f & lv->time;
                            i = 0;  
                            while(1) {
                                dd = dd /2;
                                if(dd <= 0) break;
                                i++;
                            }   
                            dd = i;
                            cc = 24;
                            bb = 8; 
                            /* printf("nn = %d dd = %d\n",nn,dd); */
                            fputc(nn,fpo);
                            fputc(dd,fpo);
                            fputc(cc,fpo);
                            fputc(bb,fpo);
                            lv->occnt = lv->occnt + 8;
                            if(ls->otrace == 8) {
                                printf("       0 - 00             ");
                                printf("ff 58 04 %02d %02d %02d %02d  ",
                                    nn,dd,cc,bb);
                                printf("Time signature nn dd cc bb\n");
                            }   
                            break;
                case 0xfa : fadecode_mlk();
                            tra = (float)6000/lv->temp;
                            tttt = 1000000 * tra;
                            get_length(tttt,wc);
                            rate = (float)lv->temp / 12000;
                            if(ls->debug) {
                                printf("len=%ld %02x %02x %02x %02x\n",
                                tttt,wc[0],wc[1],wc[2],wc[3]);
                            }
                            fputc(0x00,fpo);  /* delta time */
                            fputc(0xff,fpo);  /* tempo */
                            fputc(0x51,fpo);
                            fputc(0x03,fpo);
                            fputc(wc[1],fpo);
                            fputc(wc[2],fpo);
                            fputc(wc[3],fpo);
                            lv->occnt = lv->occnt + 7;
                            if(ls->otrace == 8) {
                                printf("       0 - 00             ");
                                printf("ff 51 03 %02x %02x %02x ",
                                    wc[1],wc[2],wc[3]);
                                printf("    tempo _/ = %f\n",
                                    (float)lv->temp/100.0);
                            }
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
                            break;
                case 0xdd : /* port , channel, member */
                            dddecode_mlk();      /* change dd to fd code */
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

int smf0sub2_dt(FILE *fpi,FILE *fpo)
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

