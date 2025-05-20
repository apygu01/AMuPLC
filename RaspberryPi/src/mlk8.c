
// ----------------------
//  AMuPLC (mlk8.c)
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
/* mlk8.c  New .mex */
int  mpiv[MAXPT][16][5]; /* Multi port iv */
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKCDE *lc;
extern MLKFRS *lr;
extern MLKPHS *ph;
extern MLKMPH *mp;
extern long cdt; /* Current dl */
extern long pdt;
extern int  pdf; /* pdt on/off flag */
extern int  ppf;
extern unsigned char MXhd[14];
extern unsigned char MXrk[8];
extern long dt_all; /* dt count */
long xdt;
extern int  ckport;
extern int  cktrak;
extern int  ckmemb;

/* -new option : new mex phrasing code out */
/* make mex file fn.mex resolution 1/3000 sec */
int newmex_mlk(char ifile[STLN])
//char ifile[STLN];
{
    char ofile[STLN];
    char wfile[STLN];
    char tfile[STLN];
    FILE *fpi,*fpo,*fpw;
    int  i;
    int  hh,mm,ss,ff;

    if(ls->debug) printf("newmex_() ifile=%s\n",ifile);
    dt_all = 0;
    lv->occnt = 0;
    lv->qfcnt = 0;
    lv->qfsmp = 0;
    ckport = 0x0f;
    cktrak = 0xff;
    ckmemb = 0xff;
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
    strcat(tfile,".tmp");
    strcat(ofile,".mex");
    fpi = fopen_h(ifile,"rb");
    fpo = fopen_h(wfile,"wb");
    nexsub1(fpi,fpo);
    fclose(fpi);   /* close ifile */
    fclose(fpo);   /* close wfile */
    ls->msglvl = 0;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlknew %s -> %s %ld Bytes dt_all %ld ",
        ifile,ofile,lv->occnt,dt_all);
        printf("Resolution 1/%.1f Sec\n",lv->mexres);
    }
    if(ls->optm == 0) {
        fpi = fopen_h(wfile,"rb");
        fpo = fopen_h(ofile,"wb");
        nexsub3(fpi,fpo);
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
        nexsub3(fpi,fpo);
        fclose(fpi);   /* close ifile */
        fclose(fpo);   /* close ofile */

        remove(ifile);
        remove(wfile);
        remove(tfile);
    }
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        dtconv_mlk(dt_all,&hh,&mm,&ss,&ff);
        printf("Mlk end %s ",ofile);
        printf("%ld Bytes Time %d:%d:%d/%d ",
            lv->occnt,hh,mm,ss,ff);
        printf("Resolution 1/%.1f Sec\n",lv->mexres);
    }
    if(ls->odump == 1) outcdump_mlk(ofile);
    return(0);
}

int nexsub1(FILE *fpi,FILE *fpo)
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
    float fdiv;  /* phrase length work */
    int  p;
    long dt;    /* delta time */
    int  f;
    int  divx;  /* divx = 1: default 1/100 flame */
    int  iv,nv;
    int  mf;
    int  vcf;   /* velocity change flag */
    int  f6f;

    if(ls->debug) printf("nexsub1_()\n");
    lv->mtcres = lv->mexres / lv->sflame;
    if(lv->mtcres > 127) {
        lv->mtcres = 127;
        lv->sflame = lv->mexres / 127;
        ls->wrncnt++;
        ls->msglvl = 4;
        if(ls->msglvl >= ls->prtlvl)
        printf("Worning! Division over than 127 change flame to %d\n",
            lv->sflame);  
    }
    lv->mlp = lv->mexres / (lv->sflame*100.0);
    if(ls->debug) 
        printf("lv->sflame=%d lv->mtcres=%d lv->mexres=%.1f lv->mlp=%.1f\n",
        lv->sflame,lv->mtcres,lv->mexres,lv->mlp);
    MXhd[12] = -lv->sflame & 0xff; /* MXhd division SMPTE format */
    MXhd[13] = lv->mtcres & 0xff;  /* MTC resolution */
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
        for(f = 0; f < 5; f++) {
            lr->fchan[p][f] = 0;     /* phrase channel */
            lr->fsval[p][f] = 0;     /* phrase start value */
            lr->fctno[p][f] = 0;     /* phrase control number */
            lr->feval[p][f] = 0;     /* phrase end value */
            lr->fnval[p][f] = 0;     /* phrase note value */
            lr->factf[p][f] = 0;     /* phrase act flag off */
            lr->fxtyp[p][f] = 0;     /* phrase extention type */
            lr->fxval[p][f] = 0;     /* phrase extention value */
        }
    }
    for(p = 0; p < MAXPT; p++) {
        for(e = 0; e < 16; e++) {
            for(f = 0; f < 5; f++) {
                mp->ct[p][e][f] = 0;       /* phrase current delta time */
                mp->cn[p][e][f] = 0;       /* phrase control number */
                mp->fl[p][e][f] = 0;       /* phrase length */
                mp->iv[p][e][f] = 0;       /* phrase increment value */
                mp->dl[p][e][f] = 0;       /* phrase time interval */
                mp->cv[p][e][f] = 0;       /* phrase current value */
                mp->ev[p][e][f] = 0;       /* phrase end value */
                mp->cp[p][e][f] = 0;       /* phrase current point */
                mp->af[p][e][f] = 0;       /* phrase activ on/off flag */
		mp->pm[p][e][f] = 0;       /* phrase member */
		mp->pt[p][e][f] = 0;       /* phrase track  */
		mp->xt[p][e][f] = 0;       /* phrase extention type */
		mp->xv[p][e][f] = 0;       /* phrase extention value */
            }
        }
    }
    k = 0;
    j = 0;
    dt = 0;
    pdt = 0;
    pdf = 0;
    ppf = 0;
    f6f = 0;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            ctoval_mlk(iword);   /* set code to value */
            k = 0;
            if(ls->otrace == 8 && f6f == 1) {
                if(iword[0] != 0xf6) printf("\n");
                f6f = 0;
            }
            switch (iword[0]) {
                case 0xb1 : b1decode_mlk();
                            fflg = lc->c[1] & 0x80;
                            if(!fflg) {
                                // Ph_Lag(dt,fpo); // 2019.3.30
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
                            Ph_Lag(dt,fpo);
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
                                        if(ls->otrace == 8) printf("\n");
                                        Ph_Lag(dt,fpo);
                                        dt = 0;
                                    }
                                }
                                fputc(iword[2],fpo);
                                if(iword[3] & 0x80) {
                                    if(iword[3] != 0xf7) {  /* Sys Ex. eof ? */
                                        Ph_Lag(dt,fpo);
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
                                        Ph_Lag(dt,fpo);
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
                                f6f = 0;
                            }
                            else f6f = 1;
                            break;
                case 0xf8 : f8decode_mlk();
                            /* add 2007.6 */
                            if(ls->score) {
                                iword[1] |= 0x80;  /* f8 code flag set */
                                fputc(iword[0],fpo);
                                fputc(iword[1],fpo);
                                fputc(iword[2],fpo);
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 4;
                                if(ls->otrace == 8) {
                                    printf("                        ");
                                    printf("  %02x %02x %02x %02x",
                                    iword[0],iword[1],iword[2],iword[3]);
                                    printf(
                                    "  kscl %d  ksin %d  ritm %d  time %d \n",
                                    lv->kscl,lv->ksin,lv->ritm,lv->time);
                                }
                            }
                            break;
                case 0xfa : fadecode_mlk();
                            /* add 2007.6 */
                            if(ls->score) {
                                if(lv->tsym > 21) break;
                                iword[1] |= 0x80;  /* fa code flag set */
                                fputc(iword[0],fpo);
                                fputc(iword[1],fpo);
                                fputc(iword[2],fpo);
                                fputc(iword[3],fpo);
                                lv->occnt = lv->occnt + 4;
                                if(ls->otrace == 8) {
                                    printf("                        ");
                                    printf("  %02x %02x %02x %02x",
                                    iword[0],iword[1],iword[2],iword[3]);
                                    printf(
                                    "  Note type %d  tempo %d\n",
                                    lv->tsym,lv->temp);
                                }
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
                            break;
                case 0xe4 : 
                            e4decode_mlk();
                            p = lv->port;     /* port */
                            if(p >= MAXPT) break;
                            f = lr->ftype[p];
                            if(f > 4) f = 4; /* controller over */
                            /* e = lr->fchan[p][f] - 1; */
                            e = lr->fchan[p][f];
                            if(e > 15 || e < 0) e = 0; /* Channel Error */
                            mp->cn[p][e][f] = lr->fctno[p][f];
                            fv = lr->feval[p][f] - lr->fsval[p][f];
			    iv = 1;
                            if( fv < 0 ) {
                                iv = -iv;
                                fv = lr->fsval[p][f] - lr->feval[p][f];
                            }
                            /* fl = lv->smfdiv * lr->fnval[p][f] / 100000; */
                            /*
                            fdiv = lv->mlp * 1500;
			    fl = fdiv * lr->fnval[p][f] / 100000;
                            */
			    fl = lv->mexres * lr->fnval[p][f] / 100000;
                            mp->fl[p][e][f] = fl;
                            if( fv > fl) {
                                mp->iv[p][e][f] = iv * fv / fl; 
                                mp->dl[p][e][f] = 1;  /* dl = 1 */
                            }
                            else {
                                /*
                                mp->iv[p][e][f] = iv * fv / 30; 
                                mp->dl[p][e][f] = fl / 30;
                                */
                                mp->iv[p][e][f] = iv * fv / lv->sflame; 
                                mp->dl[p][e][f] = fl / lv->sflame;

                            }
			    if(lr->fxtyp[p][f] == 1) { /* Interval */
                                mf = 1;
                                if(iv < 0) mf = -1;
				iv = lr->fxval[p][f];
                                if(iv == 0) iv = 1;
                                mp->iv[p][e][f] = iv * mf;
				nv = fv / iv;
                                if(nv == 0) nv = 1;
                                mp->dl[p][e][f] = fl / nv;
                            }
			    if(lr->fxtyp[p][f] == 2) { /* N_of_control_d */
                                nv = lr->fxval[p][f];
				nv = nv - 1;
			        if(nv == 0) nv = 1;
                                iv = iv * fv / nv;
				if(iv < 0) mp->iv[p][e][f] = iv - 1;
                                else mp->iv[p][e][f] = iv + 1; /*  kuriage */
                                mp->dl[p][e][f] = fl / nv;
                            }
                            mp->ct[p][e][f] = dt_all; /* current dt_all */
                            mp->cv[p][e][f] = lr->fsval[p][f];
                            mp->ev[p][e][f] = lr->feval[p][f];
			    mp->pm[p][e][f] = lv->memb;
			    mp->pt[p][e][f] = lv->trak;
                            mp->af[p][e][f] = 1;
                            if(ls->otrace == 8) {
                              if(f == 0) printf("Volume");
                              if(f == 1) ph_outmsg(mp->cn[p][e][f]);
                              if(f == 2) printf("Pitch vend");
                              printf("[%d] Ch.%d St/End:%d/%d ",
				  p,e+1,mp->cv[p][e][f],mp->ev[p][e][f]);
                              printf(
				  "Inc.%d dl.%d Len.%ld Member %d Track %d\n",
                                  mp->iv[p][e][f],mp->dl[p][e][f],
				  mp->fl[p][e][f],lv->memb,lv->trak);
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
                            if(iword[1] == ckport && iword[2] == cktrak &&
                            iword[3] == ckmemb) break;
                            ckport = iword[1];
                            cktrak = iword[2];
                            ckmemb = iword[3];
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
    Ph_Lag(dt,fpo);
    /* fputc(0x00,fpo);    Put end of track */
    fputc(0xff,fpo);    /* Put end of track */
    fputc(0x2f,fpo);    /* Put end of track */
    fputc(0x00,fpo);    /* Put end of track */
    lv->occnt = lv->occnt + 4;
    if(ls->otrace == 8) {
        /* printf("       0 - 00             ff 2f 00     end of track\n"); */
        printf("    ff 2f 00     end of track\n");
    }
    return(0);
}

/* remove fd 80 xx xx */
int nexsub2(FILE *fpi,FILE *fpo)
//FILE *fpi;
//FILE *fpo;
{
    int  c;
    unsigned char wc[4];
    long k;
    int  j;
    int  c1[100];
    int  c2[100];
    int  c3[100];
    int  c4[100];
    static int pt,tk,mb;
    int  fdchk;
    int  t2,t3,t4;

    k = 0;
    j = 0;
    fdchk = 0;
    lv->occnt = 0;
    if(ls->debug) printf("nexsub2_()\n");
    while((c = fgetc(fpi)) != EOF) {
        k++;
	switch(c) {
	    case  0xf1 : /* f1 xx xx xx ? */
                         t2 = fgetc(fpi);
	                 if(t2 & 0x80) {
                             t3 = fgetc(fpi);
                             t4 = fgetc(fpi);
                             fputc(c,fpo);
                             fputc(t2,fpo);
                             fputc(t3,fpo);
                             fputc(t4,fpo);
	                     if(ls->debug == 9) printf("%02x %02x %02x %02x\n",
	                         c,t2,t3,t4);
                             lv->occnt = lv->occnt + 4;
	                 }
	                 else {
		             fputc(c,fpo);
                             fputc(t2,fpo);
	                     if(ls->debug == 9) printf("%02x %02x ",c,t2);
                             lv->occnt = lv->occnt + 2;
	                 }
			 break;
	    case  0xfb : /* fb xx xx xx ? */
                         t2 = fgetc(fpi);
	                 if(t2 & 0x80) {
                             t3 = fgetc(fpi);
                             t4 = fgetc(fpi);
                             fputc(c,fpo);
                             fputc(t2,fpo);
                             fputc(t3,fpo);
                             fputc(t4,fpo);
	                     if(ls->debug == 9) printf("%02x %02x %02x %02x\n",
	                         c,t2,t3,t4);
                             lv->occnt = lv->occnt + 4;
	                 }
	                 else {
		             fputc(c,fpo);
                             fputc(t2,fpo);
	                     if(ls->debug == 9) printf("%02x %02x ",c,t2);
                             lv->occnt = lv->occnt + 2;
	                 }
			 break;
	    case  0xe6 : /* e6 xx xx xx ? */
                         t2 = fgetc(fpi);
	                 if(t2 & 0x80) {
                             t3 = fgetc(fpi);
                             t4 = fgetc(fpi);
                             fputc(c,fpo);
                             fputc(t2,fpo);
                             fputc(t3,fpo);
                             fputc(t4,fpo);
	                     if(ls->debug == 9) printf("%02x %02x %02x %02x\n",
	                         c,t2,t3,t4);
                             lv->occnt = lv->occnt + 4;
	                 }
	                 else {
		             fputc(c,fpo);
                             fputc(t2,fpo);
	                     if(ls->debug == 9) printf("%02x %02x ",c,t2);
                             lv->occnt = lv->occnt + 2;
	                 }
			 break;
	     case 0xfd : /* fd 8x xx xx Port Track Member code Check */
                         c1[j] = c;
                         c2[j] = fgetc(fpi);
                         c3[j] = fgetc(fpi);
                         c4[j] = fgetc(fpi);
                         if(c2[j] & 0x80) {
		             if(ls->debug == 9)
				 printf("(%02x %02x %02x %02x)\n",
		                 c1[j],c2[j],c3[j],c4[j]);
		             if(c2[j] > 0x8f) fdchk++;  /* Max port 16 = 8f */
                             j++;
		             if(fdchk) {
                                 fputc(c1[j-1],fpo);
                                 fputc(c2[j-1],fpo);
                                 fputc(c3[j-1],fpo);
                                 fputc(c4[j-1],fpo);
		                 if(ls->debug == 9)
				     printf("%02x %02x %02x %02x\n",
		                     c1[j-1],c2[j-1],c3[j-1],c4[j-1]);
                                 lv->occnt = lv->occnt + 4;
		                 fdchk = 0;
		                 j = 0;
				 break;
		             }
                             if(j >= 100) {
                                 fputc(c1[j-1],fpo);
                                 fputc(c2[j-1],fpo);
                                 fputc(c3[j-1],fpo);
                                 fputc(c4[j-1],fpo);
		                 if(ls->debug == 9)
				     printf("%02x %02x %02x %02x\n",
			                 c1[j-1],c2[j-1],c3[j-1],c4[j-1]);
                                 lv->occnt = lv->occnt + 4;
                                 j = 0;
                                 break;
                             }
                         }
                         else {
                             fputc(c1[j],fpo);
                             fputc(c2[j],fpo);
                             fputc(c3[j],fpo);
                             fputc(c4[j],fpo);
		             if(ls->debug == 9)
			         printf("%02x %02x %02x %02x\n",
		                     c1[j],c2[j],c3[j],c4[j]);
                             lv->occnt = lv->occnt + 4;
                             j = 0;
                         }
			 break;
	     default   : /*   */
	                 if(j) {
		             if(c2[j-1] != pt || c3[j-1] != tk ||
				 c4[j-1] != mb) { 
                                 fputc(c1[j-1],fpo);
                                 fputc(c2[j-1],fpo);
                                 fputc(c3[j-1],fpo);
                                 fputc(c4[j-1],fpo);
		                 if(ls->debug == 9)
		               	     printf("%02x %02x %02x %02x\n",
		                         c1[j-1],c2[j-1],c3[j-1],c4[j-1]);
                                 lv->occnt = lv->occnt + 4;
		                 pt = c2[j-1];
		                 tk = c3[j-1];
		                 mb = c4[j-1];
		             }
		             j = 0;
	                 }
                         fputc(c,fpo);    /* Put data */
	                 if(ls->debug == 9) printf("%02x ",c);
                         lv->occnt++;
			 break;
        }
    }
    return(0);
}

int nexsub3(FILE *fpi,FILE *fpo)
//FILE *fpi;
//FILE *fpo;
{
    int  c;
    unsigned char wc[4];
    long k;

    k = 0;
    if(ls->debug) printf("nexsub3_()\n");
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

int Ph_Lag(long dt,FILE *fpo)
//long dt;
//FILE *fpo;
{
    unsigned char iword[4];
    int  i,j,p;
    long d;
    long mindl;
    long mdl;
    long cct;

    if(ls->debug == 8) {
        printf("Ph_Lag Cullent port=%d trak=%d memb=%d\n",
            lv->port,lv->trak,lv->memb);
    }
    if(dt == 0) {
        LagOut(dt,fpo);
        return(0);
    }
    /* mindl = 600 * 36;  1h */
    mindl = 9999999;  /* ? */
    for(p = 0; p < MAXPT; p++) {
        for(i = 0; i < 16; i++) {
            for(j = 0; j < 5 ; j++) {
                mpiv[p][i][j] = 0;
                if(mp->af[p][i][j]) {
                    mdl = mp->dl[p][i][j];
                    if( mdl > 0) {
                        if( mdl < mindl) mindl = mdl;
                    }
                }
            }
        }
    }
    if(ls->debug == 8) printf("dt=%ld mindl=%ld pdt=%ld\n",dt,mindl,pdt);
    if(pdt < mindl) {
        if(dt < mindl) {
            LagOut(dt,fpo);
            pdt = pdt + dt;
            pdf = 1;
            ppf = 1;
            return(0);
        }
    }
    pdt = 0;
    cdt = 0;
    xdt = 0;
    cct = dt_all;
    for(d = 0; d < dt; d++) {
        mp_afset();
        ckmp_varlen(cct+d,fpo);
	xdt++;
    }
    // add 2013.11.2 ===================
    iword[1] = 0xff & lv->port;
    iword[2] = 0xff & lv->trak;
    iword[3] = 0xff & lv->memb;
    if(ls->otrace == 8) {
        printf("                        ");
        printf("  fd %02x %02x %02x",
        iword[1] | 0x80,iword[2],iword[3]);
        printf("  Port[%d] Track %d Member %d\n",
        iword[1],iword[2],iword[3]);
    }
    fputc(0xfd,fpo);
    iword[1] |= 0x80;      // fd code flag set
    fputc(iword[1],fpo);
    fputc(iword[2],fpo);
    fputc(iword[3],fpo);
    lv->occnt = lv->occnt + 4;
    // printf("\nPass===============\n");

    /* printf("dt_all=%ld xdt=%ld cdt=%ld dt=%ld\n",dt_all,xdt,cdt,dt); */
    if(xdt) {  /* nokori */
	LagOut(xdt,fpo);
    }
    return(0);
}

int ckmp_varlen(long cct,FILE *fpo)
//long cct;  /* Current ck dt */
//FILE *fpo;
{
    int  i,j,k;

    for(i = 0; i < MAXPT; i++) {
        for(j = 0; j < 16; j++) {
            for(k = 0; k < 5 ; k++) {
                if(mp->af[i][j][k]) {
                    if(mp->ct[i][j][k] <= cct) {
                        mp_varlen(i,j,k,cct,fpo);
			xdt = 0;
			return(0);
                    }
                }
	    }
        }
    }
    return(0);
}

int mp_varlen(int p,int e,int f,long cct,FILE *fpo)
//int  p;
//int  e;
//int  f;
//long cct;
//FILE *fpo;
{
    long wdt;

    mp_chgpt(p,e,f,fpo);
    if(mp->cp[p][e][f]) {
        cdt = cdt + mp->dl[p][e][f];
    }
    Ph_LagOut(p,xdt,fpo);
    mp_out(p,e,f,fpo);
    mp->ct[p][e][f] = cct + mp->dl[p][e][f];;
    mp->cp[p][e][f] = mp->cp[p][e][f] + mp->dl[p][e][f];
    mp->cv[p][e][f] = mp->cv[p][e][f] + mp->iv[p][e][f];
    mpiv[p][e][f] = 0;
    mp->af[p][e][f] = 0;
    if(p != lv->port)
        mp_chgpt(lv->port,lv->memb,lv->trak,fpo);
    return(0);
}

/* port change fd code out ,Dummy member & track */
int mp_chgpt(int p,int e,int f,FILE *fpo)
//int  p; /* port */
//int  e;
//int  f;
//FILE *fpo;
{
    unsigned char wp;
    unsigned char wm;
    unsigned char wt;

    wp = (char)p;
    wp |= 0x80;      /* fd code flag set */
    wm = (char)mp->pm[p][e][f] & 0x7f;
    wt = (char)mp->pt[p][e][f] & 0x7f;
    if(ls->otrace == 8) {
        printf("                        ");
        printf("  fd %02x %02x %02x",
            wp,wt,wm);
        printf("  Port[%d] Track %d Member %d\n",
            p,wt,wm);
    }
    fputc(0xfd,fpo);
    fputc(wp,fpo);
    fputc(wt,fpo);
    fputc(wm,fpo);
    lv->occnt = lv->occnt + 4;
    return(0);
}

/* Multi port Phrase Active Flag Set */
int mp_afset()
{
    int  i,j,p;
    int  af;

    for(p = 0; p < MAXPT; p++) {
        for(i = 0; i < 16; i++) {
            for(j = 0; j < 5 ; j++) {
                af = 0;
                if(mp->iv[p][i][j] > 0) { /* positiv */
                    mpiv[p][i][j]++;
                    if(mpiv[p][i][j] >= mp->iv[p][i][j]) af++;
                    if(mp->cv[p][i][j] <= mp->ev[p][i][j]) af++;
                    else {
                        af = af - 1;
                        mp_clear(p,i,j);
                    }
                }
                if(mp->iv[p][i][j] < 0) { /* negativ */
                    mpiv[p][i][j]--;
                    if(mpiv[p][i][j] <= mp->iv[p][i][j]) af++;
                    if(mp->cv[p][i][j] > mp->ev[p][i][j]) af++;
                    else {
                        af = af - 1;
                        mp_clear(p,i,j);
                    }
                }
		/*
                if(mp->cp[p][i][j] <= mp->fl[p][i][j]) af++;
                else {
                    af = af - 1;
                    mp_clear(p,i,j);
                }
		*/
                if(af == 2) {
                    mp->af[p][i][j] = 1; /* Terget Active Flag Set */
		}
            }
        }
    }
    return(0);
}

int mp_clear(int p,int e,int f)
//int  p;  /* port */
//int  e;
//int  f;
{
    mp->cn[p][e][f] = 0;       /* phrase control number */
    mp->fl[p][e][f] = 0;       /* phrase length */
    mp->iv[p][e][f] = 0;       /* phrase increment value */
    mp->dl[p][e][f] = 0;       /* phrase time interval */
    mp->cv[p][e][f] = 0;       /* phrase current value */
    mp->ev[p][e][f] = 0;       /* phrase end value */
    mp->cp[p][e][f] = 0;       /* phrase current point */
    mp->af[p][e][f] = 0;       /* phrase activ on/off flag */
    return(0);
}

/* Multi port Phrasing code out */
int mp_out(int p,int e,int f,FILE *fpo)
//int  p;   /* port */
//int  e;   /* channel */
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
    int  fctn;   /* Phrase control number */

    if(f == 2) {  /* f=0:Volume f=1:Controle Change f=2:Pitch Vend */
        mp_pitch(p,e,f,fpo);
        return(0);
    }
    bit14 = 0;
    if((mp->cv[p][e][f] > 127) || (mp->ev[p][e][f] > 127)) bit14 = 1;
    if(bit14) {   /* 14 bit code */
        bx = 0x0f & e;
        bx |= 0x00b0;
        msb = mp->cv[p][e][f] / 128;  /* MSB */
        cno = 0x7f & mp->cn[p][e][f];
        if(msb != wmsb) {
            wmsb = msb;
            fputc((char)bx,fpo);   /* Bx */
            fputc((char)cno,fpo);  /* control # */
            fputc((char)msb,fpo);
            lv->occnt = lv->occnt + 3;
        }
        fputc((char)bx,fpo);
        fputc((char)(cno+32),fpo);
        lsb = mp->cv[p][e][f] - (msb * 128);  /* LSB */
        fputc((char)lsb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 8) {
            if(msb != wmsb) {
                printf("    %02x %02x %02x ",bx,cno,msb);
                printf("%02x %02x %02x    ",bx,cno+32,lsb);
                printf(" Ch.%d  ",e+1);
            }
            else {
                printf("    %02x %02x %02x    ",bx,cno+32,lsb);
                printf(" Ch.%d  ",e);
            }
        }
    }
    else {
        bx = 0x0f & e;
        bx |= 0x00b0;
        fputc((char)bx,fpo); /* Bx */
        cno = 0x7f & mp->cn[p][e][f];
        fputc((char)cno,fpo);  /* control # */
        msb = 0x007f & mp->cv[p][e][f]; /* MSB */
        fputc((char)msb,fpo);
        lv->occnt = lv->occnt + 3;
        if(ls->otrace == 8) {
            printf("    %02x %02x %02x    ",bx,cno,msb);
            printf(" Ch.%d  ",e+1);
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
            case  4 : fctn = mp->cn[p][e][f]; /* Controle Change */
                      break;
            default : break;
        }
        ph_outmsg(fctn);
        printf(" Val.%d\n",mp->cv[p][e][f]);
    }
    return(0);
}

/* Multi port pitch vend */
int mp_pitch(int p,int e,int f,FILE *fpo) 
//int  p;
//int  e;
//int  f;
//FILE *fpo;
{
    int  ex;
    int  msb;
    int  lsb;

    ex = 0x0f & e; /* Channel */
    ex |= 0x00e0;
    msb = mp->cv[p][e][f] / 128;  /* MSB */
    fputc((char)ex,fpo);   /* Ex */
    fputc((char)msb,fpo);
    lsb = mp->cv[p][e][f] - (msb * 128);  /* LSB */
    fputc((char)lsb,fpo);
    lv->occnt = lv->occnt + 3;
    if(ls->otrace == 8) {
        printf("    %02x %02x %02x    ",ex,msb,lsb);
        printf(" Ch.%d  *Pitch vend  Val.%d\n",e+1,mp->cv[p][e][f]);
    }
    return(0);
}

int Ph_LagOut(int p,long dt,FILE *fpo)
//int  p;
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
