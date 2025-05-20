
// ----------------------
//  AMuPLC (mks2.c)
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

/* mks2.c */
#include "mks.h"
extern MESV *ev;
extern FILE *fpi,*fpo;

int MTrk_mks(int memb,int pass)
//int  memb;
//int  pass;
{
    int  i;
    int  trace;
    unsigned char c;
    unsigned char c1;
    unsigned char cw;
    unsigned char iword[4];
    int  nxtgetf;

    if(ev->outf == 0) {
        trace = ev->trace;
        ev->trace = 0;
    }
    if(ev->debug) printf("\nMTrk_mks() memb=%d ev->trace=%d pass=%d =======\n",
        memb,ev->trace,pass);
    ev->memb = memb;
    /* .mex heder pass */
    ev->cvp = 22;
    ev->f1cnt = 0;
    ev->lags = 0;
    SetMTrk_mks();
    ev->dckn = 0;
    ev->lags = 0;
    if(ev->trace) printf("   Delta-time          Event        Comments\n");
    while(1) {
        nxtgetf = 0;
        if(nxtgetf == 1) c = c1;
        else c = mgetc_h();
        switch(c) {
            case  0xff : /* ff meta event */
                         /*
                         if(ev->memb != ev->ckmemb) {
                              if(ev->format) break;
                         }
                         if(ev->debug) {
                             printf("ffmeta_mks()\n");
                         }
                         lagout_mks();
                         fputc_h(c);
                         ffmeta_mks();
                         */
                         break;
            case  0xf0 : /* f0 status */
                         if(ev->memb != ev->ckmemb) {
                              if(ev->format) break;
                         }
                         if(ev->debug) printf("f0status_mks()\n");
                         lagout_mks();
                         fputc_h(c);
                         f0status_mks();
                         break;
            case  0xf1 : ev->f1cnt++;
                         iword[0] = c;
                         iword[1] = mgetc_h();
                         iword[2] = mgetc_h();
                         iword[3] = mgetc_h();
                         iword[0] &= 0x00;
                         iword[1] &= 0x7f;
                         compchk_mks(iword);
                         ev->lag = ev->lw;
                         ev->lags += ev->lag;
                         /*
                         printf("F1 %02x %02x %02x ev->lag=%d ev->lags=%d\n",
                         iword[1],iword[2],iword[3],ev->lag,ev->lags);
                         */
                         break;
            case  0xf8 : /* f8status */
                         if(ev->memb != 0) {
                              if(ev->format) break;
                         }
                         iword[0] = c;
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                             f8status_mks(iword);
                         }
                         break;
            case  0xfa : /* fastatus */
                         /* Out 2013.11.5 
                         if(ev->memb != 0) {
                              if(ev->format) break;
                         }
                         */
                         iword[0] = c;
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                             fastatus_mks(iword);
                         }
                         break;
            case  0xfd : iword[0] = c;           /* fd code check */
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                             ev->ckport = iword[1] & 0x7f;
                             ev->cktrak = iword[2];
                             ev->ckmemb = iword[3];
                             /*
                             printf(
                             "\nFD ev->ckport=%d ev->cktrak=%d ev->ckmemb=%d\n",
                             ev->ckport,ev->cktrak,ev->ckmemb);
                             */
                         }
                         break;
            case  0xfb : iword[0] = c;           /* fb code check */
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                             iword[0] &= 0x00;
                             iword[1] &= 0x7f;
                             compchk_mks(iword);
                             ev->sequ = ev->iw1;
                             ev->barn = ev->iw2;
                             if(ev->trace) {
                                 printf("-m%02d%05d-___\n",ev->sequ,ev->barn);
                             }
                             /*
                             printf(
                             "\nFB %02x %02x %02x  ev->sequ=%d ev->barn=%d\n",
                             iword[1],iword[2],iword[3],
                             ev->sequ,ev->barn);
                             */
                         }
                         break;
            /*  2012.10.15 Comment out for Pitch Bend data
            case  0xe5 : iword[0] = c; 
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else ev->ckport = iword[1] & 0x7f;
                         break;
            case  0xe6 : iword[0] = c;           // e6 code check
                         iword[1] = mgetc_h();
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             fputc_h(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = mgetc_h();
                             iword[3] = mgetc_h();
                         }
                         break;
            */
            default    : /* midi data */
                         mididec_mks(c);
                         break;
        }
        if(ev->cvp >= ev->eof) {
            /* ff 2f 00 end of eof */
            lagout_mks();
            fputc_h(0xff);
            fputc_h(0x2f);
            fputc_h(0x00);
            if(ev->trace) printf("    end of track\n=============\n");
            break;
        }
    }
    if(ev->debug) {
        printf("$$$$$$$$$$$ ev->dckn=%d ev->f1cnt=%d ev->occnt=%d ",
        ev->dckn,ev->f1cnt,ev->occnt);
        printf("ev->cvp=%d ev->eof=%d\n",ev->cvp,ev->eof);
    }
    if(ev->outf == 0) {
        ev->trace = trace;
    }
    return(0);
}

int mididec_mks(unsigned char c)
//unsigned char c;
{
    unsigned char cw;
    unsigned char c1,c2,c3,c4;
    int  ch;
    int  keyp;
    int  cno;
    int  pno;
    int  lsb;
    int  val;

    if(ev->memb != ev->ckmemb) {
        if(ev->format) return(0);
    }
    cw = c & 0xf0;
    switch(cw) {
        case  0x80  : /* Note off */
                      c1 = mgetc_h();                 
                      c2 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      keyp = c1 & 0x7f;
                      val = c2 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      fputc_h(c2);
                      if(ev->trace) 
                      printf("    Ch.%d Note off key# %d Val.%d\n",ch,keyp,val);
                      break;
        case  0x90  : /* Note on */
                      c1 = mgetc_h();                 
                      c2 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      keyp = c1 & 0x7f;
                      val = c2 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      fputc_h(c2);
                      if(ev->trace) {
                          if(c2 == 0x00) {
                             printf("    Ch.%d Note off key# %d Val.%d\n",
                             ch,keyp,val);
                          }
                          else {
                             printf("    Ch.%d Note on key# %d Val.%d\n",
                             ch,keyp,val);
                          }
                      }
                      break;
        case  0xa0  : /* Polyphonic key pressure */
                      c1 = mgetc_h();                 
                      c2 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      pno = c1 & 0x7f;
                      val = c2 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      fputc_h(c2);
                      if(ev->trace) 
                      printf("    Ch.%d Pol.key.pressure key# %d Val.%d\n",
                      ch,pno,val);
                      break;
        case  0xb0  : /* Control change */
                      c1 = mgetc_h();                 
                      c2 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      cno = c1 & 0x7f;
                      val = c2 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      fputc_h(c2);
                      if(ev->trace) 
                      printf("    Ch.%d Control change Cno.# %d Val.%d\n",
                      ch,cno,val);
                      break;
        case  0xc0  : /* Program change */
                      c1 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      val = c1 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      if(ev->trace) 
                      printf("       Ch.%d Program change Val.%d\n",ch,val);
                      break;
        case  0xd0  : /* Channel pressure */
                      c1 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      val = c1 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      if(ev->trace) 
                      printf("       Ch.%d Channel pressure Val.%d\n",ch,val);
                      break;
        case  0xe0  : /* Pitch Bend */
                      c1 = mgetc_h();                 
                      c2 = mgetc_h();                 
                      ch = (c & 0x0f) + 1;
                      lsb = c1 & 0x7f;
                      val = c2 & 0x7f;
                      lagout_mks();
                      fputc_h(c);
                      fputc_h(c1);
                      fputc_h(c2);
                      if(ev->trace) 
                      printf("    Ch.%d Pitch Bend LSB %d MSB %d\n",
                      ch,lsb,val);
                      break;
        default     : break;
    }
    return(0);
}

int lagout_mks()
{
    int  lag;
    float lagf;
    float divt;
    float tempo;

    tempo = ev->tempo;
    lag = ev->lags;
    if(ev->divf == 0) {
        divt = ev->lags * (tempo / 6000.0);
        lagf = (divt * ev->res) / 2400;
        lag = lagf + 0.5;
    }
    if(ev->divf == 1) {
        lag = (ev->lags * ev->smfdiv) / 2400;
    }
    if(ev->debug) {
        printf("\nlagout_mks() ev->lags=%ld ev->res=%d tempo=%f lag=%d\n",
        ev->lags,ev->res,tempo,lag);
    }
    VarLen_mks(lag);
    ev->lags = 0;
    return(0);
}

int VarLen_mks(register long value)
//register long value;
{
    long buffer;
    union MCODE u;
    unsigned char c;
    int  i;

    if(ev->trace) printf("%8ld - ",value);
    buffer = value & 0x7f;
    while((value >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value & 0x7f);
    }
    i = 0;
    while(1) {
        c = buffer & 0xff;
        fputc_h(c);
        i++;
        if(buffer & 0x80) buffer >>= 8;
        else break;
    }
    if(ev->trace) {
        if(i == 1) printf("         ");
        if(i == 2) printf("      ");
        if(i == 3) printf("   ");
    }
    return(0);
}

