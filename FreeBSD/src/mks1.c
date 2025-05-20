
// ----------------------
//  AMuPLC (mks1.c)
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

/* mks1.c */
#include "mks.h"
extern MESV *ev;
extern FILE *fpi,*fpo;
// char mgetc_h();

int Mesmain_mks(char *fn)
//char *fn;
{
    int  i;
    char inf[FNLEN];
    char outf[FNLEN];
    unsigned char iword[4];
    int  c;

    /* .mex check */
    strcpy(inf,fn);
    strcat(inf,".mex");
    fpi = fopen_h(inf,"r");
    strcpy(outf,fn);
    strcat(outf,".mid");
    fpo = fopen_h(outf,"wb");
    if(ev->debug) printf("Mesmain_mks() fn=%s inf=%s outf=%s\n",
        fn,inf,outf);
    ev->eof = 0;
    i = 0;
    while(1) {
        c = fgetc(fpi);
        ev->em[i] = c & 0xff;
        if(c == EOF) break;
        i++;
        if( i > MAXDATA) break;
    }
    ev->eof = i;
    /* while(fgetc(fpi) != EOF) ev->eof++;  add 2012.10.15 */
    /* MXrk ? */
    /*
    printf("ev->em[] = %c %c %c %c\n",
        ev->em[14],ev->em[15],ev->em[16],ev->em[17]);
    if(ev->em[14] == 'M' && ev->em[15] == 'X' && ev->em[16] == 'r' &&
        ev->em[17] == 'k') {
        iword[0] = ev->em[18];
        iword[1] = ev->em[19];
        iword[2] = ev->em[20];
        iword[3] = ev->em[21];
        compchk_mks(iword);
        printf("MXrk len :  ev->lw=%d\n",ev->lw);
    }
    */
    if(ev->debug) printf("%s size ev->eof=%d\n",inf,ev->eof);
    fclose(fpi);
    SetMtb_mks();
    for(i = 0 ; i < MAXMB; i++) {
        if(ev->Mtb[i]) ev->trks++;
    }
    ev->occnt = 0;
    ev->outf = 1;
    SetMThd_mks();
    if(ev->format == 0) {
        ev->outf = 0;
        MTrk_mks(0,1);
        ev->outf = 1;
        MTrk_mks(0,2);
    }
    if(ev->format == 1) {
        for(i = 0 ; i < MAXMB; i++) {
            if(ev->Mtb[i]) {
                ev->outf = 0;
                MTrk_mks(i,1);
                ev->outf = 1;
                MTrk_mks(i,2);
            }
        }
    }
    fclose(fpo);
    printf("Mks end SMF format %d Tracks %d Resolution %d\n",
        ev->format,ev->trks,ev->res);
    return(0);
}

int SetMtb_mks()
{
    int  i;
    unsigned char c,c1;
    unsigned char iword[6];

    for(i = 0; i < MAXMB; i++) ev->Mtb[i] = 0;
    ev->cvp = 22;
    while(1) {
        c = mgetc_h();
        switch(c) {
            case  0xfd : iword[0] = c;           /* fd code check */
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(c1) {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                             ev->ckport = iword[1] & 0x7f;
                             ev->cktrak = iword[2];
                             ev->ckmemb = iword[3];
                             /*
                             printf(
                             "FD ev->ckport=%d ev->cktrak=%d ev->ckmemb=%d\n",
                             ev->ckport,ev->cktrak,ev->ckmemb);
                             */
                             if(ev->ckmemb >= 0 && ev->ckmemb < MAXMB) {
                                 ev->Mtb[ev->ckmemb] = 1;
                             }
                         }
                         break;
             defaut    : break;
        }
        if(ev->cvp >= ev->eof) break;
    }
    return(0);
}

char mgetc_h()
{
    return(ev->em[ev->cvp++]);
}

int fputc_h(char c)
//char c;
{
    if(ev->trace) printf("%02x ",c & 0xff); 
    ev->dckn++;
    if(ev->outf) {
        fputc(c,fpo);
        ev->occnt++;
    }
    return(0);
}

int compchr_mks(unsigned char iword[4],int idata)
//unsigned char iword[4];
//int  idata;
{
    union MCODE u;

    u.wl = idata;
    if(ev->comptyp == 0) {
        iword[1] = u.wc[0];
        iword[0] = u.wc[1];
        iword[3] = u.wc[2];
        iword[2] = u.wc[3];
    }
    if(ev->comptyp == 1) {
        iword[0] = u.wc[0];
        iword[1] = u.wc[1];
        iword[2] = u.wc[2];
        iword[3] = u.wc[3];
    }
    if(ev->comptyp == 2) {
        iword[3] = u.wc[0];
        iword[2] = u.wc[1];
        iword[1] = u.wc[2];
        iword[0] = u.wc[3];
    }
    return(0);
}

int compchk_mks(unsigned char iword[4])
//unsigned char iword[4];
{
    union MCODE u;

    if(ev->comptyp == 0) {
        u.wc[0] = iword[1];
        u.wc[1] = iword[0];
        u.wc[2] = iword[3];
        u.wc[3] = iword[2];
        ev->iw1 = u.wi[0];
        ev->iw2 = u.wi[1];
        ev->lw = u.wl;
    }
    if(ev->comptyp == 1) {
        u.wc[0] = iword[0];
        u.wc[1] = iword[1];
        u.wc[2] = iword[2];
        u.wc[3] = iword[3];
        ev->iw1 = u.wi[0];
        ev->iw2 = u.wi[1];
        ev->lw = u.wl;
    }
    if(ev->comptyp == 2) {
        u.wc[0] = iword[3];
        u.wc[1] = iword[2];
        u.wc[2] = iword[1];
        u.wc[3] = iword[0];
        ev->iw1 = u.wi[1];
        ev->iw2 = u.wi[0];
        ev->lw = u.wl;
    }
    return(0);
}

