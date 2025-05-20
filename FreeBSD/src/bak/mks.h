// ----------------------
//  AMuPLC (mks.h) clang
// ----------------------
// 2025.4.14 gcc -> clang

#define VERSION "1.02"
#define SUPPORT "Release"
//#define SUPPORT "Debug"

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"
//#define MAXDATA  10000000         // 10 MB
#define MAXDATA  6000000          // 6 MB

/*
#define OS "Windows"
#define TACTDIR "C:Program Files/AMuPLC/dat/"
#define MAXDATA  1000000          // 1 MB
*/

/*
#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"
#define MAXDATA  6000000          // 6 MB
*/

/*
#define OS "Raspberry"
#define TACTDIR "/usr/local/etc/tact/"
#define MAXDATA  1000000          // 1 MB
*/

/*
#define OS "CentOS"
#define TACTDIR "/var/www/local/etc/tact/"
#define MAXDATA  10000000         // 6 MB
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXPT 16                  /* max midi port */
#define MAXMB    256
#define FNLEN    256

#define COMPUTER_TYPE "PCBSD"
/*
#define COMPUTER_TYPE "PCBSD"
#define COMPUTER_TYPE "Linux"
#define COMPUTER_TYPE "IBMPC"
#define COMPUTER_TYPE "SUN"
*/

/* set default */
#define DIVISION 0      /* 0 : quarter-note resolution  1 : smpte */

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
    unsigned int  wl;
};

typedef struct {
    int  divf;         /* division flag */
    int  comptyp;
    int  f1cnt;
    int  dckn;         /* data check num */
    int  debug;
    int  trace;
    int  cvp;
    int  eof;
    int  Mtb[MAXMB];   /* member table */
    unsigned char em[MAXDATA];  /* .mex -> memory */
    int  ckport;
    int  cktrak;
    int  ckmemb;
    int  memb;
    int  sequ;
    int  barn;
    long lag;
    long lags;
    int  iw1;
    int  iw2;
    long lw;
    int  format;        /* 0: smf0 1: smf1 */
    int  trks;          /* No. fo tracks */
    int  res;           /* quarter-note resolution */
    int  flame;         /* MTC SMPTE format -24 -25 -29 -30 */
    int  mtcres;        /* division */
    int  smfdiv;        /* sform * mtcres */
    float divx;         /* divx = 3000 / 2 * ev->smfdiv */
    int  outf;          /* fputc_h() out falag */
    int  occnt;
    int  tsym;          /* FA status Note type */
    int  tempo;         /* FA status tempo */
    int  kscl;          /* F8 status kscl */
    int  ksin;          /* F8 status key signature */
    int  ritm;          /* F8 status rithm */
    int  time;          /* F8 status time signature */
    int  nval[22];      /* note val */
} MESV;

// mksm.c
int main(int argc,char *argv[]);
int dump_mks(char *fn);
int help_mks();
FILE *fopen_h(char *file, char *mode);
int init_ev();

// mks1.c
int Mesmain_mks(char *fn);
int SetMtb_mks();
char mgetc_h();
int fputc_h(char c);
int compchr_mks(unsigned char iword[4],int idata);
int compchk_mks(unsigned char iword[4]);

// mks2.c
int MTrk_mks(int memb,int pass);
int mididec_mks(unsigned char c);
int lagout_mks();
int VarLen_mks(register long value);

// mks3.c
int init_div();
int init_mtc();
int SetMThd_mks();
int SetMTrk_mks();
int ffmeta_mks();
int f0status_mks();
int f7status_mks();
int f8status_mks(unsigned char iword[4]);
int fastatus_mks(unsigned char iword[4]);

//   $$$$$$$$$$$$$$$$$$  //

