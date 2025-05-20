
// ======================
//  AMuPLC (mmv.h) RaspberryPi
// ======================

#define VERSION "1.03"

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#define OS "RaspberryPi"
//#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"

/*
#define OS "Windows"
#define TACTDIR "C:/Program Files/AMuPLC/dat/"
*/

/*
#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"
*/

/*
#define OS "CentOS"
#define TACTDIR "/var/www/local/etc/tact/"
*/

//#define COMPUTER_TYPE "SUN"
#define MS_C      0
#define UNIX_C    1

#if UNIX_C == 1
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MAXA  20        /* max. argument number */
#define STLN  80        /* string get length */
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b
#define MAXSN 8192      /* maximam source number */
#define MXMB  8192      /* maximam member */
#define PATHLEN 80

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
} MMKARG;

// mmc.c
FILE *fopen_h(char *file, char *mode);
int main(int argc,char *argv[]);
int mmvfchk1_mmv(char mfile[STLN],char partc);
int mmvfchk2_mmv(char mfile[STLN],char partc);
int mmvmain(char inf[STLN]);
int selfil_mmv(char *mfile,int track[MXMB],char partf[MXMB][36],
    int port[MXMB],int channel[MXMB],int member[MXMB]);
int help_main();
int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps);
int strcat_h(char *tos,char *from);
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,
    int *prtn,int *yfile,int *xfile);
int fmmv_mmv(char sfile[STLN],char *sfix);
int mksDir_mmv(char *sfile);
int mktDir_mmv();
int anscpy(char *sfile,char *sfix);
int sufix2(char pmf[STLN],char *sfixf);
int filemake_h(int n,char inf[STLN],char *fext,
    char dfile[STLN],int *yflp,int *xflp);
/// =====================================================

