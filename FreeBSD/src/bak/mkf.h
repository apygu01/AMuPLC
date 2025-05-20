
// ----------------------
//  AMuPLC (mkf.h) clang
// ----------------------
// 2025.4.19 gcc -> clang

#define VERSION "1.01"
// #define SUPPORT "Debug"
#define SUPPORT "Release"

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
#define MAXDATA  10000000         // 10 MB

/*
#define OS "Windows"
#define TACTDIR "C:Program Files/AMuPLC/dat/"
*/

/*
#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"
#define MAXDATA  10000000         // 10 MB
*/

/*
#define OS "Raspberry"
#define TACTDIR "/var/www/local/etc/tact/"
#define MAXDATA  1000000          // 1 MB
*/

/*
#define OS "CentOS"
#define TACTDIR "/var/www/local/etc/tact/"
#define MAXDATA  10000000         // 10 MB
*/

/*
#define COMPUTER_TYPE "PC9801"
#define COMPUTER_TYPE "IBMPC"
#define COMPUTER_TYPE "SUN"
*/
#define COMPUTER_TYPE "IBMPC"
#define TURBO_C   0
#define UNIX_C    1

#if TURBO_C == 1
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#endif

#if UNIX_C == 1
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MAXA  20        /* max. argument number */
#define STLN  20        /* string get length */
#define MEXP  8000      /* max. expression number (megsym.dat size) */
#define MTMP  100       /* max. temp sign. */
#define MSTC  100       /* max. card stack size */
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b
#define STCTYP 3        /* smpte time code type 30 flame/sec(non dorop) */
#define MAXSN 8192      /* maximam source number */
#define MXMB  8192      /* maximam member */
#define PATHLEN  128

typedef struct {
    char card[82];
    int  debug;
    int  allf;
    int  prtlvl;      /* print level */
    int  memb;
    int  port;
    int  chan;
    int  trak;
    int  errcnt;
    int  epf;         /* Exec pass option flag */
    char ep[PATHLEN];
} MMKARG;

/*
// add 2015.1.5
int  help_main();
int  mkmultif_h();
int  filemake_h();
int  selfil_mec();
int  mecfchk1_mec();
int  mecfchk2_mec();
int  adsufix();
int  mkfmain();
*/

// mkf.c
int main(int argc,char *argv[]);
int mecfchk1_mec(char mfile[STLN],char partc);
int mecfchk2_mec(char mfile[STLN],char partc);
int mkfmain(char inf[STLN]);
int selfil_mec(char *mfile,int track[MXMB],char partf[MXMB][36],
    int port[MXMB],int channel[MXMB],int member[MXMB]);
int help_main();
int filemake_h(int n,char *inf,char *fext,
    char *dfile,int *yflp,int *xflp);
int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps);
int adsufix(char pmf[80],char *infn);
int strcat_h(char *tos,char *from);
FILE *fopen_h(char *file, char *mode);
int mkmultif_h(char *infn,char *outf,char *ftyp,
    int fnmb,int *prtn,int *yfile,int *xfile);
//============================================

