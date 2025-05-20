
// ----------------------
//  AMuPLC (mks3.c)
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

/* mks3.c */
#include "mks.h"
extern MESV *ev;
extern FILE *fpi,*fpo;
int iv[16][5];
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

int init_div()
{
    if(!ev->res) ev->res = 480;
    ev->flame = 0;
    ev->smfdiv = 0;
    ev->mtcres = 0;
    return(0);
}

int init_mtc()
{
    ev->res = 0;
    if(!ev->flame) ev->flame = 30;
    if(!ev->mtcres) ev->mtcres = 100;
    ev->smfdiv = ev->flame * ev->mtcres;
    return(0);
}

int SetMThd_mks()
{
    unsigned char wc[4];
    int  i;

    if(ev->format == 0) ev->trks = 1;
    ev->divx = 5;   /* default 1/600 sec  dt = 300 */
    if(ev->debug) {
        printf(
        "SetMThd_mks() format=%d trks=%d res=%d flame=%d mtcres=%d smfdiv=%d\n",
        ev->format,ev->trks,ev->res,ev->flame,ev->mtcres,ev->smfdiv);
    }
    compchr_mks(wc,ev->format);
    MThd[8] = wc[2];
    MThd[9] = wc[3];
    compchr_mks(wc,ev->trks);
    MThd[10] = wc[2];
    MThd[11] = wc[3];
    if(ev->res) {
        compchr_mks(wc,(long)6); /* MThd length */
        MThd[4] = wc[0];
        MThd[5] = wc[1];
        MThd[6] = wc[2];
        MThd[7] = wc[3];
        compchr_mks(wc,ev->res);
        /*
        printf("wc[0]=%02x wc[1]=%02x wc[2]=%02x wc[3]=%02x\n",
            wc[0],wc[1],wc[2],wc[3]);
        */
        MThd[12] = wc[2];  /* MThd division quarter-note format */
        MThd[13] = wc[3];
    }
    else {
        if(ev->smfdiv == 3000) ev->divx = 1;
        else ev->divx = (float)1500 / ev->smfdiv;
            /* division = dt
              dt = lv->stlag/ev->divx; 
                            _/ = 60     _/ = 120
              ev->divx = 10    1/300 sec   (dt = 150)
              ev->divx =  5    1/600 sec   (dt = 300)
              ev->divx =  2    1/1000 sec  (dt = 500)
              ev->divx =  1    1/3000 sec  (dt = 1500)

              dt = 60 * 600 / tempo 
              tempo : _/ = 120 -> ff 51 03 500000 
              ex. _/ = 60  : dt = 600 
                  _/ = 90  : dt = 400
                  _/ = 120 : dt = 300  <- default setting
           */
        compchr_mks(wc,-ev->flame); /* Smpte format */
        /*
        printf("wc[0]=%02x wc[1]=%02x wc[2]=%02x wc[3]=%02x\n",
            wc[0],wc[1],wc[2],wc[3]);
        */
        MThd[12] = wc[3];  /* MThd division SMPTE format */
        compchr_mks(wc,ev->mtcres);
        MThd[13] = wc[3];  /* MTC resolution */
    }
    if(ev->debug) {
        printf("ev->divx=%f\n",ev->divx);
        printf("MThd division =%02x %02x\n",MThd[12],MThd[13]);
    }
    if(ev->trace) printf("           ");
    for(i = 0; i < 4; i++) fputc_h(MThd[i]); /* Put SMF Header Format 0 */
    if(ev->trace) printf("   MThd\n");
    if(ev->trace) printf("           ");
    for(i = 4; i < 8; i++) fputc_h(MThd[i]);
    if(ev->trace) printf("   chank length\n");
    if(ev->trace) printf("           ");
    for(i = 8; i < 10; i++) fputc_h(MThd[i]);
    if(ev->trace) printf("         format %d\n",ev->format);
    if(ev->trace) printf("           ");
    for(i = 10; i < 12; i++) fputc_h(MThd[i]);
    if(ev->trace) printf("         number of tracks %d\n",ev->trks);
    if(ev->trace) printf("           ");
    for(i = 12; i < 14; i++) fputc_h(MThd[i]);
    if(ev->trace) {
        if(ev->res) printf("         resolution %d\n\n",ev->res);
        else printf("         division %d %d\n\n",-ev->flame,ev->mtcres);
    }
    ev->occnt = 14;
    if(ev->debug) printf("ev->dckn=%d ev->occnt=%d\n",ev->dckn,ev->occnt);
    return(0);
}

int SetMTrk_mks()
{
    int  i;
    int  cklen;
    unsigned char wc[4];

    if(ev->debug) printf("SetMTrk_mks() ev->outf=%d ev->dckn=%d ev->occnt=%d\n",
        ev->outf,ev->dckn,ev->occnt);
    cklen = ev->dckn;
    if(ev->trace) printf("           ");
    for(i = 0; i < 4; i++) fputc_h(MTrk[i]);
    if(ev->trace) printf("   MTrk\n");
    if(ev->trace) printf("           ");
    compchr_mks(wc,cklen);
    MTrk[4] = wc[0];
    MTrk[5] = wc[1];
    MTrk[6] = wc[2];
    MTrk[7] = wc[3];
    for(; i < 8; i++) fputc_h(MTrk[i]);
    if(ev->trace) printf("   chank length %d\n\n",cklen);
    return(0);
}

int ffmeta_mks()
{
    int  i;
    unsigned char c;

    i = 0;
    while(1) {
        c = mgetc_h(); 
        fputc_h(c);
        if(c == 0xf7) {
            if(ev->trace) printf("\n");
            break;
        }
        i++;
        if(i >= ev->eof) break;
    }
    return(0);
}

int f0status_mks()
{
    int  i;
    unsigned char c;

    i = 0;
    fputc_h(0x09); /* temp length */
    while(1) {
        c = mgetc_h();
        fputc_h(c);
        if(c == 0xf7) {
            if(ev->trace) printf("\n");
            break;
        }
        i++;
        if(i >= ev->eof) break;
    }
    return(0);
}

int f7status_mks()
{
    return(0);
}

/*   F8 code ....................................
 Time Signature
 FF 58 04 xx yy AA BB

Key and time : F8wxyyzz
 w(kscl)     : moll or dur  (none=0,dur=1,moll=2,major=3,minor=4)
 x(ksin)     : key signeture
 yy(ritm)    : rithm
 zz(time)    : time base
 ex. $###3/4 x_moll
         w(kscl) = 1
         x(ksin) = 3
         yy(ritm) = 3
         zz(time) = 4
*/
int f8status_mks(unsigned char iword[4])
//unsigned char iword[4];
{
    unsigned char w;
    int  sf;
    int  mi;

    ev->kscl = iword[1] & 0x70;
    ev->kscl = ev->kscl >> 4;
    ev->ksin = iword[1] & 0x0f;
    ev->ritm = iword[2] & 0xff;
    ev->time = iword[3] & 0xff;
    if(ev->debug) {
        printf("f8status_mks()         ");
        printf("F8 %02x %02x %02x  ",iword[1],iword[2],iword[3]);
        printf("kscl=%d ksin=%d ritm=%d time=%d\n",
            ev->kscl,ev->ksin,ev->ritm,ev->time);
    }
    /* Key Signature FF 59 02 sf mi */
    return(99);
    lagout_mks();
    fputc_h(0xff);
    fputc_h(0x59);
    fputc_h(0x02);
    mi = ev->ksin;
    w = mi & 0x0f;
    if(mi > 7) {
        mi = mi - 8;
        w = 0xff - mi;
    }
    fputc_h(w);
    mi = 0;
    if(ev->kscl == 1 || ev->kscl == 3) mi = 0;
    if(ev->kscl == 2 || ev->kscl == 4) mi = 1;
    w = mi & 0x0f;
    fputc_h(w);
    if(ev->trace) printf("  Key Signature\n");
    /* Time Signature FF 58 04 xx yy AA BB */ 
    lagout_mks();
    fputc_h(0xff);
    fputc_h(0x58);
    fputc_h(0x04);    /* len */
    w = ev->ritm & 0x7f;
    fputc_h(w);
    if(ev->time == 2)  w = 0x01;
    if(ev->time == 4)  w = 0x02;
    if(ev->time == 8)  w = 0x03;
    if(ev->time == 16)  w = 0x04;
    if(ev->time == 32)  w = 0x05;
    if(ev->time == 64)  w = 0x06;
    if(ev->time == 128)  w = 0x07;
    fputc_h(w);
    fputc_h(0x18);    /* AA */
    fputc_h(0x08);    /* BB */
    if(ev->trace) printf("  Time Signature\n");
    return(0);
}

/*
 Set Tempo
 FF 51 03 YY YY YY

Tempo         : FAxxyyyy    xx ignore
 ww & 0x80 : flag
              : FAwwyyyy    ww < 15

note            value        rest     no.      name
 %xx |O|       800,000        Rb        0
 %xx O..       700,000        Rw..      1
 %xx O.        600,000        Rw.       2
 %xx O         400,000        Rw        3      whole
 %xx o/..      350,000        Rh..      4
 %xx o/.       300,000        Rh.       5
 %xx o/        200,000        Rh        6       half
 %xx _/..      175,000        R..       7
 %xx _/.       150,000        R.        8
 %xx _/        100,000        R         9          4
 %xx _/)..      87,500        r..      10
 %xx _/).       75,000        r.       11
 %xx _/)        50,000        r        12          8
 %xx _/))..     43,750        rr..     13
 %xx _/)).      37,500        rr.      14
 %xx _/))       25,000        rr       15         16
 %xx _/)))..    21,875        rrr..    16
 %xx _/))).     18,750        rrr.     17
 %xx _/)))      12,500        rrr      18         32
 %xx _/)))).     9,375        rrrr.    19
 %xx _/))))      6,250        rrrr     20         64
 %xx _/)))))     3,125        rrrrr    21        128

 ww(tsym)     : note type 0-21
 yyyy(temp)   : tempo val *100
                ex. [m.m. _/ = 90] ww(tsym) = 9
                                   yyyy(temp) = 9000
                    use mr->nval,mr->tsym,mr->temp
 
 ex.
              YYYYYY
 _/ = 60      1000000  micro Sec
 _/ = 120      500000
*/
int fastatus_mks(unsigned char iword[4])
//unsigned char iword[4];
{
    unsigned char wk[4];
    int  tempo;
    float tf;

    wk[0] = 0x00;
    wk[1] = iword[1] & 0x7f;
    wk[2] = iword[2];
    wk[3] = iword[3];
    compchk_mks(wk);
    ev->tsym = ev->iw1;     
    ev->tempo = ev->iw2;     
    /* test 07 A1 20
    compchr_mks(wk,500000);
    printf("%02x %02x %02x\n",wk[1],wk[2],wk[3]);
    */
    tf = (100000/ev->nval[ev->tsym]) * 1000000;
    tempo = tf * (6000.0/(float)ev->tempo);
    if(ev->debug) {
        printf("fastatus_mks()         ");
        printf("FA %02x %02x %02x  ",iword[1],iword[2],iword[3]);
        printf("tsym=%d ev->tempo=%d tempo=%d\n",ev->tsym,ev->tempo,tempo);
    }
    compchr_mks(wk,tempo);
    lagout_mks();
    fputc_h(0xff);
    fputc_h(0x51);
    fputc_h(0x03);
    fputc_h(wk[1]);
    fputc_h(wk[2]);
    fputc_h(wk[3]);
    if(ev->trace) printf("  Set tempo\n");
    return(0);
}


