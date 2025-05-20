
// ----------------------
//  AMuPLC (mlk6.c)
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
/* -smf0 format 0 .mid */
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKCDE *lc;
extern MECMST *mm;
extern MLKFRS *lr;
extern MLKPHS *ph;
extern int iv[16][5];
extern long cdt; /* Current dl */
extern long pdt;
extern int  pdf; /* pdt on/off flag */
extern int  ppf;
unsigned char MThd[14] = {
   0x4d, 0x54, 0x68, 0x64,  /* MThd */
   0x00, 0x00, 0x00, 0x06,  /* chunk length */
   0x00, 0x00,              /* format 0 */
   0x00, 0x01,              /* one track */
   0xe2, 0x78               /* SMPTE 30 flame per sec resolution 120 */
};
unsigned char MTrk[8] = {
   0x4d, 0x54, 0x72, 0x6b,  /* MTrk */
   0x00, 0x00, 0x00, 0x00   /* chunk length 0 bytes */
};

/* make mex file fn.mid resolution 480 _/ = 120 */
int mksmf0_mid(char ifile[STLN])
//char ifile[STLN];
{
    char ofile[STLN];
    char wfile[STLN];
    FILE *fpi,*fpo,*fpw;
    int  i;

    if(ls->debug) {
        printf("mksmf0_mid() resolution %d ifile=%s ",
            lv->smfdiv,ifile);
        if(lv->fix) printf("fix tempo _/ = 120\n");
        else printf("Variable tempo\n");
    }
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
    smf0sub1(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close wfile */
    fpi = fopen_h(wfile,"rb");
    fpo = fopen_h(ofile,"wb");
    smf0sub2(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close ofile */
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlksmf %s -> %s %ld Bytes\n",ifile,ofile,lv->occnt);
    }
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlk end SMF Format 0 Resolution %d\n",lv->smfdiv);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    return(0);
}

int smf0sub1(FILE *fpi,FILE *fpo)
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
    int  f;
    float divx;  /* divx = 3000 / 2 * lv->smfdiv */
    int  mf;    /* minus flag */
    int  iv,nv;
    int  nn,dd,cc,bb; /* Time signature */
    long tttt;   /* tempo micro sec */
    float tra;
    float tune;
    float hosei;
    int  mod;
    float rate;
    float slag;
    int  sf;  /* sharp/flat */
    int  mi;  /* Major/Minor */

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
    if(ls->debug) printf("smf0sub1_() divx=%f lv->smfdiv=%d\n",
        divx,lv->smfdiv);
    if(ls->debug == 8) {
        ls->dchk = 8;    /* SMF 0 debug */
        printf("MThd division =%02x %02x\n",MThd[12],MThd[13]);
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
    /* delta time tempo _/ = 120 out
    fputc(0x00,fpo);
    fputc(0xff,fpo);
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
    */
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
        for(f = 0; f < 5; f++) {
            lr->fchan[p][f] = 0;     /* phrase channel */
            lr->fsval[p][f] = 0;     /* phrase start value */
            lr->fctno[p][f] = 0;     /* phrase control number */
            lr->feval[p][f] = 0;     /* phrase end value */
            lr->fnval[p][f] = 0;     /* phrase note value */
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
                            dt = dt + hosei;
                            /* hosei = 0; */
                            if(ls->otrace == 8) {
                                printf("-m%02d%05d-___\n",
                                    lv->movn,lv->bnum);
                            }
                            break;
                case 0xf4 : /* Delta time */
                            f4decode_mlk();
                            if(lv->fix) {
                                dt = dt + lv->stlag/divx;
                                /* 1/3000 sec -> 1/600 sec */
                                break;
                            }
                            tune = slag / divx;
                            mod = slag / divx;
                            tune = tune - mod;
                            hosei = hosei + tune;
                            if(hosei > 0) {
                                dt = dt + 1;
                                hosei = hosei - 1.0;
                            }
                            if(ls->debug) {
                                printf("hosei=%f tune=%f",
                                    hosei,tune);
                                printf(" mod=%d slag=%f divx=%f\n",
                                    mod,slag,divx);
                            }
                            slag = lv->stlag * rate;
                            dt = dt + slag/divx;
                            if(ls->debug) {
                                printf("dt=%ld lv->temp=%d rate=%f\n",
                                dt,lv->temp,rate);
                            }
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
                            printf("lv->ritm=%d lv->time=%d\n",
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
                            /* FF 59 02 sf mi    Key Signature */
                            /*
                            printf("lv->ksin=%d lv->kscl=%d\n",
                                lv->ksin,lv->kscl);
                            */
                            mi = 0;
                            if(lv->kscl == 1 || lv->kscl == 3) mi = 0;
                            if(lv->kscl == 2 || lv->kscl == 4) mi = 1;
                            fputc(0x00,fpo);    /* Put dt 0 */
                            fputc(0xff,fpo);    /* Put Time signature */
                            fputc(0x59,fpo);
                            fputc(0x02,fpo);
                            if(lv->ksin > 7) sf = 0xff & (-lv->ksin + 7);
                            else sf = 0x7f & lv->ksin;
                            fputc(sf,fpo);
                            fputc(mi,fpo);
                            lv->occnt = lv->occnt + 6;
                            if(ls->otrace == 8) {
                                printf("       0 - 00             ");
                                printf("ff 59 02 %02d %02d        ",sf,mi);
                                printf("Key Signature sf mi\n");
                            }
                            break;
                case 0xfa : fadecode_mlk();
                            if(lv->fix) break;
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
                case 0xe3 : e3decode_mlk();
                            break;
                case 0xe4 : 
                            e4decode_mlk();
                            p = lv->port;     /* port */
                            if(p >= MAXPT) break;
                            f = lr->ftype[p];
                            if(f > 4) f = 4;  /* Controller over */
                            e = lr->fchan[p][f];
                            if(e > 15 || e < 0) e = 0; /* Channel Error */
                            ph->cn[e][f] = lr->fctno[p][f];
                            fv = lr->feval[p][f] - lr->fsval[p][f];
                            ph->iv[e][f] = 1;
			    mf = 1;
                            if( fv < 0 ) {
				mf = -1;
                                ph->iv[e][f] = -1;
                                fv = lr->fsval[p][f] - lr->feval[p][f];
                            }
                            fl = lv->smfdiv * lr->fnval[p][f] / 100000;
                            ph->fl[e][f] = fl;
                            if( fv > fl) {
                                ph->iv[e][f] = ph->iv[e][f]*fv/fl; 
                                ph->dl[e][f] = 1;  /* dt = 1 */
                            }
                            if( fl > fv ) {
                                ph->dl[e][f] = fl/fv; /* dt */
                            }
			    if(lr->fxtyp[p][f] == 1) { /* Interval */
				iv = lr->fxval[p][f];
                                if(iv == 0) iv = 1;
                                if(mf < 0) ph->iv[e][f] = -iv;
				else ph->iv[e][f] = iv;
				nv = fv / iv;
                                if(nv == 0) nv = 1;
                                ph->dl[e][f] = fl / nv;
                            }
			    if(lr->fxtyp[p][f] == 2) { /* N_of_control_d */
                                nv = lr->fxval[p][f];
				nv = nv - 1;
                                if(nv == 0) nv = 1;
                                iv = iv * fv / nv;
                                ph->iv[e][f] = iv + 1; /* .1 -> 1 kuriage */
                                ph->dl[e][f] = fl / nv;
                            }
                            ph->cv[e][f] = lr->fsval[p][f];
                            ph->ev[e][f] = lr->feval[p][f];
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

int smf0sub2(FILE *fpi,FILE *fpo)
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

int VarLen(long dt,FILE *fpo)
//long dt;
//FILE *fpo;
{
    int  i,j;
    long d;
    long mindl;
    long mdl;

    if(dt < 0) dt = 0;
    if(dt == 0) {
        WriteVarLen(dt,fpo);
        return(0);
    }
    mindl = 600 * 36;  /* 1h */
    for(i = 0; i < 16; i++) {
        for(j = 0; j < 5 ; j++) {
            iv[i][j] = 0;
            if(ph->af[i][j]) {
                mdl = ph->dl[i][j];
                if( mdl > 0) {
                    if( mdl < mindl) mindl = mdl;
                }
            }
        }
    }
    if(ls->debug == 8) printf("dt=%ld mindl=%ld pdt=%ld\n",dt,mindl,pdt);
    if(pdt < mindl) {
        if(dt < mindl) {
            WriteVarLen(dt,fpo);
            pdt = pdt + dt;
            pdf = 1;
            ppf = 1;
            return(0);
        }
    }
    pdt = 0;
    cdt = 0;
    for(d = 0; d < dt; d++) {
        ph_afset();
        if(!pdf) {  /* pdt flag check */
            if(cdt >= dt) break;
        }
        else pdf = 0;
        /* ph_varlen(dt,fpo); */
    }
    if(ls->debug == 8) printf("cdt=%ld dt=%ld\n",cdt,dt);
    if(cdt >= 0) cdt = dt - cdt;
    WriteVarLen(cdt,fpo);
    return(0);
}

int ph_varlen(long dt,FILE *fpo)
//long dt;
//FILE *fpo;
{
    int  i,j;
    int  af;
    long wdt;

    af = 0;
    for(i = 0; i < 16; i++) {
        for(j = 0; j < 5 ; j++) {
            if(ph->af[i][j]) af++;
        }
    }
    if(!af) return(1);
    /* printf("af=%d\n",af); */
    for(i = 0; i < 16; i++) {
        for(j = 0; j < 5 ; j++) {
            if(ph->af[i][j]) {
                wdt = cdt + ph->dl[i][j];
                if(wdt <= dt) {
                    if(ph->cp[i][j]) {
                        cdt = cdt + ph->dl[i][j];
                        WriteVarLen((long)ph->dl[i][j],fpo);
                    } else WriteVarLen((long)0,fpo); /* Farst */
                    ph_out(i+1,j,fpo);
                    ph->cp[i][j] = ph->cp[i][j] + ph->dl[i][j];
                    ph->cv[i][j] = ph->cv[i][j] + ph->iv[i][j];
                    iv[i][j] = 0;
                    ph->af[i][j] = 0;
                }
                else {
                    if(ppf) {
                        WriteVarLen((long)0,fpo);
                        ph_out(i+1,j,fpo);
                        ph->cp[i][j] = ph->cp[i][j] + ph->dl[i][j];
                        ph->cv[i][j] = ph->cv[i][j] + ph->iv[i][j];
                        iv[i][j] = 0;
                        ph->af[i][j] = 0;
                        ppf = 0;
                    }
                }
            }
        }
    }
    return(0);
}

/* Phrase Active Flag Set */
int ph_afset()
{
    int  i,j;
    int  af;

    for(i = 0; i < 16; i++) {
        for(j = 0; j < 5 ; j++) {
            af = 0;
            if(ph->iv[i][j] > 0) { /* positiv */
                iv[i][j]++;
                if(iv[i][j] >= ph->iv[i][j]) af++;
                if(ph->cv[i][j] <= ph->ev[i][j]) af++;
                else {
                    af = af - 1;
                    ph_clear(i,j);
                }
            }
            if(ph->iv[i][j] < 0) { /* negativ */
                iv[i][j]--;
                if(iv[i][j] <= ph->iv[i][j]) af++;
                if(ph->cv[i][j] > ph->ev[i][j]) af++;
                else {
                    af = af - 1;
                    ph_clear(i,j);
                }
            }
            /*
            if(ph->cp[i][j] < ph->fl[i][j]) af++;
            else {
                af = af - 1;
                ph_clear(i,j);
            }
            */
            if(af == 2) {
                ph->af[i][j] = 1; /* Terget Active Flag Set */
            }
        }
    }
    return(0);
}

int ph_clear(int e,int f)
//int  e;
//int  f;
{
    ph->cn[e][f] = 0;       /* phrase control number */
    ph->fl[e][f] = 0;       /* phrase length */
    ph->iv[e][f] = 0;       /* phrase increment value */
    ph->dl[e][f] = 0;       /* phrase time interval */
    ph->cv[e][f] = 0;       /* phrase current value */
    ph->ev[e][f] = 0;       /* phrase end value */
    ph->cp[e][f] = 0;       /* phrase current point */
    ph->af[e][f] = 0;       /* phrase activ on/off flag */
    return(0);
}

/* Phrasing code out */
int ph_out(int channel,int f,FILE *fpo)
//int  channel;   /* channel */
//int  f;   /* flase */
//FILE *fpo;
{
    int  bit14;
    int  bx;
    int  cno;
    int  msb;
    int  lsb;
    static int wmsb = 0;
    int  port;
    int  e;
    int  fctn;   /* Phrase control number */

    e = channel - 1;
    if(f == 2) {  /* f=0:Volume f=1:Controle Change f=2:Pitch Vend */
        pitch_smf0(e,f,fpo);
        return(0);
    }
    bit14 = 0;
    if((ph->cv[e][f] > 127) || (ph->ev[e][f] > 127)) bit14 = 1;
    if(bit14) {   /* 14 bit code */
        bx = 0x0f & e;
        bx |= 0x00b0;
        msb = ph->cv[e][f] / 128;  /* MSB */
        cno = 0x7f & ph->cn[e][f];
        if(msb != wmsb) {
            wmsb = msb;
            fputc((char)bx,fpo);   /* Bx */
            fputc((char)cno,fpo);  /* control # */
            fputc((char)msb,fpo);
            lv->occnt = lv->occnt + 3;
        }
        fputc((char)bx,fpo);
        fputc((char)(cno+32),fpo);
        lsb = ph->cv[e][f] - (msb * 128);  /* LSB */
        fputc((char)lsb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 8) {
            if(msb != wmsb) {
                printf("    %02x %02x %02x ",bx,cno,msb);
                printf("%02x %02x %02x    ",bx,cno+32,lsb);
                printf(" Ch.%d  ",channel);
            }
            else {
                printf("    %02x %02x %02x    ",bx,cno+32,lsb);
                printf(" Ch.%d  ",channel);
            }
        }
    }
    else {
        bx = 0x0f & e;
        bx |= 0x00b0;
        fputc((char)bx,fpo); /* Bx */
        cno = 0x7f & ph->cn[e][f];
        fputc((char)cno,fpo);  /* control # */
        msb = 0x007f & ph->cv[e][f]; /* MSB */
        fputc((char)msb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 8) {
            printf("    %02x %02x %02x    ",bx,cno,msb);
            printf(" Ch.%d  ",channel);
        }
    }
    if(ls->otrace == 8) {
        fctn = 0;
        switch(f) {
            case  0 : fctn = 7; /* Volume */
                      break;
            case  1 : fctn = 1; /* Modulation */
                      break;
            case  2 : fctn = 150; /* Pitch Vend */
                      break;
            case  3 : fctn = 11; /* Expression */
                      break;
            case  4 : fctn = ph->cn[e][f]; /* Controle Change */
                      break;
            default : break;
        }
        ph_outmsg(fctn);
        printf(" Val.%d\n",ph->cv[e][f]);
    }
    return(0);
}

int ph_outmsg(int fctn)
//int  fctn;   /* Phrase control number */
{
    switch (fctn) {
        case   1 : printf("Modulation ");
                   break;
        case   2 : printf("Breath control");
                   break;
        case   4 : printf("Foot controller");
                   break;
        case   5 : printf("Poltament time");
                   break;
        case   6 : printf("Data entry ");
                   break;
        case   7 : printf("Main volume");
                   break;
        case   8 : printf("Balance ");
                   break;
        case  10 : printf("Panpot ");
                   break;
        case  11 : printf("Expression ");
                   break;
        case  64 : printf("Damper pedal");
                   break;
        case  65 : printf("Portamento ");
                   break;
        case  66 : printf("Sustenute  ");
                   break;
        case  67 : printf("Soft pedal ");
                   break;
        case  69 : printf("Hold2 ");
                   break;
        case  91 : printf("External efect depth");
                   break;
        case  92 : printf("Toremolo depth");
                   break;
        case  93 : printf("Chorus depth");
                   break;
        case  94 : printf("Seleste depth");
                   break;
        case  95 : printf("Feather depth");
                   break;
        default  : if(fctn > 7 && fctn < 64) printf("Continuous controller");
                   if(fctn > 67 && fctn < 91) printf("Undefined ");
                   printf("No.%d ",fctn);
                   break;
    }
    return(0);
}

int pitch_smf0(int channel,int f,FILE *fpo)     /* pitch vend */
//int  channel;
//int  f;
//FILE *fpo;
{
    int  ex;
    int  msb;
    int  lsb;
    int  e;

    ex = 0x0f & channel; /* Channel */
    ex |= 0x00e0;
    e = channel - 1;
    msb = ph->cv[e][f] / 128;  /* MSB */
    fputc((char)ex,fpo);   /* Ex */
    fputc((char)msb,fpo);
    lsb = ph->cv[e][f] - (msb * 128);  /* LSB */
    fputc((char)lsb,fpo);
    lv->occnt = lv->occnt + 3;
    if(ls->otrace == 8) {
        printf("    %02x %02x %02x    ",ex,msb,lsb);
        printf(" Ch.%d  Pitch vend  Val.%d\n",channel,ph->cv[e][f]);
    }
    return(0);
}
/*   FD code ....................................
Note off      : FD8xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(voff)     : verociti off (0-120)

Note on       : FD9xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(veon)     : verociti on : dynamik(0-9)

Pol.key.p.    : FDAxyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(kprs)     : key pressure

Cont.change   : FDBxyyzz
 x(chan)      : channel no.
 yy(cntn)     : control no.
 zz(cntv)     : control value

Prog.change   : FDCxyyzz
 x(chan)      : channel no.
 yy(prgn)     : program number
 zz(prgv)     : program change val (??)

Chan.pres.    : FDDxyyzz (after tatch)
 x(chan)      : channel no.
 yy(ptyp)     : pressure type(vib. etc.)
 zz(prsv)     : pressure value
 ex. 4<  3>  2v

Pitch, vend.   : FDExyyzz
 x(chan)      : channel no.
 yy(pvls)     : vvvvvvv LSB val.
 zz(pvms)     : uuuuuuuu MSB val.
*/
int fddec_mlk()
{
    int  i;
    int  pfg;

    lv->fdtyp = lc->c[1] & 0xf0;
    lv->fdtyp = lv->fdtyp >> 4;
    lv->fdch = lc->c[1] & 0x0f;
    switch (lv->fdtyp) {
        case  8 : lv->keyp = lc->c[2] & 0xff;
                  lv->voff = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      printf("     Ch.%d  Note off",lv->fdch+1);
                      printf(" key# %d Val.%d\n",lv->keyp,lv->voff);
                  }
                  break;
        case  9 : lv->keyp = lc->c[2] & 0xff;
                  lv->veon = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      if(lv->veon) printf("     Ch.%d  Note on ",lv->fdch+1);
                      else printf("     Ch.%d  Note off",lv->fdch+1);
                      printf(" key# %d Val.%d\n",lv->keyp,lv->veon);
                  }
                  break;
        case 10 : lv->keyp = lc->c[2] & 0xff;
                  lv->kprs = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      printf("     Ch.%d  Pol.key pressure",lv->fdch+1);
                      printf(" key# %d Val.%d\n",lv->keyp,lv->kprs);
                  }
                  break;
        case 11 : lv->cntn = lc->c[2] & 0xff;
                  lv->cntv = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      printf("     Ch.%d  ",lv->fdch+1);
                      ph_outmsg(lv->cntn);
                      printf(" Val.%d\n",lv->cntv);
                  }
                  break;
        case 12 : lv->prgn = lc->c[2] & 0xff;
                  if(ls->otrace == 8) {
                      printf("        Ch.%d  Program change",lv->fdch+1);
                      printf("  No.%d\n",lv->prgn+1);  /* +1 add 2005.9.9 */
                  }
                  break;
        case 13 : lv->ptyp = lc->c[2] & 0xff;
                  lv->prsv = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      printf("        Ch.%d  Channel pressure",lv->fdch+1);
                      printf("  pty# %d Val.=%d\n",lv->ptyp,lv->prsv);
                  }
                  break;
        case 14 : lv->pvls = lc->c[2] & 0xff;
                  lv->pvms = lc->c[3] & 0xff;
                  if(ls->otrace == 8) {
                      printf("     Ch.%d  Pitch vend",lv->fdch+1);
                      printf("  LSB=%d MSB=%d\n",lv->pvls,lv->pvms);
                  }
                  break;
        default : break;
    }
    return(0);
}

int WriteVarLen(register long value,FILE *fpo)
//register long value;
//FILE *fpo;
{
    register long buffer;
    int  i;

    if(ls->otrace == 8) printf("%8ld -",value);
    buffer = value & 0x7f;
    while((value >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value & 0x7f);
    }
    i = 0;
    while(1) {
        fputc(buffer,fpo);
        lv->occnt++;
        i++;
        if(ls->otrace == 8)
            printf(" %02x",(unsigned char)buffer);
        if(buffer & 0x80) buffer >>= 8;
        else break;
    }
    if(ls->otrace == 8) {
        if(i == 1) printf("         ");
        if(i == 2) printf("      ");
        if(i == 3) printf("   ");
    }
    return(0);
}

int get_length(long len,unsigned char iword[4])
//long len;
//unsigned char iword[4];
{
    union MCODE u;

    u.wl = len;
    if(ls->comptyp == 0) {    /* MSDOS */
        iword[3] = u.wc[0];
        iword[2] = u.wc[1];
        iword[1] = u.wc[2];
        iword[0] = u.wc[3];
    }
    if(ls->comptyp == 1) {    /* SUN ? */
        iword[0] = u.wc[0];
        iword[1] = u.wc[1];
        iword[2] = u.wc[2];
        iword[3] = u.wc[3];
    }
    if(ls->comptyp == 2) {    /* DEC VMS ? */
        iword[3] = u.wc[0];
        iword[2] = u.wc[1];
        iword[1] = u.wc[2];
        iword[0] = u.wc[3];
    }
    return(0);
}
